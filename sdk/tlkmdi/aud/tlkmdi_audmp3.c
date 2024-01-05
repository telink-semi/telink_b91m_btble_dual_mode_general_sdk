/********************************************************************************************************
 * @file	tlkmdi_audmp3.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#if TLK_MDI_MP3_ENABLE
#include "drivers.h"
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/aud/tlkmdi_audmp3.h"
#include "tlkalg/audio/mp3/tlkalg_mp3.h"
#include "tlkdev/tlkdev.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"
#include "tlksys/prt/tlkpti_audio.h"

#define TLKMDI_AUDMP3_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_MP3 << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDMP3_DBG_SIGN       "[MP3]"


#define TLKMDI_MP3_LABEL_HLEN          10 //Label Head Length
#define TLKMDI_MP3_FILE_CACHE_SIZE     2048
#define TLKMDI_MP3_FILE_CACHE_HALF     1024 //==TLKMDI_MP3_FILE_CACHE_THIRD/2
#define TLKMDI_MP3_FILE_CACHE_THIRD    680

#ifndef FR_OK
#define FR_OK	0
#endif
#define TLKMDI_MP3_INFO_CACHE_SIZE     (MINIMP3_MAX_SAMPLES_PER_FRAME)
#if (TLK_FS_FAT_ENABLE)
	static FIL sTlkMdiMp3File;
#else
	static uint08 sTlkMdiMp3File;
#endif

extern int rand(void);
extern unsigned int tlkcfg_getFlashAddr(unsigned int offset);

//static bool tlkmdi_mp3_isPrivateFrame(uint08 *pData, uint16 dataLen, uint32 *pOffs, uint32 *pSize);
static uint32 tlkmdi_mp3_getPlayDuration(uint08 *pData, uint16 dataLen);
//static uint32  tlkmdi_mp3_getPrivateFramePlayDuration(uint32 PrivateFrameOffset, uint32 PrivateFrameSize);
static bool tlkmdi_mp3_getPlayInfo(void);
static bool tlkmdi_mp3_getPerformerInfo(uint08 *pData, uint16 dataLen, uint08 *pNameBuff, 
	uint08 buffLens, uint16 *pNameSize, uint08 *pNameFlag);




uint08 gTlkMdiMp3Status = 0;

sint16 *spTlkMdiMp3DecodeTempBuff = NULL; //TLKMDI_MP3_INFO_CACHE_SIZE
uint08 *spTlkMdiMp3FileInfoTempBuff = NULL;
uint08 *spTlkMdiMp3DurationTempBuff = NULL;

static mp3dec_t *spTlkMdiMp3Decode = NULL;
static tlkmdi_mp3_ctrl_t sTlkMdiMp3Ctrl;

static bool sTlkMdiMp3IsEnable = false;
static tlkapi_fifo_t sTlkMdiMp3Fifo = {0};
static uint08 sTlkMdiMp3UpdateFlags = 0;
static uint32 sTlkMdiMp3UpdateTimer = 0;



static uint08 *spTlkMdiMp3ParamBuff = NULL;
static uint08 *spTlkMdiMp3CacheBuff = NULL;
static uint08 *spTlkMdiMp3RawPcmBuff = NULL;
static uint08 sTlkMdiMp3FileCache[TLKMDI_MP3_FILE_CACHE_SIZE];


/******************************************************************************
 * Function: tlkmdi_mp3_init
 * Descript: Initial the mp3 codec and reset the control parameter.
 * Params: TLK_ENONE is success,other value is false .
 * Return: None.
*******************************************************************************/
int tlkmdi_mp3_init(void)
{
	uint08 finfo[TLKMDI_MP3_FILE_INFO_SIZE];
	unsigned int mp3FileAddr;

	mp3FileAddr = tlkcfg_getFlashAddr(TLKMDI_MP3_FILE_INFO_ADDR);
	if(mp3FileAddr == 0) return -TLK_EFAIL;
		
	//Verify macro configuration parameters
	if(mp3dec_Query_BufferSize() > TLKMDI_MP3_DEC_PARAM_SIZE 
		|| mp3dec_Query_ScratchSize() > TLKMDI_MP3_DEC_CACHE0_SIZE){
		while(true){
			tlkapi_error(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "MP3 Buffer Error");
			tlkdbg_handler();
		}
	}
	
	//Reset control parameters
	memset(&sTlkMdiMp3Ctrl, 0, sizeof(tlkmdi_mp3_ctrl_t));
	
	tlkapi_save2_init(&sTlkMdiMp3Ctrl.save, TLKMDI_MP3_FILE_INFO_SIGN, TLKMDI_MP3_FILE_INFO_VERS,
		TLKMDI_MP3_FILE_INFO_SIZE, mp3FileAddr);
	if(tlkapi_save2_load(&sTlkMdiMp3Ctrl.save, finfo, TLKMDI_MP3_FILE_INFO_SIZE) <= 0){
		sTlkMdiMp3Ctrl.fsave.playLens = 0;
		tmemset(sTlkMdiMp3Ctrl.fsave.fileName, 0, TLKMDI_MP3_NAME_SIZE);
	}else{
		tmemcpy(&sTlkMdiMp3Ctrl.fsave.playLens, finfo, 4);
		tmemcpy(sTlkMdiMp3Ctrl.fsave.fileName, finfo+4, TLKMDI_MP3_NAME_SIZE);
		sTlkMdiMp3Ctrl.fsave.fileName[TLKMDI_MP3_NAME_SIZE-2] = 0;
		sTlkMdiMp3Ctrl.fsave.fileName[TLKMDI_MP3_NAME_SIZE-1] = 0;
	}

	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_open(TLKDEV_DID_SDCARD0, 0);
	#endif
	tlkmdi_mp3_loadFile(true);
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_close(TLKDEV_DID_SDCARD0);
	#endif
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_mp3_isEnable
 * Descript: Verify the mp3 is Enable.
 * Params: true is success,other value is false .
 * Return: None.
*******************************************************************************/
bool tlkmdi_mp3_isEnable(void)
{
	return sTlkMdiMp3IsEnable;
}

/******************************************************************************
 * Function: tlkmdi_mp3_enable
 * Descript: Enable the mp3.
 * Params: true is success,other value is false .
 * Return: None.
*******************************************************************************/
bool tlkmdi_mp3_enable(bool enable)
{
	sTlkMdiMp3IsEnable = false;
	if(enable){
		if(spTlkMdiMp3CacheBuff == NULL) spTlkMdiMp3CacheBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_MP3_DEC_CACHE_SIZE);
		if(spTlkMdiMp3ParamBuff == NULL) spTlkMdiMp3ParamBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_MP3_DEC_PARAM_SIZE);
		if(spTlkMdiMp3RawPcmBuff == NULL) spTlkMdiMp3RawPcmBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_MP3_DEC_PCM_SIZE);
		if(spTlkMdiMp3ParamBuff != NULL && spTlkMdiMp3CacheBuff != NULL && spTlkMdiMp3RawPcmBuff != NULL){
			sTlkMdiMp3IsEnable = true;
		}
	}
	
	if(sTlkMdiMp3IsEnable){
		tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_enable: true");
	}else{
		tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_enable: false");
		if(spTlkMdiMp3ParamBuff != NULL){
			tlkmdi_audmem_free(spTlkMdiMp3ParamBuff);
			spTlkMdiMp3ParamBuff = NULL;
		}
		if(spTlkMdiMp3CacheBuff != NULL){
			tlkmdi_audmem_free(spTlkMdiMp3CacheBuff);
			spTlkMdiMp3CacheBuff = NULL;
		}
		if(spTlkMdiMp3RawPcmBuff != NULL){
			tlkmdi_audmem_free(spTlkMdiMp3RawPcmBuff);
			spTlkMdiMp3RawPcmBuff = NULL;
		}
	}

	//Set Buffer
	if(!sTlkMdiMp3IsEnable){
		spTlkMdiMp3Decode = NULL;
		spTlkMdiMp3DecodeTempBuff = NULL; //TLKMDI_MP3_INFO_CACHE_SIZE
		spTlkMdiMp3FileInfoTempBuff = NULL;
		spTlkMdiMp3DurationTempBuff = NULL;
		tlkapi_fifo_reset(&sTlkMdiMp3Fifo);
		#if (TLK_DEV_STORE_ENABLE)
		tlkdev_close(TLKDEV_DID_SDCARD0);
		#endif
	}else{
		uint16 offset = 0;
		#if (TLK_DEV_STORE_ENABLE)
		tlkdev_open(TLKDEV_DID_SDCARD0, 0);
		#endif
		spTlkMdiMp3Decode = mp3dec_init(spTlkMdiMp3ParamBuff, spTlkMdiMp3CacheBuff);
		tlkapi_fifo_init(&sTlkMdiMp3Fifo, false, false, spTlkMdiMp3RawPcmBuff, TLKMDI_MP3_DEC_PCM_SIZE);
		tlkmdi_mp3_clearPcmData();
		spTlkMdiMp3DecodeTempBuff   = (short*)(spTlkMdiMp3CacheBuff+TLKMDI_MP3_DEC_CACHE1_OFFS);
		spTlkMdiMp3FileInfoTempBuff = (uint08*)(spTlkMdiMp3CacheBuff+offset); offset += TLKMDI_MP3_INFO_CACHE_SIZE;
		spTlkMdiMp3DurationTempBuff = (uint08*)(spTlkMdiMp3CacheBuff+offset); offset += TLKMDI_MP3_INFO_CACHE_SIZE;
	}
	
	if(enable != sTlkMdiMp3IsEnable){
		tlkapi_error(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_enable: no memory !!!");
		tlkmdi_audmem_print();
		return false;
	}
	return true;
}

/******************************************************************************
 * Function: tlkmdi_mp3_indexIsOver,tlkmdi_mp3_XXXXXXIndex
 * Descript: verify is the last music or pick up a muisc.
 * Params: None.
 * Return: .
*******************************************************************************/
bool tlkmdi_mp3_indexIsOver(void)
{
	uint08 mode;
	if(sTlkMdiMp3Ctrl.fileCount == 0) return true;
	mode = sTlkMdiMp3Ctrl.fplay.playMode;
	if(mode == TLKMDI_MP3_PALY_MODE_ORDER && sTlkMdiMp3Ctrl.playIndex == sTlkMdiMp3Ctrl.fileCount-1){
		return true;
	}else{
		return false;
	}
}
uint tlkmdi_mp3_getNextPlay(void)
{
	uint08 mode;
	uint16 index;
	if(sTlkMdiMp3Ctrl.fileCount == 0) return 0;
	mode = sTlkMdiMp3Ctrl.fplay.playMode;
	if(mode == TLKMDI_MP3_PALY_MODE_ORDER){
		index = sTlkMdiMp3Ctrl.playIndex+1;
		if(index >= sTlkMdiMp3Ctrl.fileCount) index = 0;
	}else if(mode == TLKMDI_MP3_PALY_MODE_RANDOM){
		index = tlkmdi_mp3_getRandIndex();
	}else{
		index = sTlkMdiMp3Ctrl.playIndex;
	}
	return index;
}
void tlkmdi_mp3_setPlayIndex(uint playIndex)
{
	if(playIndex >= sTlkMdiMp3Ctrl.fileCount){
		playIndex = sTlkMdiMp3Ctrl.fileCount-1;
	}
	sTlkMdiMp3Ctrl.playIndex = playIndex;
	sTlkMdiMp3Ctrl.fsave.playLens = 0;
}
uint tlkmdi_mp3_getPlayIndex(void)
{
	return sTlkMdiMp3Ctrl.playIndex;
}
uint tlkmdi_mp3_getPlayCount(void)
{
	return sTlkMdiMp3Ctrl.fileCount;
}
uint tlkmdi_mp3_getRandIndex(void)
{
	uint16 index;
	uint32 random;

	if(sTlkMdiMp3Ctrl.fileCount == 0) return 0;
	
	index = sTlkMdiMp3Ctrl.playIndex;
	random = rand()%sTlkMdiMp3Ctrl.fileCount;
	if(random != index){
		index = random;
	}else{
		if((random & 0x01) != 0) index += 1;
		else index -= 1;
		if(index >= sTlkMdiMp3Ctrl.fileCount) index = 0;
	}
	return index;
}
uint tlkmdi_mp3_getNextIndex(void)
{
#if 1
	uint08 mode;
	uint16 index;
	if(sTlkMdiMp3Ctrl.fileCount == 0) return 0;
	mode = sTlkMdiMp3Ctrl.fplay.playMode;
	if(mode == TLKMDI_MP3_PALY_MODE_ORDER || mode == TLKMDI_MP3_PALY_MODE_LOOP){
		index = sTlkMdiMp3Ctrl.playIndex+1;
		if(index >= sTlkMdiMp3Ctrl.fileCount) index = 0;
	}else if(mode == TLKMDI_MP3_PALY_MODE_RANDOM){
		index = tlkmdi_mp3_getRandIndex();
	}else{
		index = sTlkMdiMp3Ctrl.playIndex;
	}
	return index;
#else
	uint16 index;
	if(sTlkMdiMp3Ctrl.fileCount == 0) return 0;
	index = sTlkMdiMp3Ctrl.playIndex+1;
	if(index >= sTlkMdiMp3Ctrl.fileCount) index = 0;
	return index;
#endif
}
uint tlkmdi_mp3_getPrevIndex(void)
{
#if 1
	uint08 mode;
	uint16 index;
	if(sTlkMdiMp3Ctrl.fileCount == 0) return 0;
	mode = sTlkMdiMp3Ctrl.fplay.playMode;
	if(mode == TLKMDI_MP3_PALY_MODE_ORDER || mode == TLKMDI_MP3_PALY_MODE_LOOP){
		if(sTlkMdiMp3Ctrl.playIndex != 0) index = sTlkMdiMp3Ctrl.playIndex-1;
		else index = sTlkMdiMp3Ctrl.fileCount-1;
	}else if(mode == TLKMDI_MP3_PALY_MODE_RANDOM){
		index = tlkmdi_mp3_getRandIndex();
	}else{
		index = sTlkMdiMp3Ctrl.playIndex;
	}
	return index;
#else
	uint16 index;
	if(sTlkMdiMp3Ctrl.fileCount == 0) return 0;
	if(sTlkMdiMp3Ctrl.playIndex != 0) index = sTlkMdiMp3Ctrl.playIndex-1;
	else index = sTlkMdiMp3Ctrl.fileCount-1;
	return index;
#endif
}

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Get the music info and set the music mode.
 * Params: None.
 * Return: .
*******************************************************************************/
uint32 tlkmdi_mp3_getFileSize(void)
{
	return sTlkMdiMp3Ctrl.finfo.fileSize;
}
uint32 tlkmdi_mp3_getPlayLens(void)
{
	return sTlkMdiMp3Ctrl.fsave.playLens;
}
uint32 tlkmdi_mp3_getDuration(void)
{
	return sTlkMdiMp3Ctrl.finfo.duration;
}
uint16 tlkmdi_mp3_getProgress(void)
{
	uint16 progress;
	uint32 fileSize = sTlkMdiMp3Ctrl.finfo.fileSize;
	uint64 playLens = sTlkMdiMp3Ctrl.fsave.playLens;

	if(!sTlkMdiMp3IsEnable){
		progress = 0;
	}else if(fileSize == 0){
		progress = 1000;
	}else{
		progress = (playLens*1000)/fileSize;
		if(progress > 1000) progress = 1000;
	}
	return progress;
}
uint08 tlkmdi_mp3_getFNameCode(void)
{
	return 0x01;
}
uint08 tlkmdi_mp3_getSingerCode(void)
{
	return ((sTlkMdiMp3Ctrl.finfo.nameFlag & 0xC0) >> 6);
}

uint08 *tlkmdi_mp3_getFileName(uint08 *pLength)
{
	if(pLength != nullptr) *pLength = 0;
	if(!sTlkMdiMp3IsEnable) return nullptr;

	if(pLength != nullptr){
		if(sTlkMdiMp3Ctrl.fsave.fileName[TLKMDI_MP3_NAME_SIZE-1] != 0 
			|| sTlkMdiMp3Ctrl.fsave.fileName[TLKMDI_MP3_NAME_SIZE-2] != 0){
			*pLength = TLKMDI_MP3_NAME_SIZE;
		}else{
			*pLength = sizeof(FCHAR)*tlkapi_file_strlen((FCHAR*)sTlkMdiMp3Ctrl.fsave.fileName);
		}
	}
	return sTlkMdiMp3Ctrl.fsave.fileName;
}
uint08 *tlkmdi_mp3_getSinger(uint08 *pLength)
{
	if(pLength != nullptr) *pLength = 0;
	if(!sTlkMdiMp3IsEnable) return nullptr;
	
	if(pLength != nullptr){
		if(sTlkMdiMp3Ctrl.finfo.performer[TLKMDI_MP3_PERFORMER_SIZE-1] != 0){
			*pLength = TLKMDI_MP3_PERFORMER_SIZE;
		}else{
			*pLength = (sTlkMdiMp3Ctrl.finfo.nameFlag & 0x3F);
		}
	}
	return sTlkMdiMp3Ctrl.finfo.performer;
}
uint08 tlkmdi_mp3_getPlayMode(void)
{
	return sTlkMdiMp3Ctrl.fplay.playMode;
}
void tlkmdi_mp3_setPlayMode(uint08 mode)
{
	sTlkMdiMp3Ctrl.fplay.playMode = mode;
}

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Get the music playing parameter,etc, samplerate and volume.
 * Params: None.
 * Return: .
*******************************************************************************/
uint16 tlkmdi_mp3_getSampleRate(void)
{
	return sTlkMdiMp3Ctrl.finfo.sampleRate;
}
uint08 tlkmdi_mp3_getChannels(void)
{
	return sTlkMdiMp3Ctrl.finfo.channels;
}



/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Handle the mp3 pcm data.
 * Params: None.
 * Return: .
*******************************************************************************/
void tlkmdi_mp3_clearPcmData(void)
{
	tlkapi_fifo_clear(&sTlkMdiMp3Fifo);
}
int tlkmdi_mp3_getPcmIdleLen(void)
{
	return tlkapi_fifo_idleLen(&sTlkMdiMp3Fifo);
}
int tlkmdi_mp3_getPcmDataLen(void)
{
	return tlkapi_fifo_dataLen(&sTlkMdiMp3Fifo);
}
void tlkmdi_mp3_addPcmData(uint08 *pData, uint16 dataLen)
{
	tlkapi_fifo_write(&sTlkMdiMp3Fifo, pData, dataLen);
}
uint tlkmdi_mp3_getPcmData(uint08 *pBuffer, uint16 buffLen)
{
	uint16 readLen;
	uint16 dataLen;
	
	dataLen = tlkapi_fifo_dataLen(&sTlkMdiMp3Fifo);
	if(dataLen > buffLen) readLen = buffLen;
	else readLen = dataLen;
	if(readLen != 0) tlkapi_fifo_read(&sTlkMdiMp3Fifo, pBuffer, readLen);
	return readLen;
}


/******************************************************************************
 * Function: tlkmdi_mp3_getInfo
 * Descript: Get the mp3 info.
 * Params: Mp3 info.
 * Return: None.
*******************************************************************************/
tlkmdi_mp3_finfo_t *tlkmdi_mp3_getInfo(void)
{
	return &sTlkMdiMp3Ctrl.finfo;
}

/******************************************************************************
 * Function: tlkmdi_mp3_isOver
 * Descript: verify mp3 data in tail position.
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_mp3_isOver(void)
{
	return (sTlkMdiMp3Ctrl.fplay.playLens >= sTlkMdiMp3Ctrl.finfo.fileSize);
}

void tlkmdi_mp3_reset(void)
{
	sTlkMdiMp3Ctrl.fplay.playLens = 0;
}

/******************************************************************************
 * Function: tlkmdi_mp3_play/start/close
 * Descript: control mp3 paly/stop.
 * Params: @index[IN]--Mp3 index.
 *         @pFilePath[IN]--The file path.
 *         @fileOffset[IN]--The file path.
 * Return: None.
*******************************************************************************/
bool tlkmdi_mp3_play(uint16 index)
{
	uint32 playLen;
	uint08 pathLen;
	uint08 fpath[TLKMDI_MP3_PATH_SIZE+2] = {0};

//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_play_start 01: %d", sTlkMdiMp3Ctrl.fileCount);
	if(index >= sTlkMdiMp3Ctrl.fileCount) index = 0;
	
	tlkmdi_mp3_findFilePath(fpath, TLKMDI_MP3_DIR_SIZE, true);
	pathLen = tlkapi_file_strlen((FCHAR*)fpath);
	if(pathLen == 0) return false;

//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_play_start 02");
	if(tlkmdi_mp3_findFileName(fpath+pathLen, index) != TLK_ENONE){
		return false;
	}
	if(index == sTlkMdiMp3Ctrl.playIndex){
		if (tlkmdi_mp3_getPlayMode() == TLKMDI_MP3_PALY_MODE_RANDOM) {
			playLen = 0;
		} else {
			playLen = sTlkMdiMp3Ctrl.fsave.playLens;
		}
	}else{
		playLen = 0;
	}
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_play_start 03: %d %d",
//		sTlkMdiMp3Ctrl.fsave.playLens, sTlkMdiMp3Ctrl.finfo.fileSize);
	
	if(!tlkmdi_mp3_start(fpath, playLen)){
		if(index == 0) return false;
		else return tlkmdi_mp3_play(0);
	}
	
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_play_start 04");
	if((index != sTlkMdiMp3Ctrl.playIndex) || (0 == sTlkMdiMp3Ctrl.playIndex)){
		sTlkMdiMp3Ctrl.playIndex = index;
		tmemcpy(sTlkMdiMp3Ctrl.fsave.fileName, fpath+pathLen, TLKMDI_MP3_NAME_SIZE);
		tlkmdi_mp3_updatePlayName(fpath+pathLen, true);
	}
	
	return true;
}
bool tlkmdi_mp3_start(uint08 *pFilePath, uint32 fileOffset)
{
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_start 001");
	if(spTlkMdiMp3Decode == nullptr || spTlkMdiMp3FileInfoTempBuff == nullptr
		|| spTlkMdiMp3DurationTempBuff == nullptr){
		return false;
	}
			
	spTlkMdiMp3Decode = mp3dec_init(spTlkMdiMp3ParamBuff, spTlkMdiMp3CacheBuff);
	if(spTlkMdiMp3Decode == nullptr) return false;

	tlkapi_file_close(&sTlkMdiMp3File);
	if(tlkapi_file_open(&sTlkMdiMp3File, (const FCHAR*)pFilePath, TLKAPI_FM_OPEN_ALWAYS | TLKAPI_FM_READ) != TLK_ENONE){
		return false;
	}
	
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_start 002");
	if(!tlkmdi_mp3_getPlayInfo()){
		tlkapi_file_close(&sTlkMdiMp3File);
		return false;
	}

	if(fileOffset >= sTlkMdiMp3Ctrl.finfo.fileSize){
		fileOffset = sTlkMdiMp3Ctrl.finfo.headLens;
	}else if(fileOffset < sTlkMdiMp3Ctrl.finfo.headLens){
		fileOffset = sTlkMdiMp3Ctrl.finfo.headLens;
	}else if(fileOffset >= sTlkMdiMp3Ctrl.finfo.fileSize){
		fileOffset = sTlkMdiMp3Ctrl.finfo.headLens;
	}
	sTlkMdiMp3Ctrl.fplay.playLens = fileOffset;
	
//	tlkapi_info(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "MP3 Play Info: %d %d %d", 
//		sTlkMdiMp3Ctrl.fplay.curIndex, sTlkMdiMp3Ctrl.finfo.sampleRate, sTlkMdiMp3Ctrl.fplay.sa, sTlkMdiMp3Ctrl.fplay.playLens);
	
	tlkmdi_mp3_clearPcmData();
	sTlkMdiMp3Ctrl.fplay.cacheLens = 0;
	sTlkMdiMp3Ctrl.fplay.cacheOffs = 0;
	
	//Jump file to decode frame
	tlkapi_file_seek(&sTlkMdiMp3File, sTlkMdiMp3Ctrl.fplay.playLens);
	tlkmdi_mp3_updatePlayLens(sTlkMdiMp3Ctrl.fplay.playLens, true);

	spTlkMdiMp3Decode = mp3dec_init(spTlkMdiMp3ParamBuff, spTlkMdiMp3CacheBuff);
	if(spTlkMdiMp3Decode == nullptr) return false;
	
	return true;
}
void tlkmdi_mp3_close(void)
{
	tlkapi_file_close(&sTlkMdiMp3File);
}


/******************************************************************************
 * Function: tlkmdi_mp3_decode
 * Descript: decode audio data.
 * Params: None.
 * Return: Return SUCCESS or other value is false.
*******************************************************************************/
int tlkmdi_mp3_decode(void)
{
	int ret = 0;
	uint32 tempLen;
	int fOffset; //Frame Offset
	int fLength;
	int tOffset; //Temp Offset
	int freeLen;
    mp3dec_frame_info_t frame_info;
	
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 001");
	if(spTlkMdiMp3Decode == NULL || spTlkMdiMp3DecodeTempBuff == NULL) return -TLK_EPARAM;
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 002");
	if(tlkmdi_mp3_getPcmIdleLen() < (sTlkMdiMp3Ctrl.finfo.channels == 2 ? TLKMDI_MP3_PCM_SIZE : TLKMDI_MP3_PCM_SIZE/2)){
		return TLK_ENONE;
	}
	
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 003");
	if(sTlkMdiMp3Ctrl.fplay.cacheOffs == sTlkMdiMp3Ctrl.fplay.cacheLens){
		sTlkMdiMp3Ctrl.fplay.cacheOffs = 0;
		sTlkMdiMp3Ctrl.fplay.cacheLens = 0;
//		tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 004");
	}else if(sTlkMdiMp3Ctrl.fplay.cacheOffs > TLKMDI_MP3_FILE_CACHE_THIRD){
//		tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 005");
		tempLen = sTlkMdiMp3Ctrl.fplay.cacheLens-sTlkMdiMp3Ctrl.fplay.cacheOffs;
		tmemcpy(sTlkMdiMp3FileCache, sTlkMdiMp3FileCache+sTlkMdiMp3Ctrl.fplay.cacheOffs, tempLen);
		sTlkMdiMp3Ctrl.fplay.cacheLens = tempLen;
		sTlkMdiMp3Ctrl.fplay.cacheOffs = 0;
	}
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 006: %d %d %d %d", sTlkMdiMp3Ctrl.fplay.playLens, sTlkMdiMp3Ctrl.finfo.fileSize,
//		sTlkMdiMp3Ctrl.fplay.cacheLens, sTlkMdiMp3Ctrl.fplay.cacheOffs);
	if(sTlkMdiMp3Ctrl.fplay.cacheLens+TLKMDI_MP3_FILE_CACHE_THIRD < TLKMDI_MP3_FILE_CACHE_SIZE 
		&& sTlkMdiMp3Ctrl.fplay.playLens < sTlkMdiMp3Ctrl.finfo.fileSize){
		uint16 readLen;
		tempLen = 0;
		readLen = TLKMDI_MP3_FILE_CACHE_SIZE-sTlkMdiMp3Ctrl.fplay.cacheLens;
		ret = tlkapi_file_read(&sTlkMdiMp3File, sTlkMdiMp3FileCache+sTlkMdiMp3Ctrl.fplay.cacheLens, readLen, &tempLen);
		if(ret == FR_OK && tempLen <= readLen) sTlkMdiMp3Ctrl.fplay.cacheLens += tempLen;
//		tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 007: %d %d %d %d", readLen, tempLen, sTlkMdiMp3Ctrl.fplay.cacheLens, sTlkMdiMp3Ctrl.fplay.cacheOffs);
	}
	if(sTlkMdiMp3Ctrl.fplay.cacheLens == 0 || sTlkMdiMp3Ctrl.fplay.cacheLens == sTlkMdiMp3Ctrl.fplay.cacheOffs){
		tlkapi_error(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 008: %d %d %d %d", sTlkMdiMp3Ctrl.fplay.cacheLens, sTlkMdiMp3Ctrl.fplay.cacheOffs,
			sTlkMdiMp3Ctrl.fplay.playLens, sTlkMdiMp3Ctrl.finfo.fileSize);
		sTlkMdiMp3Ctrl.fplay.playLens = sTlkMdiMp3Ctrl.finfo.fileSize; //Cache error, force stop playback
		return -TLK_ENODATA;
	}
	
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 009");
	/* sync to a mp3 frame */
	fLength = 0;
	tempLen = sTlkMdiMp3Ctrl.fplay.cacheLens-sTlkMdiMp3Ctrl.fplay.cacheOffs;
	tOffset = mp3d_find_frame(sTlkMdiMp3FileCache+sTlkMdiMp3Ctrl.fplay.cacheOffs, tempLen, &freeLen, &fLength);
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 010: %d %d %d %d",  tOffset, tempLen, fLength, freeLen);
	if(tOffset >= tempLen) fOffset = tempLen;
	else fOffset = tOffset;
	if(fOffset != 0){
		sTlkMdiMp3Ctrl.fplay.playLens  += fOffset;
		sTlkMdiMp3Ctrl.fplay.cacheOffs += fOffset;
		tlkmdi_mp3_updatePlayLens(sTlkMdiMp3Ctrl.fplay.playLens, false);
	}
	if(fLength == 0 || fOffset+fLength > tempLen){
		ret = -TLK_ENODATA;
	}else{
		ret = mp3dec_decode_frame(spTlkMdiMp3Decode, sTlkMdiMp3FileCache+sTlkMdiMp3Ctrl.fplay.cacheOffs, fLength, spTlkMdiMp3DecodeTempBuff, &frame_info);
		if(ret > 0){
			tlkmdi_mp3_addPcmData((uint08*)spTlkMdiMp3DecodeTempBuff, sTlkMdiMp3Ctrl.finfo.channels == 2 ? 1152*4 : 1152*2);
			ret = 0;
		}else{
			tlkapi_error(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 011: error %d", ret);
			ret = -TLK_EDECODE;
		}
		if(fLength != 0){
			sTlkMdiMp3Ctrl.fplay.playLens  += fLength;
			sTlkMdiMp3Ctrl.fplay.cacheOffs += fLength;
			tlkmdi_mp3_updatePlayLens(sTlkMdiMp3Ctrl.fplay.playLens, false);
		}
//		tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 012: %d %d %d %d", ret, fLength, sTlkMdiMp3Ctrl.fplay.cacheOffs, sTlkMdiMp3Ctrl.fplay.cacheLens);
	}
	
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_decode 013: %d", ret);
		
    return ret;
}


/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Support the interface to get the music file.
 * Params: None.
 * Return: TLK_ENONE is success,otherwise is false.
*******************************************************************************/
int tlkmdi_mp3_loadFile(bool isForce)
{
	uint16 index;
	uint08 finfo[TLKMDI_MP3_NAME_SIZE];
	tlkmdi_mp3_flist_t listInfo;
	unsigned int fileListAddr;

	fileListAddr = tlkcfg_getFlashAddr(TLKMDI_MP3_FILE_LIST_ADDR);

	if(tlkmdi_mp3_isEnable() || fileListAddr == 0) return false;
	
	sTlkMdiMp3Ctrl.playIndex = 0;
	sTlkMdiMp3Ctrl.fileCount = 0;
	tlkapi_flash_read(fileListAddr, (uint08*)&listInfo, sizeof(tlkmdi_mp3_flist_t));
	if(!isForce && listInfo.sign == TLKMDI_MP3_FILE_LIST_SIGN && listInfo.size == TLKMDI_MP3_FILE_LIST_SIZE
		&& listInfo.vers == TLKMDI_MP3_FILE_LIST_VERS){
		if(listInfo.count > TLKMDI_MP3_FILE_LIST_MAX){
			listInfo.count = TLKMDI_MP3_FILE_LIST_MAX;
		}
		sTlkMdiMp3Ctrl.fileCount = listInfo.count;
		return TLK_ENONE;
	}
	if(listInfo.sign != 0xFF || listInfo.size != 0xFF || listInfo.vers != 0xFF){
		uint32 address;
		for(address=0; address<TLKMDI_MP3_FILE_LIST_LENS; address+=4096){
			flash_erase_sector(fileListAddr + address);
		}
	}
	if(tlkmdi_mp3_scanFileList() != TLK_ENONE) return -TLK_EFAIL;
	
	//find the current index and play position
	for(index=0; index<sTlkMdiMp3Ctrl.fileCount; index++){
		if(tlkmdi_mp3_findFileName(finfo, index) != TLK_ENONE) continue;
		if(tmemcmp(sTlkMdiMp3Ctrl.fsave.fileName, finfo, TLKMDI_MP3_NAME_SIZE) == 0){
			break;
		}
	}
	if(index != sTlkMdiMp3Ctrl.fileCount){
		sTlkMdiMp3Ctrl.playIndex = index;
	}else{
		sTlkMdiMp3Ctrl.fsave.playLens = 0;
	}
	
	return TLK_ENONE;
}
bool tlkmid_mp3_isMp3File(uint08 *pPath, uint08 lens)
{	
	uint08 cmpLen = 4*sizeof(FCHAR);
	FCHAR ftype0[] = {'.', 'm', 'p', '3'};
	FCHAR ftype1[] = {'.', 'M', 'P', '3'};
	
	if(lens < cmpLen+1) return false;
	if(tmemcmp(pPath+(lens-cmpLen), ftype0, cmpLen) == 0 || tmemcmp(pPath+(lens-cmpLen), ftype1, cmpLen) == 0){
		return true;
	}
	return false;
}
int tlkmdi_mp3_scanFileList(void)
{
	#if (!TLK_FS_FAT_ENABLE)
	return -TLK_ENOSUPPORT;
	#else
	DIR dir;
    FRESULT res;
    FILINFO fno;
	uint08 nameLen;
	uint08 pathLen;
	uint32 address;
	uint08 fpath[TLKMDI_MP3_PATH_SIZE];
	uint08 finfo[TLKMDI_MP3_FILE_LIST_SIZE];
	tlkmdi_mp3_flist_t listInfo;
	unsigned int fileListAddr;

	fileListAddr = tlkcfg_getFlashAddr(TLKMDI_MP3_FILE_LIST_ADDR);
	if(fileListAddr == 0) return -TLK_EFAIL;

	sTlkMdiMp3Ctrl.fileCount = 0;

//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_scanPlayFile: 001");	
	tmemset(&listInfo, 0, sizeof(tlkmdi_mp3_flist_t));
		
	tmemset(fpath, 0, TLKMDI_MP3_PATH_SIZE);
	tlkmdi_mp3_findFilePath(fpath, TLKMDI_MP3_PATH_SIZE-1, false);
	pathLen = tlkapi_file_strlen((FCHAR*)fpath);

//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_scanPlayFile: 004");
	res = tlkapi_file_opendir(&dir, (const FCHAR*)fpath);
	if(res != TLK_ENONE) return -TLK_EFAIL;

//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_scanPlayFile: 005");
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_scanPlayFile: start ..");

	address = sizeof(tlkmdi_mp3_flist_t);
    while(TRUE){
		res = tlkapi_file_readdir(&dir, &fno);
        if(res != FR_OK || fno.fname[0] == 0 || fno.fname[0] == 0x3F) break;
		
		nameLen = tlkapi_file_strlen((FCHAR*)fno.fname);
		if(nameLen+1 >= TLKMDI_MP3_NAME_SIZE || pathLen+nameLen+1 >= TLKMDI_MP3_PATH_SIZE){
			tlkapi_array(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_scanPlayFile: fname is too long, cannot handler", (uint08*)fno.fname, nameLen > 16 ? 16 : nameLen);
			continue; //fname is too long, cannot handler
		}
		
        if(fno.fattrib & AM_DIR){
			// TODO: now not support
        }else{
        	if(nameLen < 4 || !tlkmid_mp3_isMp3File((uint08*)fno.fname, nameLen)){
//				tlkapi_array(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_scanPlayFile: this is not mp3 file", fno.fname, 13);
			}else{
				tmemset(finfo, 0, TLKMDI_MP3_FILE_LIST_SIZE);
				tmemcpy(finfo, &fno.fsize, 4);
				tmemcpy(&finfo[4], fno.fname, nameLen);
				tlkapi_flash_write(fileListAddr + address, (uint08*)finfo, TLKMDI_MP3_FILE_LIST_SIZE);
				address += TLKMDI_MP3_FILE_LIST_SIZE;
				sTlkMdiMp3Ctrl.fileCount ++;
				if(address+TLKMDI_MP3_FILE_LIST_SIZE > TLKMDI_MP3_FILE_LIST_LENS) break;
				if(sTlkMdiMp3Ctrl.fileCount >= 0xFFF0) break;
//				tlkapi_array(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_scanPlayFile: this is mp3 file", fno.fname, 13);
			}
        }
    }
	tlkapi_file_closedir(&dir);

	listInfo.vers = TLKMDI_MP3_FILE_LIST_VERS;
	listInfo.size = TLKMDI_MP3_FILE_LIST_SIZE;
	listInfo.sign = TLKMDI_MP3_FILE_LIST_SIGN;
	listInfo.count = sTlkMdiMp3Ctrl.fileCount;
	tlkapi_flash_write(fileListAddr, (uint08*)&listInfo, sizeof(tlkmdi_mp3_flist_t));
	
	return TLK_ENONE;
#endif
}
int tlkmdi_mp3_findFilePath(uint08 *pBuff, uint16 buffLen, bool addRoot)
{
	uint08 dataLen = 0;
//	FCHAR *pPath = (FCHAR*)pBuff;
		
	#if !(TLK_FS_FAT_ENABLE)
		if(buffLen >= 6){
			pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = (uint08)'M';
			pBuff[dataLen++] = (uint08)'P';
			pBuff[dataLen++] = (uint08)'3';
			if(addRoot) pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = 0x00;
		}
	#else
		if(buffLen >= 16){
			pBuff[dataLen++] = (uint08)'1';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)':';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'M';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'P';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'3';
			pBuff[dataLen++] = 0x00;
			if(addRoot){
				pBuff[dataLen++] = (uint08)'/';
				pBuff[dataLen++] = 0x00;
			}
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = 0x00;
		}
	#endif
	return dataLen;
}
int tlkmdi_mp3_findFileName(uint08 *pBuff, uint16 index)
{
	uint32 addr;
	uint08 temp[TLKMDI_MP3_FILE_LIST_SIZE] = {0};
	unsigned int fileListAddr;
	fileListAddr = tlkcfg_getFlashAddr(TLKMDI_MP3_FILE_LIST_ADDR);
	
	if(index >= sTlkMdiMp3Ctrl.fileCount) return -TLK_EFAIL;
	memset(temp, 0, TLKMDI_MP3_FILE_LIST_SIZE);
	addr = fileListAddr + sizeof(tlkmdi_mp3_flist_t) + index*TLKMDI_MP3_FILE_LIST_SIZE;
	tlkapi_flash_read(addr, temp, TLKMDI_MP3_FILE_LIST_SIZE);
	tmemcpy(pBuff, temp+4, TLKMDI_MP3_NAME_SIZE);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Update the music info.
 * Params: None.
 * Return: .
*******************************************************************************/
bool tlkmdi_mp3_isUpdate(void)
{
	if(!sTlkMdiMp3Ctrl.isUpdate || sTlkMdiMp3UpdateFlags == 0) return false;
	if(sTlkMdiMp3UpdateTimer != 0 && clock_time_exceed(sTlkMdiMp3UpdateTimer, TLKMDI_MP3_SAVE_INTERVAL)){
		sTlkMdiMp3UpdateTimer = 0;
	}
	if(sTlkMdiMp3UpdateTimer == 0) return true;
	else return false;
}
bool tlkmdi_mp3_needUpdate(void)
{
	if(!sTlkMdiMp3Ctrl.isUpdate || sTlkMdiMp3UpdateFlags == 0) return false;
	else return true;
}

void tlkmdi_mp3_startUpdate(void)
{
	uint08 finfo[TLKMDI_MP3_FILE_INFO_SIZE];
	if(!sTlkMdiMp3Ctrl.isUpdate) return;
	sTlkMdiMp3UpdateFlags = 0;
	sTlkMdiMp3UpdateTimer = 0;
	tmemcpy(finfo, &sTlkMdiMp3Ctrl.fsave.playLens, 4);
	tmemcpy(finfo+4, sTlkMdiMp3Ctrl.fsave.fileName, TLKMDI_MP3_NAME_SIZE);
	tlkapi_save2_smartSave(&sTlkMdiMp3Ctrl.save, finfo, TLKMDI_MP3_FILE_INFO_SIZE);
}
void tlkmdi_mp3_updateEnable(bool enable)
{
	sTlkMdiMp3Ctrl.isUpdate = enable;
}
void tlkmdi_mp3_updataPlayNameByIndex(uint16 index)
{
	uint08 fname[TLKMDI_MP3_NAME_SIZE+2] = {0};

	if(index >= sTlkMdiMp3Ctrl.fileCount) return;

	if(tlkmdi_mp3_findFileName(fname, index) != TLK_ENONE){
		return;
	}
	sTlkMdiMp3Ctrl.fsave.playLens = 0;
	tlkmdi_mp3_updatePlayName(fname, true);
}
void tlkmdi_mp3_updatePlayName(uint08 *pFName, bool isForce)
{
	if(!sTlkMdiMp3Ctrl.isUpdate) return;
	if(isForce) sTlkMdiMp3UpdateFlags |= 0x80;
	tmemcpy(sTlkMdiMp3Ctrl.fsave.fileName, pFName, TLKMDI_MP3_NAME_SIZE);
	sTlkMdiMp3UpdateFlags |= 0x02;
	if((sTlkMdiMp3UpdateFlags & 0x80) != 0) sTlkMdiMp3UpdateTimer = 0;
	else if(sTlkMdiMp3UpdateTimer == 0) sTlkMdiMp3UpdateTimer = clock_time()|1;
}
void tlkmdi_mp3_updatePlayLens(uint32 playLen, bool isForce)
{
	if(!sTlkMdiMp3Ctrl.isUpdate) return;
	if(isForce) sTlkMdiMp3UpdateFlags |= 0x80;
	else if(playLen == sTlkMdiMp3Ctrl.fsave.playLens) return;
	sTlkMdiMp3Ctrl.fsave.playLens = playLen;
	sTlkMdiMp3UpdateFlags |= 0x01;
	if((sTlkMdiMp3UpdateFlags & 0x80) != 0) sTlkMdiMp3UpdateTimer = 0;
	else if(sTlkMdiMp3UpdateTimer == 0) sTlkMdiMp3UpdateTimer = clock_time()|1;
}


static bool tlkmdi_mp3_getPlayInfo(void)
{
	#if (!TLK_FS_FAT_ENABLE)
	return -TLK_ENOSUPPORT;
	#else
	FRESULT ret;
	uint32 readLen;
	uint32 id3HLen;
	#if 0
	uint32 privateOffs;
	uint32 privateSize;
	bool privateType;
	#endif
	uint08 nameCode;
	uint16 performerSize;
	uint08 performerName[TLKMDI_MP3_PERFORMER_SIZE] = {0};
	int freeLens;
	int fOffset = 0;
	int fLength = 0; //Frame Length
	mp3dec_frame_info_t fileInfo = {0};
	
	readLen = 0;
	memset(&sTlkMdiMp3Ctrl.finfo, 0, sizeof(tlkmdi_mp3_finfo_t));

	sTlkMdiMp3Ctrl.finfo.fileSize = tlkapi_file_size(&sTlkMdiMp3File);
	if(sTlkMdiMp3Ctrl.finfo.fileSize == 0) return false;

	tlkapi_file_seek(&sTlkMdiMp3File, 0);
	ret = tlkapi_file_read(&sTlkMdiMp3File, spTlkMdiMp3FileInfoTempBuff, TLKMDI_MP3_INFO_CACHE_SIZE, &readLen);
	if(ret != FR_OK || readLen == 0) return false;
	
 	if(readLen != TLKMDI_MP3_INFO_CACHE_SIZE){
		tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo-01[Not enough data]: %d %d", readLen, TLKMDI_MP3_INFO_CACHE_SIZE);
	}
	
	id3HLen = mp3dec_skip_id3v2(spTlkMdiMp3FileInfoTempBuff, readLen);
	if(sTlkMdiMp3Ctrl.finfo.fileSize <= id3HLen){
		tlkapi_error(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo-02[Length Error]: %d %d", sTlkMdiMp3Ctrl.finfo.fileSize, id3HLen);
		return false;
	}
	sTlkMdiMp3Ctrl.finfo.headLens = id3HLen;
//	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo-03[mp3 id3_len]: %d %d", id3HLen, sTlkMdiMp3Ctrl.finfo.fileSize);
//	tlkapi_array(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo-04: ", spTlkMdiMp3FileInfoTempBuff, readLen);

	#if 0
	privateOffs = 0;
	privateSize = 0;
	privateType = tlkmdi_mp3_isPrivateFrame(spTlkMdiMp3FileInfoTempBuff, readLen, &privateOffs, &privateSize);
	#endif

	sTlkMdiMp3Ctrl.finfo.nameFlag = 0;
	memset(sTlkMdiMp3Ctrl.finfo.performer, 0, TLKMDI_MP3_PERFORMER_SIZE);
	if(tlkmdi_mp3_getPerformerInfo(spTlkMdiMp3FileInfoTempBuff, readLen, performerName, TLKMDI_MP3_PERFORMER_SIZE-2,
		&performerSize, &nameCode)){
		tmemcpy(sTlkMdiMp3Ctrl.finfo.performer, performerName, performerSize);
		sTlkMdiMp3Ctrl.finfo.nameFlag = ((nameCode & 0x03) << 6) | (performerSize & 0x3F);
	}
	
	//Skip the file header and read the file information
	if(id3HLen != 0){
		uint32 tempLen;
		if(id3HLen+TLKMDI_MP3_INFO_CACHE_SIZE <= sTlkMdiMp3Ctrl.finfo.fileSize) tempLen = TLKMDI_MP3_INFO_CACHE_SIZE;
		else tempLen = sTlkMdiMp3Ctrl.finfo.fileSize-tempLen;
		tlkapi_file_seek(&sTlkMdiMp3File, id3HLen);
		ret = tlkapi_file_read(&sTlkMdiMp3File, spTlkMdiMp3FileInfoTempBuff, tempLen, &readLen);
		if(ret != FR_OK || readLen != tempLen){
			tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo-04[Read Error]: %d %d %d %d %d", ret, sTlkMdiMp3Ctrl.finfo.fileSize, id3HLen, tempLen, readLen);
			return false;
		}
	}
	#if 0
	if(privateType){
		sTlkMdiMp3Ctrl.info.duration = tlkmdi_mp3_getPrivateFramePlayDuration(privateOffs, privateSize);
	}
	if(sTlkMdiMp3Ctrl.info.duration == 0){
		sTlkMdiMp3Ctrl.info.duration = tlkmdi_mp3_getPlayDuration(spTlkMdiMp3FileInfoTempBuff, readLen);
	}
	#else
	sTlkMdiMp3Ctrl.finfo.duration = tlkmdi_mp3_getPlayDuration(spTlkMdiMp3FileInfoTempBuff, readLen);
	#endif
	
	fOffset = mp3d_find_frame(spTlkMdiMp3FileInfoTempBuff, readLen , &freeLens, &fLength);
	if(fOffset+fLength >= readLen){
		tlkapi_error(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo-05[Frame Error]: %d %d %d %d", readLen, fOffset, fLength, freeLens);
		return false;
	}
	
	mp3dec_decode_frame(spTlkMdiMp3Decode, spTlkMdiMp3FileInfoTempBuff+fOffset, fLength, NULL, (mp3dec_frame_info_t*)&fileInfo);
	if(fileInfo.hz == 0 || fLength == 0){
		tlkapi_error(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo-06[Param Error]: %d %d", fLength, fileInfo.hz);
		return false;
	}
	
	sTlkMdiMp3Ctrl.finfo.channels = fileInfo.channels;
	sTlkMdiMp3Ctrl.finfo.sampleRate = fileInfo.hz;
	#if 1
	if(sTlkMdiMp3Ctrl.finfo.duration == 0 && fLength != 0){
		uint32 temp = sTlkMdiMp3Ctrl.finfo.fileSize-sTlkMdiMp3Ctrl.finfo.headLens;
		sTlkMdiMp3Ctrl.finfo.duration = (temp*26)/(fLength*100);
	}
	#endif
	
	tlkapi_trace(TLKMDI_AUDMP3_DBG_FLAG, TLKMDI_AUDMP3_DBG_SIGN, "tlkmdi_mp3_getPlayInfo[sampleRate-%d,channels-%d,duration-%d,fLength-%d]",
		sTlkMdiMp3Ctrl.finfo.sampleRate, sTlkMdiMp3Ctrl.finfo.channels, sTlkMdiMp3Ctrl.finfo.duration, fLength);
	
	return true;
	#endif
}

#if 0
static bool tlkmdi_mp3_isPrivateFrame(uint08 *pData, uint16 dataLen, uint32 *pOffs, uint32 *pSize)
{
	uint32 index;
	uint32 frameLen;
	uint32 headLens;
	uint32 headSize;
	uint08  mp3Label[TLKMDI_MP3_LABEL_HLEN] = {0};
	
	if(dataLen < 10 || tmemcmp(pData, "ID3", 3) != 0) return false;
	
	pData += 6;
	headLens = ((pData[0] & 0x7F) << 21) | ((pData[1] & 0x7F) << 14) | ((pData[2] & 0x7F) << 7) | (pData[3] & 0x7F);
	pData += 4;
	dataLen -= 10;

	index = 0;
	if(headLens <= dataLen) headSize = headLens;
	else headSize = dataLen;	
	while(index+TLKMDI_MP3_LABEL_HLEN <= headSize){
		tmemcpy(mp3Label, pData+index, TLKMDI_MP3_LABEL_HLEN);
		frameLen = ((mp3Label[4] << 24) | (mp3Label[5] << 16) | (mp3Label[6] << 8) | mp3Label[7]);
		if(tmemcmp(mp3Label, "PRIV", 4) != 0){
			index += TLKMDI_MP3_LABEL_HLEN+frameLen;
		}else{
			*pOffs = index+TLKMDI_MP3_LABEL_HLEN;
			*pSize = frameLen;
			return true;
		}
	}
		
	return false;
}
#endif
static bool tlkmdi_mp3_getPerformerInfo(uint08 *pData, uint16 dataLen, uint08 *pNameBuff, 
	uint08 buffLens, uint16 *pNameSize, uint08 *pNameCode)
{
	#if (!TLK_FS_FAT_ENABLE)
	return -TLK_ENOSUPPORT;
	#else
	FRESULT ret;

	if(pNameBuff == nullptr || buffLens == 0) return false;

	/*1: check artist in ID3v1 buffer first.*/
	if(sTlkMdiMp3Ctrl.finfo.fileSize >= 128 && pData[3] != 0x03 && pData[3] != 0x02){
		uint32 readlen = 0;
		uint08 id3v1Buff[63] = {0}; //63=3+30+30
		//ID3v1 buffer start from the offset(128) to the end of mp3 file.
		tlkapi_file_seek(&sTlkMdiMp3File, sTlkMdiMp3Ctrl.finfo.fileSize - 128); 
		ret = tlkapi_file_read(&sTlkMdiMp3File, id3v1Buff, 63, &readlen);
		if(ret == FR_OK && readlen == 63 && tmemcmp(id3v1Buff, "TAG", 3) == 0){
			uint08 *p = id3v1Buff + (3 + 30 + 30 - 1);
			uint08 *q = id3v1Buff + (3 + 30);
			uint08 len = 0;
			*pNameCode = 1;
			while((!(*p)) && (len < 30)){
				p--;
				len++;
			}
			*pNameSize = (30 - len > buffLens) ? buffLens : (30 - len);
   			memcpy(pNameBuff, q, *pNameSize);
			return true;
		}
	}
	
	/*2: if artist is not exist in ID3v1, then check it in ID3v2 buff.*/
	if(dataLen > 10 && tmemcmp(pData, "ID3", 3) == 0)
	{	uint32 index;
		uint32 tempLen;
		uint32 frameLen;
		uint32 headLens;
		uint32 headSize;
		uint08 mp3Label[TLKMDI_MP3_LABEL_HLEN] = {0};
		
		pData += 6;
		headLens = ((pData[0] & 0x7F) << 21) | ((pData[1] & 0x7F) << 14) | ((pData[2] & 0x7F) << 7) | (pData[3] & 0x7F);
		pData += 4;
		dataLen -= 10;

		index = 0;
		if(headLens <= dataLen) headSize = headLens;
		else headSize = dataLen;
		while(index+TLKMDI_MP3_LABEL_HLEN <= headSize){
			tmemcpy(mp3Label, pData+index, TLKMDI_MP3_LABEL_HLEN);
			frameLen = ((mp3Label[4] << 24) | (mp3Label[5] << 16) | (mp3Label[6] << 8) | mp3Label[7]);
			if(tmemcmp(mp3Label, "TPE1", 4) != 0){
				index += TLKMDI_MP3_LABEL_HLEN + frameLen;
			}else{
				if(frameLen > buffLens+3) frameLen = buffLens+3;
				if(index+TLKMDI_MP3_LABEL_HLEN >= dataLen) tempLen = 0;
				else tempLen = dataLen-(index+TLKMDI_MP3_LABEL_HLEN);
				if(frameLen > tempLen) frameLen = tempLen;
				if(frameLen <= 1) return false;
				*pNameCode = pData[index+TLKMDI_MP3_LABEL_HLEN]; //0x01-Unicode, 0x00-Ascii
				tmemcpy(pNameBuff, pData+index+TLKMDI_MP3_LABEL_HLEN+1, frameLen-1);
				*pNameSize = frameLen-1;
				return true;
			}
		}
	}

	return false;
	#endif
}

static uint32 tlkmdi_mp3_getPlayDuration(uint08 *pData, uint16 dataLen)
{
	uint08 mpegType;
	uint08 rateBits; //sample_rate_bits
	uint08 layerType;
	uint32 frameLens;
	uint32 sampleRate;
	uint32 frameCount;
	uint32 playDuration;
	
	if(dataLen < 40) return 0;

	mpegType  = (pData[1] >> 3) & 0x03;
	layerType = (pData[1] >> 1) & 0x03;
	rateBits  = (pData[2] >> 2) & 0x03;

	if(mpegType == TLKMDI_MP3_MPEG_TYPE_2P5){
		sampleRate = rateBits == 0 ? 11025 : rateBits == 1 ? 12000 : rateBits == 2 ? 8000 : 0;
	}else if(mpegType == TLKMDI_MP3_MPEG_TYPE_2){
		sampleRate = rateBits == 0 ? 22050 : rateBits == 1 ? 24000 : rateBits == 2 ? 16000 : 0;
	}else if(mpegType == TLKMDI_MP3_MPEG_TYPE_1){
		sampleRate = rateBits == 0 ? 44100 : rateBits == 1 ? 48000 : rateBits == 2 ? 32000 : 0;
	}else{
		sampleRate = 0;
	}
	if(mpegType == TLKMDI_MP3_MPEG_TYPE_2P5){
		frameLens = layerType == TLKMDI_MP3_LAYER_TYPE_1 ? 384 : layerType == TLKMDI_MP3_LAYER_TYPE_2 ? 1152 : layerType == TLKMDI_MP3_LAYER_TYPE_3 ? 576 : 0;
	}else if(mpegType == TLKMDI_MP3_MPEG_TYPE_2){
		frameLens = layerType == TLKMDI_MP3_LAYER_TYPE_1 ? 384 : layerType == TLKMDI_MP3_LAYER_TYPE_2 ? 1152 : layerType == TLKMDI_MP3_LAYER_TYPE_3 ? 576 : 0;
	}else if(mpegType == TLKMDI_MP3_MPEG_TYPE_1){
		frameLens = layerType == TLKMDI_MP3_LAYER_TYPE_1 ? 384 : layerType == TLKMDI_MP3_LAYER_TYPE_2 ? 1152 : layerType == TLKMDI_MP3_LAYER_TYPE_3 ? 1152 : 0;
	}else{
		frameLens = 0;
	}
	
	if((sampleRate == 0) || (frameLens == 0)) return 0;
	
	pData += 4+32;
	dataLen -= 36;
	
	if(dataLen < 12 || ((memcmp(pData, "Xing", 4) != 0) && (memcmp(pData, "Info", 4) != 0))){
		return 0;
	}
	
	pData += 4;	
	if((pData[3] & BIT(0)) == 0) return 0;
	
	pData += 4;
	frameCount = (pData[0] << 24) | (pData[1] << 16) | (pData[2] << 8) | (pData[3] << 0);
	playDuration = (frameCount * frameLens)/sampleRate;
	
	return playDuration;
}
#if 0
static uint32 tlkmdi_mp3_getPrivateFramePlayDuration(uint32 frameOffs, uint32 frameSize)
{
	FRESULT ret;
	uint32 read_len = 0;
	uint32 read_len_total = 0;
	uint16 continue_read_len = 0;
	uint32 buf_valid_len = 0;
	uint32 value = 0;
	uint32 scale_div = 0;
	uint32 scale_total = 0;
	uint32 play_duration_s  = 0;
	uint08 value_char[1 + 10] = {0};
	uint08 scale_div_char[1 + 10] = {0};
	uint08 scale_total_char[1 + 10] = {0};
	bool is_div = true;
	uint08 digit_cnt = 0;
	uint32 power_10 = 1;
	uint16 i = 0;
	uint08 j = 0, k = 0;
	
	const char label_xmpDM[] = {"xmpDM"};
	const char key_duration[] = {"duration"};
	const char id_value[] = {"value"};
	const char id_scale[] = {"scale"};
	const char colon = ':';
	const char angle_brackets_L = '<';
	const char angle_brackets_R = '>';
	const char equal_sign = '=';
	const char double_quot = '"';
	const char div_sign = '/';
	const char digit_0 = '0';
	const char digit_9 = '9';

	tlkapi_file_seek(&sTlkMdiMp3File, frameOffs + TLKMDI_MP3_LABEL_HLEN);

	if(frameSize > TLKMDI_MP3_INFO_CACHE_SIZE){
		ret = tlkapi_file_read(&sTlkMdiMp3File, spTlkMdiMp3DurationTempBuff, TLKMDI_MP3_INFO_CACHE_SIZE, &read_len);
	}else{
		ret = tlkapi_file_read(&sTlkMdiMp3File, spTlkMdiMp3DurationTempBuff, frameSize, &read_len);
	}
	if(ret != FR_OK) return 0;

	read_len_total += read_len;
	buf_valid_len = read_len;

	for(i = 0; i < buf_valid_len;)
	{
		uint16 idx = 0;
		j = 0; k = 0;
		if(spTlkMdiMp3DurationTempBuff[i] == angle_brackets_L)
		{
			idx = i;
			while(++i < buf_valid_len){if(spTlkMdiMp3DurationTempBuff[i] == angle_brackets_R) break;}
			
			if(i < buf_valid_len)	//label end.
			{
				uint16 i_tmp  = idx + 1;
				if(memcmp(&spTlkMdiMp3DurationTempBuff[i_tmp], label_xmpDM, strlen(label_xmpDM)) == 0)
				{
					i_tmp += strlen(label_xmpDM);
					if(spTlkMdiMp3DurationTempBuff[i_tmp] == colon)
					{
						i_tmp += 1;
						if(memcmp(&spTlkMdiMp3DurationTempBuff[i_tmp], key_duration, strlen(key_duration)) == 0)
						{
							i_tmp += strlen(key_duration);
							while(i_tmp < i){
								if(memcmp(&spTlkMdiMp3DurationTempBuff[i_tmp], label_xmpDM, strlen(label_xmpDM)) == 0)break;
								else i_tmp++;
							}
							if(i_tmp == i){i++; continue;}
							
							i_tmp += strlen(label_xmpDM);
							if(spTlkMdiMp3DurationTempBuff[i_tmp] == colon)
							{
								i_tmp += 1;
								if(memcmp(&spTlkMdiMp3DurationTempBuff[i_tmp], id_value, strlen(id_value)) == 0)
								{
									i_tmp += strlen(id_value);
									if((spTlkMdiMp3DurationTempBuff[i_tmp] == equal_sign) && (spTlkMdiMp3DurationTempBuff[++i_tmp] == double_quot))
									{
										i_tmp += 1;
										j = 0;
										while(i_tmp < i){
											if((spTlkMdiMp3DurationTempBuff[i_tmp] >= digit_0) && (spTlkMdiMp3DurationTempBuff[i_tmp] <= digit_9)){
												value_char[1 + (j++)] = spTlkMdiMp3DurationTempBuff[i_tmp];
												i_tmp++;
											}else{	//if(buf[i_tmp] == double_quot)
												break;
											}
										}
										if((i_tmp == i) || (j == 0)){i++; j = 0; continue;}

										value_char[0] = j;
										i_tmp  += 1;
										while(i_tmp < i){
											if(memcmp(&spTlkMdiMp3DurationTempBuff[i_tmp], label_xmpDM, strlen(label_xmpDM)) == 0) break;
											else i_tmp++;
										}
										if(i_tmp == i){i++; continue;}

										i_tmp += strlen(label_xmpDM);
										if(spTlkMdiMp3DurationTempBuff[i_tmp] == colon)
										{	
											i_tmp += 1;
											if(memcmp(&spTlkMdiMp3DurationTempBuff[i_tmp], id_scale, strlen(id_scale)) == 0)
											{
												i_tmp += strlen(id_scale);
												if((spTlkMdiMp3DurationTempBuff[i_tmp] == equal_sign) && (spTlkMdiMp3DurationTempBuff[++i_tmp] == double_quot))
												{
													i_tmp += 1;
													j = 0;	//div length
													k = 0;	//total length
													while(i_tmp < i){
														if((spTlkMdiMp3DurationTempBuff[i_tmp] >= digit_0) && (spTlkMdiMp3DurationTempBuff[i_tmp] <= digit_9)){
															if(is_div && (k == 0)) scale_div_char[1 + (j++)] = spTlkMdiMp3DurationTempBuff[i_tmp];
															else if((!is_div) && (j > 0)) scale_total_char[1 + (k++)] = spTlkMdiMp3DurationTempBuff[i_tmp];
															else break;
															i_tmp++;
														}else if((is_div) && (j > 0) && (spTlkMdiMp3DurationTempBuff[i_tmp] == div_sign)){
															is_div = false;
															i_tmp++;
															scale_div_char[0] = j;
														}else{	//if(buf[i_tmp] == double_quot)
															break;
														}
													}
													if((i_tmp == i) || (j == 0) || (k == 0)){i++; j = 0; k = 0; continue;}
													
													scale_total_char[0] = j;
													j = 0;k = 0;
													if(i_tmp < i)
													{
														digit_cnt = value_char[0];
														for(j = 0; j < digit_cnt; j++){
															k = digit_cnt - 1 - j;
															power_10 = 1;
															while(k--) power_10 *= 10;
															value += (value_char[1 + j] - digit_0) * power_10;
														}

														digit_cnt = scale_div_char[0];
														for(j = 0; j < digit_cnt; j++){
															k = digit_cnt - 1 - j;
															power_10 = 1;
															while(k--) power_10 *= 10;
															scale_div += (scale_div_char[1 + j] - digit_0) * power_10;
														}

														digit_cnt = scale_total_char[0];
														for(j = 0; j < digit_cnt; j++){
															k = digit_cnt - 1 - j;
															power_10 = 1;
															while(k--) power_10 *= 10;
															scale_total += (scale_total_char[1 + j] - digit_0) * power_10;
														}
														
														play_duration_s = value * scale_div / scale_total;
														//printf("value = %d, scale_div = %d, scale_total = %d\r\n", value, scale_div, scale_total);

														return play_duration_s;
													}
													else
													{
														return 0;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}

				if(i >= buf_valid_len - 1)
				{
					if(frameSize > read_len_total)
					{
						if((frameSize - read_len_total) > TLKMDI_MP3_INFO_CACHE_SIZE)
							continue_read_len = TLKMDI_MP3_INFO_CACHE_SIZE;
						else
							continue_read_len = frameSize - read_len_total;

						ret = tlkapi_file_read(&sTlkMdiMp3File, spTlkMdiMp3DurationTempBuff, continue_read_len, &read_len);
						if(ret != FR_OK) return 0;

						buf_valid_len = read_len;
						read_len_total += read_len;
						i = 0;
					}
					else
					{
						return 0;
					}
				}else{
					i++;
				}
			}
			else
			{	//label not end.
				if(frameSize > read_len_total)
				{
					tmemcpy(spTlkMdiMp3DurationTempBuff, spTlkMdiMp3DurationTempBuff + idx, (i - idx));
					if((frameSize - read_len_total) > (TLKMDI_MP3_INFO_CACHE_SIZE - idx))
						continue_read_len = idx;
					else
						continue_read_len = frameSize - read_len_total;
					
					ret = tlkapi_file_read(&sTlkMdiMp3File, spTlkMdiMp3DurationTempBuff + (i - idx), continue_read_len, &read_len);
					if(ret != FR_OK) return 0;

					buf_valid_len = (i - idx) + read_len;
					read_len_total += read_len;
					i = 0;
				}
				else
				{
					return  0;
				}
			}
		}
		else
		{
			if(i >= buf_valid_len - 1)
			{
				if(frameSize > read_len_total)
				{
					if((frameSize - read_len_total) > TLKMDI_MP3_INFO_CACHE_SIZE)
						continue_read_len = TLKMDI_MP3_INFO_CACHE_SIZE;
					else
						continue_read_len = frameSize - read_len_total;

					ret = tlkapi_file_read(&sTlkMdiMp3File, spTlkMdiMp3DurationTempBuff, continue_read_len, &read_len);
					if(ret != FR_OK) return 0;

					buf_valid_len = read_len;
					read_len_total += read_len;
					i = 0;
				}
				else
				{
					return  0;
				}
			}else{
				i++;
			}
		}
	}
	return 0;
}
#endif

#endif //TLK_MDI_MP3_ENABLE


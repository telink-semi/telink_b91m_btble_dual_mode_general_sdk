/********************************************************************************************************
 * @file	tlkmdi_audtone.c
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
#if (TLK_MDI_AUDTONE_ENABLE)
#include "tlkmdi/aud/tlkmdi_audio.h"
#if (TLKMDI_AUDTONE_DECODE_MP3)
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/aud/tlkmdi_audmp3.h"
#endif
#include "tlkmdi/aud/tlkmdi_audtone.h"

#include "tlkalg/audio/mixer/tlkalg_2chmix.h"

#include "tlkdev/sys/tlkdev_codec.h"
#include "tlkstk/inner/tlkstk_myudb.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"
#if (TLKMDI_AUDTONE_DECODE_SBC)
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#endif
#if (TLKMDI_AUDTONE_DECODE_ADPCM)
#include "tlkalg/audio/adpcm/tlkalg_adpcm.h"
#endif

#define TLKMDI_AUDTONE_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_TONE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDTONE_DBG_SIGN       "[MDI]"


#if (TLKMDI_AUDTONE_DECODE_MP3)
static int tlkmdi_tone_findFilePath(uint08 *pBuff, uint16 buffLen, bool addRoot);
static int tlkmdi_tone_findFileName(uint08 *pBuff, uint16 index);
#endif
#if (TLKMDI_AUDTONE_DECODE_MP3)
static void tlkmdi_tone_mp3Handler(void);
#endif
static void tlkmdi_tone_fillHandler(void);

typedef struct{
	uint08 running;
	uint08 enable;
	uint08 curState;
	uint08 playNumb;
	uint08 playCount;
	uint16 playIndex;
	uint32 sampleRate;
	#if !(TLKMDI_AUDTONE_DECODE_MP3)
	uint08 encode;
	uint08 channel;
	uint08 dataIsOK;
	uint08 bitDepth;
	uint16 fileNumb;
	uint16 frameLen;
	uint16 encodeLen;
	uint32 fileOffset;
	uint32 readOffset;
	uint32 readLength;
	uint08 *pDecBuff;
	uint08 *pReadBuff;
	uint08 *pSaveBuff;
	#endif
}tlkmdi_tone_ctrl_t;
static tlkmdi_tone_ctrl_t sTlkMdiToneCtrl;


/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int tlkmdi_audtone_init(void)
{	
	tmemset(&sTlkMdiToneCtrl, 0, sizeof(tlkmdi_tone_ctrl_t));
	sTlkMdiToneCtrl.playIndex = 0;
	sTlkMdiToneCtrl.playCount = 1;
	#if !(TLKMDI_AUDTONE_DECODE_MP3)
	uint08 srType;
	uint08 buffer[12];
	sTlkMdiToneCtrl.dataIsOK = false;
	tlkapi_flash_read(TLKMDI_AUDTONE_FLASH_ADDR, buffer, 12);
	if(buffer[0] != 'T' || buffer[1] != 'L' || buffer[2] != 'K'){
		return -TLK_EFAIL;
	}
	sTlkMdiToneCtrl.encode = buffer[3];
	sTlkMdiToneCtrl.channel = buffer[4] & 0x0F;
	if(sTlkMdiToneCtrl.channel != 0x01 && sTlkMdiToneCtrl.channel != 0x02){
		return -TLK_EFAIL;
	}
	if(sTlkMdiToneCtrl.channel == 2){
		sTlkMdiToneCtrl.channel = 3;
	}
	srType = (buffer[4] & 0xF0) >> 4;
	if(srType == 1) sTlkMdiToneCtrl.sampleRate = 8000;
	else if(srType == 2) sTlkMdiToneCtrl.sampleRate = 16000;
	else if(srType == 3) sTlkMdiToneCtrl.sampleRate = 32000;
	else if(srType == 4) sTlkMdiToneCtrl.sampleRate = 44100;
	else if(srType == 5) sTlkMdiToneCtrl.sampleRate = 48000;
	else return -TLK_EFAIL;
	sTlkMdiToneCtrl.bitDepth = buffer[5];
	sTlkMdiToneCtrl.fileNumb = ((uint16)buffer[7] << 8) | buffer[6];
	sTlkMdiToneCtrl.frameLen = ((uint16)buffer[9] << 8) | buffer[8];
	sTlkMdiToneCtrl.encodeLen = ((uint16)buffer[11] << 8) | buffer[10];
	if(sTlkMdiToneCtrl.fileNumb == 0 || sTlkMdiToneCtrl.frameLen == 0 || sTlkMdiToneCtrl.frameLen > 10240
		|| sTlkMdiToneCtrl.encodeLen == 0 || sTlkMdiToneCtrl.encodeLen > 10240){
		return -TLK_EFAIL;
	}
	#if (TLKMDI_AUDTONE_DECODE_SBC)
	if(sTlkMdiToneCtrl.encode != TLK_TONE_ECODE_SBC){
		return -TLK_EFAIL;
	}
	#elif (TLKMDI_AUDTONE_DECODE_ADPCM)
	if(sTlkMdiToneCtrl.encode != TLK_TONE_ECODE_ADPCM){
		return -TLK_EFAIL;
	}
	#else
	return -TLK_EFAIL;
	#endif
	sTlkMdiToneCtrl.dataIsOK = true;
	#endif
	return TLK_ENONE;
}

int tlkmdi_audtone_start(uint16 handle, uint32 param)
{
	uint16 playIndex = (param & 0x0000FFFF);
	uint16 playCount = (param & 0xFFFF0000) >> 16;
	#if !(TLKMDI_AUDTONE_DECODE_MP3)
	if(!sTlkMdiToneCtrl.dataIsOK || sTlkMdiToneCtrl.playIndex >= sTlkMdiToneCtrl.fileNumb){
		return -TLK_EFAIL;
	}
	#endif
	sTlkMdiToneCtrl.playIndex = playIndex;
	sTlkMdiToneCtrl.playCount = playCount;
	sTlkMdiToneCtrl.playNumb = 0;
#if (TLKMDI_AUDTONE_DECODE_MP3)
	if(!sTlkMdiToneCtrl.enable) sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
#else
	if(!sTlkMdiToneCtrl.enable) sTlkMdiToneCtrl.curState = TLKMDI_TONE_STATUS_IDLE;
#endif
	tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_TONE, handle);
	return TLK_ENONE;
}
int tlkmdi_audtone_close(uint16 handle)
{
	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_close");
	sTlkMdiToneCtrl.running = false;
	tlkmdi_audtone_switch(handle, TLK_STATE_CLOSED);
	tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_TONE, handle);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_audtone_switch
 * Descript: Change Tone status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_switch(uint16 handle, uint08 status)
{
	bool enable;

	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_switch 01: %d %d %d", status, enable, sTlkMdiToneCtrl.enable);
	if(sTlkMdiToneCtrl.enable == enable) return true;

	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_muteSpkBuff();
	#endif
	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_switch 02: %d", enable);
	#if (TLKMDI_AUDTONE_DECODE_MP3)
	if(!tlkmdi_mp3_enable(enable) && enable){
		return false;
	}
	#endif
	if(enable){
		#if (TLKMDI_AUDTONE_DECODE_MP3)
			sTlkMdiToneCtrl.sampleRate = tlkmdi_mp3_getSampleRate();
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_LEFT, TLKDEV_CODEC_BITDEPTH_16, sTlkMdiToneCtrl.sampleRate);
			#endif
		#else
			uint08 buffer[8];
			if(!sTlkMdiToneCtrl.dataIsOK || sTlkMdiToneCtrl.playIndex >= sTlkMdiToneCtrl.fileNumb) return false;
			sTlkMdiToneCtrl.pReadBuff = tlkmdi_audmem_calloc(sTlkMdiToneCtrl.encodeLen);
			sTlkMdiToneCtrl.pSaveBuff = tlkmdi_audmem_calloc(sTlkMdiToneCtrl.frameLen);
			if(sTlkMdiToneCtrl.pReadBuff == nullptr || sTlkMdiToneCtrl.pSaveBuff == nullptr){
				if(sTlkMdiToneCtrl.pReadBuff != nullptr){
					tlkmdi_audmem_free(sTlkMdiToneCtrl.pReadBuff);
					sTlkMdiToneCtrl.pReadBuff = nullptr;
				}
				if(sTlkMdiToneCtrl.pSaveBuff != nullptr){
					tlkmdi_audmem_free(sTlkMdiToneCtrl.pSaveBuff);
					sTlkMdiToneCtrl.pSaveBuff = nullptr;
				}
				return false;
			}
			#if (TLKMDI_AUDTONE_DECODE_SBC)
			sTlkMdiToneCtrl.pDecBuff = tlkmdi_audmem_calloc(TLKMDI_AUDTONE_SBC_DEC_BUFF_SIZE);
			if(sTlkMdiToneCtrl.pDecBuff == nullptr){
				tlkmdi_audmem_free(sTlkMdiToneCtrl.pReadBuff);
				sTlkMdiToneCtrl.pReadBuff = nullptr;
				tlkmdi_audmem_free(sTlkMdiToneCtrl.pSaveBuff);
				sTlkMdiToneCtrl.pSaveBuff = nullptr;
				return false;
			}
			tlkalg_sbc_decInit(sTlkMdiToneCtrl.pDecBuff);
			#elif (TLKMDI_AUDTONE_DECODE_ADPCM)
			tlkalg_adpcm_init(0, 0);
			#endif
			tlkapi_flash_read(TLKMDI_AUDTONE_FLASH_ADDR+TLKMDI_AUDTONE_FLASH_HEAD_LENGTH+sTlkMdiToneCtrl.playIndex*8, buffer, 8);
			ARRAY_TO_UINT32L(buffer, 0, sTlkMdiToneCtrl.fileOffset);
			ARRAY_TO_UINT32L(buffer, 4, sTlkMdiToneCtrl.readLength);
			sTlkMdiToneCtrl.fileOffset += TLKMDI_AUDTONE_FLASH_ADDR;
			sTlkMdiToneCtrl.readLength += sTlkMdiToneCtrl.fileOffset;
			sTlkMdiToneCtrl.readOffset = sTlkMdiToneCtrl.fileOffset;
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, sTlkMdiToneCtrl.channel, sTlkMdiToneCtrl.bitDepth, sTlkMdiToneCtrl.sampleRate);
			#endif
		#endif
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_setSpkOffset(320);
		#endif
	}
	else{
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_close();
		#endif
		#if !(TLKMDI_AUDTONE_DECODE_MP3)
			if(sTlkMdiToneCtrl.pReadBuff != nullptr){
				tlkmdi_audmem_free(sTlkMdiToneCtrl.pReadBuff);
				sTlkMdiToneCtrl.pReadBuff = nullptr;
			}
			if(sTlkMdiToneCtrl.pSaveBuff != nullptr){
				tlkmdi_audmem_free(sTlkMdiToneCtrl.pSaveBuff);
				sTlkMdiToneCtrl.pSaveBuff = nullptr;
			}
			#if (TLKMDI_AUDTONE_DECODE_SBC)
			if(sTlkMdiToneCtrl.pDecBuff != nullptr){
				tlkmdi_audmem_free(sTlkMdiToneCtrl.pDecBuff);
				sTlkMdiToneCtrl.pDecBuff = nullptr;
			}
			tlkalg_sbc_decInit(nullptr);
			#endif
		#endif
	}

	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_audtone_switch 03");

	if(enable) tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_TONE, TLK_STATE_OPENED);
	else tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_TONE, TLK_STATE_CLOSED);
	
	sTlkMdiToneCtrl.enable = enable;
	sTlkMdiToneCtrl.running = enable;
	sTlkMdiToneCtrl.playNumb = 0;
	#if (TLKMDI_AUDTONE_DECODE_MP3)
	sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
	#else
	sTlkMdiToneCtrl.curState = TLKMDI_TONE_STATUS_IDLE;
	#endif
	return true;
}


/******************************************************************************
 * Function: tlkmdi_audtone_isBusy
 * Descript: Is the Tone Ctrl is enable. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
bool tlkmdi_audtone_isBusy(void)
{
	return (sTlkMdiToneCtrl.enable && sTlkMdiToneCtrl.running);
}

/******************************************************************************
 * Function: tlkmdi_audtone_intval
 * Descript: Get Tone Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audtone_intval(void)
{
	if(!sTlkMdiToneCtrl.running) return 0;
#if (TLK_DEV_CODEC_ENABLE)
	#if (TLKMDI_AUDTONE_DECODE_MP3)
		if(tlkdev_codec_getSpkDataLen() < (TLK_DEV_SPK_BUFF_SIZE>>1)){
			return 1000;
		}else if(tlkmdi_mp3_getPcmDataLen() < 1024){
			return 800;
		}else{
			return 3000;
		}
	#else
		return 1000;
	#endif
#else
	return 3000;
#endif
}

/******************************************************************************
 * Function: tlkmdi_audtone_irqProc
 * Descript: Call Tone mp3 handler and fill handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_irqProc(void)
{
	#if (TLKMDI_AUDTONE_DECODE_MP3)
	tlkmdi_tone_mp3Handler();
	#endif
	tlkmdi_tone_fillHandler();
	if(sTlkMdiToneCtrl.running) return true;
	else return false;
}

/******************************************************************************
 * Function: tlkmdi_tone_setParam
 * Descript: Set the playing parameter. 
 * Params: 
 *        @playIndex[IN]--Theplay index.
 *        @playcount[IN]--The play count.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_setParam(uint16 playIndex, uint08 playCount)
{
	sTlkMdiToneCtrl.playIndex = playIndex;
	sTlkMdiToneCtrl.playCount = playCount;
}

#if (TLKMDI_AUDTONE_DECODE_MP3)
/******************************************************************************
 * Function: tlkmdi_tone_start
 * Descript: Start tone to playing. 
 * Params:
 *         @index[IN]--The music index.
 * Return: Return TLK_ENONE is success other value is false.
*******************************************************************************/
bool tlkmdi_tone_start(uint16 index)
{
	uint08 pathLen;
	uint08 fpath[TLKMDI_MP3_PATH_SIZE+2] = {0};
	
	tlkmdi_tone_findFilePath(fpath, TLKMDI_MP3_DIR_SIZE, true);
	pathLen = tlkapi_file_strlen((FCHAR*)fpath);
	if(pathLen == 0) return false;
	
//	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_start 02");
	if(tlkmdi_tone_findFileName(fpath+pathLen, index) != TLK_ENONE){
		return false;
	}
	
//	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_start 03");
	if(!tlkmdi_mp3_start(fpath, 0)){
		return false;
	}
	
	sTlkMdiToneCtrl.playIndex = index;
	
	return true;
}
/******************************************************************************
 * Function: tlkmdi_tone_close
 * Descript: Stop tone to playing. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_close(void)
{
	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_muteSpkBuff();
	#endif
	tlkmdi_mp3_close();
	sTlkMdiToneCtrl.running = false;
	sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
}
#endif //#if (TLKMDI_AUDTONE_DECODE_MP3)


#if (TLKMDI_AUDTONE_DECODE_MP3)
static void tlkmdi_tone_mp3Handler(void)
{
	if(!sTlkMdiToneCtrl.running) return;
	if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_IDLE){
		tlkmdi_tone_start(sTlkMdiToneCtrl.playIndex);
		if(!sTlkMdiToneCtrl.running) return;
		if(tlkmdi_mp3_getSampleRate() != sTlkMdiToneCtrl.sampleRate){
			sTlkMdiToneCtrl.sampleRate = tlkmdi_mp3_getSampleRate();
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_close();
			tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_LEFT, TLKDEV_CODEC_BITDEPTH_16, sTlkMdiToneCtrl.sampleRate);
			tlkdev_codec_setSpkOffset(640);
			#endif
		}
		sTlkMdiToneCtrl.playNumb ++;
		sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_PLAY;
	}
	if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_WAIT || sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_DONE){
		#if (TLK_DEV_CODEC_ENABLE)
		uint16 dropLen = (tlkmdi_mp3_getChannels() == 2) ? 960 : 480;//10ms
		#endif
		#if (TLK_DEV_CODEC_ENABLE)
		if(tlkdev_codec_getSpkDataLen() >= dropLen){
			tlkdev_codec_zeroSpkBuff(0, false);
		}else{
		#endif
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_muteSpkBuff();
			#endif
			if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_DONE){
				sTlkMdiToneCtrl.running = false;
				tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_play is over");
			}else{
				sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_IDLE;
			}
		#if (TLK_DEV_CODEC_ENABLE)
		}
		#endif
	}else if(sTlkMdiToneCtrl.curState == TLKMDI_MP3_STATUS_PLAY){
		int ret;
		ret = tlkmdi_mp3_decode();
		if(ret != TLK_ENONE){
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_zeroSpkBuff(0, 0);
			#endif
		}
		if(tlkmdi_mp3_isOver()){
			tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_play: mp3 over - %d %d",
				sTlkMdiToneCtrl.playCount, sTlkMdiToneCtrl.playNumb);
			if(sTlkMdiToneCtrl.playCount != 0 && sTlkMdiToneCtrl.playNumb >= sTlkMdiToneCtrl.playCount){
				sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_DONE;
			}else{
				sTlkMdiToneCtrl.curState = TLKMDI_MP3_STATUS_WAIT;
			}
		}
	}
}
#endif
static void tlkmdi_tone_fillHandler(void)
{
#if (TLKMDI_AUDTONE_DECODE_MP3)
	uint16 volume;
	uint16 index;
	uint32 needLen;
	uint16 pcm[256];
	sint16 *ptr = NULL;

	if(!sTlkMdiToneCtrl.running) return;
//	tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_fillHandler:");
	
	needLen = tlkmdi_mp3_getChannels() == 2 ? 128*4 : 128*2;
	#if (TLK_DEV_CODEC_ENABLE)
	if(tlkmdi_mp3_getPcmDataLen() < needLen || tlkdev_codec_getSpkIdleLen() <= needLen){
		return;
	}
	#endif
	
	while(true){
		ptr = (signed short*)pcm;
		tlkmdi_mp3_getPcmData((uint08*)pcm, needLen);
		volume = tlkmdi_audio_getToneCalVolume();
		for(index=0; index<256; index++){
			ptr[index] = (ptr[index]*volume) >> TLKMDI_AUDIO_VOLUME_EXPAND;
		}
		if(needLen == 512){
			short stereo_pcm[128*2];
			for(index=0; index<128; index++){
				stereo_pcm[index] = pcm[index*2];
				stereo_pcm[index+128] = pcm[index*2+1];
			}
			tlkalg_2chnmix(&stereo_pcm[0], &stereo_pcm[128], (short*)pcm, 1, 128);
		}
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_fillSpkBuff((uint08*)pcm, 256);
		#endif
		if(tlkmdi_mp3_getPcmDataLen() < needLen
			#if (TLK_DEV_CODEC_ENABLE)
			|| tlkdev_codec_getSpkIdleLen() <= needLen
			#endif
			){
			return; //break;
		}
	}
#else
	if(!sTlkMdiToneCtrl.running) return;
	if(sTlkMdiToneCtrl.curState == TLKMDI_TONE_STATUS_IDLE){
		sTlkMdiToneCtrl.playNumb ++;
		sTlkMdiToneCtrl.curState = TLKMDI_TONE_STATUS_PLAY;
		sTlkMdiToneCtrl.readOffset = sTlkMdiToneCtrl.fileOffset;
	}
	if(sTlkMdiToneCtrl.curState == TLKMDI_TONE_STATUS_WAIT || sTlkMdiToneCtrl.curState == TLKMDI_TONE_STATUS_DONE){
		uint16 dropLen = (sTlkMdiToneCtrl.channel == 2) ? 960 : 480;//10ms
		if(tlkdev_codec_getSpkDataLen() >= dropLen){
		#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_zeroSpkBuff(0, false);
		#endif
		}else{
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_muteSpkBuff();
			#endif
			if(sTlkMdiToneCtrl.curState == TLKMDI_TONE_STATUS_DONE){
				sTlkMdiToneCtrl.running = false;
				tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_play is over");
			}else{
				sTlkMdiToneCtrl.curState = TLKMDI_TONE_STATUS_IDLE;
			}
		}
	}else if(sTlkMdiToneCtrl.curState == TLKMDI_TONE_STATUS_PLAY){
		
		if(sTlkMdiToneCtrl.readOffset+sTlkMdiToneCtrl.encodeLen > sTlkMdiToneCtrl.readLength){
			tlkapi_trace(TLKMDI_AUDTONE_DBG_FLAG, TLKMDI_AUDTONE_DBG_SIGN, "tlkmdi_tone_play: mp3 over - %d %d",
				sTlkMdiToneCtrl.playCount, sTlkMdiToneCtrl.playNumb);
			if(sTlkMdiToneCtrl.playCount != 0 && sTlkMdiToneCtrl.playNumb >= sTlkMdiToneCtrl.playCount){
				sTlkMdiToneCtrl.curState = TLKMDI_TONE_STATUS_DONE;
			}else{
				sTlkMdiToneCtrl.curState = TLKMDI_TONE_STATUS_WAIT;
			}
		}else{
			while(true){
				if(sTlkMdiToneCtrl.readOffset + sTlkMdiToneCtrl.encodeLen > sTlkMdiToneCtrl.readLength) return;
				if(tlkdev_codec_getSpkIdleLen() <= sTlkMdiToneCtrl.frameLen) return;
				tlkapi_flash_read(sTlkMdiToneCtrl.readOffset, sTlkMdiToneCtrl.pReadBuff, sTlkMdiToneCtrl.encodeLen);
				sTlkMdiToneCtrl.readOffset += sTlkMdiToneCtrl.encodeLen;
				#if (TLKMDI_AUDTONE_DECODE_SBC)
				if(sTlkMdiToneCtrl.channel == 3){
					tlkalg_sbc_dec_stereo(sTlkMdiToneCtrl.pReadBuff, sTlkMdiToneCtrl.encodeLen, sTlkMdiToneCtrl.pSaveBuff);
				}else{
					tlkalg_sbc_dec_channel0(sTlkMdiToneCtrl.pReadBuff, sTlkMdiToneCtrl.encodeLen, sTlkMdiToneCtrl.pSaveBuff);
				}
				#elif (TLKMDI_AUDTONE_DECODE_ADPCM)
				tlkalg_adpcm_adpcmToPcm(sTlkMdiToneCtrl.pReadBuff, (sint16*)sTlkMdiToneCtrl.pSaveBuff, sTlkMdiToneCtrl.frameLen >> 1);
				#endif
				#if (TLK_DEV_CODEC_ENABLE)
				tlkdev_codec_fillSpkBuff(sTlkMdiToneCtrl.pSaveBuff, sTlkMdiToneCtrl.frameLen);
				#endif
			}
		}
	}
#endif
}


#if (TLKMDI_AUDTONE_DECODE_MP3)
static int tlkmdi_tone_findFilePath(uint08 *pBuff, uint16 buffLen, bool addRoot)
{
	uint08 dataLen = 0;
	
	#if !(TLK_FS_FAT_ENABLE)
		if(buffLen >= 7){
			pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = (uint08)'T';
			pBuff[dataLen++] = (uint08)'O';
			pBuff[dataLen++] = (uint08)'N';
			pBuff[dataLen++] = (uint08)'E';
			if(addRoot) pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = 0x00;
		}
	#else
		if(buffLen >= 18){
			pBuff[dataLen++] = (uint08)'1';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)':';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'/';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'T';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'O';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'N';
			pBuff[dataLen++] = 0x00;
			pBuff[dataLen++] = (uint08)'E';
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
static int tlkmdi_tone_findFileName(uint08 *pBuff, uint16 index)
{
	uint08 length;
	uint08 aIndex = index%10;
	uint08 bIndex = (index/10)%10;
	FCHAR* pFName = (FCHAR*)pBuff;

	length = 0;
	pFName[length++] = 0x30+bIndex;
	pFName[length++] = 0x30+aIndex;
	pFName[length++] = '.';
	pFName[length++] = 'M';
	pFName[length++] = 'P';
	pFName[length++] = '3';
	pFName[length++] = 0x00;
	return TLK_ENONE;
}
#endif //#if (TLKMDI_AUDTONE_DECODE_MP3)


#endif //#if (TLK_MDI_AUDTONE_ENABLE)


/********************************************************************************************************
 * @file	tlkmdi_audmp3.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKMDI_MP3_H
#define TLKMDI_MP3_H

#if TLK_MDI_MP3_ENABLE


#define TLKMDI_MP3_PCM_SIZE         (1152*4)



#define TLKMDI_MP3_DIR_SIZE         20
#define TLKMDI_MP3_NAME_SIZE		48
#define TLKMDI_MP3_PATH_SIZE		(TLKMDI_MP3_DIR_SIZE+TLKMDI_MP3_NAME_SIZE)


#define TLKMDI_MP3_DEC_PARAM_SIZE        6672
#define TLKMDI_MP3_DEC_CACHE0_SIZE       16240 //actual:16239
#define TLKMDI_MP3_DEC_CACHE1_SIZE       4608
#define TLKMDI_MP3_DEC_CACHE1_OFFS       TLKMDI_MP3_DEC_CACHE0_SIZE
#define TLKMDI_MP3_DEC_CACHE_SIZE        (TLKMDI_MP3_DEC_CACHE0_SIZE+TLKMDI_MP3_DEC_CACHE1_SIZE) 
#define TLKMDI_MP3_DEC_PCM_SIZE          (1152*8+4)

//Total buffer = PARAM(6672)+CACHE(16240+4608)+PCM(1152*8+4)
//             = 6672+20848+9220 = 36740+3*16 = 36788

#define TLKMDI_MP3_PERFORMER_SIZE        64


#define TLKMDI_MP3_SAVE_INTERVAL         5000000  //100ms

#define TLKMDI_MP3_UPDATE_TIMER_DEF      5000000 //5s
#define TLKMDI_MP3_UPDATE_TIMER_MAX      20000000 //20s
#define TLKMDI_MP3_UPDATE_TIMER_MIN      1000000 //1s

#define TLKMDI_MP3_FILE_INFO_SIGN        0x3A
#define TLKMDI_MP3_FILE_INFO_VERS        0x02
#define TLKMDI_MP3_FILE_INFO_ADDR        TLK_CFG_FLASH_PLAY_INFO_ADDR
#define TLKMDI_MP3_FILE_INFO_SIZE        (4+TLKMDI_MP3_NAME_SIZE) //playLens+fileName

#define TLKMDI_MP3_FILE_LIST_SIGN        0x3C3C
#define TLKMDI_MP3_FILE_LIST_VERS        0x02
#define TLKMDI_MP3_FILE_LIST_SIZE        (4+TLKMDI_MP3_NAME_SIZE) //fileSize+fileName
#define TLKMDI_MP3_FILE_LIST_ADDR        TLK_CFG_FLASH_PLAY_LIST_ADDR
#define TLKMDI_MP3_FILE_LIST_LENS        TLK_CFG_FLASH_PLAY_LIST_LENS
#define TLKMDI_MP3_FILE_LIST_MAX         ((TLK_CFG_FLASH_PLAY_LIST_LENS-16)/TLKMDI_MP3_FILE_LIST_SIZE)

typedef enum{
	TLKMDI_MP3_PALY_MODE_ORDER = 0,
	TLKMDI_MP3_PALY_MODE_LOOP,
	TLKMDI_MP3_PALY_MODE_RANDOM,
	TLKMDI_MP3_PALY_MODE_MAX,
}TLKMDI_MP3_PALY_MODE_ENUM;

typedef enum{
	TLKMDI_MP3_MPEG_TYPE_2P5 = 0,
	TLKMDI_MP3_MPEG_TYPE_RESERVE,
	TLKMDI_MP3_MPEG_TYPE_2,
	TLKMDI_MP3_MPEG_TYPE_1,
}TLKMDI_MP3_MPEG_TYPE_ENUM;
typedef enum{
	TLKMDI_MP3_LAYER_TYPE_RESERVE = 0,
	TLKMDI_MP3_LAYER_TYPE_3,
	TLKMDI_MP3_LAYER_TYPE_2,
	TLKMDI_MP3_LAYER_TYPE_1,
}TLKMDI_MP3_LAYER_TYPE_ENUM;



typedef enum{
	TLKMDI_MP3_STATUS_IDLE = 0,
	TLKMDI_MP3_STATUS_WAIT,
	TLKMDI_MP3_STATUS_DONE,
	TLKMDI_MP3_STATUS_PLAY,
}TLKMDI_MP3_STATUS_ENUM;




typedef struct{
	uint32 playLens;
	uint08 fileName[TLKMDI_MP3_NAME_SIZE];
}tlkmdi_mp3_fsave_t;
typedef struct{
	uint32 count; //Prevent this position to prevent accidental power loss
	uint08 vers; //Version
	uint08 size; //TLKMDI_MP3_UNIT_SIZE
	uint16 sign; //TLKMDI_MP3_FLASH_SIGN
}tlkmdi_mp3_flist_t;
typedef struct{
	uint08 playMode;
	uint08 reserve0;
	uint16 curIndex;
	uint32 playLens;
	uint32 cacheLens;
	uint32 cacheOffs;
}tlkmdi_mp3_fplay_t;
typedef struct{
	uint32 fileSize;
	uint32 headLens;
	uint32 duration;
	uint08 channels;
	uint08 nameFlag;
	uint16 sampleRate;
	uint08 performer[TLKMDI_MP3_PERFORMER_SIZE];
}tlkmdi_mp3_finfo_t;
typedef struct{
	uint08 playMode;
	uint08 isUpdate;
	uint16 playIndex;
	uint16 fileCount;
	tlkapi_save_ctrl_t save;
	tlkmdi_mp3_fsave_t fsave;
	tlkmdi_mp3_fplay_t fplay;
	tlkmdi_mp3_finfo_t finfo;
}tlkmdi_mp3_ctrl_t;



/******************************************************************************
 * Function: tlkmdi_mp3_init
 * Descript: Initial the mp3 codec and reset the control parameter.
 * Params: TLK_ENONE is success,other value is false .
 * Return: None.
*******************************************************************************/
int tlkmdi_mp3_init(void);

/******************************************************************************
 * Function: tlkmdi_mp3_isEnable
 * Descript: Verify the mp3 is Enable.
 * Params: true is success,other value is false .
 * Return: None.
*******************************************************************************/
bool tlkmdi_mp3_isEnable(void);

/******************************************************************************
 * Function: tlkmdi_mp3_enable
 * Descript: Enable the mp3.
 * Params: true is success,other value is false .
 * Return: None.
*******************************************************************************/
bool tlkmdi_mp3_enable(bool enable);

/******************************************************************************
 * Function: tlkmdi_mp3_getInfo
 * Descript: Get the mp3 info.
 * Params: Mp3 info.
 * Return: None.
*******************************************************************************/
tlkmdi_mp3_finfo_t *tlkmdi_mp3_getInfo(void);

/******************************************************************************
 * Function: tlkmdi_mp3_play/start/close
 * Descript: control mp3 paly/stop.
 * Params: @index[IN]--Mp3 index.
 *         @pFilePath[IN]--The file path.
 *         @fileOffset[IN]--The file path.
 * Return: None.
*******************************************************************************/
bool tlkmdi_mp3_play(uint16 index);
bool tlkmdi_mp3_start(uint08 *pFilePath, uint32 fileOffset);
void tlkmdi_mp3_close(void);

/******************************************************************************
 * Function: tlkmdi_mp3_isOver
 * Descript: verify mp3 data in tail position.
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_mp3_isOver(void);
void tlkmdi_mp3_reset(void);


/******************************************************************************
 * Function: tlkmdi_mp3_decode
 * Descript: decode audio data.
 * Params: None.
 * Return: Return SUCCESS or other value is false.
*******************************************************************************/
int  tlkmdi_mp3_decode(void);

/******************************************************************************
 * Function: tlkmdi_mp3_indexIsOver,tlkmdi_mp3_XXXXXXIndex
 * Descript: verify is the last music or pick up a muisc.
 * Params: None.
 * Return: .
*******************************************************************************/
bool tlkmdi_mp3_indexIsOver(void);
uint tlkmdi_mp3_getNextPlay(void);
void tlkmdi_mp3_setPlayIndex(uint playIndex);
uint tlkmdi_mp3_getPlayIndex(void);
uint tlkmdi_mp3_getPlayCount(void);
uint tlkmdi_mp3_getRandIndex(void);
uint tlkmdi_mp3_getNextIndex(void);
uint tlkmdi_mp3_getPrevIndex(void);

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Get the music info and set the music mode.
 * Params: None.
 * Return: .
*******************************************************************************/
uint32 tlkmdi_mp3_getFileSize(void);
uint32 tlkmdi_mp3_getPlayLens(void);
uint32 tlkmdi_mp3_getDuration(void);
uint16 tlkmdi_mp3_getProgress(void);
uint08 tlkmdi_mp3_getFNameCode(void);
uint08 tlkmdi_mp3_getSingerCode(void);
uint08 *tlkmdi_mp3_getFileName(uint08 *pLength);
uint08 *tlkmdi_mp3_getSinger(uint08 *pLength);
uint08 tlkmdi_mp3_getPlayMode(void);
void tlkmdi_mp3_setPlayMode(uint08 mode);

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Get the music playing parameter,etc, samplerate and volume.
 * Params: None.
 * Return: .
*******************************************************************************/
uint08 tlkmdi_mp3_getChannels(void);
uint16 tlkmdi_mp3_getSampleRate(void);
uint16 tlkmdi_mp3_getVolPercent(void);
void   tlkmdi_mp3_setVolPercent(uint16 volume);

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Handle the mp3 pcm data.
 * Params: None.
 * Return: .
*******************************************************************************/
void tlkmdi_mp3_clearPcmData(void);
int  tlkmdi_mp3_getPcmIdleLen(void);
int  tlkmdi_mp3_getPcmDataLen(void);
void tlkmdi_mp3_addPcmData(uint08 *pData, uint16 dataLen);
uint tlkmdi_mp3_getPcmData(uint08 *pBuff, uint16 buffLen);

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Support the interface to get the music file.
 * Params: None.
 * Return: TLK_ENONE is success,otherwise is false.
*******************************************************************************/
int  tlkmdi_mp3_loadFile(bool isForce);
int  tlkmdi_mp3_scanFileList(void);
int  tlkmdi_mp3_findFilePath(uint08 *pBuff, uint16 buffLen, bool addRoot);
int  tlkmdi_mp3_findFileName(uint08 *pBuff, uint16 index);

/******************************************************************************
 * Function: tlkmdi_mp3_getXXXXXX
 * Descript: Update the music info.
 * Params: None.
 * Return: .
*******************************************************************************/
bool tlkmdi_mp3_isUpdate(void);
bool tlkmdi_mp3_needUpdate(void);
void tlkmdi_mp3_startUpdate(void);
void tlkmdi_mp3_updateEnable(bool enable);
void tlkmdi_mp3_updatePlayName(uint08 *pFName, bool isForce);
void tlkmdi_mp3_updatePlayLens(uint32 playLen, bool isForce);
void tlkmdi_mp3_updataPlayNameByIndex(uint16 index);



#endif //TLK_MDI_MP3_ENABLE

#endif //TLKMDI_MP3_H


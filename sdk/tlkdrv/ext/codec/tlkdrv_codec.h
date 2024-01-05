/********************************************************************************************************
 * @file	tlkdrv_codec.h
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
#ifndef TLKDRV_CODEC_H
#define TLKDRV_CODEC_H


#define TLKDRV_CODEC_DBG_FLAG               0//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_CODEC_DBG_SIGN               "[CODEC]"


#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
#define TLKDRV_CODEC_PA_ENABLE              0 //Only this configuration is supported
#else
#define TLKDRV_CODEC_PA_ENABLE              0
#endif

#define TLKDRV_CODEC_ICODEC_ENABLE          1
#define TLKDRV_CODEC_IISSLV_ENABLE          0
#define TLKDRV_CODEC_IISMST_ENABLE          0

#define TLKDRV_CODEC_SPK_DMA                DMA3
#define TLKDRV_CODEC_MIC_DMA                DMA2
#define TLKDRV_CODEC_SPK_FIFO               FIFO0
#define TLKDRV_CODEC_MIC_FIFO               FIFO0


typedef enum{
	TLKDRV_CODEC_DEV_NONE = 0,
#if (TLKDRV_CODEC_ICODEC_ENABLE)
	TLKDRV_CODEC_DEV_INNER,
#endif
#if (TLKDRV_CODEC_IISSLV_ENABLE || TLKDRV_CODEC_IISSLV_B92_ENABLE)
	TLKDRV_CODEC_DEV_IISSLV,
#endif
#if (TLKDRV_CODEC_IISMST_ENABLE || TLKDRV_CODEC_IISMST_B92_ENABLE)
	TLKDRV_CODEC_DEV_IISMST,
#endif
	TLKDRV_CODEC_DEV_MAX,
}TLKDRV_CODEC_DEV_ENUM;
typedef enum{
	TLKDRV_CODEC_SUBDEV_DEF  = 0x00,
	TLKDRV_CODEC_SUBDEV_MIC  = 0x01,
	TLKDRV_CODEC_SUBDEV_SPK  = 0x02,
	TLKDRV_CODEC_SUBDEV_BOTH = 0x03,
}TLKDRV_CODEC_SUBDEV_ENUM;
typedef enum{
	TLKDRV_CODEC_OPCODE_NONE = 0,
	TLKDRV_CODEC_OPCODE_SET_MUTE,  //param0:[uint08]isMute
	TLKDRV_CODEC_OPCODE_GET_MUTE,
	TLKDRV_CODEC_OPCODE_SET_VOLUME, //param0:[uint08]volume
	TLKDRV_CODEC_OPCODE_GET_VOLUME,
	TLKDRV_CODEC_OPCODE_SET_CHANNEL, //param0:[uint08]chnCnt-1/2/3, 0x01-left channel, 0x02-right channel
	TLKDRV_CODEC_OPCODE_GET_CHANNEL,
	TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, //param0:[uint08]bitDepth-16/20/24
	TLKDRV_CODEC_OPCODE_GET_BIT_DEPTH,
	TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, //param0:[uint32]sampleRate-8k/16k/32k/44.1k/48k
	TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE,
}TLKDRV_CODEC_OPCODE_ENUM;
typedef enum{
	TLKDRV_CODEC_CHANNEL_LEFT   = 0x01,
	TLKDRV_CODEC_CHANNEL_RIGHT  = 0x02,
	TLKDRV_CODEC_CHANNEL_STEREO = 0x03,
}TLKDRV_CODEC_CHANNEL_ENUM;
typedef enum{
	TLKDRV_CODEC_BITDEPTH_16 = 16,
	TLKDRV_CODEC_BITDEPTH_20 = 20,
	TLKDRV_CODEC_BITDEPTH_24 = 24,
}TLKDRV_CODEC_BITDEPTH_ENUM;
typedef enum{
	TLKDRV_CODEC_SAMPLERATE_8000  = 8000,
	TLKDRV_CODEC_SAMPLERATE_16000 = 16000,
	TLKDRV_CODEC_SAMPLERATE_32000 = 32000,
	TLKDRV_CODEC_SAMPLERATE_44100 = 44100,
	TLKDRV_CODEC_SAMPLERATE_48000 = 48000,
}TLKDRV_CODEC_SAMPLERATE_ENUM;

typedef struct{
	bool(*IsOpen)(uint08 subDev);
	int(*Init)(uint08 subDev);
	int(*Open)(uint08 subDev);
	int(*Pause)(uint08 subDev);
	int(*Close)(uint08 subDev);
	int(*Reset)(uint08 subDev);
	int(*Config)(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1);
}tlkdrv_codec_modinf_t;





bool tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_ENUM subDev);

int tlkdrv_codec_mount(TLKDRV_CODEC_DEV_ENUM dev, TLKDRV_CODEC_SUBDEV_ENUM subDev);

int tlkdrv_codec_open(TLKDRV_CODEC_SUBDEV_ENUM subDev);
int tlkdrv_codec_pause(void);
int tlkdrv_codec_close(void);
int tlkdrv_codec_reset(void);
int tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint08 opcode, uint32 param0, uint32 param1);

bool tlkdrv_codec_paIsOpen(void);
void tlkdrv_codec_paInit(void);
void tlkdrv_codec_paOpen(void);
void tlkdrv_codec_paClose(void);


int tlkdrv_codec_setMicStatus(bool isMute);
int tlkdrv_codec_setSpkStatus(bool isMute);

int tlkdrv_codec_setMicVolume(uint08 volume);
int tlkdrv_codec_setSpkVolume(uint08 volume);

int tlkdrv_codec_getMicVolume(void);
int tlkdrv_codec_getSpkVolume(void);

int tlkdrv_codec_setChannel(uint08 channel);
int tlkdrv_codec_setMicChannel(uint08 channel);
int tlkdrv_codec_setSpkChannel(uint08 channel);

int tlkdrv_codec_setBitDepth(uint08 bitDepth);
int tlkdrv_codec_setMicBitDepth(uint08 bitDepth);
int tlkdrv_codec_setSpkBitDepth(uint08 bitDepth);

int tlkdrv_codec_setSampleRate(uint32 sampleRate);
int tlkdrv_codec_setMicSampleRate(uint32 sampleRate);
int tlkdrv_codec_setSpkSampleRate(uint32 sampleRate);

uint32 tlkdrv_codec_getSampleRate(void);
uint08 tlkdrv_codec_getChannel(void);

uint tlkdrv_codec_getSpkOffset(void);
uint tlkdrv_codec_getMicOffset(void);
void tlkdrv_codec_setSpkOffset(uint16 offset);
void tlkdrv_codec_setMicOffset(uint16 offset);

void tlkdrv_codec_setSpkBuffer(uint08 *pBuffer, uint16 buffLen);
void tlkdrv_codec_setMicBuffer(uint08 *pBuffer, uint16 buffLen);

uint tlkdrv_codec_getSpkBuffLen(void);
uint tlkdrv_codec_getMicBuffLen(void);

uint tlkdrv_codec_getSpkIdleLen(void);
uint tlkdrv_codec_getSpkDataLen(void);
uint tlkdrv_codec_getMicDataLen(void);

bool tlkdrv_codec_readSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset);
bool tlkdrv_codec_readMicData(uint08 *pBuffer, uint16 buffLen, uint16 *pOffset);

void tlkdrv_codec_muteSpkBuff(void);
void tlkdrv_codec_muteMicBuff(void);

void tlkdrv_codec_zeroSpkBuff(uint16 zeroLen, bool isInc);
bool tlkdrv_codec_fillSpkBuff(uint08 *pData, uint16 dataLen);
bool tlkdrv_codec_backReadSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset, bool isBack);



#endif //TLKDRV_CODEC_H


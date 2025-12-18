/********************************************************************************************************
 * @file	tlkdev_codec.h
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
#ifndef TLKDEV_CODEC_H
#define TLKDEV_CODEC_H

#if (TLK_DEV_CODEC_ENABLE)


#ifndef TLK_DEV_MIC_BUFF_SIZE
#define TLK_DEV_MIC_BUFF_SIZE        (1024*2)
#endif
#ifndef TLK_DEV_SPK_BUFF_SIZE
#define TLK_DEV_SPK_BUFF_SIZE        (1024*4) //If SRC is enabled, the value must be greater than or equal to 4K
#endif


typedef enum{
	TLKDEV_CODEC_SUBDEV_DEF  = 0x00,
	TLKDEV_CODEC_SUBDEV_MIC  = 0x01,
	TLKDEV_CODEC_SUBDEV_SPK  = 0x02,
	TLKDEV_CODEC_SUBDEV_BOTH = 0x03,
}TLKDEV_CODEC_SUBDEV_ENUM;

typedef enum{
	TLKDEV_CODEC_CHANNEL_LEFT   = 0x01,
	TLKDEV_CODEC_CHANNEL_RIGHT  = 0x02,
	TLKDEV_CODEC_CHANNEL_STEREO = 0x03,
}TLKDEV_CODEC_CHANNEL_ENUM;
typedef enum{
	TLKDEV_CODEC_BITDEPTH_16 = 16,
	TLKDEV_CODEC_BITDEPTH_20 = 20,
	TLKDEV_CODEC_BITDEPTH_24 = 24,
}TLKDEV_CODEC_BITDEPTH_ENUM;
typedef enum{
	TLKDEV_CODEC_SAMPLERATE_8000  = 8000,
	TLKDEV_CODEC_SAMPLERATE_16000 = 16000,
	TLKDEV_CODEC_SAMPLERATE_32000 = 32000,
	TLKDEV_CODEC_SAMPLERATE_44100 = 44100,
	TLKDEV_CODEC_SAMPLERATE_48000 = 48000,
}TLKDEV_CODEC_SAMPLERATE_ENUM;


typedef struct{
	uint08 spkIsMute;
	uint08 micIsMute;
	uint08 spkVolume;
	uint08 micVolume;
	uint08 spkChannel;
	uint08 micChannel;
	uint08 selSpkVolume;
	uint08 selMicVolume;
}tlkdev_codec_t;


int tlkdev_codec_init(void);
void tlkdev_codec_deinit(void);

int tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_ENUM subDev, uint08 channel, uint08 bitDepth, uint32 sampleRate);
int tlkdev_codec_close(void);
int tlkdev_codec_extOpen(TLKDEV_CODEC_SUBDEV_ENUM subDev, uint08 spkChannel, uint08 spkBitDepth,
	uint32 spkSampleRate, uint08 micChannel, uint08 micBitDepth, uint32 micSampleRate);


uint tlkdev_codec_getSpkOffset(void);
uint tlkdev_codec_getMicOffset(void);
void tlkdev_codec_setSpkOffset(uint16 offset);
void tlkdev_codec_setMicOffset(uint16 offset);

uint tlkdev_codec_getSpkBuffLen(void);
uint tlkdev_codec_getMicBuffLen(void);
uint tlkdev_codec_getSpkIdleLen(void);
uint tlkdev_codec_getSpkDataLen(void);
uint tlkdev_codec_getMicDataLen(void);

bool tlkdev_codec_readSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset);
bool tlkdev_codec_readMicData(uint08 *pBuff, uint16 buffLen, uint16 *pOffset);

void tlkdev_codec_muteSpkBuff(void);
void tlkdev_codec_zeroSpkBuff(uint16 zeroLen, bool isInc);
bool tlkdev_codec_fillSpkBuff(uint08 *pData, uint16 dataLen);
bool tlkdev_codec_backReadSpkData(uint08 *pBuff, uint16 buffLen, uint16 offset, bool isBack);


#endif //#if (TLK_DEV_CODEC_ENABLE)

#endif //TLKDEV_CODEC_H


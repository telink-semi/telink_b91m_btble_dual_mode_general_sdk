/********************************************************************************************************
 * @file	tlkdev_codec.c
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
#if (TLK_DEV_CODEC_ENABLE)
#include "tlkdrv/ext/codec/tlkdrv_codec.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdev_codec.h"
#include "drivers.h"

static void tlkdrv_codec_micDataZoom(uint08 *pData, uint16 dataLen);
static void tlkdrv_codec_spkDataZoom(uint08 *pData, uint16 dataLen);

static int tlkdev_codec_nodeInit(uint16 did);
static int tlkdev_codec_nodeOpen(uint16 did, uint32 param);
static int tlkdev_codec_nodeClose(uint16 did);
static int tlkdev_codec_nodeState(uint16 did);
static int tlkdev_codec_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_codec_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_codec_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);

static int tlkdev_codec_recvSetSpkMuteDeal(bool isMute);
static int tlkdev_codec_recvSetMicMuteDeal(bool isMute);
static int tlkdev_codec_recvSetSpkVolumeDeal(uint volume);
static int tlkdev_codec_recvSetMicVolumeDeal(uint volume);
static int tlkdev_codec_recvGetSpkVolumeDeal(void);
static int tlkdev_codec_recvGetMicVolumeDeal(void);
static int tlkdev_codec_recvGetSpkOffsetDeal(void);
static int tlkdev_codec_recvGetMicOffsetDeal(void);
static int tlkdev_codec_recvSetSpkOffsetDeal(uint16 offset);
static int tlkdev_codec_recvSetMicOffsetDeal(uint16 offset);
static int tlkdev_codec_recvGetSpkBuffLenDeal(void);
static int tlkdev_codec_recvSetSpkBuffLenDeal(uint16 buffLen);
static int tlkdev_codec_recvGetMicBuffLenDeal(void);
static int tlkdev_codec_recvGetSpkIdleLenDeal(void);
static int tlkdev_codec_recvSetMicBuffLenDeal(uint16 buffLen);
static int tlkdev_codec_recvGetSpkDataLenDeal(void);
static int tlkdev_codec_recvGetMicDataLenDeal(void);
static int tlkdev_codec_recvOpenMicBiasDeal(void);
static int tlkdev_codec_recvCloseMicBiasDeal(void);
static int tlkdev_codec_recvMuteSpkBuffDeal(void);
static int tlkdev_codec_recvZeroSpkBuffDeal(uint16 zeroLen, bool isInc);
static int tlkdev_codec_recvGetChannelDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev);
static int tlkdev_codec_recvSetChannelDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint08 channel);
static int tlkdev_codec_recvGetBitDepthDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev);
static int tlkdev_codec_recvSetBitDepthDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint08 bitDepth);
static int tlkdev_codec_recvGetSampleRateDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev);
static int tlkdev_codec_recvSetSampleRateDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint32 sampleRate);


static const tlkdev_node_t scTlkDevCodecInner = {
	TLKDEV_DID_CODEC,  //.devID;
	"Codec-Inner",           //.pName;
	tlkdev_codec_nodeInit,  //.Init
	tlkdev_codec_nodeOpen,  //.Open
	tlkdev_codec_nodeClose, //.Close
	tlkdev_codec_nodeState, //.State
	tlkdev_codec_nodeRead,  //.Read
	tlkdev_codec_nodeWrite, //.Write
	tlkdev_codec_nodeIoctl, //.Ioctl
};
__attribute__((aligned(4))) uint16 gTlkDevCodecSpkBuffer[TLK_DEV_SPK_BUFF_SIZE>>1];
__attribute__((aligned(4))) uint16 gTlkDevCodecMicBuffer[TLK_DEV_MIC_BUFF_SIZE>>1];
static tlkdev_codec_t sTlkDevCodecCtrl;


int tlkdev_codec_init(void)
{
	tmemset(&sTlkDevCodecCtrl, 0, sizeof(tlkdev_codec_t));
	
	sTlkDevCodecCtrl.spkChannel = 1;
	sTlkDevCodecCtrl.micChannel = 1;
	
	sTlkDevCodecCtrl.selSpkVolume = 128;
	sTlkDevCodecCtrl.spkIsMute = false;
	sTlkDevCodecCtrl.selMicVolume = 128;
	sTlkDevCodecCtrl.micIsMute = false;
				
	tlkdev_appendEctypeNode(&scTlkDevCodecInner);
	
	return TLK_ENONE;
}
void tlkdev_codec_deinit(void)
{
	tlkdev_removeEctypeNode(TLKDEV_DID_CODEC);
}


int tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_ENUM subDev, uint08 channel, uint08 bitDepth, uint32 sampleRate)
{
	int ret = TLK_ENONE;
	tlkdev_codec_nodeInit(TLKDEV_DID_CODEC);
	if(ret == TLK_ENONE) ret = tlkdrv_codec_setChannel(channel);
	if(ret == TLK_ENONE) ret = tlkdrv_codec_setBitDepth(bitDepth);
	if(ret == TLK_ENONE) ret = tlkdrv_codec_setSampleRate(sampleRate);	
	if(ret == TLK_ENONE) ret = tlkdrv_codec_open(subDev);
	if(ret == TLK_ENONE) tlkdrv_codec_paOpen();
	return ret;
}
int tlkdev_codec_close(void)
{
	tlkdrv_codec_paClose();
	return tlkdrv_codec_close();
}
int tlkdev_codec_extOpen(TLKDEV_CODEC_SUBDEV_ENUM subDev, uint08 spkChannel, uint08 spkBitDepth,
	uint32 spkSampleRate, uint08 micChannel, uint08 micBitDepth, uint32 micSampleRate)
{
	int ret = TLK_ENONE;
	tlkdev_codec_nodeInit(TLKDEV_DID_CODEC);
	if(ret == TLK_ENONE){
		if(subDev == TLKDEV_CODEC_SUBDEV_SPK){
			ret = tlkdrv_codec_setSpkChannel(spkChannel);
		}else if(subDev == TLKDEV_CODEC_SUBDEV_MIC){
			ret = tlkdrv_codec_setMicChannel(micChannel);
		}else if(spkChannel == micChannel){
			ret = tlkdrv_codec_setChannel(spkChannel);
		}else{
			return -TLK_EPARAM;
		}
	}
	if(ret == TLK_ENONE){
		if(subDev == TLKDEV_CODEC_SUBDEV_SPK){
			ret = tlkdrv_codec_setSpkBitDepth(spkBitDepth);
		}else if(subDev == TLKDEV_CODEC_SUBDEV_MIC){
			ret = tlkdrv_codec_setMicBitDepth(micBitDepth);
		}else if(spkBitDepth == micBitDepth){
			ret = tlkdrv_codec_setBitDepth(spkBitDepth);
		}else{
			return -TLK_EPARAM;
		}
	}
	if(ret == TLK_ENONE){
		if(subDev == TLKDEV_CODEC_SUBDEV_SPK){
			ret = tlkdrv_codec_setSpkSampleRate(spkSampleRate);
		}else if(subDev == TLKDEV_CODEC_SUBDEV_MIC){
			ret = tlkdrv_codec_setMicSampleRate(micSampleRate);
		}else if(spkSampleRate == micSampleRate){
			ret = tlkdrv_codec_setSampleRate(spkSampleRate);
		}else{
			return -TLK_EPARAM;
		}
	}
	if(ret == TLK_ENONE) ret = tlkdrv_codec_open(subDev);
	if(ret == TLK_ENONE) tlkdrv_codec_paOpen();
	return ret;
}
bool tlkdev_codec_readSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset)
{
	return tlkdrv_codec_readSpkData(pBuffer, buffLen, offset);
}
bool tlkdev_codec_readMicData(uint08 *pBuff, uint16 buffLen, uint16 *pOffset)
{
	bool isSucc = tlkdrv_codec_readMicData(pBuff, buffLen, pOffset);
	if(isSucc) tlkdrv_codec_micDataZoom(pBuff, buffLen);
	return isSucc;
}
bool tlkdev_codec_fillSpkRawData(uint08 *pData, uint16 dataLen)
{
	return tlkdrv_codec_fillSpkBuff(pData, dataLen);
}
bool tlkdev_codec_readMicRawData(uint08 *pBuff, uint16 buffLen)
{
	return tlkdrv_codec_readMicData(pBuff, buffLen, nullptr);
}

void tlkdev_codec_muteSpkBuff(void)
{
	tlkdrv_codec_muteSpkBuff();
}
void tlkdev_codec_zeroSpkBuff(uint16 zeroLen, bool isInc)
{
	tlkdrv_codec_zeroSpkBuff(zeroLen, isInc);
}
bool tlkdev_codec_fillSpkBuff(uint08 *pData, uint16 dataLen)
{
	tlkdrv_codec_spkDataZoom(pData, dataLen);
	return tlkdrv_codec_fillSpkBuff(pData, dataLen);
}
bool tlkdev_codec_backReadSpkData(uint08 *pBuff, uint16 buffLen, uint16 offset, bool isBack)
{
	return tlkdrv_codec_backReadSpkData(pBuff, buffLen, offset, isBack);
}

uint tlkdev_codec_getSpkOffset(void)
{
	return tlkdrv_codec_getSpkOffset();
}
uint tlkdev_codec_getMicOffset(void)
{
	return tlkdrv_codec_getMicOffset();
}
void tlkdev_codec_setSpkOffset(uint16 offset)
{
	tlkdrv_codec_setSpkOffset(offset);
}
void tlkdev_codec_setMicOffset(uint16 offset)
{
	tlkdrv_codec_setMicOffset(offset);
}

uint tlkdev_codec_getSpkBuffLen(void)
{
	return tlkdrv_codec_getSpkBuffLen();
}
uint tlkdev_codec_getMicBuffLen(void)
{
	return tlkdrv_codec_getMicBuffLen();
}
uint tlkdev_codec_getSpkIdleLen(void)
{
	return tlkdrv_codec_getSpkIdleLen();
}
uint tlkdev_codec_getSpkDataLen(void)
{
	return tlkdrv_codec_getSpkDataLen();
}
uint tlkdev_codec_getMicDataLen(void)
{
	return tlkdrv_codec_getMicDataLen();
}


static int tlkdev_codec_nodeInit(uint16 did)
{
	tlkdrv_codec_setSpkBuffer((uint08*)gTlkDevCodecSpkBuffer, TLK_DEV_SPK_BUFF_SIZE);
	tlkdrv_codec_setMicBuffer((uint08*)gTlkDevCodecMicBuffer, TLK_DEV_MIC_BUFF_SIZE);

	tlkdrv_codec_paInit();
	
#if (TLKDRV_CODEC_ICODEC_ENABLE)
	tlkdrv_codec_mount(TLKDRV_CODEC_DEV_INNER, TLKDRV_CODEC_SUBDEV_BOTH);
#elif (TLKDRV_CODEC_IISSLV_ENABLE)
	tlkdrv_codec_mount(TLKDRV_CODEC_DEV_IISSLV, TLKDRV_CODEC_SUBDEV_BOTH);
#elif (TLKDRV_CODEC_IISMST_ENABLE)
	tlkdrv_codec_mount(TLKDRV_CODEC_DEV_IISMST, TLKDRV_CODEC_SUBDEV_BOTH);
#endif
		
	tlkdrv_codec_setChannel(1);
	tlkdrv_codec_setBitDepth(16);
	tlkdrv_codec_setSampleRate(48000);
	
	return TLK_ENONE;
}
static int tlkdev_codec_nodeOpen(uint16 did, uint32 param)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_codec_nodeClose(uint16 did)
{
	tlkdrv_codec_paClose();
	return tlkdrv_codec_close();
}
static int tlkdev_codec_nodeState(uint16 did)
{
	if(tlkdrv_codec_isOpen(TLKDEV_CODEC_SUBDEV_DEF)){
		return TLKDEV_STATE_OPENED;
	}
	return TLKDEV_STATE_CLOSED;
}
static int tlkdev_codec_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_codec_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	tlkdrv_codec_spkDataZoom(pData, dataLen);
	return tlkdrv_codec_fillSpkBuff(pData, dataLen);
}
static int tlkdev_codec_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKDEV_OPCODE_CODEC_SET_SPK_MUTE){
		return tlkdev_codec_recvSetSpkMuteDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_MIC_MUTE){
		return tlkdev_codec_recvSetMicMuteDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_SPK_VOLUME){
		return tlkdev_codec_recvGetSpkVolumeDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_SPK_VOLUME){
		return tlkdev_codec_recvSetSpkVolumeDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_MIC_VOLUME){
		return tlkdev_codec_recvGetMicVolumeDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_MIC_VOLUME){
		return tlkdev_codec_recvSetMicVolumeDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_SPK_OFFSET){
		return tlkdev_codec_recvGetSpkOffsetDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_SPK_OFFSET){
		return tlkdev_codec_recvSetSpkOffsetDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_MIC_OFFSET){
		return tlkdev_codec_recvGetMicOffsetDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_MIC_OFFSET){
		return tlkdev_codec_recvSetMicOffsetDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_MIC_BUFFLEN){
		return tlkdev_codec_recvGetMicBuffLenDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_MIC_BUFFLEN){
		return tlkdev_codec_recvSetMicBuffLenDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_SPK_BUFFLEN){
		return tlkdev_codec_recvGetSpkBuffLenDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_SPK_BUFFLEN){
		return tlkdev_codec_recvSetSpkBuffLenDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_MIC_DATALEN){
		return tlkdev_codec_recvGetMicDataLenDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_SPK_DATALEN){
		return tlkdev_codec_recvGetSpkDataLenDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_SPK_IDLELEN){
		return tlkdev_codec_recvGetSpkIdleLenDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_MUTE_SPK_BUFF){
		return tlkdev_codec_recvMuteSpkBuffDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_ZERO_SPK_BUFF){
		return tlkdev_codec_recvZeroSpkBuffDeal(param0, param1);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_CHANNEL){
		return tlkdev_codec_recvGetChannelDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_CHANNEL){
		return tlkdev_codec_recvSetChannelDeal(param0, param1);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_BIT_DEPTH){
		return tlkdev_codec_recvGetBitDepthDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_BIT_DEPTH){
		return tlkdev_codec_recvSetBitDepthDeal(param0, param1);
	}else if(opcode == TLKDEV_OPCODE_CODEC_GET_SAMPLE_RATE){
		return tlkdev_codec_recvGetSampleRateDeal(param0);
	}else if(opcode == TLKDEV_OPCODE_CODEC_SET_SAMPLE_RATE){
		return tlkdev_codec_recvSetSampleRateDeal(param0, param1);
	}else if(opcode == TLKDEV_OPCODE_CODEC_OPEN_MIC_BIAS){
		return tlkdev_codec_recvOpenMicBiasDeal();
	}else if(opcode == TLKDEV_OPCODE_CODEC_CLOSE_MIC_BIAS){
		return tlkdev_codec_recvCloseMicBiasDeal();
	}
	
	return -TLK_ENOSUPPORT;
}


static int tlkdev_codec_recvSetSpkMuteDeal(bool isMute)
{
	if(tlkdrv_codec_setSpkStatus(isMute) != TLK_ENONE){
		sTlkDevCodecCtrl.spkIsMute = true;
		tlkdrv_codec_muteSpkBuff();
	}else{
		sTlkDevCodecCtrl.spkIsMute = false;
	}
	return TLK_ENONE;
}
static int tlkdev_codec_recvSetMicMuteDeal(bool isMute)
{
	if(tlkdrv_codec_setMicStatus(isMute) != TLK_ENONE){
		sTlkDevCodecCtrl.micIsMute = true;
		tlkdrv_codec_muteMicBuff();
	}else{
		sTlkDevCodecCtrl.micIsMute = false;
	}
	return TLK_ENONE;
}
static int tlkdev_codec_recvSetSpkVolumeDeal(uint volume)
{
	if(volume > 100) volume = 100;
	if(tlkdrv_codec_setSpkVolume(volume) != TLK_ENONE){
		sTlkDevCodecCtrl.spkVolume = volume;
	}else{
		sTlkDevCodecCtrl.spkVolume = 100;
	}
	sTlkDevCodecCtrl.selSpkVolume = (volume<<7)/100;
	sTlkDevCodecCtrl.spkIsMute = false;
	return TLK_ENONE;
}
static int tlkdev_codec_recvSetMicVolumeDeal(uint volume)
{
	if(volume > 100) volume = 100;
	if(tlkdrv_codec_setMicVolume(volume) != TLK_ENONE){
		sTlkDevCodecCtrl.micVolume = volume;
	}else{
		sTlkDevCodecCtrl.micVolume = 100;
	}
	sTlkDevCodecCtrl.selMicVolume = (volume<<7)/100;
	sTlkDevCodecCtrl.micIsMute = false;
	return TLK_ENONE;
}
static int tlkdev_codec_recvGetSpkVolumeDeal(void)
{
	int ret = tlkdrv_codec_getSpkVolume();
	if(ret >= 0) return (uint)ret;
	else return sTlkDevCodecCtrl.spkVolume;
}
static int tlkdev_codec_recvGetMicVolumeDeal(void)
{
	int ret = tlkdrv_codec_getMicVolume();
	if(ret >= 0) return (uint)ret;
	else return sTlkDevCodecCtrl.micVolume;
}
static int tlkdev_codec_recvGetSpkOffsetDeal(void)
{
	return tlkdrv_codec_getSpkOffset();
}
static int tlkdev_codec_recvGetMicOffsetDeal(void)
{
	return tlkdrv_codec_getMicOffset();
}
static int tlkdev_codec_recvSetSpkOffsetDeal(uint16 offset)
{
	tlkdrv_codec_setSpkOffset(offset);
	return TLK_ENONE;
}
static int tlkdev_codec_recvSetMicOffsetDeal(uint16 offset)
{
	tlkdrv_codec_setMicOffset(offset);
	return TLK_ENONE;
}
static int tlkdev_codec_recvGetSpkBuffLenDeal(void)
{
	return tlkdrv_codec_getSpkBuffLen();
}
static int tlkdev_codec_recvSetSpkBuffLenDeal(uint16 buffLen)
{
	buffLen = buffLen & 0xFFFC;
	if(buffLen == 0) return -TLK_EPARAM;
	if(buffLen > TLK_DEV_SPK_BUFF_SIZE) return -TLK_EOVERFLOW;
	tlkdrv_codec_setSpkBuffer((uint08*)gTlkDevCodecSpkBuffer, buffLen);
	return TLK_ENONE;
}
static int tlkdev_codec_recvGetMicBuffLenDeal(void)
{
	return tlkdrv_codec_getMicBuffLen();
}
static int tlkdev_codec_recvSetMicBuffLenDeal(uint16 buffLen)
{
	buffLen = buffLen & 0xFFFC;
	if(buffLen == 0) return -TLK_EPARAM;
	if(buffLen > TLK_DEV_SPK_BUFF_SIZE) return -TLK_EOVERFLOW;
	tlkdrv_codec_setMicBuffer((uint08*)gTlkDevCodecMicBuffer, buffLen);
	return TLK_ENONE;
}
static int tlkdev_codec_recvGetSpkIdleLenDeal(void)
{
	return tlkdrv_codec_getSpkIdleLen();
}
static int tlkdev_codec_recvGetSpkDataLenDeal(void)
{
	return tlkdrv_codec_getSpkDataLen();
}
static int tlkdev_codec_recvGetMicDataLenDeal(void)
{
	return tlkdrv_codec_getMicDataLen();
}
static int tlkdev_codec_recvOpenMicBiasDeal(void)
{
#if(MCU_CORE_TYPE == MCU_CORE_B91)
	audio_codec_active();
	audio_codec_set_micbias(POWER_ON, MICBIAS_NORMAL_2V0_MODE);
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}
static int tlkdev_codec_recvCloseMicBiasDeal(void)
{
#if(MCU_CORE_TYPE == MCU_CORE_B91)
	audio_codec_active();
	audio_codec_set_micbias(POWER_DOWN, MICBIAS_NORMAL_2V0_MODE);
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}
static int tlkdev_codec_recvMuteSpkBuffDeal(void)
{
	tlkdrv_codec_muteSpkBuff();
	return TLK_ENONE;
}
static int tlkdev_codec_recvZeroSpkBuffDeal(uint16 zeroLen, bool isInc)
{
	tlkdrv_codec_zeroSpkBuff(zeroLen, isInc);
	return TLK_ENONE;
}
static int tlkdev_codec_recvGetChannelDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	return tlkdrv_codec_config(subDev, TLKDRV_CODEC_OPCODE_GET_CHANNEL, 0, 0);
}
static int tlkdev_codec_recvSetChannelDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint08 channel)
{
	return tlkdrv_codec_config(subDev, TLKDRV_CODEC_OPCODE_SET_CHANNEL, channel, 0);
}
static int tlkdev_codec_recvGetBitDepthDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	return tlkdrv_codec_config(subDev, TLKDRV_CODEC_OPCODE_GET_CHANNEL, 0, 0);
}
static int tlkdev_codec_recvSetBitDepthDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint08 bitDepth)
{
	return tlkdrv_codec_config(subDev, TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, bitDepth, 0);
}
static int tlkdev_codec_recvGetSampleRateDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	return tlkdrv_codec_config(subDev, TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE, 0, 0);
}
static int tlkdev_codec_recvSetSampleRateDeal(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint32 sampleRate)
{
	return tlkdrv_codec_config(subDev, TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, sampleRate, 0);
}




static void tlkdrv_codec_micDataZoom(uint08 *pData, uint16 dataLen)
{
	if(pData == nullptr || dataLen == 0 || (dataLen & 0x01) != 0) return;
	if(sTlkDevCodecCtrl.micIsMute){
		tmemset(pData, 0, dataLen);
	}else if(sTlkDevCodecCtrl.micVolume != 128){
		uint16 index;
		for(index=0; index<(dataLen>>1); index++){
			pData[index] = ((uint32)pData[index] * sTlkDevCodecCtrl.selMicVolume) >> 7;
		}
	}
}
static void tlkdrv_codec_spkDataZoom(uint08 *pData, uint16 dataLen)
{
	if(pData == nullptr || dataLen == 0 || (dataLen & 0x01) != 0) return;
	if(sTlkDevCodecCtrl.spkIsMute){
		tmemset(pData, 0, dataLen);
	}else if(sTlkDevCodecCtrl.spkVolume != 128){
		uint16 index;
		for(index=0; index<(dataLen>>1); index++){
			pData[index] = ((uint32)pData[index] * sTlkDevCodecCtrl.selSpkVolume) >> 7;
		}
	}
}


#endif //#if (TLK_DEV_CODEC_ENABLE)


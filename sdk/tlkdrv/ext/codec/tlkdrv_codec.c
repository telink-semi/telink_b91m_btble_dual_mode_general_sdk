/********************************************************************************************************
 * @file	tlkdrv_codec.c
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
#include "tlkdrv_codec.h"
#include "tlkdrv_iismst_b91.h"
#include "tlkdrv_iisslv_b91.h"
#include "tlkdrv_iismst_b92.h"
#include "tlkdrv_iisslv_b92.h"
#if (TLKDRV_CODEC_PA_ENABLE)
#include "tlkdrv_aw87318.h"
#endif
#include "drivers.h"


static const tlkdrv_codec_modinf_t *tlkdrv_codec_getDev(uint08 dev);

const uint16 scTlkDrvCodec8kParam[5] =  {8,125,12,64,64};//AUDIO_RATE_EQUAL	8000
const uint16 scTlkDrvCodec16kParam[5] = {8,125,6,64,64};
const uint16 scTlkDrvCodec32kParam[5] = {8,125,3,64,64};
const uint16 scTlkDrvCodec48kParam[5] = {2,125,0,64,64};
const uint16 scTlkDrvCodec44k1Param[5] = {76,235,11,64,64};

#if (TLKDRV_CODEC_ICODEC_ENABLE)
extern const tlkdrv_codec_modinf_t gcTlkDrvIcodecInf;
#endif
#if (TLKDRV_CODEC_IISMST_ENABLE)
	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	extern const tlkdrv_codec_modinf_t gcTlkDrvIisMstInf_b91;
	#elif (MCU_CORE_TYPE == MCU_CORE_B92)
	extern const tlkdrv_codec_modinf_t gcTlkDrvIisMstInf_b92;
	#endif
#endif
#if (TLKDRV_CODEC_IISSLV_ENABLE)
	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	extern const tlkdrv_codec_modinf_t gcTlkDrvIisSlvInf_b91;
	#elif (MCU_CORE_TYPE == MCU_CORE_B92)
	extern const tlkdrv_codec_modinf_t gcTlkDrvIisSlvInf_b92;
	#endif
#endif
static const tlkdrv_codec_modinf_t *spTlkDrvCodecModinf[TLKDRV_CODEC_DEV_MAX] = {
	nullptr,
#if (TLKDRV_CODEC_ICODEC_ENABLE)
	&gcTlkDrvIcodecInf,
#endif
#if (TLKDRV_CODEC_IISSLV_ENABLE)
	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	&gcTlkDrvIisSlvInf_b91,
	#elif (MCU_CORE_TYPE == MCU_CORE_B92)
	&gcTlkDrvIisSlvInf_b92,
	#else
	nullptr,
	#endif
#endif
#if (TLKDRV_CODEC_IISMST_ENABLE)
	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	&gcTlkDrvIisMstInf_b91,
	#elif (MCU_CORE_TYPE == MCU_CORE_B92)
	&gcTlkDrvIisMstInf_b92,
	#else
	nullptr,
	#endif
#endif
};
static uint08 sTlkDrvCodecSpkIsMute = true;
static uint08 sTlkDrvCodecMajorDev = 0xFF; //Unknown
static uint08 sTlkDrvCodecMinorDev = 0xFF; //Unknown
static uint16 gTlkDrvCodecSpkOffset = 0;
static uint16 gTlkDrvCodecMicOffset = 0;
uint16  gTlkDrvCodecSpkBuffLen = 0;
uint16  gTlkDrvCodecMicBuffLen = 0;
uint08 *gpTlkDrvCodecSpkBuffer = nullptr;
uint08 *gpTlkDrvCodecMicBuffer = nullptr;

int tlkdrv_codec_mount(TLKDRV_CODEC_DEV_ENUM dev, TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	int ret;
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	ret = pModInf->Init(subDev);
	if(ret == TLK_ENONE){
		sTlkDrvCodecMajorDev = dev;
		sTlkDrvCodecMinorDev = subDev;
	}
	return ret;
}

bool tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return false;
	return pModInf->IsOpen(subDev);
}


int tlkdrv_codec_open(TLKDRV_CODEC_SUBDEV_ENUM subDev)
{
	int ret;
	const tlkdrv_codec_modinf_t *pModInf;

//	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "=== tlkdrv_codec_open 001: %d", sTlkDrvCodecMajorDev);
	if(subDev == TLKDRV_CODEC_SUBDEV_DEF) subDev = sTlkDrvCodecMinorDev;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	ret = pModInf->Open(subDev);
	if(ret == TLK_ENONE){
		gTlkDrvCodecSpkOffset = 0;
		gTlkDrvCodecMicOffset = 0;
		tlkdrv_codec_muteSpkBuff();
	}
	return TLK_ENONE;
}
int tlkdrv_codec_pause(void)
{
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Pause == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Pause(sTlkDrvCodecMinorDev);
}
int tlkdrv_codec_close(void)
{
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close(sTlkDrvCodecMinorDev);
}
int tlkdrv_codec_reset(void)
{
	const tlkdrv_codec_modinf_t *pModInf;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Reset == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Reset(sTlkDrvCodecMinorDev);
}
int tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_ENUM subDev, uint08 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_codec_modinf_t *pModInf;
	if(subDev == TLKDRV_CODEC_SUBDEV_DEF) subDev = sTlkDrvCodecMinorDev;
	pModInf = tlkdrv_codec_getDev(sTlkDrvCodecMajorDev);
	if(pModInf == nullptr || pModInf->Config == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Config(subDev, opcode, param0, param1);
}

bool tlkdrv_codec_paIsOpen(void)
{
	#if (TLKDRV_CODEC_PA_ENABLE)
	return tlkdrv_aw87318_isOpen();
	#else
	return false;
	#endif
}
void tlkdrv_codec_paInit(void)
{
	#if (TLKDRV_CODEC_PA_ENABLE)
	tlkdrv_aw87318_init();
	#endif
}
void tlkdrv_codec_paOpen(void)
{
	#if (TLKDRV_CODEC_PA_ENABLE)
	tlkdrv_aw87318_open();
	#endif
}
void tlkdrv_codec_paClose(void)
{
	#if (TLKDRV_CODEC_PA_ENABLE)
	tlkdrv_aw87318_close();
	#endif
}


int tlkdrv_codec_setMicStatus(bool isMute)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_MUTE, isMute, 0);
}
int tlkdrv_codec_setSpkStatus(bool isMute)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_MUTE, isMute, 0);
}

int tlkdrv_codec_setMicVolume(uint08 volume)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_VOLUME, volume, 0);
}
int tlkdrv_codec_setSpkVolume(uint08 volume)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_VOLUME, volume, 0);
}

int tlkdrv_codec_getMicVolume(void)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_GET_VOLUME, 0, 0);
}
int tlkdrv_codec_getSpkVolume(void)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_GET_VOLUME, 0, 0);
}

int tlkdrv_codec_setChannel(uint08 channel)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_CHANNEL, channel, 0);
}
int tlkdrv_codec_setMicChannel(uint08 channel)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_CHANNEL, channel, 0);
}
int tlkdrv_codec_setSpkChannel(uint08 channel)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_CHANNEL, channel, 0);
}

int tlkdrv_codec_setBitDepth(uint08 bitDepth)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, bitDepth, 0);
}
int tlkdrv_codec_setMicBitDepth(uint08 bitDepth)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, bitDepth, 0);
}
int tlkdrv_codec_setSpkBitDepth(uint08 bitDepth)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH, bitDepth, 0);
}

int tlkdrv_codec_setSampleRate(uint32 sampleRate)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, sampleRate, 0);
}
int tlkdrv_codec_setMicSampleRate(uint32 sampleRate)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_MIC, TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, sampleRate, 0);
}
int tlkdrv_codec_setSpkSampleRate(uint32 sampleRate)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_SPK, TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE, sampleRate, 0);
}
uint32 tlkdrv_codec_getSampleRate(void)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE, 0, 0);
}
uint08 tlkdrv_codec_getChannel(void)
{
	return tlkdrv_codec_config(TLKDRV_CODEC_SUBDEV_DEF, TLKDRV_CODEC_OPCODE_GET_CHANNEL, 0, 0);
}



void tlkdrv_codec_setSpkBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(buffLen != 0 && buffLen < 256) return;
	gTlkDrvCodecSpkBuffLen = buffLen;
	gpTlkDrvCodecSpkBuffer = pBuffer;
}
void tlkdrv_codec_setMicBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(buffLen != 0 && buffLen < 256) return;
	gTlkDrvCodecMicBuffLen = buffLen;
	gpTlkDrvCodecMicBuffer = pBuffer;
}

uint tlkdrv_codec_getSpkOffset(void)
{
	return gTlkDrvCodecSpkOffset;
}
uint tlkdrv_codec_getMicOffset(void)
{
	return gTlkDrvCodecMicOffset;
}
void tlkdrv_codec_setSpkOffset(uint16 offset)
{
	if(offset >= gTlkDrvCodecSpkBuffLen) return;
	gTlkDrvCodecSpkOffset = offset;
}
void tlkdrv_codec_setMicOffset(uint16 offset)
{
	if(offset >= gTlkDrvCodecMicBuffLen) return;
	gTlkDrvCodecMicOffset = offset;
}

uint tlkdrv_codec_getSpkBuffLen(void)
{
	return gTlkDrvCodecSpkBuffLen;
}
uint tlkdrv_codec_getMicBuffLen(void)
{
	return gTlkDrvCodecMicBuffLen;
}


uint tlkdrv_codec_getSpkIdleLen(void)
{
	uint16 unUsed;
	uint32 wptr;
	uint32 rptr;
	
	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return 0;
	
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	
	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = gTlkDrvCodecSpkBuffLen+rptr-wptr;

	if(unUsed+180 >= gTlkDrvCodecSpkBuffLen){
		unUsed -= 180;
		gTlkDrvCodecSpkOffset += 180;
		if(gTlkDrvCodecSpkOffset >= gTlkDrvCodecSpkBuffLen){
			gTlkDrvCodecSpkOffset -= gTlkDrvCodecSpkBuffLen;
		}
//		tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_codec_getSpkIdleLen: seek 180B");
	}
	
	return unUsed;
}
uint tlkdrv_codec_getSpkDataLen(void)
{
	uint16 used;
	uint32 wptr;
	uint32 rptr;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return 0;
		
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	
	if(wptr > rptr) used = wptr-rptr;
	else used = gTlkDrvCodecSpkBuffLen+wptr-rptr;
	
	return used;
}
uint tlkdrv_codec_getMicDataLen(void)
{
	uint16 used;
	uint32 wptr;
	uint32 rptr;

	if(gTlkDrvCodecMicBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_MIC)) return 0;
	
	rptr = gTlkDrvCodecMicOffset;
	wptr = (audio_get_rx_dma_wptr(TLKDRV_CODEC_MIC_DMA))-((uint32)gpTlkDrvCodecMicBuffer);
	
	if(wptr > rptr) used = wptr-rptr;
	else used = gTlkDrvCodecMicBuffLen+wptr-rptr;
	
	return used;
}

bool tlkdrv_codec_readSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset)
{
	uint16 copyLen;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)
		|| offset >= gTlkDrvCodecSpkBuffLen || buffLen > gTlkDrvCodecSpkBuffLen){
		return false;
	}
	if(offset+buffLen <= gTlkDrvCodecSpkBuffLen){
		tmemcpy(pBuffer, ((uint08*)gpTlkDrvCodecSpkBuffer)+offset, buffLen);
	}else{
		copyLen = gTlkDrvCodecSpkBuffLen-offset;
		tmemcpy(pBuffer, ((uint08*)gpTlkDrvCodecSpkBuffer)+offset, copyLen);
		tmemcpy(pBuffer+copyLen, ((uint08*)gpTlkDrvCodecSpkBuffer), buffLen-copyLen);
	}
	return true;
}
bool tlkdrv_codec_readMicData(uint08 *pBuffer, uint16 buffLen, uint16 *pOffset)
{
	uint32 wptr;
	uint32 rptr;
	uint16 dataLen;
	uint16 tempLen;

	if(gTlkDrvCodecMicBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_MIC)) return false;

	rptr = gTlkDrvCodecMicOffset;
	wptr = (uint32)(audio_get_rx_dma_wptr(TLKDRV_CODEC_MIC_DMA) - (uint32)gpTlkDrvCodecMicBuffer);
	if(wptr >= gTlkDrvCodecMicBuffLen){
		wptr = 0;
		return false;
	}
	
	if(wptr >= rptr) dataLen = wptr-rptr;
	else dataLen = gTlkDrvCodecMicBuffLen+wptr-rptr;
	if(dataLen < buffLen) return false;
		
	if(rptr+buffLen <= gTlkDrvCodecMicBuffLen) tempLen = buffLen;
	else tempLen = gTlkDrvCodecMicBuffLen-rptr;
	if(tempLen != 0) tmemcpy(pBuffer, ((uint08*)gpTlkDrvCodecMicBuffer)+rptr, tempLen);
	if(tempLen == buffLen){
		rptr += tempLen;
	}else{
		rptr = buffLen-tempLen;
		tmemcpy(pBuffer+tempLen, ((uint08*)gpTlkDrvCodecMicBuffer), rptr);
	}
	if(pOffset != NULL) *pOffset = dataLen;
	if(rptr >= gTlkDrvCodecMicBuffLen) rptr = 0;
	gTlkDrvCodecMicOffset = rptr;
	return true;
}

void tlkdrv_codec_muteSpkBuff(void)
{
	if(sTlkDrvCodecSpkIsMute) return;
	if(gTlkDrvCodecSpkBuffLen != 0){
		tmemset(gpTlkDrvCodecSpkBuffer, 0, gTlkDrvCodecSpkBuffLen);
	}
	sTlkDrvCodecSpkIsMute = true;
}
void tlkdrv_codec_muteMicBuff(void)
{
	if(gTlkDrvCodecMicBuffLen != 0){
		tmemset(gpTlkDrvCodecMicBuffer, 0, gTlkDrvCodecMicBuffLen);
	}
}

//"dataLen == 0x0000":Fills all idle buffers; "dataLen == 0xFFFF":Fill all buffers.
void tlkdrv_codec_zeroSpkBuff(uint16 zeroLen, bool isInc)
{
	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return;
	if(zeroLen == 0xFFFF){
		tlkdrv_codec_muteSpkBuff();
	}else{
		uint16 woffset;
		uint08 *pBuffer = (uint08*)gpTlkDrvCodecSpkBuffer;
		uint16 tempLen = tlkdrv_codec_getSpkIdleLen();
		if(zeroLen == 0 || zeroLen > tempLen) zeroLen = tempLen;
		if(zeroLen == 0 || zeroLen > gTlkDrvCodecSpkBuffLen) return;
		woffset = gTlkDrvCodecSpkOffset;
		if(woffset+zeroLen <= gTlkDrvCodecSpkBuffLen) tempLen = zeroLen;
		else tempLen = gTlkDrvCodecSpkBuffLen-woffset;
		if(tempLen != 0) tmemset(pBuffer+woffset, 0, tempLen);
		woffset += tempLen;
		zeroLen -= tempLen;
		if(zeroLen != 0){
			woffset = zeroLen;
			tmemset(pBuffer, 0, zeroLen);
		}else if(woffset >= gTlkDrvCodecSpkBuffLen){
			woffset = 0;
		}
		if(isInc){
			gTlkDrvCodecSpkOffset = woffset;
		} 
	}
}

uint tlkdrv_codec_getSpkIdleLen0(void)
{
	uint32 wptr;
	uint32 rptr;
	uint16 unUsed;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return 0;
	
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	
	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = gTlkDrvCodecSpkBuffLen+rptr-wptr;

	return unUsed;
}

bool tlkdrv_codec_fillSpkBuff(uint08 *pData, uint16 dataLen)
{
	uint32 wptr;
	uint32 rptr;
	uint16 unUsed;
	uint16 offset;
	uint08 *pBuffer = (uint08*)gpTlkDrvCodecSpkBuffer;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return false;
	if(pData == NULL || dataLen == 0 || (dataLen & 0x01) != 0) return false;
	
	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "=== spkFillBuff");
	wptr = gTlkDrvCodecSpkOffset;
	rptr = (audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer);
	
	if(rptr > wptr) unUsed = rptr-wptr;
	else unUsed = gTlkDrvCodecSpkBuffLen+rptr-wptr;

	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "fill1: %d %d %d %d", rptr, wptr, unUsed, dataLen);

	if(unUsed <= dataLen){
		tlkapi_warn(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "fill2: %d %d %d %d", rptr, wptr, unUsed, dataLen);
		return false;
	}

	if(sTlkDrvCodecSpkIsMute) sTlkDrvCodecSpkIsMute = false;
	
	
	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "fill data: ok");

	if(unUsed == gTlkDrvCodecSpkBuffLen){
		wptr = rptr+32;
	}
	if(wptr >= gTlkDrvCodecSpkBuffLen){
		wptr = 0;
	}

	if(wptr+dataLen > gTlkDrvCodecSpkBuffLen){
		offset = gTlkDrvCodecSpkBuffLen-wptr;
	}else{
		offset = dataLen;
	}
	memcpy(pBuffer+wptr, pData, offset);
	if(offset < dataLen){
		memcpy(pBuffer, pData+offset, dataLen-offset);
	}
	
	wptr += dataLen;
	if(wptr >= gTlkDrvCodecSpkBuffLen){
		wptr -= gTlkDrvCodecSpkBuffLen;
	}
	gTlkDrvCodecSpkOffset = wptr;
		
	return true;
}

bool tlkdrv_codec_backReadSpkData(uint08 *pBuffer, uint16 buffLen, uint16 offset, bool isBack)
{
	uint32 rptr;
	uint16 tempLen;

	if(gTlkDrvCodecSpkBuffLen == 0 || !tlkdrv_codec_isOpen(TLKDRV_CODEC_SUBDEV_SPK)) return false;
	if(buffLen == 0 || buffLen > gTlkDrvCodecSpkBuffLen || offset >= gTlkDrvCodecSpkBuffLen){
		tlkapi_error(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_codec_backReadSpkData: fault", 0, 0);
		return false;
	}
	rptr = (uint32)((audio_get_tx_dma_rptr(TLKDRV_CODEC_SPK_DMA))-((uint32)gpTlkDrvCodecSpkBuffer));
	if(offset != 0){
		if(!isBack){
			rptr += offset;
			if(rptr >= gTlkDrvCodecSpkBuffLen) rptr -= gTlkDrvCodecSpkBuffLen;
		}else{
			if(rptr >= offset) rptr -= offset;
			else rptr = gTlkDrvCodecSpkBuffLen+rptr-offset;
		}
	}
	if(rptr >= gTlkDrvCodecSpkBuffLen) rptr = 0;
			
	if(rptr+buffLen <= gTlkDrvCodecSpkBuffLen) tempLen = buffLen;
	else tempLen = gTlkDrvCodecSpkBuffLen-rptr;
	if(tempLen != 0) tmemcpy(pBuffer, ((uint08*)(gpTlkDrvCodecSpkBuffer))+rptr, tempLen);
	if(tempLen < buffLen) tmemcpy(pBuffer+tempLen, (uint08*)gpTlkDrvCodecSpkBuffer, buffLen-tempLen);

//	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "backRead: %d %d %d %d", offset, rptr, gTlkDrvCodecSpkOffset, tempLen);

	return true;
}




static const tlkdrv_codec_modinf_t *tlkdrv_codec_getDev(uint08 dev)
{
	if(dev >= TLKDRV_CODEC_DEV_MAX) return nullptr;
	return spTlkDrvCodecModinf[dev];
}


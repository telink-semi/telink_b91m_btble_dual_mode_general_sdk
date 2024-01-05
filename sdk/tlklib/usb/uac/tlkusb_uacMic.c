/********************************************************************************************************
 * @file	tlkusb_uacMic.c
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
#include "tlklib/usb/tlkusb_stdio.h"
#include "tlklib/usb/uac/tlkusb_uacDefine.h"
#if (TLKUSB_UAC_MIC_ENABLE)
#include "tlklib/usb/uac/tlkusb_uacDesc.h"
#include "tlklib/usb/uac/tlkusb_uac.h"
#include "tlklib/usb/uac/tlkusb_uacSpk.h"
#include "tlklib/usb/uac/tlkusb_uacMic.h"

#include "string.h"
#include "drivers.h"


static uint08 tlkusb_uacmic_readData(uint08 *pBuffer, uint08 readLen);


static uint08 sTlkUsbUacMicEnable = false;
static uint08 sTlkUsbUacMicAutoZero = false;
static uint08 sTlkUsbUacMicUnitLens = (TLKUSB_AUD_MIC_SAMPLE_RATE / 1000) * 2;
static uint08 sTlkUsbUacMicBuffer[TLKUSB_AUD_MIC_BUFFER_SIZE+4];
static tlkusb_uacmic_ctrl_t sTlkUsbUacMicCtrl;
static TlkUsbUacStatusChangeCB sTlkUsbUacMicStatusCB;

int tlkusb_uacmic_init(void)
{
	sTlkUsbUacMicStatusCB = nullptr;
	tmemset(&sTlkUsbUacMicCtrl, 0, sizeof(tlkusb_uacmic_ctrl_t));
	
	sTlkUsbUacMicEnable = false;
	sTlkUsbUacMicCtrl.sampleRate = TLKUSB_AUD_MIC_SAMPLE_RATE;
	sTlkUsbUacMicCtrl.curVol = TLKUSB_AUDMIC_VOL_DEF;
	tlkapi_fifo_init(&sTlkUsbUacMicCtrl.fifo, false, false, sTlkUsbUacMicBuffer, TLKUSB_AUD_MIC_BUFFER_SIZE+4);
		
	return TLK_ENONE;
}

void tlkusb_uacmic_regStatusChangeCB(TlkUsbUacStatusChangeCB cb)
{
	sTlkUsbUacMicStatusCB = cb;
}


bool tlkusb_uacmic_getEnable(void)
{
	return sTlkUsbUacMicEnable;
}
void tlkusb_uacmic_setEnable(bool enable)
{
	sTlkUsbUacMicEnable = enable;
	if(enable){
		reg_usb_ep_ptr(TLKUSB_UAC_EDP_MIC) = 0;
		reg_usb_ep_ctrl(TLKUSB_UAC_EDP_MIC) = 0x01; //ACK first packet
		if(sTlkUsbUacMicStatusCB != nullptr){
			sTlkUsbUacMicStatusCB(true, TLKUSB_AUD_MIC_CHANNEL_COUNT, TLKUSB_AUD_MIC_RESOLUTION_BIT, TLKUSB_AUD_MIC_SAMPLE_RATE);
		}
	}else{
		if(sTlkUsbUacMicStatusCB != nullptr){
			sTlkUsbUacMicStatusCB(false, 0, 0, 0);
		}
		tlkapi_fifo_clear(&sTlkUsbUacMicCtrl.fifo);
	}
}

uint tlkusb_uacmic_getVolume(void)
{
	return (sTlkUsbUacMicCtrl.mute << 7) | (sTlkUsbUacMicCtrl.volStep & 0x7f);
}
void tlkusb_uacmic_setVolume(sint16 volume)
{
	sTlkUsbUacMicCtrl.curVol = volume;
	if(volume < TLKUSB_AUDMIC_VOL_MIN){
		sTlkUsbUacMicCtrl.volStep = 0;
	}else{
		sTlkUsbUacMicCtrl.volStep = (volume - TLKUSB_AUDMIC_VOL_MIN) / TLKUSB_AUDMIC_VOL_RES;
	}
}
void tlkusb_uacmic_enterMute(bool enable)
{
	sTlkUsbUacMicCtrl.mute = enable;
}
		

int tlkusb_uacmic_setInfCmdDeal(int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		sTlkUsbUacMicCtrl.mute = usbhw_read_ctrl_ep_data();
	}else if(type == AUDIO_FEATURE_VOLUME){
		u16 val = usbhw_read_ctrl_ep_u16();
		tlkusb_uacmic_setVolume(val);
	}else{
		
	}
	return TLK_ENONE;
}

int tlkusb_uacmic_getInfCmdDeal(int req, int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		usbhw_write_ctrl_ep_data(sTlkUsbUacMicCtrl.mute);
	}else if(type == AUDIO_FEATURE_VOLUME){
		switch (req) {
			case AUDIO_REQ_GetCurrent:
				usbhw_write_ctrl_ep_u16(sTlkUsbUacMicCtrl.curVol);
				break;
			case AUDIO_REQ_GetMinimum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDMIC_VOL_MIN);
				break;
			case AUDIO_REQ_GetMaximum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDMIC_VOL_MAX);
				break;
			case AUDIO_REQ_GetResolution:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDMIC_VOL_RES);
				break;
			default:
				return -TLK_ENOSUPPORT;
		}
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}

int tlkusb_uacmic_setEdpCmdDeal(int type)
{
	if(type == AUDIO_EPCONTROL_SamplingFreq){
		uint32 value = usbhw_read_ctrl_ep_data();
		sTlkUsbUacMicCtrl.sampleRate = value;
		value = usbhw_read_ctrl_ep_data();
		sTlkUsbUacMicCtrl.sampleRate |= value << 8;
		value = usbhw_read_ctrl_ep_data();
		sTlkUsbUacMicCtrl.sampleRate |= value << 16;
		// TODO: Sample Rate Changed
	}
	return TLK_ENONE;
}

void tlkusb_uacmic_autoZero(bool enable)
{
	sTlkUsbUacMicAutoZero = enable;
}
bool tlkusb_uacmic_sendData(uint08 *pData, uint16 dataLen, bool isCover)
{
	int ret;
	if(!sTlkUsbUacMicEnable) return false;
	TLKAPI_FIFO_SET_COVER(&sTlkUsbUacMicCtrl.fifo, isCover);
	ret = tlkapi_fifo_write(&sTlkUsbUacMicCtrl.fifo, pData, dataLen);
	if(ret <= 0) return false;
	return true;
}


_attribute_ram_code_sec_noinline_
void tlkusb_uacmic_fillData(void)
{
	uint08 buffLen;
	uint08 buffer[64];

	usbhw_clr_eps_irq(BIT(TLKUSB_UAC_EDP_MIC));
	reg_usb_ep_ptr(TLKUSB_UAC_EDP_MIC) = 0;
	
	buffLen = 0;
	if(sTlkUsbUacMicEnable){
		uint08 readLen = 64;
		if(readLen > sTlkUsbUacMicUnitLens) readLen = sTlkUsbUacMicUnitLens;
		buffLen = tlkusb_uacmic_readData(buffer, readLen);
	}
	if(buffLen != 0){
		uint08 index;
		for(index=0; index<buffLen; index++){
			reg_usb_ep_dat(TLKUSB_UAC_EDP_MIC) = buffer[index];
		}
		reg_usb_ep_ctrl(TLKUSB_UAC_EDP_MIC) = BIT(0);
	}else if(sTlkUsbUacMicAutoZero){
		uint08 index;
		for(index=0; index<sTlkUsbUacMicUnitLens; index++){
			reg_usb_ep_dat(TLKUSB_UAC_EDP_MIC) = 0x00;
		}
		reg_usb_ep_ctrl(TLKUSB_UAC_EDP_MIC) = BIT(0);
	}
}

_attribute_ram_code_sec_noinline_
static uint08 tlkusb_uacmic_readData(uint08 *pBuffer, uint08 readLen)
{
	uint16 used;
	uint16 tempLen;
	uint16 woffset;
	uint16 roffset;
	tlkapi_fifo_t *pFifo;

	pFifo = &sTlkUsbUacMicCtrl.fifo;
	
	if(pFifo->buffLen == 0) return 0;
	woffset = pFifo->woffset;
	roffset = pFifo->roffset;
	if(woffset >= roffset) used = woffset-roffset;
	else used = pFifo->buffLen+woffset-roffset;
	if(used == 0) return 0;
	if(readLen > used) readLen = used;
	
	if(roffset+readLen <= pFifo->buffLen) tempLen = readLen;
	else tempLen = pFifo->buffLen-roffset;
	tmemcpy(pBuffer, pFifo->pBuffer+roffset, tempLen);
	if(tempLen < readLen){
		tmemcpy(pBuffer+tempLen, pFifo->pBuffer, readLen-tempLen);
	}
	roffset += readLen;
	if(roffset >= pFifo->buffLen) roffset -= pFifo->buffLen;
	pFifo->roffset = roffset;
	return readLen;
}


#endif //#if (TLKUSB_UAC_MIC_ENABLE)


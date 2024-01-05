/********************************************************************************************************
 * @file	tlkusb_uacSpk.c
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
#if (TLKUSB_UAC_SPK_ENABLE)
#include "tlklib/usb/uac/tlkusb_uacDesc.h"
#include "tlklib/usb/uac/tlkusb_uac.h"
#include "tlklib/usb/uac/tlkusb_uacSpk.h"
#include "tlklib/usb/uac/tlkusb_uacMic.h"

#include "drivers.h"



static void tlkusb_uacspk_writeData(uint08 *pData, uint08 dataLen);


static uint08 sTlkUsbAudSpkEnable = 0;
static tlkusb_uacspk_ctrl_t sTlkUsbAudSpkCtrl;
static uint08 sTlkUsbAudSpkBuffer[TLKUSB_AUD_SPK_BUFFER_SIZE+4];
static TlkUsbUacStatusChangeCB sTlkUsbUacSpkStatusCB;


int tlkusb_uacspk_init(void)
{
	sTlkUsbUacSpkStatusCB = nullptr;
	tmemset(&sTlkUsbAudSpkCtrl, 0, sizeof(tlkusb_uacspk_ctrl_t));

	sTlkUsbAudSpkEnable = false;
	sTlkUsbAudSpkCtrl.sampleRate = TLKUSB_AUD_SPK_SAMPLE_RATE;
	tlkusb_uacspk_setVolume(TLKUSB_AUDSPK_VOL_MAX);
	tlkapi_fifo_init(&sTlkUsbAudSpkCtrl.fifo, false, false, sTlkUsbAudSpkBuffer, TLKUSB_AUD_SPK_BUFFER_SIZE+4);
	
	return TLK_ENONE;
}

void tlkusb_uacspk_regStatusChangeCB(TlkUsbUacStatusChangeCB cb)
{
	sTlkUsbUacSpkStatusCB = cb;
}

bool tlkusb_uacspk_getEnable(void)
{
	return sTlkUsbAudSpkEnable;
}
void tlkusb_uacspk_setEnable(bool enable)
{
	sTlkUsbAudSpkEnable = enable;
	if(enable){
		reg_usb_ep_ptr(TLKUSB_UAC_EDP_SPK) = 0;
		reg_usb_ep_ctrl(TLKUSB_UAC_EDP_SPK) = 0x01; //ACK first packet
		if(sTlkUsbUacSpkStatusCB != nullptr){
			sTlkUsbUacSpkStatusCB(true, TLKUSB_AUD_SPK_CHANNEL_COUNT, TLKUSB_AUD_SPK_RESOLUTION_BIT, TLKUSB_AUD_SPK_SAMPLE_RATE);
		}
	}else{
		if(sTlkUsbUacSpkStatusCB != nullptr){
			sTlkUsbUacSpkStatusCB(false, 0, 0, 0);
		}
		tlkapi_fifo_clear(&sTlkUsbAudSpkCtrl.fifo);
	}
}

uint tlkusb_uacspk_getVolume(void)
{
	return (sTlkUsbAudSpkCtrl.mute << 7) | (sTlkUsbAudSpkCtrl.volStep & 0x7f);
}
void tlkusb_uacspk_setVolume(uint16 volume)
{
	sTlkUsbAudSpkCtrl.curVol = volume;
	if(volume < TLKUSB_AUDSPK_VOL_MIN){
		sTlkUsbAudSpkCtrl.volStep = 0;
	}else{
		sTlkUsbAudSpkCtrl.volStep = (volume - TLKUSB_AUDSPK_VOL_MIN) / TLKUSB_AUDSPK_VOL_RES;
	}
}
void tlkusb_uacspk_enterMute(bool enable)
{
	sTlkUsbAudSpkCtrl.mute = enable;
}

void tlkusb_uacspk_autoFlush(bool enable)
{
	TLKAPI_FIFO_SET_COVER(&sTlkUsbAudSpkCtrl.fifo, enable);
}



int tlkusb_uacspk_setInfCmdDeal(int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		sTlkUsbAudSpkCtrl.mute = usbhw_read_ctrl_ep_data();
	}else if(type == AUDIO_FEATURE_VOLUME){
		uint16 val = usbhw_read_ctrl_ep_u16();
		tlkusb_uacspk_setVolume(val);
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}
int tlkusb_uacspk_getInfCmdDeal(int req, int type)
{
	if(type == AUDIO_FEATURE_MUTE){
		usbhw_write_ctrl_ep_data(sTlkUsbAudSpkCtrl.mute);
	}else if(type == AUDIO_FEATURE_VOLUME){
		switch(req){
			case AUDIO_REQ_GetCurrent:
				usbhw_write_ctrl_ep_u16(sTlkUsbAudSpkCtrl.curVol);
				break;
			case AUDIO_REQ_GetMinimum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDSPK_VOL_MIN);
				break;
			case AUDIO_REQ_GetMaximum:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDSPK_VOL_MAX);
				break;
			case AUDIO_REQ_GetResolution:
				usbhw_write_ctrl_ep_u16(TLKUSB_AUDSPK_VOL_RES);
				break;
			default:
				return -TLK_ENOSUPPORT;
		}
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}

int tlkusb_uacspk_setEdpCmdDeal(int type)
{
	if(type == AUDIO_EPCONTROL_SamplingFreq){
		uint32 value = usbhw_read_ctrl_ep_data();
		sTlkUsbAudSpkCtrl.sampleRate = value;
		value = usbhw_read_ctrl_ep_data();
		sTlkUsbAudSpkCtrl.sampleRate |= value << 8;
		value = usbhw_read_ctrl_ep_data();
		sTlkUsbAudSpkCtrl.sampleRate |= value << 16;
		// TODO: Sample Rate Changed
	}
	return TLK_ENONE;
}

uint tlkusb_uacspk_readData(uint08 *pBuff, uint16 buffLen, bool isParty)
{
	uint16 dataLen = tlkapi_fifo_dataLen(&sTlkUsbAudSpkCtrl.fifo);
	if(dataLen == 0 || (!isParty && dataLen < buffLen)) return 0;
	if(buffLen > dataLen) buffLen = dataLen;
	tlkapi_fifo_read(&sTlkUsbAudSpkCtrl.fifo, pBuff, buffLen);
	return buffLen;
}

_attribute_ram_code_sec_noinline_
void tlkusb_uacspk_recvData(void)
{
	uint08 index;
	uint08 length;
	uint08 buffer[64];
	usbhw_clr_eps_irq(BIT(TLKUSB_UAC_EDP_SPK));
	length = reg_usb_ep_ptr(TLKUSB_UAC_EDP_SPK);
	reg_usb_ep_ptr(TLKUSB_UAC_EDP_SPK) = 0;
	if(length > 64) length = 64;
	for(index=0; index<length; index++){
		buffer[index] = reg_usb_ep_dat(TLKUSB_UAC_EDP_SPK);
	}
	tlkusb_uacspk_writeData(buffer, length);
	reg_usb_ep_ctrl(TLKUSB_UAC_EDP_SPK) = FLD_EP_DAT_ACK;
}


_attribute_ram_code_sec_noinline_
static void tlkusb_uacspk_writeData(uint08 *pData, uint08 dataLen)
{
	uint16 unUsed;
	uint16 tempLen;
	uint16 woffset;
	uint16 roffset;
	tlkapi_fifo_t *pFifo;

	if(dataLen == 0) return;
	pFifo = &sTlkUsbAudSpkCtrl.fifo;
	if(pFifo->buffLen == 0) return; //Not Ready, skip
	woffset = pFifo->woffset;
	roffset = pFifo->roffset;
	if(roffset > woffset) unUsed = roffset-woffset;
	else unUsed = pFifo->buffLen+roffset-woffset;
	if(unUsed <= dataLen){
		if(pFifo->isCover){
			roffset += dataLen-unUsed+1;
			if(roffset >= pFifo->buffLen) roffset -= pFifo->buffLen;
			unUsed = dataLen+1;
		}else{
			return;
		}
	}
	
	if(woffset+dataLen <= pFifo->buffLen) tempLen = dataLen;
	else tempLen = pFifo->buffLen-woffset;
	tmemcpy(pFifo->pBuffer+woffset, pData, tempLen);
	if(tempLen < dataLen){
		tmemcpy(pFifo->pBuffer, pData+tempLen, dataLen-tempLen);
	}
	
	woffset += dataLen;
	if(woffset >= pFifo->buffLen) woffset -= pFifo->buffLen;
	pFifo->woffset = woffset;
}



#endif //#if (TLKUSB_UAC_SPK_ENABLE)


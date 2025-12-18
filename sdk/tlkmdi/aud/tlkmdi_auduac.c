/********************************************************************************************************
 * @file	tlkmdi_auduac.c
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
#if (TLK_MDI_AUDUAC_ENABLE)
#include "drivers.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmdi/aud/tlkmdi_auduac.h"

#include "tlkdev/sys/tlkdev_codec.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlklib/usb/tlkusb_stdio.h"
#include "tlklib/usb/uac/tlkusb_uac.h"
#if (TLK_ALG_EQ_ENABLE)
#include "tlkalg/audio/eq/tlkalg_eq.h"
#endif



#define TLKMDI_AUDUAC_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_UAC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDUAC_DBG_SIGN       "[MDI]"


static void tlkmdi_auduac_spkStatusChangeEvt(bool enable, uint08 channel, uint08 bitDepth, uint32 sampleRate);
static void tlkmdi_auduac_micStatusChangeEvt(bool enable, uint08 channel, uint08 bitDepth, uint32 sampleRate);

static void tlkmdi_uac_spkHandler(void);
static void tlkmdi_uac_micHandler(void);



typedef struct{
	uint08 enable;
	uint08 spkEn;
	uint08 micEn;
	uint16 handle;
	uint08 spkChannel;
	uint08 micChannel;
	uint08 spkBitDepth;
	uint08 micBitDepth;
	uint32 spkSampleRate;
	uint32 micSampleRate;
}tlkmdi_auduac_ctrl_t;

static tlkmdi_auduac_ctrl_t sTlkMdiAudUacCtrl = {0};



/******************************************************************************
 * Function: tlkmdi_auduac_init
 * Descript: Initial the A2DP sink block, register the data callback. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_auduac_init(void)
{	
	memset(&sTlkMdiAudUacCtrl, 0, sizeof(tlkmdi_auduac_ctrl_t));
	tlkusb_uacspk_regStatusChangeCB(tlkmdi_auduac_spkStatusChangeEvt);
	tlkusb_uacmic_regStatusChangeCB(tlkmdi_auduac_micStatusChangeEvt);
	
	
	return TLK_ENONE;
}

static void tlkmdi_auduac_spkStatusChangeEvt(bool enable, uint08 channel, uint08 bitDepth, uint32 sampleRate)
{
	sTlkMdiAudUacCtrl.spkEn = enable;
	if(enable){
		if(channel == 2) sTlkMdiAudUacCtrl.spkChannel = 0x03;
		else sTlkMdiAudUacCtrl.spkChannel = 1;
		sTlkMdiAudUacCtrl.spkBitDepth = bitDepth;
		sTlkMdiAudUacCtrl.spkSampleRate = sampleRate;
		if(!sTlkMdiAudUacCtrl.enable) tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_UAC, 0xFFFF);
	}else{
		if(!sTlkMdiAudUacCtrl.micEn && sTlkMdiAudUacCtrl.enable){
			tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_UAC, 0xFFFF);
		}
	}
}
static void tlkmdi_auduac_micStatusChangeEvt(bool enable, uint08 channel, uint08 bitDepth, uint32 sampleRate)
{
	sTlkMdiAudUacCtrl.micEn = enable;
	if(enable){
		if(channel == 2) sTlkMdiAudUacCtrl.micChannel = 0x03;
		else sTlkMdiAudUacCtrl.micChannel = 1;
		sTlkMdiAudUacCtrl.micBitDepth = bitDepth;
		sTlkMdiAudUacCtrl.micSampleRate = sampleRate;
		if(!sTlkMdiAudUacCtrl.enable) tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_UAC, 0xFFFF);
	}else{
		if(!sTlkMdiAudUacCtrl.spkEn && sTlkMdiAudUacCtrl.enable){
			tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_UAC, 0xFFFF);
		}
	}
}


int tlkmdi_auduac_start(uint16 handle, uint32 param)
{
	if(sTlkMdiAudUacCtrl.enable) return -TLK_EREPEAT;	
	return TLK_ENONE;
}
int tlkmdi_auduac_close(uint16 handle)
{
	if(!sTlkMdiAudUacCtrl.enable) return TLK_ENONE;
	
	tlkapi_trace(TLKMDI_AUDUAC_DBG_FLAG, TLKMDI_AUDUAC_DBG_SIGN, "tlkmdi_auduac_close ..");
	return TLK_ENONE;
}

void tlkmdi_auduac_timer(void)
{

}


/******************************************************************************
 * Function: tlkmdi_auduac_switch
 * Descript: Change A2DP SNK status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_auduac_switch(uint16 handle, uint08 status)
{
	bool enable;

	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	if(enable && sTlkMdiAudUacCtrl.enable){
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_close();
		tlkdev_codec_muteSpkBuff();
		#endif
		sTlkMdiAudUacCtrl.enable = false;
	}
	
    if(status == TLK_STATE_CLOSED){
        tlkmdi_auduac_close(handle);
	}
	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_muteSpkBuff();
	#endif
	
	sTlkMdiAudUacCtrl.handle = handle;
	sTlkMdiAudUacCtrl.enable = enable;
	
	if(enable && (sTlkMdiAudUacCtrl.micEn || sTlkMdiAudUacCtrl.spkEn)){
		uint08 subdev;
		if(sTlkMdiAudUacCtrl.micEn && sTlkMdiAudUacCtrl.spkEn) subdev = TLKDEV_CODEC_SUBDEV_BOTH;
		else if(sTlkMdiAudUacCtrl.micEn) subdev = TLKDEV_CODEC_SUBDEV_MIC;
		else subdev = TLKDEV_CODEC_SUBDEV_SPK;
		tlkdev_codec_extOpen(subdev, sTlkMdiAudUacCtrl.spkChannel, sTlkMdiAudUacCtrl.spkBitDepth, sTlkMdiAudUacCtrl.spkSampleRate,
			sTlkMdiAudUacCtrl.micChannel, sTlkMdiAudUacCtrl.micBitDepth, sTlkMdiAudUacCtrl.micSampleRate);
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_UAC, TLK_STATE_OPENED);
		tlkdev_codec_setSpkOffset(160);
	}else{
		tlkdev_codec_close();
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_UAC, TLK_STATE_CLOSED);
	}
	
	tlkapi_trace(TLKMDI_AUDUAC_DBG_FLAG, TLKMDI_AUDUAC_DBG_SIGN, "tlkmdi_auduac_switch: %d %d", handle, status);
	
	return true;
}

/******************************************************************************
 * Function: tlkmdi_auduac_isBusy
 * Descript: Is A2DP sink block enable. 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_auduac_isBusy(void)
{
	return sTlkMdiAudUacCtrl.enable;
}

/******************************************************************************
 * Function: tlkmdi_auduac_intval
 * Descript: Get A2DP sink Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_auduac_intval(void)
{
	return 1000;
}

/******************************************************************************
 * Function: tlkmdi_auduac_irqProc
 * Descript: Call A2DP SNK spk handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_auduac_irqProc(void)
{
//	tlkapi_trace(TLKMDI_AUDUAC_DBG_FLAG, TLKMDI_AUDUAC_DBG_SIGN, "tlkmdi_auduac_irqProc:");
	if(!sTlkMdiAudUacCtrl.enable) return false;
	if(sTlkMdiAudUacCtrl.spkEn) tlkmdi_uac_spkHandler();
	if(sTlkMdiAudUacCtrl.micEn) tlkmdi_uac_micHandler();
	if(sTlkMdiAudUacCtrl.enable) return true;
	else return false;
}



static void tlkmdi_uac_spkHandler(void)
{
	uint16 length;
	uint16 volume;
	sint16 *pData;
	uint08 buffer[256];

	#if (TLK_DEV_CODEC_ENABLE)
	while(true){
		length = 0;
		if(tlkdev_codec_getSpkIdleLen() > 128){
			length = tlkusb_uacspk_readData(buffer, 256, true);
		}
  		if(length == 0) break;
		
		#if (TLK_ALG_EQ_ENABLE)
		if(length == 128){
			tlkalg_eq_procss(TLKALG_EQ_TYPE_MUSIC, TLKALG_EQ_CHANNEL_STEREO, sTlkMdiAudUacCtrl.spkSampleRate, (sint16*)buffer, 32);
		}else{
			tlkalg_eq_procss(TLKALG_EQ_TYPE_MUSIC, TLKALG_EQ_CHANNEL_STEREO, sTlkMdiAudUacCtrl.spkSampleRate, (sint16*)buffer, 16);
		}
		#endif

		volume = tlkusb_uacspk_getVolume();
		//tlkapi_trace(TLKMDI_AUDUAC_DBG_FLAG, TLKMDI_AUDUAC_DBG_SIGN, "tlkmdi_uac_spkHandler: volume[%d]", volume);
		pData = (sint16*)buffer;
		for(uint index=0; index<length/2; index++){
			pData[index] = (pData[index]*volume) >> TLKMDI_AUDIO_VOLUME_EXPAND;
		}
		
		tlkdev_codec_fillSpkBuff(buffer, length);
	}
	#endif
}
static void tlkmdi_uac_micHandler(void)
{	
	uint08 buffer[96];
	if(!tlkdev_codec_readMicData(buffer, 96, nullptr)){
		return;
	}
	tlkusb_uacmic_sendData(buffer, 96, false);
}






#endif //#if (TLK_MDI_AUDUAC_ENABLE)


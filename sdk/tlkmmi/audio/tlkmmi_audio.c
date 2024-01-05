/********************************************************************************************************
 * @file	tlkmmi_audio.c
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
#if (TLKMMI_AUDIO_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkdev/sys/tlkdev_codec.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkmmi_audio.h"
#include "tlkmmi_audioAdapt.h"
#include "tlkmmi_audioCtrl.h"
#include "tlkmmi_audioInfo.h"
#include "tlkmmi_audioSch.h"
#include "tlkmmi_audioModinf.h"
#include "tlkmmi_audioMsgInner.h"
#include "tlkmmi_audioMsgOuter.h"
#include "drivers.h"
#if (TLK_DEV_CODEC_ENABLE)
#include "tlkdev/tlkdev.h"
#endif


static bool tlkmmi_audio_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static bool tlkmmi_audio_irqTimer(tlkapi_timer_t *pTimer, uint32 userArg);
#if (TLK_ALG_EQ_ENABLE)
void tlkalg_eq_loadParam(uint32 addr, uint16 index, uint08 flag);
#endif


uint16 gTlkMmiAudioCurHandle;
uint08 gTlkMmiAudioCurOptype;
uint08 gTlkMmiAudioTmrState = 0;
uint08 gTlkMmiAudioTmrCount = 0;
tlkapi_timer_t gTlkMmiAudioCurTimer;
tlkapi_timer_t gTlkMmiAudioIrqTimer;
static uint08 sTlkMmiAudioCodecIdleTmrCount = 0;



TLKSYS_MMI_TASK_DEFINE(audio, Audio);


static int tlkmmi_audio_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];
		
	#if (TLK_ALG_EQ_ENABLE)
	unsigned int eqAddr;
	eqAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_EQ_TEST_ADDR);
	if(eqAddr != 0){
		tlkalg_eq_loadParam(eqAddr, 0xffff, 0);
	}
	#endif
	
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_AUDIO;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);
	
	#if (TLK_MDI_AUDIO_ENABLE)
	tlkmdi_audio_init();
	#endif
	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendBusyCheckCB(tlkmmi_audio_isBusy, "tlkmmi_audio");
	#endif
	
	tlkmmi_audio_adaptInit(procID);
	tlkmmi_audio_infoInit();
	tlkmmi_audio_ctrlInit();
	tlkmmi_audio_schInit();
	tlkmmi_audio_adaptInitTimer(&gTlkMmiAudioCurTimer, tlkmmi_audio_timer, NULL, TLKMMI_AUDIO_TIMEOUT);
	tlkapi_timer_initNode(&gTlkMmiAudioIrqTimer, tlkmmi_audio_irqTimer, NULL, TLKMMI_AUDIO_TIMEOUT);
	gTlkMmiAudioCurOptype = TLKPTI_AUD_OPTYPE_NONE;

	return TLK_ENONE;
}
static int tlkmmi_audio_start(void)
{
	uint08 volume;

	#if (TLK_MDI_MP3_ENABLE)
	tlkmdi_mp3_init();
	#endif
	volume = tlkmdi_audio_getVoiceBtpVolume();
	tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_HFP_VOLUME, &volume, 1);
	volume = tlkmdi_audio_getMusicBtpVolume(false);
	tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_AVRCP_DEF_VOL, &volume, 1);
	return TLK_ENONE;
}
static int tlkmmi_audio_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_audio_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_audio_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_audio_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_audio_outerMsgHandler(msgID, pData, dataLen);
	}
}
static void tlkmmi_audio_handler(void)
{
	
}




/******************************************************************************
 * Function: tlkmmi_audio_isBusy
 * Descript:
 * Params:
 * Return: true/false
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isBusy(void)
{
	if(gTlkMmiAudioCurOptype != 0 || gTlkMmiAudioTmrState != 0 || sTlkMmiAudioCodecIdleTmrCount != 0) return true;
	return false;
}


/******************************************************************************
 * Function: tlkmmi_audio_disconn
 * Descript:
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_disconn(uint16 handle)
{
	tlkmmi_audio_removeItemByHandle(handle);
}

void tlkmmi_audio_startHandler(void)
{
	tlkapi_timer_updateNode(&gTlkMmiAudioIrqTimer, 1000, true);
	gTlkMmiAudioTmrState = 2;
	gTlkMmiAudioTmrCount = TLKMMI_AUDIO_TIMER_TIMEOUT;
	sTlkMmiAudioCodecIdleTmrCount = TLKMMI_AUDIO_CODEC_IDLE_TIMEOUT;
	tlksys_switch_clock(SYS_CHIP_CLOCK_96M);
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY || gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC){
		if(gTlkMmiAudioCtrl.report.enable) tlkmmi_audio_adaptInsertTimer(&gTlkMmiAudioCtrl.timer);
	}
	tlkmmi_audio_adaptInsertTimer(&gTlkMmiAudioCurTimer);
}
void tlkmmi_audio_closeHandler(void)
{
	uint08 optype = gTlkMmiAudioCurOptype;
	uint16 handle = gTlkMmiAudioCurHandle;
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: status over");
	tlksys_switch_clock(SYS_CHIP_CLOCK_48M);
	gTlkMmiAudioCurOptype = TLKPTI_AUD_OPTYPE_NONE;
	gTlkMmiAudioCurHandle = 0;
	sTlkMmiAudioCodecIdleTmrCount = 0;
	tlkapi_timer_removeNode(&gTlkMmiAudioIrqTimer);
	gTlkMmiAudioTmrState = 0;
	tlkmmi_audio_removeItem(handle, optype);
	tlkmmi_audio_adaptRemoveTimer(&gTlkMmiAudioCurTimer);
}


static bool tlkmmi_audio_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_timer 01: {optype-%d,handle-%d,count-%d,state-%d}", 
//		gTlkMmiAudioCurOptype, gTlkMmiAudioCurHandle, gTlkMmiAudioTmrCount, gTlkMmiAudioTmrState);
	if(gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_NONE){
		tlkmmi_audio_modinfTimer(gTlkMmiAudioCurOptype);
		if(gTlkMmiAudioTmrCount != 0) gTlkMmiAudioTmrCount--;
		if(!tlkmmi_audio_modinfIsBusy(gTlkMmiAudioCurOptype)){
			if(sTlkMmiAudioCodecIdleTmrCount != 0){
				sTlkMmiAudioCodecIdleTmrCount--;
			}else{
				tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler close: optype = %d", gTlkMmiAudioCurOptype);
				tlkmmi_audio_closeHandler();
			}
		}else if(gTlkMmiAudioTmrState == 0){
			tlkapi_info(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: start timer");
			tlkmmi_audio_startHandler();
		}else if(gTlkMmiAudioTmrCount == 0){
			tlkapi_fatal(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: timer fault");
			tlkapi_timer_updateNode(&gTlkMmiAudioIrqTimer, 3000, true);
			gTlkMmiAudioTmrCount = TLKMMI_AUDIO_TIMER_TIMEOUT;
		}
	}
	else if(gTlkMmiAudioTmrState != 0){
		tlkapi_info(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_handler: close timer");
		if(gTlkMmiAudioTmrState == 2){
			gTlkMmiAudioCurOptype = TLKPTI_AUD_OPTYPE_NONE;
			gTlkMmiAudioCurHandle = 0;
			tlkapi_timer_removeNode(&gTlkMmiAudioIrqTimer);
		}
		gTlkMmiAudioTmrState = 0;
		sTlkMmiAudioCodecIdleTmrCount = 0;
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_muteSpkBuff();
		#endif
		tlksys_switch_clock(SYS_CHIP_CLOCK_48M);
	}
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY || gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC){
		#if (TLK_MDI_MP3_ENABLE)
		if(tlkmdi_mp3_isUpdate()) tlkmdi_mp3_startUpdate();
		#endif
	}
	
	if(gTlkMmiAudioCurOptype != 0 || gTlkMmiAudioTmrState != 0) return true;
	else return false;
}

static bool tlkmmi_audio_irqTimer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	uint32 timeIntval;
	
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_NONE) return false;
	
	if(!tlkmmi_audio_modinfIrqProc(gTlkMmiAudioCurOptype)){
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_muteSpkBuff();
		#endif
		timeIntval = 100000;
	}else{
		timeIntval = tlkmmi_audio_modinfIntval(gTlkMmiAudioCurOptype);
		if(timeIntval == 0) timeIntval = 3000;
		else if(timeIntval < 500) timeIntval = 500;
		else if(timeIntval > 1000000) timeIntval = 1000000;
	}
	
	if(gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_NONE){
		tlkapi_timer_updateNode(&gTlkMmiAudioIrqTimer, timeIntval, true);
		gTlkMmiAudioTmrCount = TLKMMI_AUDIO_TIMER_TIMEOUT;
	}else{
		gTlkMmiAudioTmrState = 0;
		gTlkMmiAudioTmrCount = 0;
	}

	return false;
}



#endif //#if (TLKMMI_AUDIO_ENABLE)


/********************************************************************************************************
 * @file	tlkmdi_audsrc.c
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
#if (TLK_MDI_AUDSRC_ENABLE)
#include "drivers.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#include "tlkmdi/aud/tlkmdi_audmp3.h"
#include "tlkmdi/aud/tlkmdi_audsrc.h"

#include "tlkdev/sys/tlkdev_codec.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"

#define TLKMDI_AUDSRC_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_SRC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDSRC_DBG_SIGN       "[MSRC]"


extern void bt_ll_schedule_acl_bandwidth_policy_enter(uint16_t con_handle);
extern void bt_ll_schedule_acl_bandwidth_policy_exit(void);


static int  tlkmdi_audsrc_a2dpStatusEvt(uint08 *pData, uint16 dataLen);
static void tlkmdi_audsrc_keyChangedEvt(uint16 aclHandle, uint08 keyID, uint08 isPress);

#if (TLKBTP_CFG_A2DPSRC_ENABLE)
//extern void btp_avrcp_setvolume(uint16 handle, uint08 volume);
extern uint tlkbt_hci_h2cFifoUnused(void);
//extern bool btp_a2dpsrc_isInStream(uint16 handle);
extern int  btp_a2dpsrc_setSampleRate(uint16 aclHandle, uint32 sampleRate);
#endif

static void tlkmdi_audsrc_mp3Handler(void);
static void tlkmdi_audsrc_fillHandler(void);
static void tlkmdi_audsrc_resetParam(int sample_rate);

static tlkmdi_audsrc_ctrl_t sTlkMdiSrcCtrl;

static uint08 *spTlkMdiSrcPktBuff = nullptr;
static uint08 *spTlkMdiSrcEncBuff = nullptr;


/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int tlkmdi_audsrc_init(void)
{
	tmemset(&sTlkMdiSrcCtrl, 0, sizeof(tlkmdi_audsrc_ctrl_t));
	
	btp_event_regCB(BTP_EVTID_A2DPSRC_STATUS_CHANGED, tlkmdi_audsrc_a2dpStatusEvt);
	btp_avrcp_regKeyChangeCB(tlkmdi_audsrc_keyChangedEvt);
	
	return TLK_ENONE;
}

static int tlkmdi_audsrc_a2dpStatusEvt(uint08 *pData, uint16 dataLen)
{
	btp_a2dpStatusChangeEvt_t *pEvt;

	pEvt = (btp_a2dpStatusChangeEvt_t*)pData;
	if(sTlkMdiSrcCtrl.enable && sTlkMdiSrcCtrl.handle != pEvt->handle){
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_a2dpStatusEvt: other device request");
		#if (TLKBTP_CFG_A2DPSRC_ENABLE)
		if(pEvt->status == BTP_A2DP_STATUS_STREAM) btp_a2dpsrc_suspend(pEvt->handle);
		#endif
		btp_avrcp_notifyPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PAUSED);
		btp_avrcp_setPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PAUSED);
		return -TLK_EFAIL;
	}

	if(pEvt->status == BTP_A2DP_STATUS_STREAM){
		#if (TLKBTP_CFG_A2DPSRC_ENABLE)
		uint sampleRate = btp_a2dpsrc_getSampleRate(pEvt->handle);
		#else
		uint sampleRate = 0;
		#endif
		if(sTlkMdiSrcCtrl.waitStart != 0 && sampleRate != tlkmdi_mp3_getSampleRate()){
			tlkmdi_audsrc_close(pEvt->handle);
			return -TLK_EFAIL;
		}
		sTlkMdiSrcCtrl.frmSize = TLKMDI_SRC_FRAME_SIZE;
		sTlkMdiSrcCtrl.pktNumb = (pEvt->mtuSize-13)/sTlkMdiSrcCtrl.frmSize;
		if(sTlkMdiSrcCtrl.pktNumb > TLKMDI_SRC_FRAME_MAX_NUMB){
			sTlkMdiSrcCtrl.pktNumb = TLKMDI_SRC_FRAME_MAX_NUMB;
		}
		sTlkMdiSrcCtrl.pktTime = 128*sTlkMdiSrcCtrl.pktNumb;
		if(pEvt->mtuSize < 13 || sTlkMdiSrcCtrl.pktNumb == 0){
			tlkapi_error(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN,
				"tlkmdi_audsrc_a2dpStatusEvt: mtuSize[%d] is error!",
				pEvt->mtuSize);
			tlkmdi_audsrc_close(pEvt->handle);
			return -TLK_EFAIL;
		}
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, 
			"tlkmdi_audsrc_a2dpStatusEvt[START]: frmSize[%d], pktNumb[%d], pktTime[%d]",
			sTlkMdiSrcCtrl.frmSize, sTlkMdiSrcCtrl.pktNumb, sTlkMdiSrcCtrl.pktTime);
		btp_avrcp_notifyPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PLAYING);
		btp_avrcp_setPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PLAYING);
		sTlkMdiSrcCtrl.isSuppSet = btp_avrcp_isSupportSetVolume(pEvt->handle);
		sTlkMdiSrcCtrl.sampleRate = sampleRate;
		tlkmdi_audsrc_resetParam(sTlkMdiSrcCtrl.sampleRate);
		if(sTlkMdiSrcCtrl.waitStart == 0){
			tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_SRC, pEvt->handle);
		}
		sTlkMdiSrcCtrl.waitStart = 0;
		sTlkMdiSrcCtrl.waitTimer = 0;
	}
	else{
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_a2dpStatusEvt[CLOSE]: 002");
		btp_avrcp_notifyPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PAUSED);
		btp_avrcp_setPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PAUSED);
		if(!sTlkMdiSrcCtrl.isPause){
			tlkmdi_audsrc_switch(pEvt->handle, TLK_STATE_CLOSED);
			tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_SRC, pEvt->handle);
		}
	}
	return TLK_ENONE;
}
static void tlkmdi_audsrc_keyChangedEvt(uint16 aclHandle, uint08 keyID, uint08 isPress)
{
	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_keyChangedEvt:{handle-%d,keyID-%d,isPress-%d}", aclHandle, keyID, isPress);
	#if (TLKBTP_CFG_A2DPSRC_ENABLE)
	if(!isPress){
		if(keyID == BTP_AVRCP_KEYID_PLAY){
			if(!sTlkMdiSrcCtrl.enable){
				tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: play execute");
				tlkmdi_audsrc_start(aclHandle, 0);
			}else{
				tlkapi_error(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: play failure");
				btp_avrcp_notifyPlayState(aclHandle, BTP_AVRCP_PLAY_STATE_PAUSED);
			}
		}else if(keyID == BTP_AVRCP_KEYID_STOP || keyID == BTP_AVRCP_KEYID_PAUSE){
			if(sTlkMdiSrcCtrl.enable){
				tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: stop or pause execute");
				tlkmdi_audsrc_close(aclHandle);
			}else{
				tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_avrcpStatusEvt: stop or pause failure");
				btp_avrcp_notifyPlayState(aclHandle, BTP_AVRCP_PLAY_STATE_PAUSED);
			}
		}else if(keyID == BTP_AVRCP_KEYID_FORWARD){
			if(sTlkMdiSrcCtrl.enable) tlkmdi_audsrc_toNext();
		}else if(keyID == BTP_AVRCP_KEYID_BACKWARD){
			if(sTlkMdiSrcCtrl.enable) tlkmdi_audsrc_toPrev();
		}
	}
	#endif
}


int tlkmdi_audsrc_start(uint16 handle, uint32 param)
{
	sTlkMdiSrcCtrl.isPause = false;
	#if (TLKBTP_CFG_A2DPSRC_ENABLE)
	int ret;

	btp_avrcp_notifyPlayState(handle, BTP_AVRCP_PLAY_STATE_PLAYING); //start
	btp_avrcp_setPlayState(handle, BTP_AVRCP_PLAY_STATE_PLAYING);
	ret = btp_a2dpsrc_start(handle);
	if(ret != TLK_ENONE){
		btp_avrcp_notifyPlayState(handle, BTP_AVRCP_PLAY_STATE_PAUSED);
		btp_avrcp_setPlayState(handle, BTP_AVRCP_PLAY_STATE_PAUSED);
		return ret;
	}
	if((param & 0x80000000) != 0) sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getNextIndex();
	else if((param & 0x40000000) != 0) sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPrevIndex();
	else sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPlayIndex();
	if((param & 0x80000000) != 0){
		sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getNextIndex();
	}else if((param & 0x40000000) != 0){
		sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPrevIndex();
	}else if((param & 0x20000000) != 0){
		sTlkMdiSrcCtrl.playIndex = (param & 0xFFFF);
		if(sTlkMdiSrcCtrl.playIndex >= tlkmdi_mp3_getPlayCount()) sTlkMdiSrcCtrl.playIndex = 0;
	}else{
		sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPlayIndex();
	}
	if((param & 0x10000000) != 0){
		
	}

	
	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_start: %d ", sTlkMdiSrcCtrl.playIndex);
	return TLK_ENONE;
	#else
	return -TLK_ENOSUPPORT;
	#endif
}
int tlkmdi_audsrc_close(uint16 handle)
{
	sTlkMdiSrcCtrl.isPause = false;
	#if (TLKBTP_CFG_A2DPSRC_ENABLE)
	if(sTlkMdiSrcCtrl.handle != handle){
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_close: enable handle");
		return -TLK_EHANDLE;
	}
	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_close start: enable handle");
	sTlkMdiSrcCtrl.waitStart = 0;
	btp_avrcp_notifyPlayState(sTlkMdiSrcCtrl.handle, BTP_AVRCP_PLAY_STATE_PAUSED);
	tlkmdi_audsrc_switch(sTlkMdiSrcCtrl.handle, TLK_STATE_CLOSED);
	return btp_a2dpsrc_suspend(sTlkMdiSrcCtrl.handle);
	#else
	return -TLK_ENOSUPPORT;
	#endif
}

void tlkmdi_audsrc_timer(void)
{
	#if (TLKBTP_CFG_A2DPSRC_ENABLE)
	if(sTlkMdiSrcCtrl.waitStart == 1){
		int ret = btp_a2dpsrc_setSampleRate(sTlkMdiSrcCtrl.handle, tlkmdi_mp3_getSampleRate());
		if(ret == TLK_ENONE){
			sTlkMdiSrcCtrl.waitTimer = 0;
			sTlkMdiSrcCtrl.waitStart = 0;
		}else if(ret == -TLK_EBUSY){
			sTlkMdiSrcCtrl.waitTimer = clock_time()|1;
			sTlkMdiSrcCtrl.waitStart = 2;
		}else{
			tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_timer: disable - set sampleRate");
			sTlkMdiSrcCtrl.running = false;
			sTlkMdiSrcCtrl.waitStart = 3;
			tlkmdi_audsrc_close(sTlkMdiSrcCtrl.handle);
		}
	}
	if(sTlkMdiSrcCtrl.waitStart != 0 && sTlkMdiSrcCtrl.waitStart != 3 && sTlkMdiSrcCtrl.waitTimer != 0 
		&& clock_time_exceed(sTlkMdiSrcCtrl.waitTimer, TLKMDI_SRC_WAIT_RECFG_TIMEOUT)){
		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN,
			"tlkmdi_audsrc_timer: wait timeout - waitStart[%d] waitTimer[%d]",
			sTlkMdiSrcCtrl.waitStart, sTlkMdiSrcCtrl.waitTimer);
		sTlkMdiSrcCtrl.running = false;
		sTlkMdiSrcCtrl.waitStart = 3;
		tlkmdi_audsrc_close(sTlkMdiSrcCtrl.handle);
	}
	#endif
}

bool tlkmdi_audsrc_fPlay(bool isRewind, bool isStart)
{
	return false;
}

bool tlkmdi_audsrc_toNext(void)
{
	uint16 index;
	if(!sTlkMdiSrcCtrl.running) return false;
	tlkmdi_mp3_updateEnable(true);
	index = tlkmdi_mp3_getPlayIndex();
	tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, index, false);
	sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getNextIndex();
	return true;
}
bool tlkmdi_audsrc_toPrev(void)
{
	uint16 index;
	if(!sTlkMdiSrcCtrl.running) return false;
	tlkmdi_mp3_updateEnable(true);
	index = tlkmdi_mp3_getPlayIndex();
	tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_PLAY, index, false);
	sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPrevIndex();
	return true;
}

/******************************************************************************
 * Function: tlkmdi_audsrc_switch
 * Descript: Change A2DP SRC status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsrc_switch(uint16 handle, uint08 status)
{
	bool enable;
	bool isSucc = true;

	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	if(handle == 0 && enable) enable = false;
	if(!enable) tlkmdi_mp3_updateEnable(false);
	if(status == TLK_STATE_CLOSED && sTlkMdiSrcCtrl.handle != 0){
		#if (TLKBTP_CFG_A2DPSRC_ENABLE)
		btp_a2dpsrc_suspend(sTlkMdiSrcCtrl.handle);
		#endif
	}
	if(status == TLK_STATE_PAUSED && sTlkMdiSrcCtrl.handle != 0)
	{
		sTlkMdiSrcCtrl.isPause = true;
		#if (TLKBTP_CFG_A2DPSRC_ENABLE)
		btp_a2dpsrc_suspend(sTlkMdiSrcCtrl.handle);
		#endif
	}else if(status == TLK_STATE_OPENED && sTlkMdiSrcCtrl.handle != 0 && sTlkMdiSrcCtrl.isPause){
		sTlkMdiSrcCtrl.isPause = false;
		tlkmdi_audsrc_start(sTlkMdiSrcCtrl.handle, 0);
	}else{
		sTlkMdiSrcCtrl.isPause = false;
	}
	if(sTlkMdiSrcCtrl.enable == enable) return true;

	if(!enable) sTlkMdiSrcCtrl.enable = false;
	if(!tlkmdi_mp3_enable(enable) && enable){
		#if (TLKBTP_CFG_A2DPSRC_ENABLE)
		tlkmdi_audsrc_close(handle);
		#endif
		tlkapi_error(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_switch: enable failure - %d", enable);
		return false;
	}
	
	if(enable){
		if(spTlkMdiSrcPktBuff == nullptr) spTlkMdiSrcPktBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SRC_PKT_BUFF_SIZE);
		if(spTlkMdiSrcEncBuff == nullptr) spTlkMdiSrcEncBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SRC_SBC_ENC_BUFF_SIZE);
		if(spTlkMdiSrcPktBuff == nullptr || spTlkMdiSrcEncBuff == nullptr){
			isSucc = false;
			enable = false;
			tlkapi_error(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_switch: tlkmdi_audmem_calloc failure");
			tlkmdi_mp3_enable(false);
		}
	}
	if(!enable){
		if(spTlkMdiSrcPktBuff != nullptr){
			tlkmdi_audmem_free(spTlkMdiSrcPktBuff);
			spTlkMdiSrcPktBuff = nullptr;
		}
		if(spTlkMdiSrcEncBuff != nullptr){
			tlkmdi_audmem_free(spTlkMdiSrcEncBuff);
			spTlkMdiSrcEncBuff = nullptr;
		}
	}

	sTlkMdiSrcCtrl.enable = enable;
	sTlkMdiSrcCtrl.running = enable;
	sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	sTlkMdiSrcCtrl.handle = handle;
	tlkalg_sbc_encInit(spTlkMdiSrcEncBuff);
	
	sTlkMdiSrcCtrl.refTime = clock_time();
	sTlkMdiSrcCtrl.lagTime = 0;
	sTlkMdiSrcCtrl.timeStamp = 0;
	sTlkMdiSrcCtrl.unitTime = 0;
	sTlkMdiSrcCtrl.waitTimer = 0;
	sTlkMdiSrcCtrl.waitStart = 0;
//	sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getPlayIndex();

	tlkmdi_mp3_startUpdate();
	
	if(enable){
		if(sTlkMdiSrcCtrl.sampleRate == 0) sTlkMdiSrcCtrl.sampleRate = 44100;
		tlkmdi_audsrc_resetParam(sTlkMdiSrcCtrl.sampleRate);
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, TLK_STATE_OPENED);
		bt_ll_schedule_acl_bandwidth_policy_enter(sTlkMdiSrcCtrl.handle);
	}else{
		bt_ll_schedule_acl_bandwidth_policy_exit();
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, TLK_STATE_CLOSED);
	}

	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "tlkmdi_audsrc_switch: 0x%x, status-%d, isSucc-%d", handle, status, isSucc);
	
	return isSucc;
}

/******************************************************************************
 * Function: tlkmdi_audsrc_isBusy
 * Descript: Is A2DP source block enable. 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_audsrc_isBusy(void)
{
	return (sTlkMdiSrcCtrl.enable && sTlkMdiSrcCtrl.running);
}

/******************************************************************************
 * Function: tlkmdi_audsrc_intval
 * Descript: Get A2DP source Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsrc_intval(void)
{
	if(sTlkMdiSrcCtrl.waitStart != 0){
		return 5000;
	}else if(sTlkMdiSrcCtrl.sendFail){
		return 2200;
	}else if(sTlkMdiSrcCtrl.lagTime >= sTlkMdiSrcCtrl.unitTime){
		return 1000;
	}else if(tlkmdi_mp3_getPcmDataLen() < 1024){
		return 1200;
	}else if(sTlkMdiSrcCtrl.unitTime > 1500){
		return 1500;
	}else{
		return sTlkMdiSrcCtrl.unitTime;
	}
}

/******************************************************************************
 * Function: tlkmdi_audsrc_irqProc
 * Descript: Call A2DP SRC mp3 handler and fill handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsrc_irqProc(void)
{
//	my_dump_str_data(TLKMDI_SNK_DBG_ENABLE, "tlkmdi_audsrc_irqProc:", 0, 0);
	if(!sTlkMdiSrcCtrl.enable) return false;
	tlkmdi_audsrc_mp3Handler();
	tlkmdi_audsrc_fillHandler();
	if(sTlkMdiSrcCtrl.running) return true;
	else return false;
}



static void tlkmdi_audsrc_resetParam(int sample_rate)
{
	if(sample_rate == 0) sample_rate = 44100;
	sample_rate /= 100;
	sTlkMdiSrcCtrl.unitTime = (10000*128*SYSTEM_TIMER_TICK_1US+sample_rate/2)/sample_rate;
	sTlkMdiSrcCtrl.seqNumber = 0;
	sTlkMdiSrcCtrl.timeStamp = 0;
	sTlkMdiSrcCtrl.refTime = clock_time()-4*sTlkMdiSrcCtrl.unitTime;
	sTlkMdiSrcCtrl.lagTime = 0;
	sTlkMdiSrcCtrl.cacheFrm = 0;
	sTlkMdiSrcCtrl.sndFrame = 0;
	sTlkMdiSrcCtrl.sendFail = false;
}


static bool tlkmdi_audsrc_playStart(uint16 index)
{
	bool isSucc;

	tlkmdi_mp3_updateEnable(true);
	isSucc = tlkmdi_mp3_play(index);
	if(!isSucc){
		tlkmdi_mp3_updateEnable(false);
		sTlkMdiSrcCtrl.running = false;
		sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_PLAY;
	}else{
		sTlkMdiSrcCtrl.running = true;
		sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
	}
	return isSucc;
}

static void tlkmdi_audsrc_mp3Handler(void) //in irq
{
	if(!sTlkMdiSrcCtrl.running) return;
	if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_IDLE){
		tlkmdi_audsrc_playStart(sTlkMdiSrcCtrl.playIndex);
		if(!sTlkMdiSrcCtrl.running) return;
		if(tlkmdi_mp3_getSampleRate() != sTlkMdiSrcCtrl.sampleRate){
			sTlkMdiSrcCtrl.waitStart = 1;
			sTlkMdiSrcCtrl.waitTimer = clock_time()|1;
			tlkmdi_audsrc_resetParam(tlkmdi_mp3_getSampleRate());
		}else{
			sTlkMdiSrcCtrl.waitStart = 2;
			sTlkMdiSrcCtrl.waitTimer = clock_time()|1;
			btp_a2dpsrc_restart(sTlkMdiSrcCtrl.handle);
		}
		sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_PLAY;
		tlkmdi_audio_sendPlayStartEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, sTlkMdiSrcCtrl.playIndex, true);
	}
	if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_WAIT || sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_DONE){
		uint16 index = tlkmdi_mp3_getPlayIndex();
		if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_DONE){
			sTlkMdiSrcCtrl.running = false;
		}else{
			sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_IDLE;
		}
		tlkmdi_audio_sendPlayOverEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SRC, index, true);
	}else if(sTlkMdiSrcCtrl.mp3State == TLKMDI_MP3_STATUS_PLAY){
		tlkmdi_mp3_decode();
		if(tlkmdi_mp3_isOver()){
			tlkmdi_mp3_reset();
			if(tlkmdi_mp3_indexIsOver()){
				tlkmdi_mp3_setPlayIndex(0);
				sTlkMdiSrcCtrl.playIndex = 0;
				tlkmdi_mp3_updataPlayNameByIndex(0);
				sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_DONE;
			}else{
				sTlkMdiSrcCtrl.mp3State = TLKMDI_MP3_STATUS_WAIT;
				sTlkMdiSrcCtrl.playIndex = tlkmdi_mp3_getNextPlay();
			}
		}
	}
}
static void tlkmdi_audsrc_fillHandler(void)
{
	uint16 volume;
	uint08 frameSize;
	uint16 readLens;
	uint32 refTime;

//	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "=== app_audio_a2dpSrcPlayProcs 001");
	if(!sTlkMdiSrcCtrl.running) return;
	if(sTlkMdiSrcCtrl.unitTime == 0 || sTlkMdiSrcCtrl.handle == 0){  
		sTlkMdiSrcCtrl.refTime = clock_time()-4*sTlkMdiSrcCtrl.unitTime; 
		return; 
	}
	if(sTlkMdiSrcCtrl.waitTimer != 0){
		sTlkMdiSrcCtrl.refTime = clock_time()-4*sTlkMdiSrcCtrl.unitTime;
		return;
	}
		
	frameSize = 85; // set as 77 if bitpool is 32;
	
//	tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "=== app_audio_a2dpSrcPlayProcs 002");
	refTime = clock_time();
	sTlkMdiSrcCtrl.lagTime += refTime-sTlkMdiSrcCtrl.refTime;
	sTlkMdiSrcCtrl.refTime = refTime;
	if(sTlkMdiSrcCtrl.lagTime >= sTlkMdiSrcCtrl.unitTime){
		sTlkMdiSrcCtrl.lagTime -= sTlkMdiSrcCtrl.unitTime;
		if(sTlkMdiSrcCtrl.cacheFrm+1 < sTlkMdiSrcCtrl.pktNumb){
			sTlkMdiSrcCtrl.cacheFrm ++;
		}
		sTlkMdiSrcCtrl.timeStamp += 128;
	}
	
	readLens = tlkmdi_mp3_getChannels() == 2 ? 128*4 : 128 *2;
	if(sTlkMdiSrcCtrl.sndFrame < sTlkMdiSrcCtrl.pktNumb && sTlkMdiSrcCtrl.cacheFrm != 0
		&& tlkmdi_mp3_getPcmDataLen() >= readLens) 
	{
		uint16 value;
		uint16 index;
		uint16 offset;
		uint16 pcm[256];
		uint08 *pBuffer;
		
		tlkmdi_mp3_getPcmData((uint08*)pcm, readLens);
		sTlkMdiSrcCtrl.cacheFrm --;
		sint16 *p =(sint16*)pcm;
				
//		tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "=== app_audio_a2dpSrcPlayProcs 004");
		volume = tlkmdi_audio_getHeadsetCalVolume();
		if(!sTlkMdiSrcCtrl.isSuppSet){
			for(index=0; index<256; index++){
				p[index] = (p[index]*volume) >> TLKMDI_AUDIO_VOLUME_EXPAND;
			}
		}
		if(readLens == 256){
			for(index=0; index<128; index++){
				value = pcm[127-index];
				offset = 254-(index << 1);
				pcm[offset+0] = value;
				pcm[offset+1] = value;
			}
		}
		pBuffer = spTlkMdiSrcPktBuff+(1+sTlkMdiSrcCtrl.sndFrame*frameSize);
		tlkalg_sbc_encJoint((uint08*)pcm, 128*4, pBuffer);
		sTlkMdiSrcCtrl.sndFrame ++;
	}
	#if (TLKBTP_CFG_A2DPSRC_ENABLE)
	if(sTlkMdiSrcCtrl.sndFrame >= sTlkMdiSrcCtrl.pktNumb){// retry send last send fail pkt;
		if(tlkbt_hci_h2cFifoUnused() < 2){
			sTlkMdiSrcCtrl.sendFail = true;
		}else{
			int ret;
			uint16 pktLen;
			uint32 timeStamp;
			timeStamp = sTlkMdiSrcCtrl.timeStamp-sTlkMdiSrcCtrl.pktTime-sTlkMdiSrcCtrl.cacheFrm*128;
			pktLen = 1+frameSize*sTlkMdiSrcCtrl.pktNumb;
			spTlkMdiSrcPktBuff[0] = sTlkMdiSrcCtrl.pktNumb;
			ret = btp_a2dpsrc_sendMediaData(sTlkMdiSrcCtrl.handle, sTlkMdiSrcCtrl.seqNumber, timeStamp, spTlkMdiSrcPktBuff, pktLen);
			if(ret == TLK_ENONE){
				sTlkMdiSrcCtrl.sendFail = false;
				sTlkMdiSrcCtrl.sndFrame = 0;
				sTlkMdiSrcCtrl.seqNumber ++;
	//			tlkapi_trace(TLKMDI_AUDSRC_DBG_FLAG, TLKMDI_AUDSRC_DBG_SIGN, "=== app_audio_a2dpSrcPlayProcs 005");
			}else{
				sTlkMdiSrcCtrl.sendFail = true;
			}
		}
	}
	if(sTlkMdiSrcCtrl.sendFail && sTlkMdiSrcCtrl.sndFrame < sTlkMdiSrcCtrl.pktNumb){
		sTlkMdiSrcCtrl.sendFail = false;
	}
	#endif
}

#endif //#if (TLK_MDI_AUDSRC_ENABLE)

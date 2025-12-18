/********************************************************************************************************
 * @file	tlkmdi_audsnk.c
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
#if (TLK_MDI_AUDSNK_ENABLE)
#include "drivers.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#include "tlkmdi/aud/tlkmdi_audsnk.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"

#include "tlkdev/sys/tlkdev_codec.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"

#if (TLK_ALG_EQ_ENABLE)
#include "tlkalg/audio/eq/tlkalg_eq.h"
#endif


#define TLKMDI_AUDSNK_DOUBLE_CHANNEL_ENABLE     1

#define TLKMDI_AUDSNK_VOLUME_STEP    3
#define TLKMDI_AUDSNK_WAIT_FRAMES    5

#define TLKMDI_AUDSNK_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_SNK << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDSNK_DBG_SIGN       "[MDI]"

extern void bt_ll_schedule_acl_bandwidth_policy_enter(uint16 con_handle);
extern void bt_ll_schedule_acl_bandwidth_policy_exit(void);

static int tlkmdi_audsnk_statusChangedEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_audsnk_avrcpStatusChangedEvt(uint08 *pData, uint16 dataLen);

#if (TLKBTP_CFG_A2DPSNK_ENABLE)
static bool tlkmdi_snk_initBuffer(bool enable);
#endif
static void tlkmdi_snk_spkHandler(void);

static int tlkmdi_snk_parseSbcParam(uint08 *data);
static uint tlkmdi_snk_getPlaybackData(uint08 *pBuffer);
static int tlkmdi_snk_addRawFrame(uint08 id, uint08 *p, int len);
static int tlkmdi_snk_firstInit(uint32 para, uint08 *pData, int dataLen);


typedef struct{
	uint08 enable;
	uint08 firstInit;
	uint08 waitFrame;
	uint16 handle;
	uint16 frameSize;
	uint16 sampleRate;
	uint16 frameSample;
	uint32 encParma;
	uint32 runTimer;
	tlkmdi_snk_decFunc decFunc;
}tlkmdi_audsnk_ctrl_t;


static uint32  sTlkMdiSnkBuffLen;
static uint08 *spTlkMdiSnkBuffer;
static tlkapi_qfifo_t spTlkMdiSnkFifo;
static tlkmdi_audsnk_ctrl_t sTlkMdiSnkCtrl = {0};

#if (TLKBTP_CFG_A2DPSNK_ENABLE)
static uint08 *spTlkMdiSnkSBCDecBuff = nullptr;
static uint08 *spTlkMdiSnkRawRcvBuff = nullptr;
#endif
static uint08 *spTlkMdiSnkTmpBuff = nullptr;


/******************************************************************************
 * Function: tlkmdi_audsnk_init
 * Descript: Initial the A2DP sink block, register the data callback. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_audsnk_init(void)
{
	spTlkMdiSnkBuffer = nullptr;
	
	tmemset(&spTlkMdiSnkFifo, 0, sizeof(tlkapi_qfifo_t));
	memset(&sTlkMdiSnkCtrl, 0, sizeof(tlkmdi_audsnk_ctrl_t));

	sTlkMdiSnkCtrl.encParma = 0;
	sTlkMdiSnkCtrl.firstInit = true;

	#if (TLKBTP_CFG_A2DPSNK_ENABLE)
	btp_a2dpsnk_regRecvDataCB(tlkmdi_snk_addEncFrame);
	#endif
	btp_event_regCB(BTP_EVTID_A2DPSNK_STATUS_CHANGED, tlkmdi_audsnk_statusChangedEvt);
	btp_event_regCB(BTP_EVTID_AVRCP_STATUS_CHANGED, tlkmdi_audsnk_avrcpStatusChangedEvt);
	
	return TLK_ENONE;
}

static int tlkmdi_audsnk_statusChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_a2dpStatusChangeEvt_t *pEvt;
	pEvt = (btp_a2dpStatusChangeEvt_t*)pData;
	if(pEvt->status == BTP_A2DP_STATUS_STREAM){
		btp_avrcp_setPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PLAYING);
		tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_SNK, pEvt->handle);
	}else{
		btp_avrcp_setPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PAUSED);
		tlkmdi_audsnk_switch(pEvt->handle, TLK_STATE_CLOSED);
		tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_SNK, pEvt->handle);
	}
	return TLK_ENONE;
}
static int tlkmdi_audsnk_avrcpStatusChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_avrcpStatusChangeEvt_t *pEvt;
	pEvt = (btp_avrcpStatusChangeEvt_t*)pData;
	if(!pEvt->isNoty) return TLK_ENONE;
	if(pEvt->status == BTP_AVRCP_PLAY_STATE_PLAYING){
		if(!sTlkMdiSnkCtrl.enable){
			tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_SNK, pEvt->handle);
		}
	}else if(pEvt->status == BTP_AVRCP_PLAY_STATE_PAUSED || pEvt->status == BTP_AVRCP_PLAY_STATE_STOPPED){
		if(sTlkMdiSnkCtrl.enable){
			tlkmdi_audsnk_switch(pEvt->handle, TLK_STATE_CLOSED);
			tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_SNK, pEvt->handle);
		}
	}
	return TLK_ENONE;
}

int tlkmdi_audsnk_start(uint16 handle, uint32 param)
{
	if(sTlkMdiSnkCtrl.enable) return -TLK_EREPEAT;
	btp_avrcp_sendKeyPress(handle, BTP_AVRCP_KEYID_PLAY);
	btp_avrcp_sendKeyRelease(handle, BTP_AVRCP_KEYID_PLAY);
	return TLK_ENONE;
}
int tlkmdi_audsnk_close(uint16 handle)
{
	tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_close ..");
	btp_avrcp_sendKeyPress(handle, BTP_AVRCP_KEYID_PAUSE);
	btp_avrcp_sendKeyRelease(handle, BTP_AVRCP_KEYID_PAUSE);

	return TLK_ENONE;
}

void tlkmdi_audsnk_timer(void)
{
	if(sTlkMdiSnkCtrl.enable && sTlkMdiSnkCtrl.runTimer != 0 
		&& clock_time_exceed(sTlkMdiSnkCtrl.runTimer, 3000000)){
		sTlkMdiSnkCtrl.runTimer = clock_time() | 1;
		tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_timer: timeout");
		tlkmdi_audsnk_close(sTlkMdiSnkCtrl.handle);
	}
}

bool tlkmdi_audsnk_fPlay(bool isRewind, bool isStart)
{
	if(!sTlkMdiSnkCtrl.enable || sTlkMdiSnkCtrl.handle == 0){
		return false;
	}
	if(isRewind){
//		if(isStart) btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_BACKWARD);
//		else btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_BACKWARD);
		if(isStart) btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_REWIND);
		else btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_REWIND);
	}else{
//		if(isStart) btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FORWARD);
//		else btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FORWARD);
		if(isStart) btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FAST_FORWARD);
		else btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FAST_FORWARD);
	}
	return true;
}
bool tlkmdi_audsnk_toNext(void)
{
	if(!sTlkMdiSnkCtrl.enable) return -TLK_EREPEAT;
	btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FORWARD);
	btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_FORWARD);
	return TLK_ENONE;
}
bool tlkmdi_audsnk_toPrev(void)
{
	if(!sTlkMdiSnkCtrl.enable) return -TLK_EREPEAT;
	btp_avrcp_sendKeyPress(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_BACKWARD);
	btp_avrcp_sendKeyRelease(sTlkMdiSnkCtrl.handle, BTP_AVRCP_KEYID_BACKWARD);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_audsnk_switch
 * Descript: Change A2DP SNK status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsnk_switch(uint16 handle, uint08 status)
{
#if (TLKBTP_CFG_A2DPSNK_ENABLE)
	bool isSucc;
	bool enable;
	uint sampleRate = 0;
	
	if(status == TLK_STATE_CLOSED && sTlkMdiSnkCtrl.handle != handle) return false;
	if(status == TLK_STATE_OPENED) enable = true;
	else enable = false;
	if(sTlkMdiSnkCtrl.enable == enable) return true;

	if(!enable) sTlkMdiSnkCtrl.enable = false;
	if(enable){
		sampleRate = btp_a2dpsnk_getSampleRate(handle);
		if(sampleRate == 0){
			tlkmdi_audsnk_close(handle);
			tlkapi_error(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_switch: failure - get sample rate");
			return false;
		}
	}
	isSucc = tlkmdi_snk_initBuffer(enable);
	if(enable && !isSucc){
		tlkmdi_audsnk_close(handle);
		tlkapi_error(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_switch: failure - buffer");
		return false;
	}
    if(status == TLK_STATE_CLOSED && (btp_a2dpsnk_getStatus(handle) == BTP_A2DP_STATUS_STREAM)){
//        tlkmdi_audsnk_close(handle);
	}
	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_muteSpkBuff();
	#endif
	
	sTlkMdiSnkCtrl.handle = handle;
	sTlkMdiSnkCtrl.enable = enable;
	sTlkMdiSnkCtrl.encParma = 0;
	sTlkMdiSnkCtrl.runTimer = clock_time()|1;
	sTlkMdiSnkCtrl.firstInit = true;
	sTlkMdiSnkCtrl.waitFrame = TLKMDI_AUDSNK_WAIT_FRAMES;
	tlkapi_qfifo_clear(&spTlkMdiSnkFifo);
	if(enable){
		bt_ll_schedule_acl_bandwidth_policy_enter(sTlkMdiSnkCtrl.handle);
		sTlkMdiSnkCtrl.sampleRate = sampleRate;
		#if (TLK_DEV_CODEC_ENABLE)
		#if (TLKMDI_AUDSNK_DOUBLE_CHANNEL_ENABLE)
		tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_STEREO, TLKDEV_CODEC_BITDEPTH_16, sampleRate);
		#else
		tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_LEFT, TLKDEV_CODEC_BITDEPTH_16, sampleRate);
		#endif
		tlkdev_codec_setSpkOffset(320);
		#endif
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SNK, TLK_STATE_OPENED);
	}else{
		bt_ll_schedule_acl_bandwidth_policy_exit();
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_close();
		#endif
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_A2DP_SNK, TLK_STATE_CLOSED);
	}
	
	tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_switch: %d %d %d", handle, status, isSucc);
	
	return isSucc;
#else
	return false;
#endif
}

/******************************************************************************
 * Function: tlkmdi_audsnk_isBusy
 * Descript: Is A2DP sink block enable. 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_audsnk_isBusy(void)
{
	return sTlkMdiSnkCtrl.enable;
}

/******************************************************************************
 * Function: tlkmdi_audsnk_intval
 * Descript: Get A2DP sink Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsnk_intval(void)
{
	return 1000;
}

/******************************************************************************
 * Function: tlkmdi_audsnk_irqProc
 * Descript: Call A2DP SNK spk handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsnk_irqProc(void)
{
//	tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_audsnk_irqProc:");
	if(!sTlkMdiSnkCtrl.enable) return false;
	tlkmdi_snk_spkHandler();
	if(sTlkMdiSnkCtrl.enable) return true;
	else return false;
}



int tlkmdi_snk_initFifo(void)
{
	uint32 r = irq_disable();
	if(spTlkMdiSnkBuffer == nullptr || sTlkMdiSnkBuffLen == 0 || sTlkMdiSnkCtrl.frameSize == 0){
		tlkapi_qfifo_reset(&spTlkMdiSnkFifo);
	}else{
		uint16 buffNumb;
		uint16 buffSize;
		buffSize = sTlkMdiSnkCtrl.frameSize;
		buffNumb = sTlkMdiSnkBuffLen/buffSize;
		tlkapi_qfifo_init(&spTlkMdiSnkFifo, buffNumb, buffSize, spTlkMdiSnkBuffer, sTlkMdiSnkBuffLen);
	}
	irq_restore(r);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_snk_setBuffer
 * Descript: A2DP SNK alloc buffer for audio data. 
 * Params: 
 *        @pBuffer[IN]--The data buffer.
 *        @buffLen[IN]--The buffer length.
 * Return: TLK_ENONE is success, others value is false.
*******************************************************************************/
int	tlkmdi_snk_setBuffer(uint08 *pBuffer, uint32 buffLen)
{
	if(pBuffer != nullptr && buffLen != 0){
		spTlkMdiSnkBuffer = pBuffer;
		sTlkMdiSnkBuffLen = buffLen;
		tlkmdi_snk_initFifo();
	}else{
		spTlkMdiSnkBuffer = 0;
		sTlkMdiSnkBuffLen = nullptr;
		tlkapi_qfifo_reset(&spTlkMdiSnkFifo);
	}
	return TLK_ENONE;
}

#if (TLKBTP_CFG_A2DPSNK_ENABLE)
static bool tlkmdi_snk_initBuffer(bool enable)
{
	bool isSucc = true;
	
	if(enable){
		if(spTlkMdiSnkRawRcvBuff == nullptr) spTlkMdiSnkRawRcvBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SNK_RAW_RCV_BUFF_SIZE);
		if(spTlkMdiSnkSBCDecBuff == NULL) spTlkMdiSnkSBCDecBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SNK_SBC_DEC_BUFF_SIZE);
		if(spTlkMdiSnkTmpBuff == nullptr) spTlkMdiSnkTmpBuff = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SNK_DEC_TEMP_SIZE);
		if(spTlkMdiSnkRawRcvBuff == NULL || spTlkMdiSnkSBCDecBuff == NULL || spTlkMdiSnkTmpBuff == nullptr){
			isSucc = false;
			enable = false;
		}
	}
	if(!enable){
		if(spTlkMdiSnkRawRcvBuff != NULL){
			tlkmdi_audmem_free(spTlkMdiSnkRawRcvBuff);
			spTlkMdiSnkRawRcvBuff = NULL;
		}
		if(spTlkMdiSnkSBCDecBuff != NULL){
			tlkmdi_audmem_free(spTlkMdiSnkSBCDecBuff);
			spTlkMdiSnkSBCDecBuff = NULL;
		}
		if(spTlkMdiSnkTmpBuff != nullptr){
			tlkmdi_audmem_free(spTlkMdiSnkTmpBuff);
			spTlkMdiSnkTmpBuff = nullptr;
		}
	}
	tlkmdi_snk_setBuffer(spTlkMdiSnkRawRcvBuff, TLKMDI_SNK_RAW_RCV_BUFF_SIZE);
	tlkalg_sbc_decInit(spTlkMdiSnkSBCDecBuff);
	
	return isSucc;
}
#endif

uint tlkmdi_snk_getTimer(void)
{
	return 1000;
}



static void tlkmdi_snk_spkHandler(void)
{
	uint index;
	uint length;
	uint16 volume;
	sint16 *pData;
	
	if(spTlkMdiSnkTmpBuff == nullptr || sTlkMdiSnkCtrl.waitFrame != 0) return;

	while(true){
		length = 0;
		#if (TLK_DEV_CODEC_ENABLE)
		if(tlkdev_codec_getSpkIdleLen() > 1024){
			length = tlkmdi_snk_getPlaybackData((uint08*)spTlkMdiSnkTmpBuff); //length=256
		}
		#else
		length = tlkmdi_snk_getPlaybackData((uint08*)spTlkMdiSnkTmpBuff);
		#endif
		if(length == 0) {
			if(tlkdev_codec_getSpkDataLen() < 512) tlkdev_codec_zeroSpkBuff(512, true);
			break;
		}
		
		#if (TLK_ALG_EQ_ENABLE)
		#if (TLKMDI_AUDSNK_DOUBLE_CHANNEL_ENABLE)
		tlkalg_eq_procss(TLKALG_EQ_TYPE_MUSIC, TLKALG_EQ_CHANNEL_STEREO, sTlkMdiSnkCtrl.sampleRate, (sint16*)spTlkMdiSnkTmpBuff, 64*2);
		#else
		tlkalg_eq_procss(TLKALG_EQ_TYPE_MUSIC, TLKALG_EQ_CHANNEL_STEREO, sTlkMdiSnkCtrl.sampleRate, (sint16*)spTlkMdiSnkTmpBuff, 64);
		#endif
		#endif
		
		volume = tlkmdi_audio_getMusicCalVolume(true);
		pData = (sint16*)spTlkMdiSnkTmpBuff;
		for(index=0; index<length/2; index++){
			pData[index] = (pData[index]*volume) >> TLKMDI_AUDIO_VOLUME_EXPAND;
		}
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_fillSpkBuff(spTlkMdiSnkTmpBuff, length);
		#endif
	}
}

/******************************************************************************
 * Function: tlkmdi_snk_addEncFrame
 * Descript: A2DP SNK Encode the data. 
 * Params: 
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 * Return: None.
*******************************************************************************/
void tlkmdi_snk_addEncFrame(uint16 aclHandle, uint08 *pData, uint16 dataLen)
{
	uint08 pktNumber;

//	tlkapi_array(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "tlkmdi_snk_addEncFrame:", pData, dataLen > 16 ? 16 : dataLen);
	
//	pktType = pData[13];
	pktNumber = pData[12];
	if(pData[13] != 0x9c){			//non-sbc packet
//		tlkapi_array(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "<enc: not SBC packet>", p, 64);
		return;
	}
	if(sTlkMdiSnkCtrl.handle != aclHandle){
		return;
	}

	sTlkMdiSnkCtrl.runTimer = clock_time()|1;
	
	uint32 param = pData[13] | (pData[14] << 8) | (pData[15] << 16);
	if(sTlkMdiSnkCtrl.firstInit || sTlkMdiSnkCtrl.encParma != param){
		uint sampleRate;
//		tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "==Reinit");
		tlkapi_qfifo_clear(&spTlkMdiSnkFifo);
		sampleRate = sTlkMdiSnkCtrl.sampleRate;
		tlkmdi_snk_firstInit(param, pData, dataLen);
		if(sTlkMdiSnkCtrl.sampleRate != sampleRate){
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_close();
			#if (TLKMDI_AUDSNK_DOUBLE_CHANNEL_ENABLE)
			tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_STEREO, TLKDEV_CODEC_BITDEPTH_16, sTlkMdiSnkCtrl.sampleRate);
			#else
			tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_SPK, TLKDEV_CODEC_CHANNEL_LEFT, TLKDEV_CODEC_BITDEPTH_16, sTlkMdiSnkCtrl.sampleRate);
			#endif
			tlkdev_codec_setSpkOffset(320);
			#endif
		}
		sTlkMdiSnkCtrl.encParma = param;
		sTlkMdiSnkCtrl.firstInit = false;
		#if (TLKMDI_AUDSNK_DOUBLE_CHANNEL_ENABLE)
		sTlkMdiSnkCtrl.decFunc = (tlkmdi_snk_decFunc)tlkalg_sbc_dec_stereo;
		#else
		sTlkMdiSnkCtrl.decFunc = (tlkmdi_snk_decFunc)tlkalg_sbc_dec_channel0;
		#endif
//		tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "==Init Param: %d %d %d %d", sTlkMdiSnkCtrl.sampleRate, sTlkMdiSnkCtrl.frameSize, sTlkMdiSnkCtrl.frameSample, 0);
	}
	
	if(pktNumber * sTlkMdiSnkCtrl.frameSize == dataLen-13){
		tlkmdi_snk_addRawFrame(pData[7], pData + 13, dataLen-13);
		if(sTlkMdiSnkCtrl.waitFrame != 0) sTlkMdiSnkCtrl.waitFrame--;
	}else{
		tlkapi_error(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "<enc: receive packet length error>: %d %d %d %d", sTlkMdiSnkCtrl.frameSize, dataLen, pktNumber, 0);
	}
}

int tlkmdi_snk_firstInit(uint32 param, uint08 *pData, int dataLen)
{
	tlkmdi_snk_parseSbcParam(pData+13);
	tlkmdi_snk_initFifo();
		
	return TLK_ENONE;
}



static int tlkmdi_snk_addRawFrame(uint08 id, uint08 *pData, int dataLen)
{
	uint08 *pBuffer;
	
	if(spTlkMdiSnkBuffer == NULL) return 0;
	
	if(sTlkMdiSnkCtrl.frameSize == 0) return -TLK_ENOREADY;
	if((dataLen % sTlkMdiSnkCtrl.frameSize) != 0){
		return -TLK_EPARAM;
	}
	
	while(dataLen >= sTlkMdiSnkCtrl.frameSize){
		pBuffer = tlkapi_qfifo_getBuff(&spTlkMdiSnkFifo);
		if(pBuffer == nullptr) break;
		tmemcpy(pBuffer, pData, sTlkMdiSnkCtrl.frameSize);
		pData   += sTlkMdiSnkCtrl.frameSize;
		dataLen -= sTlkMdiSnkCtrl.frameSize;
		tlkapi_qfifo_dropBuff(&spTlkMdiSnkFifo);
	}
	
	return TLK_ENONE;
}

static int tlkmdi_snk_parseSbcParam(uint08 *data)
{
	uint08 temp;
	uint08 sbcBlock;
	uint08 subBand;
	uint08 bitPool;
	uint16 frameSize;
	uint16 sampleRate;
	
	//read frequency from data
	temp = data[1] & TLKMDI_SNK_SBC_FREQ_MASK;
    if(temp == TLKMDI_SNK_SBC_FREQ_16000) sampleRate = 16000;
    else if(temp == TLKMDI_SNK_SBC_FREQ_32000) sampleRate = 32000;
    else if(temp == TLKMDI_SNK_SBC_FREQ_44100) sampleRate = 44100;
    else sampleRate = 48000;
    
	//read block length from data
	temp = data[1] & TLKMDI_SNK_SBC_BLOCK_MASK;
    if(temp == TLKMDI_SNK_SBC_BLOCK_4) sbcBlock = 4;
    else if(temp == TLKMDI_SNK_SBC_BLOCK_8) sbcBlock = 8;
    else if(temp == TLKMDI_SNK_SBC_BLOCK_12) sbcBlock = 12;
    else sbcBlock = 16; //TLKMDI_SNK_SBC_BLOCK_16
    
	//read subbands from data
	temp = data[1] & TLKMDI_SNK_SBC_SUBBAND_MASK;
    if(temp == TLKMDI_SNK_SBC_SUBBAND_4) subBand = 4;
    else subBand = 8; //TLKMDI_SNK_SBC_SUBBAND_8
	
	//read bitpool from data
	bitPool = data[2];
	
    //read channel mode from data&calculate frame length
	temp = data[1] & TLKMDI_SNK_SBC_TYPE_MASK;
	if(temp == TLKMDI_SNK_SBC_TYPE_MONO) frameSize = 4 + subBand/2 + (sbcBlock*bitPool)/8;
	else if (temp == TLKMDI_SNK_SBC_TYPE_DUAL_MODE) frameSize = 4 + subBand + (sbcBlock*bitPool)/4;
	else if (temp == TLKMDI_SNK_SBC_TYPE_STEREO) frameSize = 4 + subBand + (sbcBlock*bitPool)/8;
	else frameSize = 4 + subBand + (sbcBlock*bitPool + subBand)/8; //TLKMDI_SNK_SBC_TYPE_JOINT_STEREO

	sTlkMdiSnkCtrl.frameSize  = frameSize;
	sTlkMdiSnkCtrl.sampleRate = sampleRate;
	#if (TLKMDI_AUDSNK_DOUBLE_CHANNEL_ENABLE)
	sTlkMdiSnkCtrl.frameSample = 256;
	#else
	sTlkMdiSnkCtrl.frameSample = 128;
	#endif
	
	tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "<enc: sbc param - %d %d %d %d>", sampleRate, sbcBlock, subBand, bitPool);
	tlkapi_trace(TLKMDI_AUDSNK_DBG_FLAG, TLKMDI_AUDSNK_DBG_SIGN, "<enc: sbc fsize - %d>", frameSize);
		
	return TLK_ENONE;
}

static uint tlkmdi_snk_getPlaybackData(uint08 *pBuffer)
{
	uint08 *pData;
	uint16 outLen;

	if(tlkapi_qfifo_isEmpty(&spTlkMdiSnkFifo) || sTlkMdiSnkCtrl.decFunc == nullptr) return 0;
	
	//&spTlkMdiSnkFifo
	pData = tlkapi_qfifo_getData(&spTlkMdiSnkFifo);
	if(pData == nullptr){
		outLen = 0;
	}else{
		//ret =
		sTlkMdiSnkCtrl.decFunc(pData, sTlkMdiSnkCtrl.frameSample, pBuffer);
		tlkapi_qfifo_dropData(&spTlkMdiSnkFifo);
		outLen = sTlkMdiSnkCtrl.frameSample*2;
	}

	return outLen;
}



#endif //#if (TLK_MDI_AUDSNK_ENABLE)


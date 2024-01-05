/********************************************************************************************************
 * @file	tlkmmi_audioCtrl.c
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
#include "tlksys/prt/tlkpti_audio.h"
#include "tlkmmi_audioAdapt.h"
#include "tlkmmi_audio.h"
#include "tlkmmi_audioInfo.h"
#include "tlkmmi_audioMsgOuter.h"
#include "tlkmmi_audioMsgInner.h"
#include "tlkmmi_audioCtrl.h"
#include "tlkmmi_audioSch.h"
#include "tlkmmi_audioModinf.h"

#include "tlksys/prt/tlkpto_stdio.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmdi/aud/tlkmdi_audhfp.h"
#include "tlkmdi/aud/tlkmdi_audsnk.h"
#include "tlkmdi/aud/tlkmdi_audsrc.h"
#include "tlkmdi/aud/tlkmdi_audsco.h"
#include "tlkmdi/aud/tlkmdi_audmp3.h"
#include "tlkmdi/aud/tlkmdi_auduac.h"
#include "tlkmdi/aud/tlkmdi_audplay.h"
#include "tlkmdi/aud/tlkmdi_audtone.h"

#include "string.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"

#include "tlksys/tlksys_stdio.h"


static bool tlkmmi_audio_ctrlTimer(tlkapi_timer_t *pTimer, uint32 userArg);
static bool tlkmmi_audio_ctrlQueue(tlkapi_queue_t *pProcs, uint32 userArg);


extern uint08 gTlkMmiAudioCurOptype;
extern uint16 gTlkMmiAudioCurHandle;

tlkmmi_audio_ctrl_t gTlkMmiAudioCtrl;


/******************************************************************************
 * Function: tlkmmi_audio_ctrlInit
 * Descript: Initial the audio control info. 
 * Params: None.
 * Return: Return TLK_ENONE is success, other is false.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_ctrlInit(void)
{
	uint08 volume;
	uint08 enable;
	uint16 interval;

	tmemset(&gTlkMmiAudioCtrl, 0, sizeof(tlkmmi_audio_ctrl_t));
		
	tlkmmi_audio_adaptInitTimer(&gTlkMmiAudioCtrl.timer, tlkmmi_audio_ctrlTimer, NULL, TLKMMI_AUDIO_TIMEOUT);
	tlkmmi_audio_adaptInitQueue(&gTlkMmiAudioCtrl.procs, tlkmmi_audio_ctrlQueue, NULL);
	
	volume = 0;
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_TONE, &volume);
	tlkmdi_audio_setToneVolume(volume, false);
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_MUSIC, &volume);
	tlkmdi_audio_setMusicVolume(volume, false);
//	btp_avrcp_setSpkVolume(volume);
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_VOICE, &volume);
	tlkmdi_audio_setVoiceVolume(volume, false);
	tlkmdi_audio_infoGetVolume(TLKPRT_COMM_VOLUME_TYPE_HEADSET, &volume);
	tlkmdi_audio_setHeadsetVolume(volume, false);
	
	if(tlkmdi_audio_infoGetReport(&enable, &interval) == TLK_ENONE){
		gTlkMmiAudioCtrl.report.enable = enable;
		gTlkMmiAudioCtrl.report.interval = (interval*100)/TLKMMI_AUDIO_TIMEOUT_MS;
		gTlkMmiAudioCtrl.report.timeout = gTlkMmiAudioCtrl.report.interval;
	}

	return TLK_ENONE;
}

void tlkmmi_audio_validOptype(uint08 *pOptype, uint16 *pHandle)
{
#if (TLK_STK_BTP_ENABLE)
	uint16 srcHandle = 0;
	uint16 snkHandle = 0;
	#if (TLKBTP_CFG_A2DPSRC_ENABLE)
	srcHandle = btp_a2dp_getSrcHandle();
	snkHandle = btp_a2dp_getSnkHandle();
	#endif
	if(srcHandle == 0 && snkHandle == 0){
		if(pOptype != nullptr) *pOptype = TLKPTI_AUD_OPTYPE_PLAY;
		if(pHandle != nullptr) *pHandle = TLK_INVALID_HANDLE;
	}else{
		if(srcHandle != 0 && (snkHandle == 0 || TLKMMI_AUDIO_SRC_PRIORITY >= TLKMMI_AUDIO_SNK_PRIORITY)){
			if(pOptype != nullptr) *pOptype = TLKPTI_AUD_OPTYPE_SRC;
			if(pHandle != nullptr) *pHandle = srcHandle;
		}else{
			if(pOptype != nullptr) *pOptype = TLKPTI_AUD_OPTYPE_SNK;
			if(pHandle != nullptr) *pHandle = snkHandle;
		}
	}
#else
	*pOptype = 0;
	*pHandle = 0;
#endif
}

int tlkmmi_audio_extendPlay(uint16 index, uint08 offset)
{
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SNK 
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_NONE){
		uint08 optype;
		uint16 handle;
		tlkmmi_audio_validOptype(&optype, &handle);
		if(gTlkMmiAudioCurOptype != optype){
			uint32 param;
			param = 0x20000000 | index;
			if(offset != 0) param |= 0x10000000 | ((uint32)(offset & 0x7F) << 16);
			return tlkmmi_audio_modinfStart(optype, handle, param);
		}
		return TLK_ENONE;
	}
	return -TLK_EFAIL;
}

/******************************************************************************
 * Function: tlkmmi_audio_startPlay
 * Descript: Insert the music fileindex into the audio status control list,
 *           and switch to play this music.
 * Params:
 *         @fileIndex[IN]--The music file index to playing.
 * Return: true/false.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_startPlay(void)
{
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SNK 
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_NONE){
		uint08 optype;
		uint16 handle;
		tlkmmi_audio_validOptype(&optype, &handle);
		if(gTlkMmiAudioCurOptype != optype){
			return tlkmmi_audio_modinfStart(optype, handle, 0);
		}
		return TLK_ENONE;
	}
	return -TLK_EFAIL;
}

/******************************************************************************
 * Function: tlkmmi_audio_closePlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_closePlay(void)
{
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SNK 
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY){
		tlkmmi_audio_modinfClose(gTlkMmiAudioCurOptype, gTlkMmiAudioCurHandle);
	}
}

/******************************************************************************
 * Function: tlkmmi_audio_fastPlay
 * Descript: 
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_fastPlay(bool isRewind, bool isStart)
{
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_NONE) return false;
	return tlkmmi_audio_modinfFPlay(gTlkMmiAudioCurOptype, isRewind, isStart);
}

/******************************************************************************
 * Function: tlkmmi_audio_playNext
 * Descript: play next music.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_playNext(void)
{
	if(gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_NONE){
		return tlkmmi_audio_modinfToNext(gTlkMmiAudioCurOptype);
	}else{
		uint08 optype;
		uint16 handle;
		tlkmmi_audio_validOptype(&optype, &handle);
		if(tlkmmi_audio_modinfStart(optype, handle, 0x80000000) != TLK_ENONE) return false;
		return true;
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_playNext
 * Descript: play pre music.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_playPrev(void)
{
	if(gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_NONE){
		return tlkmmi_audio_modinfToPrev(gTlkMmiAudioCurOptype);
	}else{
		uint08 optype;
		uint16 handle;
		tlkmmi_audio_validOptype(&optype, &handle);
		if(tlkmmi_audio_modinfStart(optype, handle, 0x40000000) != TLK_ENONE) return false;
		return true;
	}
}

/******************************************************************************
 * Function: tlkmmi_audio_isLocalPlay
 * Descript: Checks whether the current playback is local.
 * Params: None.
 * Return: TRUE-local play, false-other play.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isLocalPlay(void)
{
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY) return true;
	else return false;
}
/******************************************************************************
 * Function: tlkmmi_audio_startLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_startLocalPlay(void)
{
	tlkmmi_audio_modinfStart(TLKPTI_AUD_OPTYPE_PLAY, TLK_INVALID_HANDLE, 0xFFFFFFFF);
}
/******************************************************************************
 * Function: tlkmmi_audio_stopLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_stopLocalPlay(void)
{
	tlkmmi_audio_modinfClose(TLKPTI_AUD_OPTYPE_PLAY, TLK_INVALID_HANDLE);
}


/******************************************************************************
 * Function: tlkmmi_audio_startTone
 * Descript: start the play tone.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_startTone(uint16 fileIndex, uint16 playCount)
{
	uint32 param = (((uint32)playCount)<<16) | fileIndex;
	return tlkmmi_audio_modinfStart(TLKPTI_AUD_OPTYPE_TONE, TLK_INVALID_HANDLE, param);
}

/******************************************************************************
 * Function: tlkmmi_audio_stopTone
 * Descript: stop the play tone.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_stopTone(void)
{
	tlkmmi_audio_modinfClose(TLKPTI_AUD_OPTYPE_TONE, TLK_INVALID_HANDLE);
}

/******************************************************************************
 * Function: tlkmmi_audio_getCurChannel
 * Descript: Get current audio channel.
 * Params: @channel[OUT]--The audio channel.
 * Return: Return the TLK_ENONE means success, other value is failure.
 * Others: None.
*******************************************************************************/
//int tlkmmi_audio_getCurChn(void)
int tlkmmi_audio_getCurChannel(uint08 *pChannel)
{
	tlkmmi_audio_schItem_t *pStatus;

	if(pChannel == nullptr) return -TLK_EPARAM;

	pStatus = tlkmmi_audio_getCurItem();
	if(pStatus == nullptr){
		*pChannel = TLKPRT_COMM_AUDIO_CHN_NONE;
	}else{
		tlkmmi_audio_optypeToChannel(pStatus->optype, pChannel);
	}
	if((*pChannel) == TLKPRT_COMM_AUDIO_CHN_NONE) return -TLK_EFAIL;
	return TLK_ENONE;
}

int tlkmmi_audio_getCurVolType(uint08 *pVolType)
{
	switch(gTlkMmiAudioCurOptype){
		case TLKPTI_AUD_OPTYPE_TONE:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_TONE;
			break;
		case TLKPTI_AUD_OPTYPE_HF:
		case TLKPTI_AUD_OPTYPE_AG:
		case TLKPTI_AUD_OPTYPE_SCO:
		case TLKPTI_AUD_OPTYPE_DSCO:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_VOICE;
			break;
		case TLKPTI_AUD_OPTYPE_SRC:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_HEADSET;
			break;
		case TLKPTI_AUD_OPTYPE_SNK:
		case TLKPTI_AUD_OPTYPE_PLAY:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_MUSIC;
			break;
		default:
			*pVolType = TLKPRT_COMM_VOLUME_TYPE_AUTO;
			break;
	}
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkmmi_audio_getVolume
 * Descript: Get audio volume.
 * Params: 
 *         @voltype[IN]--The audio channel.
 *         @pVolume[OUT]--The audio volume.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_getVolume(uint08 voltype, uint08 *pVolType)
{
	if(voltype > TLKPRT_COMM_VOLUME_TYPE_TONE) return 0;
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(pVolType != nullptr) *pVolType = voltype;
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		return tlkmdi_audio_getToneRawVolume();
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		return tlkmdi_audio_getVoiceRawVolume();
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_MUSIC){
		return tlkmdi_audio_getMusicRawVolume(false);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		return tlkmdi_audio_getHeadsetRawVolume();
	}else{
		return 0;
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_volumeInc
 * Descript: adjust audio channel increment.
 * Params: @channel[IN]--The audio channel.
 * Return: Return true means success, other value is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_volumeInc(uint08 voltype)
{
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		tlkmdi_audio_toneVolumeInc();
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		uint08 volume;
		tlkmdi_audio_voiceVolumeInc();
		volume = tlkmdi_audio_getVoiceBtpVolume();
		tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_HFP_VOLUME, &volume, 1);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		tlkmdi_audio_headsetVolumeInc();
		if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC){
			uint08 buffLen = 0;
			uint08 buffer[6];
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF);
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF00) >> 8;
			buffer[buffLen++] = 0x01;
			buffer[buffLen++] = tlkmdi_audio_getHeadsetBtpVolume(); //IOS Volume
			buffer[buffLen++] = tlkmdi_audio_getHeadsetBtpVolume(); //Android Volume
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_volumeInc: %d, %d", 
				buffer[3], buffer[4]);
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_AVRCP_VOLUME, buffer, buffLen);
		}
	}else{
		tlkmdi_audio_musicVolumeInc();
		if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SNK){
			uint08 buffLen = 0;
			uint08 buffer[6];
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF);
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF00) >> 8;
			buffer[buffLen++] = 0x00;
			buffer[buffLen++] = tlkmdi_audio_getMusicBtpVolume(true); //IOS Volume
			buffer[buffLen++] = tlkmdi_audio_getMusicBtpVolume(false); //Android Volume
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_volumeInc: %d, %d", 
				buffer[3], buffer[4]);
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_AVRCP_VOLUME, buffer, buffLen);
		}
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_volumeDec
 * Descript: adjust audio channel decrement.
 * Params: @channel[IN]--The audio channel.
 * Return: Return true means success, other value is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_volumeDec(uint08 voltype)
{
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		tlkmdi_audio_toneVolumeDec();
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		uint08 volume;
		tlkmdi_audio_voiceVolumeDec();
		volume = tlkmdi_audio_getVoiceBtpVolume();
		tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_HFP_VOLUME, &volume, 1);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		tlkmdi_audio_headsetVolumeDec();
		if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC){
			uint08 buffLen = 0;
			uint08 buffer[6];
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF);
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF00) >> 8;
			buffer[buffLen++] = 0x01;
			buffer[buffLen++] = tlkmdi_audio_getHeadsetBtpVolume();
			buffer[buffLen++] = tlkmdi_audio_getHeadsetBtpVolume();
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_volumeDec: %d, %d", 
				buffer[3], buffer[4]);
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_AVRCP_VOLUME, buffer, buffLen);
		}
	}else{
		tlkmdi_audio_musicVolumeDec();
		if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SNK){
			uint08 buffLen = 0;
			uint08 buffer[6];
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF);
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF00) >> 8;
			buffer[buffLen++] = 0x00;
			buffer[buffLen++] = tlkmdi_audio_getMusicBtpVolume(true);
			buffer[buffLen++] = tlkmdi_audio_getMusicBtpVolume(false);
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_volumeDec: %d, %d", 
				buffer[3], buffer[4]);
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_AVRCP_VOLUME, buffer, buffLen);
		}
	}
}
/******************************************************************************
 * Function: tlkmmi_audio_setVolume
 * Descript: Set audio volume.
 * Params: 
 *         @channel[IN]--The audio channel.
 *         @volume[IN]--The audio volume.
 * Return: Return true is success/other is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_setVolume(uint08 voltype, uint08 volume)
{
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_AUTO){
		tlkmmi_audio_getCurVolType(&voltype);
	}
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE){
		tlkmdi_audio_setToneVolume(volume, true);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE){
		tlkmdi_audio_setVoiceVolume(volume, true);
		volume = tlkmdi_audio_getVoiceBtpVolume();
		tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_HFP_VOLUME, &volume, 1);
	}else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET){
		tlkmdi_audio_setHeadsetVolume(volume, true);
		if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC){
			uint08 buffLen = 0;
			uint08 buffer[6];
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF);
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF00) >> 8;
			buffer[buffLen++] = 0x01;
			buffer[buffLen++] = tlkmdi_audio_getHeadsetBtpVolume();
			buffer[buffLen++] = tlkmdi_audio_getHeadsetBtpVolume();
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_AVRCP_VOLUME, buffer, buffLen);
		}
	}else{
		tlkmdi_audio_setMusicVolume(volume, true);
		if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SNK){
			uint08 buffLen = 0;
			uint08 buffer[6];
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF);
			buffer[buffLen++] = (gTlkMmiAudioCurHandle & 0xFF00) >> 8;
			buffer[buffLen++] = 0x00;
			buffer[buffLen++] = tlkmdi_audio_getMusicBtpVolume(true);
			buffer[buffLen++] = tlkmdi_audio_getMusicBtpVolume(false);
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_SET_AVRCP_VOLUME, buffer, buffLen);
		}
	}
}

/******************************************************************************
 * Function: tlkmdi_audio_setReport
 * Descript: Set audio volume.
 * Params: 
 *         @enable[IN]--enable the audio info report.
 *         @interval[IN]--The value How often report.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_setReport(uint08 enable, uint16 interval)
{
	if(enable == true){
		if(interval < 5 || interval > 1800) return -TLK_EPARAM;
	}
	if(tlkmdi_audio_infoSetReport(enable, interval) != TLK_ENONE) return -TLK_EFAIL;
	
	gTlkMmiAudioCtrl.report.enable = enable;
	if(enable){
		gTlkMmiAudioCtrl.report.interval = (interval*100)/TLKMMI_AUDIO_TIMEOUT_MS;
		gTlkMmiAudioCtrl.report.timeout = gTlkMmiAudioCtrl.report.interval;
	}
	tlkmmi_audio_adaptInsertTimer(&gTlkMmiAudioCtrl.timer);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_channelToOptype
 * Descript: channel relect to a optype.
 * Params: 
 *         @pChannel[IN]--The channel of audio info.
 *         @optype[IN]--the audio operate.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_channelToOptype(uint08 channel, uint08 *pOptype)
{
	uint08 optype;
	
	if(pOptype == nullptr) return -TLK_EPARAM;
	if(channel == TLKPRT_COMM_AUDIO_CHN_NONE){
		optype = gTlkMmiAudioCurOptype;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_PLAY){
		optype = TLKPTI_AUD_OPTYPE_PLAY;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_TONE){
		optype = TLKPTI_AUD_OPTYPE_TONE;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_SCO){
		optype = TLKPTI_AUD_OPTYPE_SCO;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_HFP_HF){
		optype = TLKPTI_AUD_OPTYPE_HF;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_HFP_AG){
		optype = TLKPTI_AUD_OPTYPE_AG;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_A2DP_SRC){
		optype = TLKPTI_AUD_OPTYPE_SRC;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_A2DP_SNK){
		optype = TLKPTI_AUD_OPTYPE_SNK;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_UAC){
		optype = TLKPTI_AUD_OPTYPE_UAC;
	}else if(channel == TLKPRT_COMM_AUDIO_CHN_DSCO){
		optype = TLKPTI_AUD_OPTYPE_DSCO;
	}else{
		optype = TLKPTI_AUD_OPTYPE_NONE;
	}
	*pOptype = optype;
	if(optype == TLKPTI_AUD_OPTYPE_NONE) return -TLK_EFAIL;
	else return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_optypeToChannel
 * Descript: optype relect to a Channel.
 * Params: 
 *         @optype[IN]--the audio operate.
 *         @pChannel[IN]--The channel of audio info.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_optypeToChannel(uint08 optype, uint08 *pChannel)
{
	uint08 channel;
	if(pChannel == nullptr) return -TLK_EPARAM;
	if(optype == TLKPTI_AUD_OPTYPE_PLAY){
		channel = TLKPRT_COMM_AUDIO_CHN_PLAY;
	}else if(optype == TLKPTI_AUD_OPTYPE_TONE){
		channel = TLKPRT_COMM_AUDIO_CHN_TONE;
	}else if(optype == TLKPTI_AUD_OPTYPE_SNK){
		channel = TLKPRT_COMM_AUDIO_CHN_A2DP_SNK;
	}else if(optype == TLKPTI_AUD_OPTYPE_SRC){
		channel = TLKPRT_COMM_AUDIO_CHN_A2DP_SRC;
	}else if(optype == TLKPTI_AUD_OPTYPE_HF){
		channel = TLKPRT_COMM_AUDIO_CHN_HFP_HF;
	}else if(optype == TLKPTI_AUD_OPTYPE_AG){
		channel = TLKPRT_COMM_AUDIO_CHN_HFP_AG;
	}else if(optype == TLKPTI_AUD_OPTYPE_SCO){
		channel = TLKPRT_COMM_AUDIO_CHN_SCO;
	}else if(optype == TLKPTI_AUD_OPTYPE_UAC){
		channel = TLKPRT_COMM_AUDIO_CHN_UAC;
	}else if(optype == TLKPTI_AUD_OPTYPE_DSCO){
		channel = TLKPRT_COMM_AUDIO_CHN_DSCO;
	}else{
		channel = TLKPRT_COMM_AUDIO_CHN_NONE;
	}
	*pChannel = channel;
	if(channel == TLKPRT_COMM_AUDIO_CHN_NONE) return -TLK_EFAIL;
	else return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_optypeChanged
 * Descript: Handle audio optype change.
 * Params: 
 *         @Newoptype[IN]--the new audio operate.
 *         @newHandle[IN]--The new handle of audio.
 *         @oldOptype[IN]--The old audio operate.
 *         @oldHandle[IN]--THe old handle of audio.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_optypeChanged(uint08 newOptype, uint16 newHandle, uint08 oldOptype, uint16 oldHandle)
{
	if(gTlkMmiAudioCurHandle == newHandle && gTlkMmiAudioCurOptype == newOptype) return;
	gTlkMmiAudioCurHandle = newHandle;
	gTlkMmiAudioCurOptype = newOptype;
	
	if((gTlkMmiAudioCurHandle != 0) && (gTlkMmiAudioCurOptype != 0)) {
		tlkmmi_audio_startHandler();
		tlkmmi_audio_adaptInsertTimer(&gTlkMmiAudioCtrl.timer);
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_optypeChanged: gTlkMmiAudioCurOptype -%d", gTlkMmiAudioCurOptype);

		tlkmmi_audio_infoSave();
	}else {
		tlkmmi_audio_closeHandler();
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_optypeChanged: close -%d", gTlkMmiAudioCurOptype);
	}
	
}



static bool tlkmmi_audio_ctrlTimer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	bool isBusy = false;
	if(gTlkMmiAudioCtrl.report.enable && (gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY 
		|| gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC)){
		if(gTlkMmiAudioCtrl.report.timeout != 0) gTlkMmiAudioCtrl.report.timeout --;
		if(gTlkMmiAudioCtrl.report.timeout == 0){
			gTlkMmiAudioCtrl.report.timeout = gTlkMmiAudioCtrl.report.interval;
			gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
			tlkmmi_audio_adaptAppendQueue(&gTlkMmiAudioCtrl.procs);
		}
		isBusy = true;
	}
	
	if(tlkmmi_audio_infoIsChange()){
		if(tlkmmi_audio_infoIsUpdate()){
			tlkmmi_audio_infoSave();
		}
		isBusy = true;
	}
	return isBusy;
}
static bool tlkmmi_audio_ctrlQueue(tlkapi_queue_t *pProcs, uint32 userArg)
{
	if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE) != 0){
		tlkmmi_audio_sendStatusChangeEvt();
	}
	if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_VOLUME_CHANGE) != 0){
		tlkmmi_audio_sendVolumeChangeEvt();
	}
	
	if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_PROGR100) != 0){
		if(tlkmmi_audio_sendProgressEvt(gTlkMmiAudioCurOptype, 1000) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGR100;
		}
	}else if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE) != 0){
		tlkmmi_audio_sendSongChangeEvt();
	}else if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_PROGR000) != 0){
		if(tlkmmi_audio_sendProgressEvt(gTlkMmiAudioCurOptype, 0) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGR000;
		}
	}else if((gTlkMmiAudioCtrl.report.busys & TLKMMI_AUDIO_REPORT_BUSY_PROGRESS) != 0){
		#if (TLK_MDI_MP3_ENABLE)
		if(tlkmmi_audio_sendProgressEvt(gTlkMmiAudioCurOptype, tlkmdi_mp3_getProgress()) == TLK_ENONE){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
		}
		#else
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
		#endif
	}
	
	if(gTlkMmiAudioCtrl.report.busys != TLKMMI_AUDIO_REPORT_BUSY_NONE) return true;
	return false;
}


int tlkmmi_audio_sendProgressEvt(uint08 optype, uint16 progress)
{
	uint08 channel;
	uint08 buffLen;
	uint08 buffer[4];
	
	tlkmmi_audio_optypeToChannel(optype, &channel);
	if(channel == TLKPRT_COMM_AUDIO_CHN_NONE) channel = 0;

	buffLen = 0;
	buffer[buffLen++] = channel;
	buffer[buffLen++] = (progress & 0xFF);
	buffer[buffLen++] = (progress & 0xFF00) >> 8;
	return tlkmmi_audio_sendCommEvt(TLKPRT_COMM_EVTID_AUDIO_PROGRESS_REPORT, buffer, buffLen);
}
void tlkmmi_audio_sendSongChangeEvt(void)
{
	int ret;
	uint08 length = 0;
	uint08 codec = 0;
	uint08 *pName = nullptr;
	uint08 buffLen;
	uint08 buffer[86];

//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_sendSongChangeEvt 01");
	buffLen = 0;
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_PLAY){
		buffer[buffLen++] = TLKPRT_COMM_AUDIO_CHN_PLAY;
	}else if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_SRC){
		buffer[buffLen++] = TLKPRT_COMM_AUDIO_CHN_A2DP_SRC;
	}else{
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
		return;
	}
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_sendSongChangeEvt 02");
	buffer[buffLen++] = gTlkMmiAudioCtrl.report.newIndex & 0xFF;
	buffer[buffLen++] = (gTlkMmiAudioCtrl.report.newIndex & 0xFF00) >> 8;
	#if (TLK_MDI_MP3_ENABLE)
	if(!tlkmdi_mp3_isEnable()){
		length = 0;
	}else{
		pName = tlkmdi_mp3_getFileName(&length);
		codec = tlkmdi_mp3_getFNameCode();
	}
	#else
	length = 0;
	#endif
	if(length > 80) length = 80;
	buffer[buffLen++] = length;
	buffer[buffLen++] = codec;
	if(length != 0){
		tmemcpy(buffer+buffLen, pName, length);
		buffLen += length;
	}

	ret = tlkmmi_audio_sendCommEvt(TLKPRT_COMM_EVTID_AUDIO_SONG_CHANGE, buffer, buffLen);
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_sendSongChangeEvt 03");
	if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
		if(gTlkMmiAudioCtrl.report.enable) gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_PROGR000;
	}else{
		gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
	}
	tlkmmi_audio_adaptAppendQueue(&gTlkMmiAudioCtrl.procs);
}
void tlkmmi_audio_sendStatusChangeEvt(void)
{
	uint08 buffer[2];
	if(gTlkMmiAudioCtrl.report.statusChn0 != 0){
		int ret;
		buffer[0] = gTlkMmiAudioCtrl.report.statusChn0;
		buffer[1] = gTlkMmiAudioCtrl.report.statusVal0;
		ret = tlkmmi_audio_sendCommEvt(TLKPRT_COMM_EVTID_AUDIO_STATUS_CHANGE, buffer, 2);
		if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE;
			gTlkMmiAudioCtrl.report.statusChn0 = gTlkMmiAudioCtrl.report.statusChn1;
			gTlkMmiAudioCtrl.report.statusVal0 = gTlkMmiAudioCtrl.report.statusVal1;
			gTlkMmiAudioCtrl.report.statusChn1 = 0;
			gTlkMmiAudioCtrl.report.statusVal1 = 0;
		}
	}
	if(gTlkMmiAudioCtrl.report.statusChn0 != 0){
		int ret;
		buffer[0] = gTlkMmiAudioCtrl.report.statusChn0;
		buffer[1] = gTlkMmiAudioCtrl.report.statusVal0;
		ret = tlkmmi_audio_sendCommEvt(TLKPRT_COMM_EVTID_AUDIO_STATUS_CHANGE, buffer, 2);
		if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
			gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE;
			gTlkMmiAudioCtrl.report.statusChn0 = 0;
			gTlkMmiAudioCtrl.report.statusVal0 = 0;
		}
	}
	if(gTlkMmiAudioCtrl.report.statusChn0 != 0){
		gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE;
		tlkmmi_audio_adaptAppendQueue(&gTlkMmiAudioCtrl.procs);
	}
}
void tlkmmi_audio_sendVolumeChangeEvt(void)
{
	int ret;
	uint08 buffer[2];
	buffer[0] = gTlkMmiAudioCtrl.report.volType;
	buffer[1] = gTlkMmiAudioCtrl.report.volValue;
	ret = tlkmmi_audio_sendCommEvt(TLKPRT_COMM_EVTID_AUDIO_VOLUME_CHANGE, buffer, 2);
	if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
		gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_VOLUME_CHANGE;
	}else{
		gTlkMmiAudioCtrl.report.busys|= TLKMMI_AUDIO_REPORT_BUSY_VOLUME_CHANGE;
		tlkmmi_audio_adaptAppendQueue(&gTlkMmiAudioCtrl.procs);
	}
}


#endif //#if (TLKMMI_AUDIO_ENABLE)


/********************************************************************************************************
 * @file	tlkmmi_audioMsgOuter.c
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
#include "tlkmmi_audio.h"
#include "tlkmmi_audioCtrl.h"
#include "tlkmmi_audioSch.h"
#include "tlkmmi_audioMsgOuter.h"
#include "tlkmmi_audioMsgInner.h"

#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmdi/aud/tlkmdi_audhfp.h"
#include "tlkmdi/aud/tlkmdi_audsnk.h"
#include "tlkmdi/aud/tlkmdi_audsrc.h"
#include "tlkmdi/aud/tlkmdi_audsco.h"
#include "tlkmdi/aud/tlkmdi_audmp3.h"
#include "tlkmdi/aud/tlkmdi_auduac.h"
#include "tlkmdi/aud/tlkmdi_audplay.h"
#include "tlkmdi/aud/tlkmdi_audtone.h"


static void tlkmmi_audio_playStartDeal(void);
static void tlkmmi_audio_playCloseDeal(void);
static void tlkmmi_audio_extendPlayDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_playNextDeal(void);
static void tlkmmi_audio_playPrevDeal(void);
static void tlkmmi_audio_fastForwardDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_fastRewindDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_getStateDeal(void);
static void tlkmmi_audio_getProgressDeal(void);
static void tlkmmi_audio_getDurationDeal(void);
static void tlkmmi_audio_getFileNameDeal(void);
static void tlkmmi_audio_getSingerDeal(void);
static void tlkmmi_audio_getPlayModeDeal(void);
static void tlkmmi_audio_setPlayModeDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_getVolumeDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_setVolumeDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_incVolumeDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_decVolumeDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_startReportDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_closeReportDeal(void);


extern uint08 gTlkMmiAudioCurOptype;



int tlkmmi_audio_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen)
{
	if(msgID == TLKPRT_COMM_CMDID_AUDIO_PLAY_START){
		tlkmmi_audio_playStartDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_PLAY_CLOSE){
		tlkmmi_audio_playCloseDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_EXTEND_PLAY){
		tlkmmi_audio_extendPlayDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_PLAY_NEXT){
		tlkmmi_audio_playNextDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_PLAY_PREV){
		tlkmmi_audio_playPrevDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_FAST_FORWARD){
		tlkmmi_audio_fastForwardDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_FAST_REWIND){
		tlkmmi_audio_fastRewindDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_GET_STATE){
		tlkmmi_audio_getStateDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_GET_PROGRESS){
		tlkmmi_audio_getProgressDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_GET_DURATION){
		tlkmmi_audio_getDurationDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_GET_FILENAME){
		tlkmmi_audio_getFileNameDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_GET_SINGER){
		tlkmmi_audio_getSingerDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_GET_PLAY_MODE){
		tlkmmi_audio_getPlayModeDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_SET_PLAY_MODE){
		tlkmmi_audio_setPlayModeDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_GET_VOLUME){
		tlkmmi_audio_getVolumeDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_SET_VOLUME){
		tlkmmi_audio_setVolumeDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_INC_VOLUME){
		tlkmmi_audio_incVolumeDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_DEC_VOLUME){
		tlkmmi_audio_decVolumeDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_START_REPORT){
		tlkmmi_audio_startReportDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_AUDIO_CLOSE_REPORT){
		tlkmmi_audio_closeReportDeal();
	}else{
		tlkmmi_audio_sendCommRsp(msgID, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
	return TLK_ENONE;
}


static void tlkmmi_audio_playStartDeal(void)
{
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_PLAY_START, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, nullptr, 0);
	tlkmmi_audio_startPlay();
}
static void tlkmmi_audio_playCloseDeal(void)
{
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_PLAY_CLOSE, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, nullptr, 0);
	tlkmmi_audio_closePlay();
}
static void tlkmmi_audio_extendPlayDeal(uint08 *pData, uint08 dataLen)
{
	uint16 index;
	uint08 offset;
	
	if(dataLen < 3){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_EXTEND_PLAY, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EPARAM, nullptr, 0);
		return;
	}
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_EXTEND_PLAY, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, nullptr, 0);
	index = ((uint16)pData[1] << 8) | pData[0];
	offset = pData[1];
	tlkmmi_audio_extendPlay(index, offset);
}
static void tlkmmi_audio_playNextDeal(void)
{
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_PLAY_NEXT, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, nullptr, 0);
	tlkmmi_audio_playNext();
}
static void tlkmmi_audio_playPrevDeal(void)
{
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_PLAY_PREV, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, nullptr, 0);
	tlkmmi_audio_playPrev();
}
static void tlkmmi_audio_fastForwardDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen == 0){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_FAST_FORWARD, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EPARAM, nullptr, 0);
		return;
	}
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_FAST_FORWARD, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, nullptr, 0);
	tlkmmi_audio_fastPlay(false, pData[0]);
}
static void tlkmmi_audio_fastRewindDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen == 0){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_FAST_REWIND, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EPARAM, nullptr, 0);
		return;
	}
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_FAST_REWIND, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, nullptr, 0);
	tlkmmi_audio_fastPlay(true, pData[0]);
}
static void tlkmmi_audio_getStateDeal(void)
{
	uint08 channel;
	uint08 buffLen;
	uint08 buffer[4];

	tlkmmi_audio_getCurChannel(&channel);

	buffLen = 0;
	buffer[buffLen++] = channel;
	if(gTlkMmiAudioCurOptype == TLKPTI_AUD_OPTYPE_NONE){
		buffer[buffLen++] = 0; //close
	}else{
		buffer[buffLen++] = 1; //start
	}
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_STATE, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_audio_getProgressDeal(void)
{
	uint08 channel;
	uint32 progress;
	uint08 buffLen;
	uint08 buffer[4];

	#if (TLK_MDI_MP3_ENABLE)
	progress = tlkmdi_mp3_getProgress();
	#else
	progress = 0;
	#endif
	tlkmmi_audio_getCurChannel(&channel);
	
	buffLen = 0;
	buffer[buffLen++] = channel;
	buffer[buffLen++] = (progress & 0xFF);
	buffer[buffLen++] = (progress & 0xFF00) >> 8;
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_PROGRESS, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_audio_getDurationDeal(void)
{
	uint32 duration;
	uint08 buffLen;
	uint08 buffer[4];
	
	#if (TLK_MDI_MP3_ENABLE)
	if(gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_PLAY && gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_SRC){
		duration = 0;
	}else if(!tlkmdi_mp3_isEnable()){
		duration = 0;
	}else{
		duration = tlkmdi_mp3_getDuration();
	}
	#else
	duration = 0;
	#endif
	
	buffLen = 0;
	buffer[buffLen++] = (duration & 0xFF);
	buffer[buffLen++] = (duration & 0xFF00) >> 8;
	buffer[buffLen++] = (duration & 0xFF0000) >> 16;
	buffer[buffLen++] = (duration & 0xFF000000) >> 24;
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_DURATION, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_audio_getFileNameDeal(void)
{
	uint08 length = 0;
	uint08 codec = 0;
	uint08 *pName = nullptr;
	uint08 buffLen;
	uint08 buffer[86];

	#if (TLK_MDI_MP3_ENABLE)
	if(gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_PLAY && gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_SRC){
		length = 0;
	}else if(!tlkmdi_mp3_isEnable()){
		length = 0;
	}else{
		pName = tlkmdi_mp3_getFileName(&length);
		codec = tlkmdi_mp3_getFNameCode();
	}
	#else
	length = 0;
	#endif
	if(length > 85) length = 85;

	buffLen = 0;
	buffer[buffLen++] = length;
	buffer[buffLen++] = codec;
	if(length != 0){
		tmemcpy(buffer+buffLen, pName, length);
		buffLen += length;
	}
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_FILENAME, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_audio_getSingerDeal(void)
{
	uint08 length = 0;
	uint08 codec = 0;
	uint08 *pName = nullptr;
	uint08 buffLen;
	uint08 buffer[86];
	
	#if (TLK_MDI_MP3_ENABLE)
	if(gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_PLAY && gTlkMmiAudioCurOptype != TLKPTI_AUD_OPTYPE_SRC){
		length = 0;
	}else if(!tlkmdi_mp3_isEnable()){
		length = 0;
	}else{
		pName = tlkmdi_mp3_getSinger(&length);
		codec = tlkmdi_mp3_getSingerCode();
	}
	#else
	length = 0;
	#endif
	if(length > 85) length = 85;

	buffLen = 0;
	buffer[buffLen++] = length;
	buffer[buffLen++] = codec;
	if(length != 0){
		tmemcpy(buffer+buffLen, pName, length);
		buffLen += length;
	}
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_SINGER, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_audio_getPlayModeDeal(void)
{
	uint08 buffLen;
	uint08 buffer[4];
	
	buffLen = 0;
	#if (TLK_MDI_MP3_ENABLE)
	buffer[buffLen++] = tlkmdi_mp3_getPlayMode();
	#else
	buffer[buffLen++] = 0;
	#endif
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_PLAY_MODE, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_audio_setPlayModeDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_MDI_MP3_ENABLE)
	uint08 playMode;
#endif

	if(dataLen < 1){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_SET_PLAY_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
#if (TLK_MDI_MP3_ENABLE)
	playMode  = pData[0];
	if(playMode >= TLKMDI_MP3_PALY_MODE_MAX){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_SET_PLAY_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EPARAM, nullptr, 0);
		return;
	}

	tlkmdi_mp3_setPlayMode(playMode);
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_SET_PLAY_MODE, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, nullptr, 0);
#else
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_SET_PLAY_MODE, TLKPRT_COMM_RSP_STATUE_FAILURE,
		TLK_ENOSUPPORT, nullptr, 0);
#endif
}
static void tlkmmi_audio_getVolumeDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 1){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_VOLUME, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}else{
		uint08 volume;
		uint08 volType;
		uint08 buffLen;
		uint08 buffer[4];
		buffLen = 0;
		volType = pData[0];
		volume = tlkmmi_audio_getVolume(volType, &volType);
		buffer[buffLen++] = volType;
		buffer[buffLen++] = volume;
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_GET_VOLUME, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
	}
}
static void tlkmmi_audio_setVolumeDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 2){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_SET_VOLUME, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}else{
		uint08 volType = pData[0];
		uint08 volValue = pData[1];
		tlkmmi_audio_setVolume(volType, volValue);
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_SET_VOLUME, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_audio_incVolumeDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 1){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_INC_VOLUME, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}else{
		uint08 volType = pData[0];
		tlkmmi_audio_volumeInc(volType);
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_INC_VOLUME, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_audio_decVolumeDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 1){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_DEC_VOLUME, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}else{
		uint08 volType = pData[0];
		tlkmmi_audio_volumeDec(volType);
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_DEC_VOLUME, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_audio_startReportDeal(uint08 *pData, uint08 dataLen)
{
	uint16 interval;
	
	interval = (((uint16)pData[1])<<8)|pData[0];
	if(dataLen < 2){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_START_REPORT, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	if(interval > 1800 || interval < 10){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_START_REPORT, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EPARAM, nullptr, 0);
		return;
	}

	if(tlkmdi_audio_setReport(true, interval) != TLK_ENONE){
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_START_REPORT, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFAIL, nullptr, 0);
	}else{
		tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_START_REPORT, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_audio_closeReportDeal(void)
{
	tlkmdi_audio_setReport(false, 0);
	tlkmmi_audio_sendCommRsp(TLKPRT_COMM_CMDID_AUDIO_CLOSE_REPORT, TLKPRT_COMM_RSP_STATUE_SUCCESS,
		TLK_ENONE, nullptr, 0);	
}


#endif //#if (TLKMMI_AUDIO_ENABLE)


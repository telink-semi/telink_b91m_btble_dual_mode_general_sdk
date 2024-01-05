/********************************************************************************************************
 * @file	tlkmmi_audioMsgInner.c
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
#include "tlksys/prt/tlkpti_stdio.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_audio.h"
#include "tlkmmi_audioAdapt.h"
#include "tlkmmi_audioCtrl.h"
#include "tlkmmi_audioInfo.h"
#include "tlkmmi_audioModinf.h"
#include "tlkmmi_audioSch.h"
#include "tlkmmi_audioMsgInner.h"
#include "tlkmmi_audioMsgOuter.h"

#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkmdi/aud/tlkmdi_audio.h"

static void tlkmmi_audio_startEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_closeEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_playStartEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_playOverEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_volumeChangeEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_statusChangeEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_startToneCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_btConnectEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_btDisconnEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_hfpChgVolumeEvtDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_audio_avrcpChgVolumeEvtDeal(uint08 *pData, uint08 dataLen);

extern tlkmmi_audio_ctrl_t gTlkMmiAudioCtrl;


/******************************************************************************
 * Function: tlkmmi_audio_sendCommXxx.
 * Descript: send the Audio command or Response or Event.
 * Params:
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_AUDIO;
	head[headLen++] = cmdID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_audio_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_AUDIO;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_audio_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_AUDIO;
	head[headLen++] = evtID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}



int tlkmmi_audio_innerMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_msgCallback: minorID-%d", msgID);
	if(msgID == TLKPTI_AUD_MSGID_START_EVT){
		tlkmmi_audio_startEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_CLOSE_EVT){
		tlkmmi_audio_closeEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_PLAY_START_EVT){
		tlkmmi_audio_playStartEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_PLAY_CLOSE_EVT){
		tlkmmi_audio_playOverEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_VOLUME_CHANGE_EVT){
		tlkmmi_audio_volumeChangeEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_STATUS_CHANGE_EVT){
		tlkmmi_audio_statusChangeEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_START_TONE_CMD){
		tlkmmi_audio_startToneCmdDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_BT_CONNECT_EVT){
		tlkmmi_audio_btConnectEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_BT_DISCONN_EVT){
		tlkmmi_audio_btDisconnEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_HFP_CHG_VOLUME_EVT){
		tlkmmi_audio_hfpChgVolumeEvtDeal(pData, dataLen);
	}else if(msgID == TLKPTI_AUD_MSGID_AVRCP_CHG_VOLUME_EVT){
		tlkmmi_audio_avrcpChgVolumeEvtDeal(pData, dataLen);
	}
	
	if(gTlkMmiAudioCtrl.report.busys != TLKMMI_AUDIO_REPORT_BUSY_NONE){
		tlkmmi_audio_adaptAppendQueue(&gTlkMmiAudioCtrl.procs);
	}
	return TLK_ENONE;
}



static void tlkmmi_audio_startEvtDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint16 handle;
	uint08 optype;
	optype = pData[0];
	handle = ((uint16)pData[2]<<8) | pData[1];
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_startEvtDeal: optype-%d", optype);
	ret = tlkmmi_audio_insertItem(handle, optype);
	if(ret != TLK_ENONE && ret != -TLK_EREPEAT){
		tlkapi_error(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_startEvtDeal: failure - optype-%d", optype);
		tlkmmi_audio_modinfClose(optype, handle);
	}
}
static void tlkmmi_audio_closeEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint16 handle;
	uint08 optype;
	optype = pData[0];
	handle = ((uint16)pData[2]<<8) | pData[1];
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_closeEvtDeal: optype-%d,handle-%d", optype, handle);
	tlkmmi_audio_removeItem(handle, optype);
}
static void tlkmmi_audio_playStartEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint16 index = ((uint16)pData[2]<<8)|pData[1];
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_playStartEvtDeal: index-%d", index);
	gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
	gTlkMmiAudioCtrl.report.newIndex = index;
	gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE;
}
static void tlkmmi_audio_playOverEvtDeal(uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_playOverEvtDeal");
	if(gTlkMmiAudioCtrl.report.enable) gTlkMmiAudioCtrl.report.busys |= TLKMMI_AUDIO_REPORT_BUSY_PROGR100;
	gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGR000;
	gTlkMmiAudioCtrl.report.busys &= ~TLKMMI_AUDIO_REPORT_BUSY_PROGRESS;
	gTlkMmiAudioCtrl.report.newIndex = 0;
}
static void tlkmmi_audio_volumeChangeEvtDeal(uint08 *pData, uint08 dataLen)
{
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_volumeChangeEvtDeal");
	gTlkMmiAudioCtrl.report.volType = pData[0];
	gTlkMmiAudioCtrl.report.volValue = pData[1];
	tlkmmi_audio_sendVolumeChangeEvt();
	tlkmdi_audio_infoSetVolume(pData[0], pData[1]);
	tlkmmi_audio_adaptInsertTimer(&gTlkMmiAudioCtrl.timer);
}
static void tlkmmi_audio_statusChangeEvtDeal(uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_statusChangeEvtDeal");
	if(gTlkMmiAudioCtrl.report.statusChn0 == 0){
		gTlkMmiAudioCtrl.report.statusChn0 = pData[0];
		gTlkMmiAudioCtrl.report.statusVal0 = pData[1];
	}else if(gTlkMmiAudioCtrl.report.statusChn1 == 0){
		gTlkMmiAudioCtrl.report.statusChn1 = pData[0];
		gTlkMmiAudioCtrl.report.statusVal1 = pData[1];
	}
	tlkmmi_audio_sendStatusChangeEvt();
}
static void tlkmmi_audio_startToneCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint16 index;
	uint08 count;
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_startToneCmdDeal");
	index = ((uint16)pData[1]<<8)|pData[0];
	count = pData[2];
	tlkmmi_audio_startTone(index, count);
}
static void tlkmmi_audio_btConnectEvtDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_STK_BTP_ENABLE)
	uint16 handle;
	uint08 ptype;
	uint08 usrID;

	handle = ((uint16)pData[1] << 8) | pData[0];
	ptype  = pData[2];
	usrID  = pData[3];
	if(ptype == BTP_PTYPE_A2DP){
		int srcIndex;
		int snkIndex;
		int playIndex;
		int dstOptype;
		srcIndex = tlkmmi_audio_itemIndexByOptype(TLKPTI_AUD_OPTYPE_SRC);
		snkIndex = tlkmmi_audio_itemIndexByOptype(TLKPTI_AUD_OPTYPE_SNK);
		playIndex = tlkmmi_audio_itemIndex(TLK_INVALID_HANDLE, TLKPTI_AUD_OPTYPE_PLAY);
		if(playIndex >= 0){
			tlkmmi_audio_removeItem(TLK_INVALID_HANDLE, TLKPTI_AUD_OPTYPE_PLAY);
		}
		if(usrID == BTP_USRID_SERVER) dstOptype = TLKPTI_AUD_OPTYPE_SRC;
		else dstOptype = TLKPTI_AUD_OPTYPE_SNK;
		if(playIndex >= 0 && srcIndex < 0 && snkIndex < 0 && dstOptype == TLKPTI_AUD_OPTYPE_SRC){
			tlkmmi_audio_modinfStart(dstOptype, handle, 0);
		}
	}
#endif
}
static void tlkmmi_audio_btDisconnEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint16 handle = ((uint16)pData[1] << 8) | pData[0];
	tlkmmi_audio_removeItemByHandle(handle);
}
static void tlkmmi_audio_hfpChgVolumeEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 volume = pData[0];
	tlkmdi_audio_setVoiceBtpVolume(volume, true);
}
static void tlkmmi_audio_avrcpChgVolumeEvtDeal(uint08 *pData, uint08 dataLen)
{
	uint08 isIos = pData[2];
	uint08 isSrc = pData[3];
	uint08 volume = pData[4];
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_avrcpChgVolumeEvtDeal: isSrc[%d] isIos[%d] volume[%d]",
		isSrc, isIos, volume);
	if(isSrc){
		tlkmdi_audio_setHeadsetBtpVolume(volume, true);
	}else{
		tlkmdi_audio_setMusicBtpVolume(volume, isIos, true);
	}
}





#endif //#if (TLKMMI_AUDIO_ENABLE)


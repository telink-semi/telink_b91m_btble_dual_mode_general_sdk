/********************************************************************************************************
 * @file	tlkmdi_audio.c
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
#if (TLK_MDI_AUDIO_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"

#define TLKMDI_AUD_DBG_FLAG         ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD << 16) | TLK_DEBUG_DBG_FLAG_ALL) 
#define TLKMDI_AUD_DBG_SIGN         "[MDI]"


static uint08 sTlkMdiAudioToneVolumeIndex = TLKMDI_AUDIO_HEADSET_INDEX_DEF;
static const uint08 scTlkMdiAudioToneRawVolTable[16] = { 0, 7, 14, 20, 27, 34, 40, 47,  50,  60,  67,  74,  80,  87,  94,  100 };
static const uint16 scTlkMdiAudioToneCalVolTable[16] = { 0, 2,  5, 11, 20, 36, 58, 92, 147, 230, 328, 410, 512, 649, 812, 1024 };

static uint08 sTlkMdiAudioVoiceVolumeIndex = TLKMDI_AUDIO_VOICE_INDEX_DEF;
static const uint08 scTlkMdiAudioVoiceBtpVolTable[16] = { 0, 1,  2,  3,  4,  5,  6,  7,   8,   9,  10,  11,  12,  13,  14,   15 };
static const uint08 scTlkMdiAudioVoiceRawVolTable[16] = { 0, 7, 14, 20, 27, 34, 40, 47,  50,  60,  67,  74,  80,  87,  94,  100 };
static const uint16 scTlkMdiAudioVoiceCalVolTable[16] = { 0, 2,  5, 11, 20, 36, 58, 92, 147, 230, 328, 410, 512, 649, 812, 1024 };

//IOS[0~17], android[0~16]
//BTP-IOS  { 0, 7, 15, 23, 31, 39, 47, 55, 63, 71, 79, 87,  95, 103, 111, 119, 127 }
//BTP-AND  { 0, 8, 17, 25, 34, 42, 51, 59, 68, 76, 85, 93, 102, 110, 119, 127 }
//BTP-AND  { 0, 8, 16, 25, 33, 42, 50, 59, 67, 76, 84, 93, 101, 110, 118, 127 }
//RAW-IOS  { 0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 63, 69,  75,  81,  87,  94, 100}
//RAW-AND  { 0, 7, 13, 20, 27, 33, 40, 46, 53, 60, 67, 73,  80,  87,  93, 100}
//CAL-IOS  { 0, 7, 15, 23, 31, 39, 50, 75, 105, 138, 180, 230, 255, 350, 500, 700, 1024}
//CAL-AND  { 0, 7, 15, 23, 31, 39, 50, 75, 150, 180, 230, 255, 350, 500, 700, 1024}
//Merge the IOS-TABLE with the Android-TABLE. The calculation is based on the IOS table.
static const uint08 scTlkMdiAudioMusicBtpVolTable[34] = 
	{ 0, 0, 7, 8, 15, 16, 23, 25, 31, 33, 39, 42, 47, 50, 55, 59, 63, 67, 71, 76, 79, 84, 87, 93, 95, 101, 103, 110, 111, 118, 119, 127, 127, 127 };
static const uint08 scTlkMdiAudioMusicRawVolTable[34] = 
	{ 0, 0, 6, 7, 13, 13, 19, 20, 25, 27, 31, 33, 38, 40, 44, 46, 50, 53, 56, 60, 63, 67, 69, 73, 75,  80,  81,  87,  87,  93,  94, 100, 100, 100};
static const uint16 scTlkMdiAudioMusicCalVolTable[34] =
	{ 0, 0, 7, 7, 15, 15, 23, 23, 31, 31, 39, 39, 50, 50, 75, 75, 105, 150, 138, 180, 180, 230, 230, 255, 255, 350, 350, 500, 500, 700, 700, 1024, 1024, 1024};
static uint08 sTlkMdiAudioMusicVolumeIndex = TLKMDI_AUDIO_MUSIC_INDEX_DEF;

static uint08 sTlkMdiAudioHeadsetVolumeIndex = TLKMDI_AUDIO_HEADSET_INDEX_DEF;
static const uint08 scTlkMdiAudioHeadsetBtpVolTable[16] = { 0, 8, 16, 25, 33, 42, 50, 59, 67, 76, 84, 93, 101, 110, 118, 127 };
static const uint08 scTlkMdiAudioHeadsetRawVolTable[16] = { 0, 7, 14, 20, 27, 34, 40, 47, 50, 60, 67, 74,  80,  87,  94, 100 };
//static const uint16 scTlkMdiAudioHeadsetCalVolTable[16] = { 0, 2,  5, 11, 20, 36, 58, 92, 147, 230, 328, 410, 512, 649, 812, 1024 };
static const uint16 scTlkMdiAudioHeadsetCalVolTable[16] = { 0, 68, 137, 205, 273, 341, 410, 478, 546, 614, 682, 750, 819, 887, 956, 1024 };


int tlkmdi_audio_init(void)
{
	#if(TLK_MDI_AUDIO_ENABLE)
	tlkmdi_audmem_init();
	#endif
	#if (TLK_MDI_MP3_ENABLE)
	tlkmdi_mp3_init();
	#endif
	#if (TLK_MDI_AUDTONE_ENABLE)
	tlkmdi_audtone_init();
	#endif
	#if (TLK_MDI_AUDPLAY_ENABLE)
	tlkmdi_audplay_init();
	#endif
	#if (TLK_MDI_AUDHFP_ENABLE)
	tlkmdi_audhfp_init();
	#endif
	#if (TLK_MDI_AUDSCO_ENABLE)
	tlkmdi_audsco_init();
	#endif
	#if (TLK_MDI_AUDSRC_ENABLE)
	tlkmdi_audsrc_init();
	#endif
	#if (TLK_MDI_AUDSNK_ENABLE)
	tlkmdi_audsnk_init();
	#endif
	#if (TLK_MDI_AUDUAC_ENABLE)
	tlkmdi_auduac_init();
	#endif
	
    return TLK_ENONE;
}

int tlkmdi_audio_sendStartEvt(uint08 audChn, uint16 handle)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (handle & 0x00FF);
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	return tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_START_EVT, buffer, buffLen);
}
int tlkmdi_audio_sendCloseEvt(uint08 audChn, uint16 handle)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (handle & 0x00FF);
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	return tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_CLOSE_EVT, buffer, buffLen);
}

int tlkmdi_audio_sendPlayStartEvt(uint08 audChn, uint16 playIndex, bool isIrq)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (playIndex & 0x00FF);
	buffer[buffLen++] = (playIndex & 0xFF00) >> 8;
	if(!isIrq){
		return tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_PLAY_START_EVT, buffer, buffLen);
	}else{
		return tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_PLAY_START_EVT, buffer, buffLen);
	}
}
int tlkmdi_audio_sendPlayOverEvt(uint08 audChn, uint16 playIndex, bool isIrq)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = (playIndex & 0x00FF);
	buffer[buffLen++] = (playIndex & 0xFF00) >> 8;
	if(!isIrq){
		return tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_PLAY_CLOSE_EVT, buffer, buffLen);
	}else{
		return tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_PLAY_CLOSE_EVT, buffer, buffLen);
	}
}

int tlkmdi_audio_sendVolumeChangeEvt(uint08 audChn, uint08 volume)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = volume;
	return tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_VOLUME_CHANGE_EVT, buffer, buffLen);
}
int tlkmdi_audio_sendStatusChangeEvt(uint08 audChn, uint08 status)
{
	uint08 buffLen;
	uint08 buffer[4];
	buffLen = 0;
	buffer[buffLen++] = audChn;
	buffer[buffLen++] = status;
	return tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_STATUS_CHANGE_EVT, buffer, buffLen);
}




uint tlkmdi_audio_getToneRawVolume(void)
{
	return scTlkMdiAudioToneRawVolTable[sTlkMdiAudioToneVolumeIndex];
}
uint tlkmdi_audio_getToneCalVolume(void)
{
	return scTlkMdiAudioToneCalVolTable[sTlkMdiAudioToneVolumeIndex];
}
void tlkmdi_audio_setToneRawVolume(uint08 volume, bool isReport)
{
	uint08 index = tlkmdi_audio_getToneRawVolumeIndex(volume);
	if(index != sTlkMdiAudioToneVolumeIndex){
		sTlkMdiAudioToneVolumeIndex = index;
		if(isReport){
			tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, tlkmdi_audio_getToneRawVolume());
		}
	}
}
uint tlkmdi_audio_getToneVolume(void)
{
	return tlkmdi_audio_getToneRawVolume();
}
void tlkmdi_audio_toneVolumeInc(void)
{
	if(sTlkMdiAudioToneVolumeIndex >= 15){
		sTlkMdiAudioToneVolumeIndex = 15;
		return;
	}
	sTlkMdiAudioToneVolumeIndex ++;
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, tlkmdi_audio_getToneRawVolume());
}
void tlkmdi_audio_toneVolumeDec(void)
{
	if(sTlkMdiAudioToneVolumeIndex == 0) return;
	sTlkMdiAudioToneVolumeIndex --;
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_TONE, tlkmdi_audio_getToneRawVolume());
}
void tlkmdi_audio_setToneVolume(uint08 volume, bool isReport)
{
	tlkmdi_audio_setToneRawVolume(volume, isReport);
}
uint tlkmdi_audio_getToneRawVolumeIndex(uint08 rawVol)
{
	uint08 index;
	for(index=0; index<15; index++){
		if(rawVol >= scTlkMdiAudioToneRawVolTable[index] && rawVol < scTlkMdiAudioToneRawVolTable[index+1]) break;
	}
	return index;
}



uint tlkmdi_audio_getMusicBtpVolume(bool isIos)
{
	if(isIos) return scTlkMdiAudioMusicBtpVolTable[sTlkMdiAudioMusicVolumeIndex];
	else return scTlkMdiAudioMusicBtpVolTable[sTlkMdiAudioMusicVolumeIndex+1];
}
uint tlkmdi_audio_getMusicRawVolume(bool isIos)
{
	if(isIos) return scTlkMdiAudioMusicRawVolTable[sTlkMdiAudioMusicVolumeIndex];
	else return scTlkMdiAudioMusicRawVolTable[sTlkMdiAudioMusicVolumeIndex+1];
}
uint tlkmdi_audio_getMusicCalVolume(bool isIos)
{
	if(isIos) return scTlkMdiAudioMusicCalVolTable[sTlkMdiAudioMusicVolumeIndex];
	else return scTlkMdiAudioMusicCalVolTable[sTlkMdiAudioMusicVolumeIndex+1];
}
void tlkmdi_audio_setMusicRawVolume(uint08 volume, bool isReport)
{
	uint08 index = tlkmdi_audio_getMusicRawVolumeIndex(volume);
	if(index != sTlkMdiAudioMusicVolumeIndex){
		sTlkMdiAudioMusicVolumeIndex = index;
		if(isReport){
			tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, tlkmdi_audio_getMusicRawVolume(false));
		}
	}
}
void tlkmdi_audio_setMusicBtpVolume(uint08 volume, bool isIos, bool isReport)
{
	uint08 index = tlkmdi_audio_getMusicBtpVolumeIndex(volume, isIos);
	tlkapi_trace(TLKMDI_AUD_DBG_FLAG, TLKMDI_AUD_DBG_SIGN, "setMusicBtpVolume: volume[%d] index[%d-%d]", 
		volume, index, sTlkMdiAudioMusicVolumeIndex);
	if(index != sTlkMdiAudioMusicVolumeIndex){
		sTlkMdiAudioMusicVolumeIndex = index;
		if(isReport){
			tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, tlkmdi_audio_getMusicRawVolume(isIos));
		}
	}
}
void tlkmdi_audio_musicVolumeInc(void)
{
	if(sTlkMdiAudioMusicVolumeIndex >= 30){
		sTlkMdiAudioMusicVolumeIndex = 30;
		return;
	} 
	sTlkMdiAudioMusicVolumeIndex += 2;
	if(sTlkMdiAudioMusicVolumeIndex >= 30) sTlkMdiAudioMusicVolumeIndex = 30;
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, tlkmdi_audio_getMusicRawVolume(false));
}
void tlkmdi_audio_musicVolumeDec(void)
{
	if(sTlkMdiAudioMusicVolumeIndex == 0) return;
	if(sTlkMdiAudioMusicVolumeIndex == 1) sTlkMdiAudioMusicVolumeIndex = 0;
	else sTlkMdiAudioMusicVolumeIndex -= 2;	
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_MUSIC, tlkmdi_audio_getMusicRawVolume(false));
}
uint tlkmdi_audio_getMusicVolume(void)
{
	return tlkmdi_audio_getMusicRawVolume(false);
}
void tlkmdi_audio_setMusicVolume(uint08 volume, bool isReport)
{
	tlkmdi_audio_setMusicRawVolume(volume, isReport);
}
uint tlkmdi_audio_getMusicRawVolumeIndex(uint08 rawVol)
{
	uint08 index;
	for(index=0; index<31; index+=2){
		if(rawVol >= scTlkMdiAudioMusicRawVolTable[index] && rawVol < scTlkMdiAudioMusicRawVolTable[index+2]) break;
	}
	return index;
}
uint tlkmdi_audio_getMusicBtpVolumeIndex(uint08 btpVol, bool isIos)
{
	uint08 index;
	if(isIos){
		for(index=0; index<31; index+=2){
			if(btpVol >= scTlkMdiAudioMusicBtpVolTable[index] && btpVol < scTlkMdiAudioMusicBtpVolTable[index+2]) break;
		}
	}else{
		#if 1
		for(index=1; index<32; index+=2){
			if(btpVol >= scTlkMdiAudioMusicBtpVolTable[index] && btpVol < scTlkMdiAudioMusicBtpVolTable[index+2]) break;
		}
		index -= 1;
		#else
		if(btpVol < scTlkMdiAudioMusicBtpVolTable[3]){
			index = 0;
		}else{
			for(index=1; index<32; index+=2){
				if(btpVol >= scTlkMdiAudioMusicBtpVolTable[index] && btpVol < scTlkMdiAudioMusicBtpVolTable[index+2]) break;
			}
			if(index == 33) index = 32;
			else index += 1;
		}
		#endif
	}
	return index;
}



uint tlkmdi_audio_getVoiceBtpVolume(void)
{
	return scTlkMdiAudioVoiceBtpVolTable[sTlkMdiAudioVoiceVolumeIndex];
}
uint tlkmdi_audio_getVoiceRawVolume(void)
{
	return scTlkMdiAudioVoiceRawVolTable[sTlkMdiAudioVoiceVolumeIndex];
}
uint tlkmdi_audio_getVoiceCalVolume(void)
{
	return scTlkMdiAudioVoiceCalVolTable[sTlkMdiAudioVoiceVolumeIndex];
}
void tlkmdi_audio_setVoiceRawVolume(uint08 volume, bool isReport)
{
	uint08 index = tlkmdi_audio_getVoiceRawVolumeIndex(volume);
	if(index != sTlkMdiAudioVoiceVolumeIndex){
		sTlkMdiAudioVoiceVolumeIndex = index;
		if(isReport){
			tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, tlkmdi_audio_getVoiceRawVolume());
		}
	}
}
void tlkmdi_audio_setVoiceBtpVolume(uint08 volume, bool isReport)
{
	uint08 index = tlkmdi_audio_getVoiceBtpVolumeIndex(volume);
	if(index != sTlkMdiAudioVoiceVolumeIndex){
		sTlkMdiAudioVoiceVolumeIndex = index;
		if(isReport){
			tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, tlkmdi_audio_getVoiceRawVolume());
		}
	}
}
uint tlkmdi_audio_getVoiceVolume(void)
{
	return tlkmdi_audio_getVoiceRawVolume();
}
void tlkmdi_audio_voiceVolumeInc(void)
{
	if(sTlkMdiAudioVoiceVolumeIndex >= 15){
		sTlkMdiAudioVoiceVolumeIndex = 15;
		return;
	}
	sTlkMdiAudioVoiceVolumeIndex ++;
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, tlkmdi_audio_getVoiceRawVolume());
}
void tlkmdi_audio_voiceVolumeDec(void)
{
	if(sTlkMdiAudioVoiceVolumeIndex == 0) return;
	sTlkMdiAudioVoiceVolumeIndex --;
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_VOICE, tlkmdi_audio_getVoiceRawVolume());
}
void tlkmdi_audio_setVoiceVolume(uint08 volume, bool isReport)
{
	tlkmdi_audio_setVoiceRawVolume(volume, isReport);
}
uint tlkmdi_audio_getVoiceRawVolumeIndex(uint08 rawVol)
{
	uint08 index;
	for(index=0; index<15; index++){
		if(rawVol >= scTlkMdiAudioVoiceRawVolTable[index] && rawVol < scTlkMdiAudioVoiceRawVolTable[index+1]) break;
	}
	return index;
}
uint tlkmdi_audio_getVoiceBtpVolumeIndex(uint08 btpVol)
{
	uint08 index;
	for(index=0; index<15; index++){
		if(btpVol >= scTlkMdiAudioVoiceBtpVolTable[index] && btpVol < scTlkMdiAudioVoiceBtpVolTable[index+1]) break;
	}
	return index;
}


uint tlkmdi_audio_getHeadsetBtpVolume(void)
{
	return scTlkMdiAudioHeadsetBtpVolTable[sTlkMdiAudioHeadsetVolumeIndex];
}
uint tlkmdi_audio_getHeadsetRawVolume(void)
{
	return scTlkMdiAudioHeadsetRawVolTable[sTlkMdiAudioHeadsetVolumeIndex];
}
uint tlkmdi_audio_getHeadsetCalVolume(void)
{
	return scTlkMdiAudioHeadsetCalVolTable[sTlkMdiAudioHeadsetVolumeIndex];
}
void tlkmdi_audio_setHeadsetRawVolume(uint08 volume, bool isReport)
{
	uint08 index = tlkmdi_audio_getHeadsetRawVolumeIndex(volume);
	if(index != sTlkMdiAudioHeadsetVolumeIndex){
		sTlkMdiAudioHeadsetVolumeIndex = index;
		if(isReport){
			tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, tlkmdi_audio_getHeadsetRawVolume());
		}
	}
}
void tlkmdi_audio_setHeadsetBtpVolume(uint08 volume, bool isReport)
{
	uint08 index = tlkmdi_audio_getHeadsetBtpVolumeIndex(volume);
	if(index != sTlkMdiAudioHeadsetVolumeIndex){
		sTlkMdiAudioHeadsetVolumeIndex = index;
		if(isReport){
			tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, tlkmdi_audio_getHeadsetRawVolume());
		}
	}
}
uint tlkmdi_audio_getHeadsetVolume(void)
{
	return tlkmdi_audio_getHeadsetRawVolume();
}
void tlkmdi_audio_headsetVolumeInc(void)
{
	if(sTlkMdiAudioHeadsetVolumeIndex >= 15){
		sTlkMdiAudioHeadsetVolumeIndex = 15;
		return;
	}
	sTlkMdiAudioHeadsetVolumeIndex ++;
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, tlkmdi_audio_getHeadsetRawVolume());
}
void tlkmdi_audio_headsetVolumeDec(void)
{
	if(sTlkMdiAudioHeadsetVolumeIndex == 0) return;
	sTlkMdiAudioHeadsetVolumeIndex --;
	tlkmdi_audio_sendVolumeChangeEvt(TLKPRT_COMM_VOLUME_TYPE_HEADSET, tlkmdi_audio_getHeadsetRawVolume());
}
void tlkmdi_audio_setHeadsetVolume(uint08 volume, bool isReport)
{
	tlkmdi_audio_setHeadsetRawVolume(volume, isReport);
}
uint tlkmdi_audio_getHeadsetRawVolumeIndex(uint08 rawVol)
{
	uint08 index;
	for(index=0; index<15; index++){
		if(rawVol >= scTlkMdiAudioHeadsetRawVolTable[index] && rawVol < scTlkMdiAudioHeadsetRawVolTable[index+1]) break;
	}
	return index;
}
uint tlkmdi_audio_getHeadsetBtpVolumeIndex(uint08 btpVol)
{
	uint08 index;
	for(index=0; index<15; index++){
		if(btpVol >= scTlkMdiAudioHeadsetBtpVolTable[index] && btpVol < scTlkMdiAudioHeadsetBtpVolTable[index+1]) break;
	}
	return index;
}




#endif //#if (TLK_MDI_AUDIO_ENABLE)


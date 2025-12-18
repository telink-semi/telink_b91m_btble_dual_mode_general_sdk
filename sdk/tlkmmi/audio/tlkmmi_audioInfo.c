/********************************************************************************************************
 * @file	tlkmmi_audioInfo.c
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
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmmi_audio.h"
#include "tlkmmi_audioCtrl.h"
#include "tlkmmi_audioInfo.h"


tlkmmi_audio_infoCtrl_t gTlkMmiAudioInfoCtrl;


/******************************************************************************
 * Function: tlkmmi_audio_infoInit
 * Descript: Initial the audio info.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_infoInit(void)
{
	tmemset(&gTlkMmiAudioInfoCtrl, 0, sizeof(tlkmmi_audio_infoCtrl_t));
	
	tlkapi_save2_init(&gTlkMmiAudioInfoCtrl.save, TLKMDI_AUDIO_VOLUME_SAVE_SIGN, TLKMDI_AUDIO_VOLUME_SAVE_VERS,
		TLKMDI_AUDIO_VOLUME_SAVE_SIZE, TLKMDI_AUDIO_VOLUME_SAVE_ADDR);

	gTlkMmiAudioInfoCtrl.item.tone = 60;
	gTlkMmiAudioInfoCtrl.item.music = 60;
	gTlkMmiAudioInfoCtrl.item.voice = 60;
	gTlkMmiAudioInfoCtrl.item.headset = 60;
	gTlkMmiAudioInfoCtrl.item.resv001 = 0xFF;
	gTlkMmiAudioInfoCtrl.item.enReport = false;
	gTlkMmiAudioInfoCtrl.item.interval = 0;
	gTlkMmiAudioInfoCtrl.item.resv002[0] = 0xFF;
	gTlkMmiAudioInfoCtrl.item.resv002[1] = 0xFF;
	gTlkMmiAudioInfoCtrl.item.resv002[2] = 0xFF;
	gTlkMmiAudioInfoCtrl.item.resv002[3] = 0xFF;
	
	tlkmmi_audio_infoLoad();
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_infoIsChange
 * Descript: Check whether audio info has changed.
 * Params:
 * Return: Operating results. TRUE means changed, others means no changed.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_infoIsChange(void)
{
	return gTlkMmiAudioInfoCtrl.isChange;
}

/******************************************************************************
 * Function: tlkmmi_audio_infoIsUpdate
 * Descript: Check whether audio info is need update.
 * Params:
 * Return: Operating results. TRUE means changed, others means no changed.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_infoIsUpdate(void)
{
	if(gTlkMmiAudioInfoCtrl.interval != 0) gTlkMmiAudioInfoCtrl.interval--;
	if(gTlkMmiAudioInfoCtrl.interval == 0) return true;
	return false;
}


/******************************************************************************
 * Function: tlkmmi_audio_infoLoad
 * Descript: load the audio info.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_infoLoad(void)
{
	uint08 volume[TLKMDI_AUDIO_VOLUME_SAVE_SIZE];
	
	if(tlkapi_save2_load(&gTlkMmiAudioInfoCtrl.save, volume, TLKMDI_AUDIO_VOLUME_SAVE_SIZE) <= 0){
		return -TLK_EFAIL;
	}
	
	gTlkMmiAudioInfoCtrl.item.tone = volume[0] > 100 ? 100 : volume[0];
	gTlkMmiAudioInfoCtrl.item.music = volume[1] > 100 ? 100 : volume[1];
	gTlkMmiAudioInfoCtrl.item.voice = volume[2] > 100 ? 100 : volume[2];
	gTlkMmiAudioInfoCtrl.item.headset = volume[3] > 100 ? 100 : volume[3];
	gTlkMmiAudioInfoCtrl.item.resv001 = 0xFF;
	gTlkMmiAudioInfoCtrl.item.enReport = volume[5];
	gTlkMmiAudioInfoCtrl.item.interval = ((uint16)volume[7] << 8) | volume[6];
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_infoSave
 * Descript: save the audio info.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_infoSave(void)
{
	uint08 volume[TLKMDI_AUDIO_VOLUME_SAVE_SIZE];
	
	if(!gTlkMmiAudioInfoCtrl.isChange) return TLK_ENONE;
	
	volume[0] = gTlkMmiAudioInfoCtrl.item.tone;
	volume[1] = gTlkMmiAudioInfoCtrl.item.music;
	volume[2] = gTlkMmiAudioInfoCtrl.item.voice;
	volume[3] = gTlkMmiAudioInfoCtrl.item.headset;
	volume[4] = gTlkMmiAudioInfoCtrl.item.resv001;
	volume[5] = gTlkMmiAudioInfoCtrl.item.enReport;
	volume[6] = (gTlkMmiAudioInfoCtrl.item.interval & 0xFF);
	volume[7] = (gTlkMmiAudioInfoCtrl.item.interval & 0xFF00) >> 8;
	volume[8] = gTlkMmiAudioInfoCtrl.item.resv002[0];
	volume[9] = gTlkMmiAudioInfoCtrl.item.resv002[1];
	volume[10] = gTlkMmiAudioInfoCtrl.item.resv002[2];
	volume[11] = gTlkMmiAudioInfoCtrl.item.resv002[3];
	gTlkMmiAudioInfoCtrl.isChange = false;
	gTlkMmiAudioInfoCtrl.interval = 0;
	return tlkapi_save2_smartSave(&gTlkMmiAudioInfoCtrl.save, volume, TLKMDI_AUDIO_VOLUME_SAVE_SIZE);
}

/******************************************************************************
 * Function: tlkmdi_audio_infoGetVolume
 * Descript: Get the volume.
 * Params:
 *         @voltype[IN]--The Audio operate type.
 *         @pVolume[IN]--The current audio volume.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoGetVolume(uint08 voltype, uint08 *pVolume)
{
	uint08 *pTemp;

	if(pVolume == nullptr) return -TLK_EPARAM;
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE) pTemp = &gTlkMmiAudioInfoCtrl.item.tone;
 	else if(voltype == TLKPRT_COMM_VOLUME_TYPE_MUSIC) pTemp = &gTlkMmiAudioInfoCtrl.item.music;
	else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE) pTemp = &gTlkMmiAudioInfoCtrl.item.voice;
	else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET) pTemp = &gTlkMmiAudioInfoCtrl.item.headset;
	else pTemp = nullptr;
	if(pTemp == nullptr) return -TLK_EFAIL;
	*pVolume = *pTemp;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_audio_infoGetReport
 * Descript: Get the audio report.
 * Params: @pEnable[IN]--enable the report or not.
 *         @pInterval[IN]--The interval.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoGetReport(uint08 *pEnable, uint16 *pInterval)
{
	if(pEnable == nullptr || pInterval == nullptr) return -TLK_EPARAM;
	*pEnable = gTlkMmiAudioInfoCtrl.item.enReport;
	*pInterval = gTlkMmiAudioInfoCtrl.item.interval;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_audio_infoSetVolume
 * Descript: Set the audio volume.
 * Params:
 *         @optype[IN]--The operate type.
 *         @volume[IN]--The volume.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoSetVolume(uint08 voltype, uint08 volume)
{
	uint08 *pVolume;

	if(volume > 100) return -TLK_EPARAM;
	if(voltype == TLKPRT_COMM_VOLUME_TYPE_TONE) pVolume = &gTlkMmiAudioInfoCtrl.item.tone;
 	else if(voltype == TLKPRT_COMM_VOLUME_TYPE_MUSIC) pVolume = &gTlkMmiAudioInfoCtrl.item.music;
	else if(voltype == TLKPRT_COMM_VOLUME_TYPE_VOICE) pVolume = &gTlkMmiAudioInfoCtrl.item.voice;
	else if(voltype == TLKPRT_COMM_VOLUME_TYPE_HEADSET) pVolume = &gTlkMmiAudioInfoCtrl.item.headset;
	else pVolume = nullptr;
	if(pVolume == nullptr) return -TLK_EFAIL;
	if((*pVolume) == volume) return -TLK_EREPEAT;
	*pVolume = volume;
	gTlkMmiAudioInfoCtrl.isChange = true;
	if(gTlkMmiAudioInfoCtrl.interval == 0){
		gTlkMmiAudioInfoCtrl.interval = TLKMMI_AUDIO_INFO_TIMEOUT;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_infoInit
 * Descript: Enable the audio report info.
 * Params:
 *         @enable[IN]--The enable value.
 *         @interval[IN]--The time value.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoSetReport(uint08 enable, uint16 interval)
{
	if(gTlkMmiAudioInfoCtrl.item.enReport == enable && gTlkMmiAudioInfoCtrl.item.interval == interval){
		return -TLK_EREPEAT;
	}
	gTlkMmiAudioInfoCtrl.item.enReport = enable;
	if(enable) gTlkMmiAudioInfoCtrl.item.interval = interval;
	gTlkMmiAudioInfoCtrl.isChange = true;
	if(gTlkMmiAudioInfoCtrl.interval == 0){
		gTlkMmiAudioInfoCtrl.interval = TLKMMI_AUDIO_INFO_TIMEOUT;
	}
	return TLK_ENONE;
}



#endif //#if (TLKMMI_AUDIO_ENABLE)


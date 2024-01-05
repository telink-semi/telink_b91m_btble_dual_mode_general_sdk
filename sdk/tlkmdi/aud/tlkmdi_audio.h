/********************************************************************************************************
 * @file	tlkmdi_audio.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKMDI_AUDIO_H
#define TLKMDI_AUDIO_H

#if (TLK_MDI_AUDIO_ENABLE)


#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmdi/aud/tlkmdi_audhfp.h"
#include "tlkmdi/aud/tlkmdi_audsnk.h"
#include "tlkmdi/aud/tlkmdi_audsrc.h"
#include "tlkmdi/aud/tlkmdi_audsco.h"
#include "tlkmdi/aud/tlkmdi_audmp3.h"
#include "tlkmdi/aud/tlkmdi_auduac.h"
#include "tlkmdi/aud/tlkmdi_audplay.h"
#include "tlkmdi/aud/tlkmdi_audtone.h"
#include "tlkmdi/aud/tlkmdi_auddsco.h"


#define TLKMDI_AUDIO_VOLUME_EXPAND            10

#define TLKMDI_AUDIO_TONE_INDEX_DEF           9 //60% - scTlkMdiAudioToneRawVolTable[TLKMDI_AUDIO_TONE_INDEX_DEF]
#define TLKMDI_AUDIO_VOICE_INDEX_DEF          9 //60% - scTlkMdiAudioVoiceRawVolTable[TLKMDI_AUDIO_VOICE_INDEX_DEF]
#define TLKMDI_AUDIO_MUSIC_INDEX_DEF          19 //60% - scTlkMdiAudioMusicRawVolTable[TLKMDI_AUDIO_MUSIC_INDEX_DEF]
#define TLKMDI_AUDIO_HEADSET_INDEX_DEF        9 //60% - scTlkMdiAudioHesdSetRawVolTable[TLKMDI_AUDIO_HEADSET_INDEX_DEF]


int tlkmdi_audio_init(void);

int tlkmdi_audio_sendStartEvt(uint08 audChn, uint16 handle);
int tlkmdi_audio_sendCloseEvt(uint08 audChn, uint16 handle);

int tlkmdi_audio_sendPlayStartEvt(uint08 audChn, uint16 playIndex, bool isIrq);
int tlkmdi_audio_sendPlayOverEvt(uint08 audChn, uint16 playIndex, bool isIrq);

int tlkmdi_audio_sendVolumeChangeEvt(uint08 audChn, uint08 volume);
int tlkmdi_audio_sendStatusChangeEvt(uint08 audChn, uint08 status);


uint tlkmdi_audio_getToneRawVolume(void);
uint tlkmdi_audio_getToneCalVolume(void);
void tlkmdi_audio_setToneRawVolume(uint08 volume, bool isReport);
uint tlkmdi_audio_getToneVolume(void);
void tlkmdi_audio_toneVolumeInc(void);
void tlkmdi_audio_toneVolumeDec(void);
void tlkmdi_audio_setToneVolume(uint08 volume, bool isReport);
uint tlkmdi_audio_getToneRawVolumeIndex(uint08 rawVol);


uint tlkmdi_audio_getMusicBtpVolume(bool isIos);
uint tlkmdi_audio_getMusicRawVolume(bool isIos);
uint tlkmdi_audio_getMusicCalVolume(bool isIos);
void tlkmdi_audio_setMusicRawVolume(uint08 volume, bool isReport);
void tlkmdi_audio_setMusicBtpVolume(uint08 volume, bool isIos, bool isReport);
void tlkmdi_audio_musicVolumeInc(void);
void tlkmdi_audio_musicVolumeDec(void);
uint tlkmdi_audio_getMusicVolume(void);
void tlkmdi_audio_setMusicVolume(uint08 volume, bool isReport);
uint tlkmdi_audio_getMusicRawVolumeIndex(uint08 rawVol);
uint tlkmdi_audio_getMusicBtpVolumeIndex(uint08 btpVol, bool isIos);


uint tlkmdi_audio_getVoiceBtpVolume(void);
uint tlkmdi_audio_getVoiceRawVolume(void);
uint tlkmdi_audio_getVoiceCalVolume(void);
void tlkmdi_audio_setVoiceRawVolume(uint08 volume, bool isReport);
void tlkmdi_audio_setVoiceBtpVolume(uint08 volume, bool isReport);
uint tlkmdi_audio_getVoiceVolume(void);
void tlkmdi_audio_voiceVolumeInc(void);
void tlkmdi_audio_voiceVolumeDec(void);
void tlkmdi_audio_setVoiceVolume(uint08 volume, bool isReport);
uint tlkmdi_audio_getVoiceRawVolumeIndex(uint08 rawVol);
uint tlkmdi_audio_getVoiceBtpVolumeIndex(uint08 btpVol);


uint tlkmdi_audio_getHeadsetBtpVolume(void);
uint tlkmdi_audio_getHeadsetRawVolume(void);
uint tlkmdi_audio_getHeadsetCalVolume(void);
void tlkmdi_audio_setHeadsetRawVolume(uint08 volume, bool isReport);
void tlkmdi_audio_setHeadsetBtpVolume(uint08 volume, bool isReport);
uint tlkmdi_audio_getHeadsetVolume(void);
void tlkmdi_audio_headsetVolumeInc(void);
void tlkmdi_audio_headsetVolumeDec(void);
void tlkmdi_audio_setHeadsetVolume(uint08 volume, bool isReport);
uint tlkmdi_audio_getHeadsetRawVolumeIndex(uint08 rawVol);
uint tlkmdi_audio_getHeadsetBtpVolumeIndex(uint08 btpVol);



#endif //#if (TLK_MDI_AUDIO_ENABLE)

#endif //TLKMDI_AUD_HF_H


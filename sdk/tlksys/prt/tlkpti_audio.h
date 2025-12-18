/********************************************************************************************************
 * @file	tlkpti_audio.h
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
#ifndef TLKPTI_AUDIO_H
#define TLKPTI_AUDIO_H



typedef enum{
	TLKPTI_AUD_MSGID_NONE = 0x00,
	TLKPTI_AUD_MSGID_START_EVT,
	TLKPTI_AUD_MSGID_CLOSE_EVT,
	TLKPTI_AUD_MSGID_PLAY_START_EVT,
	TLKPTI_AUD_MSGID_PLAY_CLOSE_EVT,
	TLKPTI_AUD_MSGID_VOLUME_CHANGE_EVT,
	TLKPTI_AUD_MSGID_STATUS_CHANGE_EVT,

	TLKPTI_AUD_MSGID_START_TONE_CMD,

	TLKPTI_AUD_MSGID_BT_CONNECT_EVT,
	TLKPTI_AUD_MSGID_BT_DISCONN_EVT,
	TLKPTI_AUD_MSGID_HFP_CHG_VOLUME_EVT,
	TLKPTI_AUD_MSGID_AVRCP_CHG_VOLUME_EVT,
}TLKPTI_AUD_MSGID_ENUM;


typedef enum{
	TLKPTI_AUD_OPTYPE_NONE = 0,
	TLKPTI_AUD_OPTYPE_TONE, //Play the prompt tone locally
	TLKPTI_AUD_OPTYPE_PLAY, //Play the music locally
	TLKPTI_AUD_OPTYPE_HF,
	TLKPTI_AUD_OPTYPE_AG,
	TLKPTI_AUD_OPTYPE_SCO, //Status control of SCO link establishment. SCO is commonly used in phone calls or speech recognition.
	TLKPTI_AUD_OPTYPE_SRC, //Connect the headset and play music to the headset
	TLKPTI_AUD_OPTYPE_SNK, //Connect a mobile phone and listen to music on the mobile phone.
	TLKPTI_AUD_OPTYPE_UAC,
	TLKPTI_AUD_OPTYPE_DSCO,
	TLKPTI_AUD_OPTYPE_MAX,
}TLKPTI_AUD_OPTYPE_ENUM;


#endif //TLKPTI_AUDIO_H


/********************************************************************************************************
 * @file	tlkmmi_audioCtrl.h
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
#ifndef TLKMMI_AUDIO_CTRL_H
#define TLKMMI_AUDIO_CTRL_H


#if (TLKMMI_AUDIO_ENABLE)


#define TLKMMI_AUDIO_VOLUME_TONE_STEP         5
#define TLKMMI_AUDIO_VOLUME_MUSIC_STEP        3
#define TLKMMI_AUDIO_VOLUME_VOICE_STEP        6
#define TLKMMI_AUDIO_VOLUME_HEADSET_STEP      6



typedef enum{
	TLKMMI_AUDIO_REPORT_BUSY_NONE     = 0x0000,
	TLKMMI_AUDIO_REPORT_BUSY_PROGR100 = 0x0001,
	TLKMMI_AUDIO_REPORT_BUSY_PROGR000 = 0x0002,
	TLKMMI_AUDIO_REPORT_BUSY_PROGRESS = 0x0004,
	TLKMMI_AUDIO_REPORT_BUSY_SONG_CHANGE   = 0x0800,
	TLKMMI_AUDIO_REPORT_BUSY_STATUS_CHANGE = 0x0010,
	TLKMMI_AUDIO_REPORT_BUSY_VOLUME_CHANGE = 0x0020,
}TLKMMI_AUDIO_REPORT_BUSYS_ENUM;



typedef struct{
	uint16 busys;
	uint16 timeout;
	uint16 interval;
	uint16 newIndex;
	uint08 enable;
	uint08 reserve;
	uint08 volType;
	uint08 volValue;
	uint08 statusChn0;
	uint08 statusVal0;
	uint08 statusChn1;
	uint08 statusVal1;
}tlkmmi_audio_report_t;
typedef struct{
	uint16 mp3Timeout;
	uint16 mp3Interval;
}tlkmmi_audio_update_t;
typedef struct{
	tlkapi_timer_t timer;
	tlkapi_queue_t procs;
	tlkmmi_audio_report_t report;
}tlkmmi_audio_ctrl_t;


extern tlkmmi_audio_ctrl_t gTlkMmiAudioCtrl;

/******************************************************************************
 * Function: tlkmmi_audio_ctrlInit
 * Descript: Initial the audio control info. 
 * Params: None.
 * Return: Return TLK_ENONE is success, other is false.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_ctrlInit(void);

void tlkmmi_audio_validOptype(uint08 *pOptype, uint16 *pHandle);

int tlkmmi_audio_extendPlay(uint16 index, uint08 offset);

/******************************************************************************
 * Function: tlkmmi_audio_startPlay
 * Descript: Insert the music fileindex into the audio status control list,
 *           and switch to play this music.
 * Params:
 *         @fileIndex[IN]--The music file index to playing.
 * Return: true/false.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_startPlay(void);

/******************************************************************************
 * Function: tlkmmi_audio_closePlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_closePlay(void);

/******************************************************************************
 * Function: tlkmmi_audio_fastPlay
 * Descript: 
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_fastPlay(bool isRewind, bool isStart);

/******************************************************************************
 * Function: tlkmmi_audio_playNext
 * Descript: play next music.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_playNext(void);

/******************************************************************************
 * Function: tlkmmi_audio_playNext
 * Descript: play pre music.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_playPrev(void);

/******************************************************************************
 * Function: tlkmmi_audio_isLocalPlay
 * Descript: Checks whether the current playback is local.
 * Params: None.
 * Return: TRUE-local play, false-other play.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isLocalPlay(void);

/******************************************************************************
 * Function: tlkmmi_audio_startLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_startLocalPlay(void);

/******************************************************************************
 * Function: tlkmmi_audio_stopLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_stopLocalPlay(void);

/******************************************************************************
 * Function: tlkmmi_audio_startTone
 * Descript: start the play tone.
 * Params: None.
 * Return: true is success/false if failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_startTone(uint16 fileIndex, uint16 playCount);

/******************************************************************************
 * Function: tlkmmi_audio_stopTone
 * Descript: stop the play tone.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_stopTone(void);

/******************************************************************************
 * Function: tlkmmi_audio_getVolume
 * Descript: Get audio volume.
 * Params: 
 *         @channel[IN]--The audio channel.
 *         @pVolume[OUT]--The audio volume.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_getVolume(uint08 voltype, uint08 *pVolType);

/******************************************************************************
 * Function: tlkmmi_audio_volumeInc
 * Descript: adjust audio channel increment.
 * Params: @voltype[IN]--The audio channel.
 * Return: Return true means success, other value is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_volumeInc(uint08 voltype);

/******************************************************************************
 * Function: tlkmmi_audio_volumeDec
 * Descript: adjust audio channel decrement.
 * Params: @voltype[IN]--The audio channel.
 * Return: Return true means success, other value is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_volumeDec(uint08 voltype);

/******************************************************************************
 * Function: tlkmmi_audio_setVolume
 * Descript: Set audio volume.
 * Params: 
 *         @channel[IN]--The audio channel.
 *         @volume[IN]--The audio volume.
 * Return: Return true is success/other is failure.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_setVolume(uint08 voltype, uint08 volume);

/******************************************************************************
 * Function: tlkmdi_audio_setReport
 * Descript: Set audio volume.
 * Params: 
 *         @enable[IN]--enable the audio info report.
 *         @interval[IN]--The value How often report.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_audio_setReport(uint08 enable, uint16 interval);

/******************************************************************************
 * Function: tlkmmi_audio_getCurChannel
 * Descript: Get current audio channel.
 * Params: @channel[OUT]--The audio channel.
 * Return: Return the TLK_ENONE means success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_getCurChannel(uint08 *pChannel);

/******************************************************************************
 * Function: tlkmmi_audio_channelToOptype
 * Descript: channel relect to a optype.
 * Params: 
 *         @pChannel[IN]--The channel of audio info.
 *         @optype[IN]--the audio operate.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_channelToOptype(uint08 channel, uint08 *pOptype);

/******************************************************************************
 * Function: tlkmmi_audio_optypeToChannel
 * Descript: optype relect to a Channel.
 * Params: 
 *         @optype[IN]--the audio operate.
 *         @pChannel[IN]--The channel of audio info.
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_optypeToChannel(uint08 optype, uint08 *pChannel);


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
void tlkmmi_audio_optypeChanged(uint08 newOptype, uint16 newHandle, uint08 oldOptype, uint16 oldHandle);


int  tlkmmi_audio_sendProgressEvt(uint08 optype, uint16 progress);
void tlkmmi_audio_sendSongChangeEvt(void);
void tlkmmi_audio_sendStatusChangeEvt(void);
void tlkmmi_audio_sendVolumeChangeEvt(void);



#endif //#if (TLKMMI_AUDIO_ENABLE)

#endif //TLKMMI_AUDIO_CTRL_H


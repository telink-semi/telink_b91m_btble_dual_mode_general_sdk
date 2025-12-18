/********************************************************************************************************
 * @file	tlkmmi_audio.h
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
#ifndef TLKMMI_AUDIO_H
#define TLKMMI_AUDIO_H

#if (TLKMMI_AUDIO_ENABLE)


#define TLKMMI_AUDIO_TIMEOUT          100000 //200ms
#define TLKMMI_AUDIO_TIMEOUT_MS       100
#define TLKMMI_AUDIO_TIMER_TIMEOUT    (200000/TLKMMI_AUDIO_TIMEOUT) //Prevent timer termination, which may cause problems

#define TLKMMI_AUDIO_INFO_TIMEOUT     (5000000/TLKMMI_AUDIO_TIMEOUT) //5s
#define TLKMMI_AUDIO_CODEC_IDLE_TIMEOUT    (300000/TLKMMI_AUDIO_TIMEOUT) //If codec is not used for more than this time, codec will automatically shut down


#define TLKMMI_AUDIO_DBG_FLAG         ((TLK_MAJOR_DBGID_MMI_AUDIO << 24) | (TLK_MINOR_DBGID_MMI_AUDIO << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_AUDIO_DBG_SIGN         "[MMI]"



/******************************************************************************
 * Function: tlkmmi_audio_isBusy
 * Descript:
 * Params:
 * Return: true/false
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isBusy(void);

/******************************************************************************
 * Function: tlkmmi_audio_insertItem
 * Descript: Suspend the current audio task when a new audio task insert 
 *           and start running.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_audio_insertItem(uint16 aclHandle, uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_removeItem
 * Descript: Suspend the current task and delete it.Then resume the task 
 *           which been suspend before.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_audio_removeItem(uint16 aclHandle, uint08 optype);

/******************************************************************************
 * Function: tlkmmi_audio_isLocalPlay
 * Descript: Checks whether the current playback is local.
 * Params: None.
 * Return: TRUE-local play, false-other play.
 * Others: None.
*******************************************************************************/
extern bool tlkmmi_audio_isLocalPlay(void);

/******************************************************************************
 * Function: tlkmmi_audio_stopLocalPlay
 * Descript: Suspend the current music and adjust the audio handle and it's 
 *           operation type out of the audio status control list.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
extern void tlkmmi_audio_stopLocalPlay(void);

extern bool tlkmmi_audio_startTone(uint16 fileIndex, uint16 playCount);


void tlkmmi_audio_startHandler(void);
void tlkmmi_audio_closeHandler(void);



#endif //#if (TLKMMI_AUDIO_ENABLE)

#endif //TLKMMI_AUDIO_H


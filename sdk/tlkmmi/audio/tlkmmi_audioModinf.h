/********************************************************************************************************
 * @file	tlkmmi_audioModinf.h
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
#ifndef TLKMMI_AUDIO_MODINF_H
#define TLKMMI_AUDIO_MODINF_H

#if (TLKMMI_AUDIO_ENABLE)



typedef struct{
	int (*Start)(uint16 handle, uint32 param);
	int (*Close)(uint16 handle);
	void(*Timer)(void);
	bool(*FPlay)(bool isRewind, bool isStart); //FastPlay
	bool(*ToNext)(void);
	bool(*ToPrev)(void);
	bool(*Switch)(uint16 handle, uint08 status);
	bool(*IsBusy)(void);
	uint(*Intval)(void); //Interval
	bool(*IrqProc)(void);
}tlkmmi_audio_modinf_t;


/******************************************************************************
 * Function: tlkmmi_audio_modinfStart
 * Descript: Invoke the opening interface of the corresponding audio module to
 *     enable audio transmission.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 *     @handle[IN]--The connection handle of the current audio initiator.
 *     @param[IN]--Control parameters of the current audio initiator.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_modinfStart(TLKPTI_AUD_OPTYPE_ENUM optype, uint16 handle, uint32 param);

/******************************************************************************
 * Function: tlkmmi_audio_modinfClose
 * Descript: Invoke the closing interface of the corresponding audio module to
 *     disable audio transmission.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 *     @handle[IN]--The connection handle of the current audio initiator.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_modinfClose(TLKPTI_AUD_OPTYPE_ENUM optype, uint16 handle);

/******************************************************************************
 * Function: tlkmmi_audio_modinfTimer
 * Descript: Invoke the timing callback interface of the corresponding audio 
 *     module to input the timing control source for the audio module.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_modinfTimer(TLKPTI_AUD_OPTYPE_ENUM optype);

bool tlkmmi_audio_modinfFPlay(TLKPTI_AUD_OPTYPE_ENUM optype, bool isRewind, bool isStart);

/******************************************************************************
 * Function: tlkmmi_audio_modinfSwitch
 * Descript: Call the switching interface of the corresponding audio module to
 *     officially start or turn off the audio input or output.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 *     @handle[IN]--The connection handle of the current audio initiator.
 *     @status[IN]--The state of the audio control.
 *                  TLK_STATE_OPENED -- The audio is on.
 *                  TLK_STATE_CLOSED -- The audio is off.
 *                  TLK_STATE_PAUSED -- The audio is paused.
 * Return: Operating results. True means success, others means failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfSwitch(TLKPTI_AUD_OPTYPE_ENUM optype, uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmmi_audio_modinfToNext
 * Descript: Invoke the next track interface of the corresponding audio module
 *     to cause the audio to play the next track.
 * Params: 
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. true means success, others means failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToNext(TLKPTI_AUD_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfToPrev
 * Descript: Invoke the previous track interface of the corresponding audio 
 *     module to cause the audio to play the previous track.
 * Params: 
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. True means success, others means failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToPrev(TLKPTI_AUD_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfIsBusy
 * Descript: Check the status of the corresponding audio module.
 * Params: 
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. True means audio is busy, others means audio is
 *         idle.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfIsBusy(TLKPTI_AUD_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfIntval
 * Descript: Gets the interval time at which the next state of the current audio 
 *     arrives.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: The interval time at which the next state of the current audio 
 *     arrives.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_modinfIntval(TLKPTI_AUD_OPTYPE_ENUM optype);

/******************************************************************************
 * Function: tlkmmi_audio_modinfIrqProc
 * Descript: Invoke the timing callback interface of the corresponding audio 
 *     module to input the timing control source for the audio module.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. True means success, others means failure.
 * Others: 
 *     tlkmmi_audio_modinfTimer -- Software timer. There's a big time error.
 *     tlkmmi_audio_modinfIrqProc -- Hardware timer, time accuracy is guaranteed.
*******************************************************************************/
bool tlkmmi_audio_modinfIrqProc(TLKPTI_AUD_OPTYPE_ENUM optype);



#endif //#if (TLKMMI_AUDIO_ENABLE)

#endif //TLKMMI_AUDIO_MODINF_H


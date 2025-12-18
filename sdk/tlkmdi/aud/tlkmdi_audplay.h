/********************************************************************************************************
 * @file	tlkmdi_audplay.h
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
#ifndef TLKMDI_PLAY_H
#define TLKMDI_PLAY_H

#if (TLK_MDI_AUDPLAY_ENABLE)



/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int  tlkmdi_audplay_init(void);

int tlkmdi_audplay_start(uint16 handle, uint32 param);
int tlkmdi_audplay_close(uint16 handle);

bool tlkmdi_audplay_fPlay(bool isRewind, bool isStart);

/******************************************************************************
 * Function: tlkmdi_audplay_toNext
 * Descript: Play the next music. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_toNext(void);

/******************************************************************************
 * Function: tlkmdi_audplay_toPrev
 * Descript: Play the prev' music. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_toPrev(void);

/******************************************************************************
 * Function: tlkmdi_audplay_switch
 * Descript: Change the audio status. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_switch(uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmdi_audplay_isBusy
 * Descript: Verify the audio control block is enable. 
 * Params: None.
 * Return: 'true' means success, otherwise failure.
*******************************************************************************/
bool tlkmdi_audplay_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_audplay_isBusy
 * Descript: Get the audio play interval, this value will determine 
 *           what the time to restart the timer . 
 * Params: None.
 * Return: interval value.
*******************************************************************************/
uint tlkmdi_audplay_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_audplay_irqProc
 * Descript: Trigger audio handler. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_audplay_irqProc(void);

/******************************************************************************
 * Function: tlkmdi_play_start
 * Descript: Play the Music. 
 * Params: @index[IN]--The music item.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_play_start(uint16 index);

/******************************************************************************
 * Function: tlkmdi_play_close
 * Descript: Stop the Music. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_play_close(void);

/******************************************************************************
 * Function: tlkmdi_play_pause
 * Descript: suspend the Music. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_play_pause(void);

/******************************************************************************
 * Function: tlkmdi_play_prev
 * Descript: Play the prev' music. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_play_prev(void);

/******************************************************************************
 * Function: tlkmdi_play_next
 * Descript: Play the next music. 
 * Params: None.
 * Return: Return true is success, otherwise is failure.
*******************************************************************************/
bool tlkmdi_play_next(void);

/******************************************************************************
 * Function: tlkmdi_play_setParam
 * Descript: Set the audio Parameter. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_play_setParam(uint16 fileIndex);



#endif //#if (TLK_MDI_AUDPLAY_ENABLE)

#endif //TLKMDI_PLAY_H


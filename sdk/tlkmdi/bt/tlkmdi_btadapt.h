/********************************************************************************************************
 * @file	tlkmdi_btadapt.h
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
#ifndef TLKMDI_BT_ADAPT_H
#define TLKMDI_BT_ADAPT_H

#if (TLK_STK_BT_ENABLE)



/******************************************************************************
 * Function: tlkmdi_btadapt_init
 * Descript: 
 * Params:
 * Return: Return TLK_ENONE is success/other value is faiure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btadapt_init(uint08 procID);

/******************************************************************************
 * Function: tlkmdi_btadapt_handler
 * Descript: Set up a timer and info process and start scedule. 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btadapt_handler(void);

/******************************************************************************
 * Function: tlkmdi_btadapt_isbusy
 * Descript: If there is a process or timer and less than 1ms, the module 
 *           is considered busy, it is the basis of the schedule. 
 * Params:  
 * Return: true is busy/false is idle.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btadapt_isbusy(void);
bool tlkmdi_btadapt_isPmBusy(void);

/******************************************************************************
 * Function: tlkmdi_btadapt_initTimer
 * Descript: 
 * Params:
 *     @pTimer[IN]--
 *     @userArg[IN]--
 *     @timeout[IN]--Unit: us.
 *     @timerCB[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btadapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  tlkmdi_btadapt_initProcs(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg);
void tlkmdi_btadapt_deinitTimer(tlkapi_timer_t *pTimer);
void tlkmdi_btadapt_deinitProcs(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmdi_btadapt_isHaveTimer, tlkmdi_btadapt_isHaveProcs
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pTimer[IN]--
 *     @pProcs[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btadapt_isHaveTimer(tlkapi_timer_t *pTimer);
bool tlkmdi_btadapt_isHaveProcs(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmdi_adapt_appendAgent, tlkmdi_adapt_removeAgent
 * Descript: 
 * Params:
 *     @pAdapt[IN]--
 *     @pAgent[IN]--
 * Return: None.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btadapt_appendProcs(tlkapi_queue_t *pProcs);
int  tlkmdi_btadapt_removeProcs(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmdi_btadapt_updateTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 *     @timeout[IN]--Unit: us.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btadapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
/******************************************************************************
 * Function: tlkmdi_btadapt_insertTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btadapt_insertTimer(tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkmdi_btadapt_removeTimer
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 * 	@pAdapt[IN]--The adapt self-manages handle.
 * 	@pTimer[IN]--Timer.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btadapt_removeTimer(tlkapi_timer_t *pTimer);



#endif //#if (TLK_STK_BT_ENABLE)

#endif //TLKMDI_BT_ADAPT_H


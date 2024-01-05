/********************************************************************************************************
 * @file	tlkapi_timer.h
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
#ifndef TLKAPI_TIMER_H
#define TLKAPI_TIMER_H


/******************************************************************************
 * Function: tlkapi_timer_init
 * Descript: Initialize parameters related to hardware scheduled tasks.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
*******************************************************************************/
int tlkapi_timer_init(void);

/******************************************************************************
 * Function: tlkapi_timer_clear
 * Descript: Clear all timers saved in the list.
 * Params: None.
 * Return: None.
 * Notice: 
 *     1.No callback is generated when a scheduled task is cleared.
*******************************************************************************/
void tlkapi_timer_clear(void);

/******************************************************************************
 * Function: tlkapi_timer_interval
 * Descript: Gets the time required for the last timer to arrive. Unit:us.
 * Params: None.
 * Return: None.
*******************************************************************************/
uint tlkapi_timer_interval(void);

/******************************************************************************
 * Function: tlkapi_timer_isbusy, tlkapi_timer_isPmBusy
 * Descript: If there is a process or timer and less than 1ms, the module 
 *           is considered busy, it is the basis of the schedule. 
 * Params:  
 * Return: true is busy/false is idle.
 * Others: None.
*******************************************************************************/
bool tlkapi_timer_isNull(void);

/******************************************************************************
 * Function: tlkapi_timer_wakeup
 * Descript: Restart the timer, which is usually called after the PM wakes up.
 * Params: 
 *     @mode[IN]--Wake-up source. Refer pm_sleep_wakeup_src_e. Not used in this
 *                interface.
 * Return: None.
 * Notice: None.
*******************************************************************************/
void tlkapi_timer_wakeup(int wakeSource);

/******************************************************************************
 * Function: tlkapi_timer_handler
 * Descript: The processing interface of the timer is automatically invoked by
 *           the timer.
 * Params: None.
 * Return: None.
 * Notice: 
 *     1.Since this interface is called in the interrupt function 
 *       "timer0_irq_handler", users should use this interface with caution and
 *       call the tlkapi_timer_wakeup interface if necessary.
*******************************************************************************/
void tlkapi_timer_handler(void);

/******************************************************************************
 * Function: tlkapi_timer_isout
 * Descript: Detects whether the current timer has timed out.
 * Params:
 *     @timer[IN]--Reference timer.
 *     @ticks[IN]--Timeout. unit: 1/16 us:
 * Return: True means the timer is timeout, false means not.
 * Others: None.
*******************************************************************************/
bool tlkapi_timer_isout(uint32 timer, uint32 ticks);

/******************************************************************************
 * Function: tlkapi_timer_initNode
 * Descript: Initializes the timing scheduler and work queue.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @userArg[IN]--Parameters passed in by the user will be returned on the call.
 *     @timeout[IN]--Scheduling interval set by the user. Unit:us.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_initNode(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);

/******************************************************************************
 * Function: tlkmdi_timer_isHave
 * Descript: Check whether scheduled tasks exists in the task list.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 * Return: Return TRUE if has, FALSE otherwise.
*******************************************************************************/
bool tlkapi_timer_isHaveNode(tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkmdi_timer_update
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 *     @timeout[IN]--Unit: us.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_updateNode(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);

/******************************************************************************
 * Function: tlkmdi_timer_insert
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_insertNode(tlkapi_timer_t *pTimer);

/******************************************************************************
 * Function: tlkmdi_timer_remove
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 * 	@pAdapt[IN]--The adapt self-manages handle.
 * 	@pTimer[IN]--Timer.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkapi_timer_removeNode(tlkapi_timer_t *pTimer);




#endif //#ifndef TLKAPI_TIMER_H


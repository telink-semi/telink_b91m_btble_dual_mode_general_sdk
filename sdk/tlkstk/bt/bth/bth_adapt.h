/********************************************************************************************************
 * @file	bth_adapt.h
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
#ifndef BTH_ADAPT_H
#define BTH_ADAPT_H


/******************************************************************************
 * Function: bth_adapt_init
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
 *******************************************************************************/
int bth_adapt_init(void);

/******************************************************************************
 * Function: bth_adapt_handler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
 *******************************************************************************/
void bth_adapt_handler(void);

/******************************************************************************
 * Function: bth_adapt_interval
 * Descript: Gets the execution time of the next task in the adapter.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
 *******************************************************************************/
uint bth_adapt_interval(void);

/******************************************************************************
 * Function: bth_adapt_initTimer, bth_adapt_deinitTimer
 *           bth_adapt_initQueue, bth_adapt_deinitQueue
 * Descript: Initializes and deinitializes the timing scheduler and work queue.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 *     @userArg[IN]--Parameters passed in by the user will be returned on the call.
 *     @timeout[IN]--Scheduling interval set by the user. Unit:us.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 *     @queueCB[IN]--The callback interface for the work to be executed.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
 *******************************************************************************/
int bth_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int bth_adapt_initQueue(tlkapi_queue_t *pQueue, TlkApiQueueCB queueCB, uint32 userArg);
void bth_adapt_deinitTimer(tlkapi_timer_t *pTimer);
void bth_adapt_deinitQueue(tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: bth_adapt_isHaveTimer, bth_adapt_isHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
 *******************************************************************************/
bool bth_adapt_isHaveTimer(tlkapi_timer_t *pTimer);
bool bth_adapt_isHaveQueue(tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: bth_adapt_appendQueue, bth_adapt_removeQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
 *******************************************************************************/
int bth_adapt_appendQueue(tlkapi_queue_t *pQueue);
int bth_adapt_removeQueue(tlkapi_queue_t *pQueue);

/******************************************************************************
 * Function: bth_adapt_updateTimer, bth_adapt_insertTimer,
 *           bth_adapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
 *******************************************************************************/
int bth_adapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int bth_adapt_insertTimer(tlkapi_timer_t *pTimer);
int bth_adapt_removeTimer(tlkapi_timer_t *pTimer);


#endif // BTH_ADAPT_H

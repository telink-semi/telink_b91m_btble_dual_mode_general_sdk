/********************************************************************************************************
 * @file	tlkos_adapt.c
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
#if (TLK_CFG_OS_ENABLE)
#include "tlklib/os/tlkos_stdio.h"
#include "tlklib/os/tlkos_adapt.h"


static tlkapi_adapt_t sTlkOsAdapt;



/******************************************************************************
 * Function: tlkos_adapt_init
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_init(void)
{
	return tlkapi_adapt_init(&sTlkOsAdapt);
}

/******************************************************************************
 * Function: tlkos_adapt_handler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkos_adapt_handler(void)
{
	tlkapi_adapt_handler(&sTlkOsAdapt);
}

/******************************************************************************
 * Function: tlkos_adapt_initTimer, tlkos_adapt_deinitTimer
 *           tlkos_adapt_initQueue, tlkos_adapt_deinitQueue
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
int tlkos_adapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	if(tlkos_adapt_isHaveTimer(pTimer)) return -TLK_EEXIST;
	return tlkapi_adapt_initTimer(pTimer, timerCB, userArg, timeout);
}
int tlkos_adapt_initQueue(tlkapi_queue_t *pQueue, TlkApiQueueCB queueCB, uint32 userArg)
{
	if(tlkos_adapt_isHaveQueue(pQueue)) return -TLK_EEXIST;
	return tlkapi_adapt_initQueue(pQueue, queueCB, userArg);
}
void tlkos_adapt_deinitTimer(tlkapi_timer_t *pTimer)
{
	tlkapi_adapt_deinitTimer(&sTlkOsAdapt, pTimer);
}
void tlkos_adapt_deinitQueue(tlkapi_queue_t *pQueue)
{
	tlkapi_adapt_deinitQueue(&sTlkOsAdapt, pQueue);
}

/******************************************************************************
 * Function: tlkos_adapt_isHaveTimer, tlkos_adapt_isHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkos_adapt_isHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_isHaveTimer(&sTlkOsAdapt, pTimer);
}
bool tlkos_adapt_isHaveQueue(tlkapi_queue_t *pQueue)
{
	return tlkapi_adapt_isHaveQueue(&sTlkOsAdapt, pQueue);
}

/******************************************************************************
 * Function: tlkos_adapt_appendQueue, tlkos_adapt_removeQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_appendQueue(tlkapi_queue_t *pQueue)
{	
	return tlkapi_adapt_appendQueue(&sTlkOsAdapt, pQueue);
}
int tlkos_adapt_removeQueue(tlkapi_queue_t *pQueue)
{
	return tlkapi_adapt_removeQueue(&sTlkOsAdapt, pQueue);
}

/******************************************************************************
 * Function: tlkos_adapt_updateTimer, tlkos_adapt_insertTimer,
 *           tlkos_adapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkos_adapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlkapi_adapt_updateTimer(&sTlkOsAdapt, pTimer, timeout, isInsert);
}
int tlkos_adapt_insertTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_insertTimer(&sTlkOsAdapt, pTimer, true);
}
int tlkos_adapt_removeTimer(tlkapi_timer_t *pTimer)
{
	return tlkapi_adapt_removeTimer(&sTlkOsAdapt, pTimer);
}





#endif //#if (TLK_CFG_OS_ENABLE)


/********************************************************************************************************
 * @file	tlksys_adapt.c
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
#if (TLK_CFG_SYS_ENABLE)
#if (TLK_CFG_OS_ENABLE)
#include "tlkapi/tlkapi_os.h"
#endif
#include "tlksys_stdio.h"
#include "tlksys_adapt.h"
#if (TLK_CFG_OS_ENABLE)
#include "tlksys_mutex.h"
#endif

extern tlkapi_adapt_t *tlksys_proc_getAdapter(uint08 procID);

#define TLKSYS_ADAPT_MUTEX_ENABLE     0


/******************************************************************************
 * Function: tlksys_adapt_init
 * Descript: Initializes the adapter control parameters.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_init(uint08 procID)
{
	return tlkapi_adapt_init(tlksys_proc_getAdapter(procID));
}
void tlksys_adapt_deinit(uint08 procID)
{
	tlkapi_adapt_clear(tlksys_proc_getAdapter(procID));
}

/******************************************************************************
 * Function: tlktsk_adapt_run_once
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlksys_adapt_handler(uint08 procID)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	tlkapi_adapt_handler(tlksys_proc_getAdapter(procID));
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
}

/******************************************************************************
 * Function: tlksys_adapt_interval
 * Descript: Gets the execution time of the next task in the process.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlksys_adapt_interval(uint08 procID)
{
	uint ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return 0xFFFFFFFF;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_interval(tlksys_proc_getAdapter(procID));
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_adapt_interval
 * Descript: Gets the execution time of the next task in the process.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlksys_adapt_intervalTicks(uint08 procID)
{
	uint ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return 0xFFFFFFFF;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_interval(tlksys_proc_getAdapter(procID));
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_adapt_initTimer, tlksys_adapt_deinitTimer
 *           tlksys_adapt_initQueue, tlksys_adapt_deinitTimer
 * Descript: Initializes and deinitializes the timing scheduler and work queue.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 *     @userArg[IN]--Parameters passed in by the user will be returned on the call.
 *     @timeout[IN]--Scheduling interval set by the user. Unit:us.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 *     @queueCB[IN]--The callback interface for the work to be executed.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_initTimer(uint08 procID, tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	int ret = TLK_ENONE;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	if(tlkapi_adapt_isHaveTimer(tlksys_proc_getAdapter(procID), pTimer)){
		ret = -TLK_EEXIST;
	}
	if(ret == TLK_ENONE){
		ret = tlkapi_adapt_initTimer(pTimer, timerCB, userArg, timeout);
	}
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_initQueue(uint08 procID, tlkapi_queue_t *pQueue, TlkApiQueueCB queueCB, uint32 userArg)
{
	int ret = TLK_ENONE;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	if(tlkapi_adapt_isHaveQueue(tlksys_proc_getAdapter(procID), pQueue)){
		ret = -TLK_EEXIST;
	}
	if(ret == TLK_ENONE){
		ret = tlkapi_adapt_initQueue(pQueue, queueCB, userArg);
	}
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
void tlksys_adapt_deinitTimer(uint08 procID, tlkapi_timer_t *pTimer)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	tlkapi_adapt_deinitTimer(tlksys_proc_getAdapter(procID), pTimer);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
}
void tlksys_adapt_deinitQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	tlkapi_adapt_deinitQueue(tlksys_proc_getAdapter(procID), pQueue);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
}

/******************************************************************************
 * Function: tlksys_adapt_isHaveTimer, tlksys_adapt_isHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlksys_adapt_isHaveTimer(uint08 procID, tlkapi_timer_t *pTimer)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return false;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_isHaveTimer(tlksys_proc_getAdapter(procID), pTimer);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
bool tlksys_adapt_isHaveQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return false;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_isHaveQueue(tlksys_proc_getAdapter(procID), pQueue);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_adapt_appendQueue, tlksys_adapt_removeQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_appendQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_appendQueue(tlksys_proc_getAdapter(procID), pQueue);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_removeQueue(uint08 procID, tlkapi_queue_t *pQueue)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_removeQueue(tlksys_proc_getAdapter(procID), pQueue);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}

/******************************************************************************
 * Function: tlksys_adapt_updateTimer, tlksys_adapt_insertTimer,
 *           tlksys_adapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @procID[IN]--Id of the process to which the adapter belongs.
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_adapt_updateTimer(uint08 procID, tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_updateTimer(tlksys_proc_getAdapter(procID), pTimer, timeout, isInsert);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_insertTimer(uint08 procID, tlkapi_timer_t *pTimer, bool isUpdate)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_insertTimer(tlksys_proc_getAdapter(procID), pTimer, true);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}
int tlksys_adapt_removeTimer(uint08 procID, tlkapi_timer_t *pTimer)
{
	int ret;
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_lock(procID);
	#endif
	ret = tlkapi_adapt_removeTimer(tlksys_proc_getAdapter(procID), pTimer);
	#if (TLKSYS_ADAPT_MUTEX_ENABLE)
	tlksys_mutex_unlock(procID);
	#endif
	return ret;
}


#endif //#if (TLK_CFG_SYS_ENABLE)


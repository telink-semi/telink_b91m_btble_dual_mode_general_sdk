/********************************************************************************************************
 * @file	tlkmmi_lemgrAdapt.c
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
#if (TLKMMI_LEMGR_ENABLE)
#include "tlkmmi_lemgr.h"
#include "tlkmmi_lemgrAdapt.h"
#include "tlksys/tlksys_stdio.h"


static uint08 sTlkMmiLemgrProcID = 0;


/******************************************************************************
 * Function: tlkmmi_lemgr_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_lemgr_adaptInit(uint08 procID)
{
	sTlkMmiLemgrProcID = procID;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_lemgr_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_lemgr_adaptHandler(void)
{
	tlksys_adapt_handler(sTlkMmiLemgrProcID);
}

/******************************************************************************
 * Function: tlkmmi_lemgr_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_lemgr_adaptInterval(void)
{
	return tlksys_adapt_interval(sTlkMmiLemgrProcID);
}

/******************************************************************************
 * Function: tlkmmi_lemgr_adaptInitTimer, tlkmmi_lemgr_adaptDeinitTimer
 *           tlkmmi_lemgr_adaptInitQueue, tlkmmi_lemgr_adaptDeinitQueue
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
int tlkmmi_lemgr_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	return tlksys_adapt_initTimer(sTlkMmiLemgrProcID, pTimer, timerCB, userArg, timeout);
}
int tlkmmi_lemgr_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg)
{
	return tlksys_adapt_initQueue(sTlkMmiLemgrProcID, pProcs, queueCB, userArg);
}
void tlkmmi_lemgr_adaptDeinitTimer(tlkapi_timer_t *pTimer)
{
	tlksys_adapt_deinitTimer(sTlkMmiLemgrProcID, pTimer);
}
void tlkmmi_lemgr_adaptDeinitQueue(tlkapi_queue_t *pProcs)
{
	tlksys_adapt_deinitQueue(sTlkMmiLemgrProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_lemgr_adaptIsHaveTimer, tlkmmi_lemgr_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_lemgr_adaptIsHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_isHaveTimer(sTlkMmiLemgrProcID, pTimer);
}
bool tlkmmi_lemgr_adaptIsHaveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_isHaveQueue(sTlkMmiLemgrProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_lemgr_adaptAppendQueue, tlkmmi_lemgr_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_lemgr_adaptAppendQueue(tlkapi_queue_t *pProcs)
{	
	return tlksys_adapt_appendQueue(sTlkMmiLemgrProcID, pProcs);
}
int tlkmmi_lemgr_adaptRemoveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_removeQueue(sTlkMmiLemgrProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_lemgr_adaptUpdateTimer, tlkmmi_lemgr_adaptInsertTimer,
 *           tlkmmi_lemgr_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_lemgr_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlksys_adapt_updateTimer(sTlkMmiLemgrProcID, pTimer, timeout, isInsert);
}
int tlkmmi_lemgr_adaptInsertTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_insertTimer(sTlkMmiLemgrProcID, pTimer, true);
}
int tlkmmi_lemgr_adaptRemoveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_removeTimer(sTlkMmiLemgrProcID, pTimer);
}



#endif //#if (TLKMMI_LEMGR_ENABLE)


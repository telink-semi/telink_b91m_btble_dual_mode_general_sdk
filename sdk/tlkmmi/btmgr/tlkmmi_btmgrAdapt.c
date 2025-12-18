/********************************************************************************************************
 * @file	tlkmmi_btmgrAdapt.c
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
#if (TLKMMI_BTMGR_ENABLE)
#include "tlkmmi_btmgr.h"
#include "tlkmmi_btmgrAdapt.h"
#include "tlksys/tlksys_stdio.h"


static uint08 sTlkMmiBtmgrProcID = 0;

/******************************************************************************
 * Function: tlkmmi_btmgr_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_adaptInit(uint08 procID)
{
	sTlkMmiBtmgrProcID = procID;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_adaptHandler(void)
{
	tlksys_adapt_handler(sTlkMmiBtmgrProcID);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_btmgr_adaptInterval(void)
{
	return tlksys_adapt_interval(sTlkMmiBtmgrProcID);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_adaptInitTimer, tlkmmi_btmgr_adaptDeinitTimer
 *           tlkmmi_btmgr_adaptInitQueue, tlkmmi_btmgr_adaptDeinitQueue
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
int tlkmmi_btmgr_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	return tlksys_adapt_initTimer(sTlkMmiBtmgrProcID, pTimer, timerCB, userArg, timeout);
}
int tlkmmi_btmgr_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg)
{
	return tlksys_adapt_initQueue(sTlkMmiBtmgrProcID, pProcs, queueCB, userArg);
}
void tlkmmi_btmgr_adaptDeinitTimer(tlkapi_timer_t *pTimer)
{
	tlksys_adapt_deinitTimer(sTlkMmiBtmgrProcID, pTimer);
}
void tlkmmi_btmgr_adaptDeinitQueue(tlkapi_queue_t *pProcs)
{
	tlksys_adapt_deinitQueue(sTlkMmiBtmgrProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_adaptIsHaveTimer, tlkmmi_btmgr_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_btmgr_adaptIsHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_isHaveTimer(sTlkMmiBtmgrProcID, pTimer);
}
bool tlkmmi_btmgr_adaptIsHaveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_isHaveQueue(sTlkMmiBtmgrProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_adaptAppendQueue, tlkmmi_btmgr_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_adaptAppendQueue(tlkapi_queue_t *pProcs)
{	
	return tlksys_adapt_appendQueue(sTlkMmiBtmgrProcID, pProcs);
}
int tlkmmi_btmgr_adaptRemoveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_removeQueue(sTlkMmiBtmgrProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_adaptUpdateTimer, tlkmmi_btmgr_adaptInsertTimer,
 *           tlkmmi_btmgr_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlksys_adapt_updateTimer(sTlkMmiBtmgrProcID, pTimer, timeout, isInsert);
}
int tlkmmi_btmgr_adaptInsertTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_insertTimer(sTlkMmiBtmgrProcID, pTimer, true);
}
int tlkmmi_btmgr_adaptRemoveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_removeTimer(sTlkMmiBtmgrProcID, pTimer);
}



#endif //#if (TLKMMI_BTMGR_ENABLE)


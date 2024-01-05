/********************************************************************************************************
 * @file	tlkmdi_btadapt.c
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
#if (TLK_STK_BT_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi_btadapt.h"


static uint08 sTlkMdiAtsProcID = 0;


/******************************************************************************
 * Function: tlkmdi_btadapt_init
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btadapt_init(uint08 procID)
{
	sTlkMdiAtsProcID = procID;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btadapt_handler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btadapt_handler(void)
{
	tlksys_adapt_handler(sTlkMdiAtsProcID);
}

/******************************************************************************
 * Function: tlkmdi_btadapt_interval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmdi_btadapt_interval(void)
{
	return tlksys_adapt_interval(sTlkMdiAtsProcID);
}

/******************************************************************************
 * Function: tlkmdi_btadapt_initTimer, tlkmdi_btadapt_deinitTimer
 *           tlkmdi_btadapt_initQueue, tlkmdi_btadapt_deinitQueue
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
int tlkmdi_btadapt_initTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	return tlksys_adapt_initTimer(sTlkMdiAtsProcID, pTimer, timerCB, userArg, timeout);
}
int tlkmdi_btadapt_initQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg)
{
	return tlksys_adapt_initQueue(sTlkMdiAtsProcID, pProcs, queueCB, userArg);
}
void tlkmdi_btadapt_deinitTimer(tlkapi_timer_t *pTimer)
{
	tlksys_adapt_deinitTimer(sTlkMdiAtsProcID, pTimer);
}
void tlkmdi_btadapt_deinitQueue(tlkapi_queue_t *pProcs)
{
	tlksys_adapt_deinitQueue(sTlkMdiAtsProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmdi_btadapt_isHaveTimer, tlkmdi_btadapt_isHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btadapt_isHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_isHaveTimer(sTlkMdiAtsProcID, pTimer);
}
bool tlkmdi_btadapt_isHaveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_isHaveQueue(sTlkMdiAtsProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmdi_btadapt_appendQueue, tlkmdi_btadapt_removeQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btadapt_appendQueue(tlkapi_queue_t *pProcs)
{	
	return tlksys_adapt_appendQueue(sTlkMdiAtsProcID, pProcs);
}
int tlkmdi_btadapt_removeQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_removeQueue(sTlkMdiAtsProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmdi_btadapt_updateTimer, tlkmdi_btadapt_insertTimer,
 *           tlkmdi_btadapt_removeTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btadapt_updateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlksys_adapt_updateTimer(sTlkMdiAtsProcID, pTimer, timeout, isInsert);
}
int tlkmdi_btadapt_insertTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_insertTimer(sTlkMdiAtsProcID, pTimer, true);
}
int tlkmdi_btadapt_removeTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_removeTimer(sTlkMdiAtsProcID, pTimer);
}



#endif //#if (TLK_STK_BT_ENABLE)


/********************************************************************************************************
 * @file	tlkmmi_audioAdapt.c
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
#if (TLKMMI_AUDIO_ENABLE)
#include "tlkmmi_audio.h"
#include "tlkmmi_audioAdapt.h"
#include "tlksys/tlksys_stdio.h"


static uint08 sTlkMmiAudioProcID = 0;

/******************************************************************************
 * Function: tlkmmi_audio_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_adaptInit(uint08 procID)
{
	sTlkMmiAudioProcID = procID;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_adaptHandler(void)
{
	tlksys_adapt_handler(sTlkMmiAudioProcID);
}

/******************************************************************************
 * Function: tlkmmi_audio_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_adaptInterval(void)
{
	return tlksys_adapt_interval(sTlkMmiAudioProcID);
}

/******************************************************************************
 * Function: tlkmmi_audio_adaptInitTimer, tlkmmi_audio_adaptDeinitTimer
 *           tlkmmi_audio_adaptInitQueue, tlkmmi_audio_adaptDeinitQueue
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
int tlkmmi_audio_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	return tlksys_adapt_initTimer(sTlkMmiAudioProcID, pTimer, timerCB, userArg, timeout);
}
int tlkmmi_audio_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg)
{
	return tlksys_adapt_initQueue(sTlkMmiAudioProcID, pProcs, queueCB, userArg);
}
void tlkmmi_audio_adaptDeinitTimer(tlkapi_timer_t *pTimer)
{
	tlksys_adapt_deinitTimer(sTlkMmiAudioProcID, pTimer);
}
void tlkmmi_audio_adaptDeinitQueue(tlkapi_queue_t *pProcs)
{
	tlksys_adapt_deinitQueue(sTlkMmiAudioProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_audio_adaptIsHaveTimer, tlkmmi_audio_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_adaptIsHaveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_isHaveTimer(sTlkMmiAudioProcID, pTimer);
}
bool tlkmmi_audio_adaptIsHaveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_isHaveQueue(sTlkMmiAudioProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_audio_adaptAppendQueue, tlkmmi_audio_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_adaptAppendQueue(tlkapi_queue_t *pProcs)
{	
	return tlksys_adapt_appendQueue(sTlkMmiAudioProcID, pProcs);
}
int tlkmmi_audio_adaptRemoveQueue(tlkapi_queue_t *pProcs)
{
	return tlksys_adapt_removeQueue(sTlkMmiAudioProcID, pProcs);
}

/******************************************************************************
 * Function: tlkmmi_audio_adaptUpdateTimer, tlkmmi_audio_adaptInsertTimer,
 *           tlkmmi_audio_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	return tlksys_adapt_updateTimer(sTlkMmiAudioProcID, pTimer, timeout, isInsert);
}
int tlkmmi_audio_adaptInsertTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_insertTimer(sTlkMmiAudioProcID, pTimer, true);
}
int tlkmmi_audio_adaptRemoveTimer(tlkapi_timer_t *pTimer)
{
	return tlksys_adapt_removeTimer(sTlkMmiAudioProcID, pTimer);
}


#endif //#if (TLKMMI_AUDIO_ENABLE)


/********************************************************************************************************
 * @file	tlkmmi_fileAdapt.h
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
#ifndef TLKMMI_FILE_ADAPT_H
#define TLKMMI_FILE_ADAPT_H

#if (TLKMMI_FILE_ENABLE)



/******************************************************************************
 * Function: tlkmmi_file_adaptInit
 * Descript: Initializes the adapter control parameters.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_file_adaptInit(uint08 procID);

/******************************************************************************
 * Function: tlkmmi_file_adaptHandler
 * Descript: Implement the function of traversing the scheduled task and work
 *           queue in the adapter to ensure the timeliness of execution.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_file_adaptHandler(void);

/******************************************************************************
 * Function: tlkmmi_file_adaptInterval
 * Descript: Gets the execution time of the next task in the process.
 * Params: None.
 * Return: The execution time of the next task in the process.
 * Others: None.
*******************************************************************************/
uint tlkmmi_file_adaptInterval(void);

/******************************************************************************
 * Function: tlkmmi_file_adaptInitTimer, tlkmmi_file_adaptDeinitTimer
 *           tlkmmi_file_adaptInitQueue, tlkmmi_file_adaptDeinitQueue
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
int  tlkmmi_file_adaptInitTimer(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout);
int  tlkmmi_file_adaptInitQueue(tlkapi_queue_t *pProcs, TlkApiQueueCB queueCB, uint32 userArg);
void tlkmmi_file_adaptDeinitTimer(tlkapi_timer_t *pTimer);
void tlkmmi_file_adaptDeinitQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_file_adaptIsHaveTimer, tlkmmi_file_adaptIsHaveQueue
 * Descript: Check whether scheduled tasks and cache work exist in the adaptation.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @pQueue[IN]--Work queue node.
 * Return: Return TRUE if has, FALSE otherwise.
 * Others: None.
*******************************************************************************/
bool tlkmmi_file_adaptIsHaveTimer(tlkapi_timer_t *pTimer);
bool tlkmmi_file_adaptIsHaveQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_file_adaptAppendQueue, tlkmmi_file_adaptRemoveQueue
 * Descript: Implement the function of adding and deleting work queues.
 * Params:
 *     @pQueue[IN]--Work queue node.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_file_adaptAppendQueue(tlkapi_queue_t *pProcs);
int  tlkmmi_file_adaptRemoveQueue(tlkapi_queue_t *pProcs);

/******************************************************************************
 * Function: tlkmmi_file_adaptUpdateTimer, tlkmmi_file_adaptInsertTimer,
 *           tlkmmi_file_adaptRemoveTimer
 * Descript: Implement the function of adding, deleting and updating timer.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @isUpdate[IN]--True,Timer recount; False,Depending on the state of the
 *       other parameters, the timer may continue the previous counting logic.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_file_adaptUpdateTimer(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert);
int  tlkmmi_file_adaptInsertTimer(tlkapi_timer_t *pTimer);
int  tlkmmi_file_adaptRemoveTimer(tlkapi_timer_t *pTimer);




#endif //#if (TLKMMI_FILE_ENABLE)

#endif //TLKMMI_FILE_ADAPT_H


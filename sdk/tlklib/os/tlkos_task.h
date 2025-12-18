/********************************************************************************************************
 * @file	tlkos_task.h
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
#ifndef TLKOS_TASK_H
#define TLKOS_TASK_H

#if (TLK_CFG_OS_ENABLE)


typedef void(*TlkOsTaskEnterCB)(void *pUsrArg);

typedef enum{
	TLKOS_TASK_STATE_NONE = 0,
	TLKOS_TASK_STATE_READY, /* The task being queried is in a read or pending ready list. */
	TLKOS_TASK_STATE_BLOCK, /* The task being queried is in the Blocked state. */
	TLKOS_TASK_STATE_RUNNING, /* A task is querying the state of itself, so must be running. */
	TLKOS_TASK_STATE_SUSPEND, /* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	TLKOS_TASK_STATE_DELETED, /* The task being queried has been deleted, but its TCB has not yet been freed. */
	TLKOS_TASK_STATE_INVALID, /* Used as an 'invalid state' value. */
}TLKOS_TASK_STATE_ENUM;



typedef struct{
	ulong handle;
}tlkos_task_t;



/******************************************************************************
 * Function: tlkos_task_create
 * Descript: Create a thread in the operating system (OS).
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 *     @pName[IN]--Name of the thread in which the task is to be run.
 *     @priority[IN]--Priority of the thread in which the task is to be run
 *     @stackSize[IN]--The stack size of the thread for which the task is to 
 *         be run. Unit: Byte.
 *     @enter[IN]--The execution entry of the thread for which the task is to 
 *         be run.
 *     @pUsrArg[IN]--The user parameters of the thread for which the task is 
 *         to be run are sent back to the user with "enter()". 
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_create(tlkos_task_t *pTask, char *pName, uint08 priority,  
	uint16 stackSize, TlkOsTaskEnterCB enter, void *pUsrArg);

/******************************************************************************
 * Function: tlkos_task_destroy
 * Descript: Destroy threads that have been created.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_destroy(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_delayMs
 * Descript: Delayed execution thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 *     @delayMs[IN]--Duration of the delay. Unit:us.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_delayMs(tlkos_task_t *pTask, uint delayMs);

/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Suspends the running of the thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_suspend(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_resume
 * Descript: Resuming the running of the thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_resume(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_suspendAll
 * Descript: Suspends the running of all thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_suspendAll(void);

/******************************************************************************
 * Function: tlkos_task_destroy
 * Descript: Resuming the running of all thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_resumeAll(void);

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Gets the current status of the task.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: The current status of the task. Negative values mean failure, others 
 *     refer to TLKOS_TASK_STATE_ENUM.
 *****************************************************************************/
int tlkos_task_getState(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_getPriority
 * Descript: Gets the priority of the task.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: The priority of the task. Negative values mean failure.
 *****************************************************************************/
int tlkos_task_getPriority(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_setPriority
 * Descript: Sets the priority of the task.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 *     @priority[IN]--The priority of the task to be set.
 * Return: Operating results. Negative values mean failure, others mean success.
 *****************************************************************************/
int tlkos_task_setPriority(tlkos_task_t *pTask, uint priority);

/******************************************************************************
 * Function: tlkos_task_getStackValid
 * Descript: Query the size of the remaining stack.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: The size of the remaining stack. Unit-Bytes. Negative values mean
 *     failure.
 *****************************************************************************/
int tlkos_task_getStackValid(tlkos_task_t *pTask);

/******************************************************************************
 * Function: tlkos_task_getCurHandle
 * Descript: Gets the handle to the currently running task.
 * Params: None.
 * Return: The handle to the currently running task. Zero means failure.
 *****************************************************************************/
ulong tlkos_task_getCurHandle(void);



#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_TASK_H


/********************************************************************************************************
 * @file	tlkos_task.c
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
#include "tlklib/os/tlkos_task.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/task.h"
#endif


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
int tlkos_task_create(tlkos_task_t *pTask, char *pName, uint08 priority, uint16 stackSize, 
	TlkOsTaskEnterCB enter, void *pUsrArg)
{
	if(pTask == nullptr || stackSize == 0 || enter == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	ulong handle = 0;
	BaseType_t ret;
	ret = xTaskCreate((TaskFunction_t)enter, pName, stackSize >> 2, pUsrArg, priority, (TaskHandle_t*)&handle);
	if(ret != pdPASS || handle == 0) return -TLK_EFAIL;
	pTask->handle = handle;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_destroy
 * Descript: Destroy threads that have been created.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_destroy(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return -TLK_ENOREADY;
	vTaskDelete((TaskHandle_t)(pTask->handle));
	pTask->handle = 0;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_delayMs
 * Descript: Delayed execution thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 *     @delayMs[IN]--Duration of the delay. Unit:us.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_delayMs(tlkos_task_t *pTask, uint delayMs)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	vTaskDelay(delayMs);
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}


/******************************************************************************
 * Function: tlkos_task_suspend
 * Descript: Suspends the running of the thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_suspend(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return -TLK_ENOREADY;
	vTaskSuspend((TaskHandle_t)(pTask->handle));
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_resume
 * Descript: Resuming the running of the thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_resume(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return -TLK_ENOREADY;
	vTaskResume((TaskHandle_t)(pTask->handle));
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_suspendAll
 * Descript: Suspends the running of all thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_suspendAll(void)
{
#if (TLK_OS_FREERTOS_ENABLE)
	vTaskSuspendAll();
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif

}

/******************************************************************************
 * Function: tlkos_task_destroy
 * Descript: Resuming the running of all thread.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkos_task_resumeAll(void)
{
#if (TLK_OS_FREERTOS_ENABLE)
	xTaskResumeAll();
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_getState
 * Descript: Gets the current status of the task.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: The current status of the task. Negative values mean failure, others 
 *     refer to TLKOS_TASK_STATE_ENUM.
 *****************************************************************************/
int tlkos_task_getState(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return TLKOS_TASK_STATE_NONE;
	eTaskState state = eTaskGetState((const TaskHandle_t)(pTask->handle));
	if(state == eRunning) return TLKOS_TASK_STATE_RUNNING;
    else if(state == eReady) return TLKOS_TASK_STATE_READY;
    else if(state == eBlocked) return TLKOS_TASK_STATE_BLOCK;
    else if(state == eSuspended) return TLKOS_TASK_STATE_SUSPEND;
    else if(state == eDeleted) return TLKOS_TASK_STATE_DELETED;
    else if(state == eInvalid) return TLKOS_TASK_STATE_INVALID;
	else return TLKOS_TASK_STATE_NONE;
#else
	return -TLK_ENOSUPPORT;
#endif		
}

/******************************************************************************
 * Function: tlkos_task_getPriority
 * Descript: Gets the priority of the task.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: The priority of the task. Negative values mean failure.
 *****************************************************************************/
int tlkos_task_getPriority(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return 0;
	return uxTaskPriorityGet((const TaskHandle_t)(pTask->handle));
#else
	return -TLK_ENOSUPPORT;
#endif	
}

/******************************************************************************
 * Function: tlkos_task_setPriority
 * Descript: Sets the priority of the task.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 *     @priority[IN]--The priority of the task to be set.
 * Return: Operating results. Negative values mean failure, others mean success.
 *****************************************************************************/
int tlkos_task_setPriority(tlkos_task_t *pTask, uint priority)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTask->handle == 0) return -TLK_ENOREADY;
	vTaskPrioritySet((TaskHandle_t)(pTask->handle), priority);
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif	
}

/******************************************************************************
 * Function: tlkos_task_getStackValid
 * Descript: Query the size of the remaining stack.
 * Params: 
 *     @pTask[IN]--The control block of the thread object.
 * Return: The size of the remaining stack. Unit-Bytes. Negative values mean
 *     failure.
 *****************************************************************************/
int tlkos_task_getStackValid(tlkos_task_t *pTask)
{
	if(pTask == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	return 4*uxTaskGetStackHighWaterMark((TaskHandle_t)(pTask->handle));
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_task_getCurHandle
 * Descript: Gets the handle to the currently running task.
 * Params: None.
 * Return: The handle to the currently running task. Zero means failure.
 *****************************************************************************/
ulong tlkos_task_getCurHandle(void)
{
#if (TLK_OS_FREERTOS_ENABLE)
	return (ulong)xTaskGetCurrentTaskHandle();
#else
	return 0;
#endif
}


#endif //#if (TLK_CFG_OS_ENABLE)


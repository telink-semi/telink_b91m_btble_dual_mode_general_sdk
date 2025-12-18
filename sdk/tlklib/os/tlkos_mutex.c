/********************************************************************************************************
 * @file	tlkos_mutex.c
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
#include "tlklib/os/tlkos_mutex.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/task.h"
#include "tlklib/os/freertos/include/queue.h"
#include "tlklib/os/freertos/include/semphr.h"
#endif

extern uint08 gTlkOsKernelIsStart;

/******************************************************************************
 * Function: tlkos_mutex_create
 * Descript: Create a mutex.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
int tlkos_mutex_create(tlkos_mutex_t *pMutex)
{
	if(pMutex == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	pMutex->handle = (ulong)xSemaphoreCreateMutex();
	if(pMutex->handle == 0) return -TLK_EFAIL;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}
/******************************************************************************
 * Function: tlkos_mutex_destroy
 * Descript: Destroy a mutex.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_mutex_destroy(tlkos_mutex_t *pMutex)
{
	if(pMutex == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pMutex->handle == 0) return -TLK_EPARAM;
	vSemaphoreDelete((QueueHandle_t)(pMutex->handle));
	pMutex->handle = 0;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_mutex_lock
 * Descript: Adds a mutex to a program segment.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_mutex_lock(tlkos_mutex_t *pMutex)
{
	if(pMutex == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pMutex->handle == 0 || !gTlkOsKernelIsStart) return -TLK_ENOREADY;
	BaseType_t ret = xSemaphoreTake((QueueHandle_t)(pMutex->handle), portMAX_DELAY);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_mutex_lock
 * Descript: Release the mutex.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_mutex_unlock(tlkos_mutex_t *pMutex)
{
	if(pMutex == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pMutex->handle == 0 || !gTlkOsKernelIsStart) return -TLK_ENOREADY;
	BaseType_t ret = xSemaphoreGive((QueueHandle_t)(pMutex->handle));
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}



#endif //#if (TLK_CFG_OS_ENABLE)


/********************************************************************************************************
 * @file	tlkos_sem.c
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
#include "tlklib/os/tlkos_sem.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/task.h"
#include "tlklib/os/freertos/include/queue.h"
#include "tlklib/os/freertos/include/semphr.h"
#endif


/******************************************************************************
 * Function: tlkos_sem_isFull
 * Descript: Checks whether the current semaphore is full.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Returns whether the current semaphore is full. True means that the
 *      semaphore is all being used.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
bool tlkos_sem_isFull(tlkos_sem_t *pSem)
{
	if(pSem == nullptr) return false;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pSem->handle == 0) return false;
	uint count = uxSemaphoreGetCount((QueueHandle_t)(pSem->handle));
	if(count == pSem->count) return true;
	return false;
#else
	return false;
#endif
}

/******************************************************************************
 * Function: tlkos_sem_isEmpty
 * Descript: Checks whether the current semaphore is empty.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Returns whether the current semaphore is empty. True means that the
 *      semaphore is not being used.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
bool tlkos_sem_isEmpty(tlkos_sem_t *pSem)
{
	if(pSem == nullptr) return true;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pSem->handle == 0) return true;
	uint count = uxSemaphoreGetCount((QueueHandle_t)(pSem->handle));
	if(count == 0) return true;
	return false;
#else
	return true;
#endif
}

/******************************************************************************
 * Function: tlkos_sem_create
 * Descript: Create a semaphore.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 *     @maxCount[IN]--Number of signals in the semaphore.
 *     @iniCount[IN]--Number of signals that were occupied during initialization.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
int tlkos_sem_create(tlkos_sem_t *pSem, uint maxCount, uint iniCount)
{
	if(pSem == nullptr || maxCount == 0 || iniCount > maxCount) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	pSem->count = maxCount;
	pSem->handle = (ulong)xSemaphoreCreateCounting(maxCount, iniCount);
	if(pSem->handle == 0) return -TLK_EFAIL;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_sem_destroy
 * Descript: Destroy semaphore.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_sem_destroy(tlkos_sem_t *pSem)
{
	if(pSem == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pSem->handle == 0) return -TLK_EPARAM;
	vSemaphoreDelete((QueueHandle_t)(pSem->handle));
	pSem->count = 0;
	pSem->handle = 0;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_sem_take
 * Descript: Retrieves one signal from the semaphore queue.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 *     @timeout[IN]--The maximum time to wait. Unit:us.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_sem_take(tlkos_sem_t *pSem, uint32 timeout)
{
	if(pSem == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pSem->handle == 0) return -TLK_ENOREADY;
	if(timeout != 0 && timeout != portMAX_DELAY){
		if(timeout < 1000000/configTICK_RATE_HZ) timeout = 1;
		else timeout = (timeout*configTICK_RATE_HZ)/1000000;
	}
	BaseType_t ret = xSemaphoreTake((QueueHandle_t)(pSem->handle), timeout);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_sem_post
 * Descript: Releases a signal into the semaphore queue.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_sem_post(tlkos_sem_t *pSem)
{
	if(pSem == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pSem->handle == 0) return -TLK_ENOREADY;
	BaseType_t ret = xSemaphoreGive((QueueHandle_t)(pSem->handle));
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}




#endif //#if (TLK_CFG_OS_ENABLE)


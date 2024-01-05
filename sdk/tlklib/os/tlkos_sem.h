/********************************************************************************************************
 * @file	tlkos_sem.h
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
#ifndef TLKOS_SEMAPHORE_H
#define TLKOS_SEMAPHORE_H

#if (TLK_CFG_OS_ENABLE)


typedef struct{
#if (TLK_OS_FREERTOS_ENABLE)
	uint count;
	ulong handle;
#else
	ulong reserve;
#endif
}tlkos_sem_t;

/******************************************************************************
 * Function: tlkos_sem_isFull
 * Descript: Checks whether the current semaphore is full.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Returns whether the current semaphore is full. True means that the
 *      semaphore is all being used.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
bool tlkos_sem_isFull(tlkos_sem_t *pSem);

/******************************************************************************
 * Function: tlkos_sem_isEmpty
 * Descript: Checks whether the current semaphore is empty.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Returns whether the current semaphore is empty. True means that the
 *      semaphore is not being used.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
bool tlkos_sem_isEmpty(tlkos_sem_t *pSem);

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
int tlkos_sem_create(tlkos_sem_t *pSem, uint maxCount, uint iniCount);

/******************************************************************************
 * Function: tlkos_sem_destroy
 * Descript: Retrieves one signal from the semaphore queue.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_sem_destroy(tlkos_sem_t *pSem);

/******************************************************************************
 * Function: tlkos_sem_take
 * Descript: Retrieves one signal from the semaphore queue.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 *     @timeout[IN]--The maximum time to wait. Unit:us.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_sem_take(tlkos_sem_t *pSem, uint32 timeout);

/******************************************************************************
 * Function: tlkos_sem_post
 * Descript: Releases a signal into the semaphore queue.
 * Params:
 *     @pSem[IN]--Seamphore control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_sem_post(tlkos_sem_t *pSem);




#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_SEMAPHORE_H


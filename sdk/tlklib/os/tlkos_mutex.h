/********************************************************************************************************
 * @file	tlkos_mutex.h
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
#ifndef TLKOS_MUTEX_H
#define TLKOS_MUTEX_H

#if (TLK_CFG_OS_ENABLE)


typedef struct{
#if (TLK_OS_FREERTOS_ENABLE)
	ulong handle;
#else
	ulong reserve;
#endif
}tlkos_mutex_t;


/******************************************************************************
 * Function: tlkos_mutex_create
 * Descript: Create a mutex.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
int tlkos_mutex_create(tlkos_mutex_t *pMutex);

/******************************************************************************
 * Function: tlkos_mutex_destroy
 * Descript: Destroy a mutex.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_mutex_destroy(tlkos_mutex_t *pMutex);

/******************************************************************************
 * Function: tlkos_mutex_lock
 * Descript: Adds a mutex to a program segment.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_mutex_lock(tlkos_mutex_t *pMutex);

/******************************************************************************
 * Function: tlkos_mutex_lock
 * Descript: Release the mutex.
 * Params:
 *     @pSem[IN]--Mutex control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_mutex_unlock(tlkos_mutex_t *pMutex);




#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_MUTEX_H


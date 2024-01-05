/********************************************************************************************************
 * @file	tlkos_timer.c
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
#include "tlklib/os/tlkos_timer.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/timers.h"
#endif



/******************************************************************************
 * Function: tlkos_timer_create
 * Descript: Creates a new software timer instance.
 * Params:
 *     @pTimer[OUT]--Timer control structure.
 *     @pName[IN]--The name of the timer to be created.
 *     @period[IN]--The period of the timer to be created.
 *     @autoReload[IN]--Automatic reload or not.
 *     @enter[IN]--The callback function of the timer to be created.
 *     @pUsrArg[IN]--The user argument of the timer to be created.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
int tlkos_timer_create(tlkos_timer_t *pTimer, char *pName, uint period, bool autoReload,
	TlkOsTimerEnterCB enter, void *pUsrArg)
{
	if(pTimer == nullptr || enter == nullptr || period == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	pTimer->handle = (ulong)xTimerCreate(pName, period, autoReload, pUsrArg, (TimerCallbackFunction_t)enter);
	if(pTimer->handle == 0) return -TLK_EFAIL;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_timer_destroy
 * Descript: Destroy a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_destroy(tlkos_timer_t *pTimer)
{
	if(pTimer == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTimer->handle == 0) return -TLK_ENOREADY;
	BaseType_t ret = xTimerDelete((TimerHandle_t)(pTimer->handle), 0);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_timer_start
 * Descript: Start a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_start(tlkos_timer_t *pTimer, uint delay)
{
	if(pTimer == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTimer->handle == 0) return -TLK_ENOREADY;
	BaseType_t ret = xTimerStart((TimerHandle_t)(pTimer->handle), delay);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_timer_reset
 * Descript: Reset a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_reset(tlkos_timer_t *pTimer)
{
	if(pTimer == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTimer->handle == 0) return -TLK_ENOREADY;
	BaseType_t ret = xTimerReset((TimerHandle_t)(pTimer->handle), 0);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_timer_stop
 * Descript: Stop a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_stop(tlkos_timer_t *pTimer)
{
	if(pTimer == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTimer->handle == 0) return -TLK_ENOREADY;
	BaseType_t ret = xTimerStop((TimerHandle_t)(pTimer->handle), 0);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_timer_setPeriod
 * Descript: Sets the period of the timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 *     @period[IN]--The period of the timer to be set.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_setPeriod(tlkos_timer_t *pTimer, uint period)
{
	if(pTimer == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pTimer->handle == 0) return -TLK_ENOREADY;
	BaseType_t ret = xTimerChangePeriod((TimerHandle_t)(pTimer->handle), period, 0);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}



#endif //#if (TLK_CFG_OS_ENABLE)


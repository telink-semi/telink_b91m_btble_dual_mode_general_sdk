/********************************************************************************************************
 * @file	tlkos_timer.h
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
#ifndef TLKOS_TIMER_H
#define TLKOS_TIMER_H

#if (TLK_CFG_OS_ENABLE)


typedef void(*TlkOsTimerEnterCB)(ulong handle, void *pUsrArg);


typedef struct{
	ulong handle;
}tlkos_timer_t;



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
	TlkOsTimerEnterCB enter, void *pUsrArg);

/******************************************************************************
 * Function: tlkos_timer_destroy
 * Descript: Destroy a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_destroy(tlkos_timer_t *pTimer);

/******************************************************************************
 * Function: tlkos_timer_start
 * Descript: Start a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_start(tlkos_timer_t *pTimer, uint delay);

/******************************************************************************
 * Function: tlkos_timer_reset
 * Descript: Reset a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_reset(tlkos_timer_t *pTimer);

/******************************************************************************
 * Function: tlkos_timer_stop
 * Descript: Stop a timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_stop(tlkos_timer_t *pTimer);

/******************************************************************************
 * Function: tlkos_timer_setPeriod
 * Descript: Sets the period of the timer.
 * Params:
 *     @pTimer[IN]--Timer control structure.
 *     @period[IN]--The period of the timer to be set.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
*******************************************************************************/
int tlkos_timer_setPeriod(tlkos_timer_t *pTimer, uint period);



#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_TIMER_H


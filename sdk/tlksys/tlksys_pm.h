/********************************************************************************************************
 * @file	tlksys_pm.h
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
#ifndef TLKSYS_PM_H
#define TLKSYS_PM_H

#if (TLK_CFG_SYS_ENABLE)



#define TLKSYS_PM_BUSY_CHECK_FUNC_MAX_NUMB      10
#define TLKSYS_PM_ENTER_SLEEP_FUNC_MAX_NUMB     4
#define TLKSYS_PM_LEAVE_SLEEP_FUNC_MAX_NUMB     4


typedef bool(*TlkSysPmBusyCheckCallback)(void);
typedef void(*TlkSysPmEnterSleepCallback)(uint mcuMode);
typedef void(*TlkSysPmLeaveSleepCallback)(uint wakeSrc);


typedef struct{
	uint08 reserve;
	uint08 busyCheckCount;
	uint08 enterSleepCount;
	uint08 leaveSleepCount;
	const char *name[TLKSYS_PM_BUSY_CHECK_FUNC_MAX_NUMB];
	TlkSysPmBusyCheckCallback  busyCheck[TLKSYS_PM_BUSY_CHECK_FUNC_MAX_NUMB];
	TlkSysPmEnterSleepCallback enterSleep[TLKSYS_PM_ENTER_SLEEP_FUNC_MAX_NUMB];
	TlkSysPmLeaveSleepCallback leaveSleep[TLKSYS_PM_LEAVE_SLEEP_FUNC_MAX_NUMB];
}tlksys_pm_cbCtrl_t;


/******************************************************************************
 * Function: tlksys_pm_init
 * Descript: 
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_init(void);

/******************************************************************************
 * Function: tlksys_pm_appendBusyCheckCB
 * Descript: Add a task function to the 'sTlkSysPmCbCtrl.busyCheck' table that
 * 			 needs to detect its busy status.
 * Params:
 *     @cb[IN]--Function name of the task function.
 *     @name[IN]--Identification name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_appendBusyCheckCB(TlkSysPmBusyCheckCallback cb, const char *name);

/******************************************************************************
 * Function: tlksys_pm_removeBusyCheckCB
 * Descript: Remove a task function from the 'sTlkSysPmCbCtrl.busyCheck' table.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_removeBusyCheckCB(TlkSysPmBusyCheckCallback cb);

/******************************************************************************
 * Function: tlksys_pm_appendEnterSleepCB
 * Descript: Add a task function to the 'sTlkSysPmCbCtrl.enterSleep' table that
 * 			 needs to go to sleep.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_appendEnterSleepCB(TlkSysPmEnterSleepCallback cb);

/******************************************************************************
 * Function: tlksys_pm_removeEnterSleepCB
 * Descript: Remove a task function from the 'sTlkSysPmCbCtrl.enterSleep' table.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_removeEnterSleepCB(TlkSysPmEnterSleepCallback cb);

/******************************************************************************
 * Function: tlksys_pm_appendLeaveSleepCB
 * Descript: Add a task function to the 'sTlkSysPmCbCtrl.leaveSleep' table that
 * 			 needs to exit sleep.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_appendLeaveSleepCB(TlkSysPmLeaveSleepCallback cb);

/******************************************************************************
 * Function: tlksys_pm_removeLeaveSleepCB
 * Descript: Remove the task function for exiting sleep from the
 * 			 'sTlkSysPmCbCtrl.leaveSleep' table.
 * Params:
 *     @cb[IN]--Task functions to be removed.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_removeLeaveSleepCB(TlkSysPmLeaveSleepCallback cb);

/******************************************************************************
 * Function: tlksys_pm_isBusy
 * Descript: Get whether the function in the 'sTlkSysPmCbCtrl.busyCheck' table
 * 			 is in the BUSY state.
 * Params:
 * Return: true means success, false means failure.
 * Others: None.
*******************************************************************************/
bool tlksys_pm_isBusy(void);

/******************************************************************************
 * Function: tlksys_pm_getBusyName
 * Descript: Get the name of the currently busy task from the
 * 			 'sTlkSysPmCbCtrl.busyCheck' table
 * Params:
 * Return: Busy task name.
 * Others: None.
*******************************************************************************/
const char *tlksys_pm_getBusyName(void);

/******************************************************************************
 * Function: tlksys_pm_enterSleep
 * Descript: Enter low power mode.
 * Params:
 *     @wake[IN]--Low power operation mode, refer to pm_sleep_mode_e.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlksys_pm_enterSleep(uint mcuMode);

/******************************************************************************
 * Function: tlksys_pm_leaveSleep
 * Descript: Leaving low power mode.
 * Params:
 *     @wake[IN]--Wake up source, refer to pm_sleep_wakeup_src_e.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlksys_pm_leaveSleep(uint wakeSrc);



#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_PM_H


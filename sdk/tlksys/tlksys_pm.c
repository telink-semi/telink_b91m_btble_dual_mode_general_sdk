/********************************************************************************************************
 * @file	tlksys_pm.c
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
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys_stdio.h"
#include "tlksys.h"
#include "tlksys_pm.h"


static tlksys_pm_cbCtrl_t sTlkSysPmCbCtrl = {0};


/******************************************************************************
 * Function: tlksys_pm_init
 * Descript: 
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_init(void)
{
	return TLK_ENONE;
}

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
int tlksys_pm_appendBusyCheckCB(TlkSysPmBusyCheckCallback cb, const char *name)
{
	if(sTlkSysPmCbCtrl.busyCheckCount >= TLKSYS_PM_BUSY_CHECK_FUNC_MAX_NUMB){
		return -TLK_EQUOTA;
	}
	sTlkSysPmCbCtrl.name[sTlkSysPmCbCtrl.busyCheckCount] = name;
	sTlkSysPmCbCtrl.busyCheck[sTlkSysPmCbCtrl.busyCheckCount++] = cb;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_pm_removeBusyCheckCB
 * Descript: Remove a task function from the 'sTlkSysPmCbCtrl.busyCheck' table.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_removeBusyCheckCB(TlkSysPmBusyCheckCallback cb)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.busyCheckCount; index++){
		if(sTlkSysPmCbCtrl.busyCheck[index] == 0 || sTlkSysPmCbCtrl.busyCheck[index] == cb) break;
	}
	if(index == sTlkSysPmCbCtrl.busyCheckCount){
		return -TLK_ENOOBJECT;
	}
	if(index+1 == sTlkSysPmCbCtrl.busyCheckCount){
		sTlkSysPmCbCtrl.name[index] = nullptr;
		sTlkSysPmCbCtrl.busyCheck[index] = 0;
	}else{
		tmemcpy((void *)sTlkSysPmCbCtrl.name[index], (const void *)sTlkSysPmCbCtrl.name[index+1],
			(sTlkSysPmCbCtrl.busyCheckCount-index-1)*sizeof(char*));
		tmemcpy(sTlkSysPmCbCtrl.busyCheck[index], sTlkSysPmCbCtrl.busyCheck[index+1], 
			(sTlkSysPmCbCtrl.busyCheckCount-index-1)*sizeof(TlkSysPmBusyCheckCallback));
	}
	sTlkSysPmCbCtrl.busyCheckCount --;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_pm_appendEnterSleepCB
 * Descript: Add a task function to the 'sTlkSysPmCbCtrl.enterSleep' table that
 * 			 needs to go to sleep.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_appendEnterSleepCB(TlkSysPmEnterSleepCallback cb)
{
	if(sTlkSysPmCbCtrl.enterSleepCount >= TLKSYS_PM_ENTER_SLEEP_FUNC_MAX_NUMB){
		return -TLK_EQUOTA;
	}
	sTlkSysPmCbCtrl.enterSleep[sTlkSysPmCbCtrl.enterSleepCount++] = cb;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_pm_removeEnterSleepCB
 * Descript: Remove a task function from the 'sTlkSysPmCbCtrl.enterSleep' table.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_removeEnterSleepCB(TlkSysPmEnterSleepCallback cb)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.enterSleepCount; index++){
		if(sTlkSysPmCbCtrl.enterSleep[index] == nullptr || sTlkSysPmCbCtrl.enterSleep[index] == cb) break;
	}
	if(index == sTlkSysPmCbCtrl.enterSleepCount){
		return -TLK_ENOOBJECT;
	}
	if(index+1 == sTlkSysPmCbCtrl.enterSleepCount){
		sTlkSysPmCbCtrl.name[index] = nullptr;
		sTlkSysPmCbCtrl.enterSleep[index] = 0;
	}else{
		tmemcpy((void *)sTlkSysPmCbCtrl.name[index], (const void *)sTlkSysPmCbCtrl.name[index+1],
			(sTlkSysPmCbCtrl.enterSleepCount-index-1)*sizeof(char*));
		tmemcpy(sTlkSysPmCbCtrl.enterSleep[index], sTlkSysPmCbCtrl.enterSleep[index+1], 
			(sTlkSysPmCbCtrl.enterSleepCount-index-1)*sizeof(TlkSysPmBusyCheckCallback));
	}
	sTlkSysPmCbCtrl.enterSleepCount --;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_pm_appendLeaveSleepCB
 * Descript: Add a task function to the 'sTlkSysPmCbCtrl.leaveSleep' table that
 * 			 needs to exit sleep.
 * Params:
 *     @cb[IN]--Function name of the task function.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_appendLeaveSleepCB(TlkSysPmLeaveSleepCallback cb)
{
	if(sTlkSysPmCbCtrl.leaveSleepCount >= TLKSYS_PM_LEAVE_SLEEP_FUNC_MAX_NUMB){
		return -TLK_EQUOTA;
	}
	sTlkSysPmCbCtrl.leaveSleep[sTlkSysPmCbCtrl.leaveSleepCount++] = cb;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_pm_removeLeaveSleepCB
 * Descript: Remove the task function for exiting sleep from the
 * 			 'sTlkSysPmCbCtrl.leaveSleep' table.
 * Params:
 *     @cb[IN]--Task functions to be removed.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlksys_pm_removeLeaveSleepCB(TlkSysPmLeaveSleepCallback cb)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.leaveSleepCount; index++){
		if(sTlkSysPmCbCtrl.leaveSleep[index] == nullptr || sTlkSysPmCbCtrl.leaveSleep[index] == cb) break;
	}
	if(index == sTlkSysPmCbCtrl.leaveSleepCount){
		return -TLK_ENOOBJECT;
	}
	if(index+1 == sTlkSysPmCbCtrl.leaveSleepCount){
		sTlkSysPmCbCtrl.name[index] = nullptr;
		sTlkSysPmCbCtrl.leaveSleep[index] = 0;
	}else{
		tmemcpy((void *)sTlkSysPmCbCtrl.name[index], (const void *)sTlkSysPmCbCtrl.name[index+1],
			(sTlkSysPmCbCtrl.leaveSleepCount-index-1)*sizeof(char*));
		tmemcpy(sTlkSysPmCbCtrl.leaveSleep[index], sTlkSysPmCbCtrl.leaveSleep[index+1], 
			(sTlkSysPmCbCtrl.leaveSleepCount-index-1)*sizeof(TlkSysPmBusyCheckCallback));
	}
	sTlkSysPmCbCtrl.leaveSleepCount --;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlksys_pm_isBusy
 * Descript: Get whether the function in the 'sTlkSysPmCbCtrl.busyCheck' table
 * 			 is in the BUSY state.
 * Params:
 * Return: true means success, false means failure.
 * Others: None.
*******************************************************************************/
bool tlksys_pm_isBusy(void)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.busyCheckCount; index++){
		if(sTlkSysPmCbCtrl.busyCheck[index] != nullptr && sTlkSysPmCbCtrl.busyCheck[index]()) break;
	}
	if(index == sTlkSysPmCbCtrl.busyCheckCount) return false;
	return true;
}

/******************************************************************************
 * Function: tlksys_pm_getBusyName
 * Descript: Get the name of the currently busy task from the
 * 			 'sTlkSysPmCbCtrl.busyCheck' table
 * Params:
 * Return: Busy task name.
 * Others: None.
*******************************************************************************/
const char *tlksys_pm_getBusyName(void)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.busyCheckCount; index++){
		if(sTlkSysPmCbCtrl.busyCheck[index] != nullptr && sTlkSysPmCbCtrl.busyCheck[index]()) break;
	}
	if(index == sTlkSysPmCbCtrl.busyCheckCount) return nullptr;
	return sTlkSysPmCbCtrl.name[index];
}

/******************************************************************************
 * Function: tlksys_pm_enterSleep
 * Descript: Enter low power mode.
 * Params:
 *     @wake[IN]--Low power operation mode, refer to pm_sleep_mode_e.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlksys_pm_enterSleep(uint mcuMode)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.enterSleepCount; index++){
		if(sTlkSysPmCbCtrl.enterSleep[index] != nullptr) sTlkSysPmCbCtrl.enterSleep[index](mcuMode);
	}
}

/******************************************************************************
 * Function: tlksys_pm_leaveSleep
 * Descript: Leaving low power mode.
 * Params:
 *     @wake[IN]--Wake up source, refer to pm_sleep_wakeup_src_e.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlksys_pm_leaveSleep(uint wakeSrc)
{
	uint08 index;
	for(index=0; index<sTlkSysPmCbCtrl.leaveSleepCount; index++){
		if(sTlkSysPmCbCtrl.leaveSleep[index] != nullptr) sTlkSysPmCbCtrl.leaveSleep[index](wakeSrc);
	}
#if ((TLK_CFG_PM_ENABLE) && (TLK_OS_FREERTOS_ENABLE))
	vPortRestoreTask();
#endif
}


#endif //#if (TLK_CFG_SYS_ENABLE)


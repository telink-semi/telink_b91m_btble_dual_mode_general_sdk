/********************************************************************************************************
 * @file	tlkdev_battery.h
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
#ifndef TLKDEV_BATTERY_H
#define TLKDEV_BATTERY_H

#if (TLK_DEV_BATTERY_ENABLE)





/******************************************************************************
 * Function: tlkdev_battery_init
 * Descript: Initial the battery's gpio and register the callback.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkdev_battery_init(void);


void tlkdev_battery_enterSleep(uint mcuMode);
void tlkdev_battery_leaveSleep(uint wakeSrc);



int tlkdev_battery_getVoltage(uint16 *pVoltage);


#endif //TLK_DEV_BATTERY_ENABLE

#endif //TLKDEV_BATTERY_H


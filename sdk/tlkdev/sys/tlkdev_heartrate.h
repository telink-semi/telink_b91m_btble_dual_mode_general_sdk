/********************************************************************************************************
 * @file	tlkdev_heartrate.h
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
#ifndef TLKDEV_HEART_RATE_H
#define TLKDEV_HEART_RATE_H

#if (TLK_DEV_HEARTRATE_ENABLE)



bool tlkdev_heartRate_isOpen(void);

int tlkdev_heartRate_init(void);
int tlkdev_heartRate_open(void);
int tlkdev_heartRate_close(void);
int tlkdev_heartRate_reset(void);
int tlkdev_heartRate_handler(uint16 opcode, uint32 param0, uint32 param1);



#endif //TLK_DEV_HEARTRATE_ENABLE

#endif //TLKDEV_HEART_RATE_H


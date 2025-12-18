/********************************************************************************************************
 * @file	tlkpti_sys.h
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
#ifndef TLKPTI_SYS_H
#define TLKPTI_SYS_H


#define TLKPTI_SYS_SERIAL_HEADLEN    6

enum TLKPTI_SYS_MSGID_ENUM{
	TLKPTI_SYS_MSGID_NONE = 0x00,
	TLKPTI_SYS_MSGID_SERIAL_SEND,
	TLKPTI_SYS_MSGID_BATTERY_REPORT,
	TLKPTI_SYS_MSGID_REG_COMM_TASK,
};






#endif //TLKPTI_SYS_H


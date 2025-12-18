/********************************************************************************************************
 * @file	tlkmmi_test.h
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
#ifndef TLKMMI_TEST_H
#define TLKMMI_TEST_H

#if (TLKMMI_TEST_ENABLE)

#define TLKMMI_TEST_DBG_FLAG         ((TLK_MAJOR_DBGID_MMI_TEST << 24) | (TLK_MINOR_DBGID_MMI_TEST << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_TEST_DBG_SIGN         "[TEST]"


#define TLKMMI_TEST_TIMEOUT          50000 //50ms


typedef enum{
	TLKMMI_TEST_BUSY_NONE   = 0x0000,
	TLKMMI_TEST_BUSY_REBOOT = 0x0001,
}TLKMMI_TEST_BUSYS_ENUM;


typedef struct{
	uint08 wmode; //workMode
	uint16 flags;
	uint16 busys;
	uint16 rebootTime;
	tlkapi_timer_t timer;
}tlkmmi_test_ctrl_t;



void tlkmmi_test_reboot(uint16 timeout);



#endif //#if (TLKMMI_TEST_ENABLE)

#endif //TLKMMI_TEST_H


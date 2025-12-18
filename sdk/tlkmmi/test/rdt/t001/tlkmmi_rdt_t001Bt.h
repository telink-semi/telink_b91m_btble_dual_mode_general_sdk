/********************************************************************************************************
 * @file	tlkmmi_rdt_t001Bt.h
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
#ifndef TLKMMI_RDT_T001_BT_H
#define TLKMMI_RDT_T001_BT_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T001_ENABLE)




typedef enum{
	RDT_CASE_T001_BTBUSY_NONE = 0,
	RDT_CASE_T001_BTBUSY_WAIT_PEER_ATTR = 0x0001,
	RDT_CASE_T001_BTBUSY_START_SCAN = 0x0002,
	RDT_CASE_T001_BTBUSY_CLOSE_SCAN = 0x0004,
}RDT_CASE_T001_BTBUSY_ENUM;


typedef struct{
	uint16 busys;
	uint08 initRole;
	uint32 devClass;
	uint08 btaddr[6];
}tlkmmi_rdt_t001Bt_t;


void tlkmmi_rdt_t001BtStart(void);
void tlkmmi_rdt_t001BtClose(void);
bool tlkmmi_rdt_t001BtTimer(void);

void tlkmmi_rdt_t001BtInput(uint08 msgID, uint08 *pData, uint16 dataLen);


#endif //#if (TLKMMI_RDT_CASE_T001_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T001_BT_H


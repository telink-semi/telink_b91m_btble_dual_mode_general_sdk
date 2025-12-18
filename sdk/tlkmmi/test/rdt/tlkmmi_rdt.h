/********************************************************************************************************
 * @file	tlkmmi_rdt.h
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
#ifndef TLKMMI_RDT_H
#define TLKMMI_RDT_H

#if (TLK_TEST_RDT_ENABLE)


void tlkmmi_rdt_reboot(void);

int tlkmmi_rdt_startTest(uint16 caseID, uint08 role);
int tlkmmi_rdt_pauseTest(void);
int tlkmmi_rdt_closeTest(void);
int tlkmmi_rdt_testInput(uint08 msgID, uint08 *pData, uint16 dataLen);


#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_H


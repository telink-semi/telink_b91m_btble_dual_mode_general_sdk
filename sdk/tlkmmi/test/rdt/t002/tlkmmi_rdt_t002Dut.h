/********************************************************************************************************
 * @file	tlkmmi_rdt_t002Dut.h
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
#ifndef TLKMMI_RDT_T002_DUT_H
#define TLKMMI_RDT_T002_DUT_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T002_ENABLE)




typedef enum{
	RDT_CASE_T002_DUT_BUSY_NONE = 0,
}RDT_CASE_T002_DUT_BUSY_ENUM;


typedef struct{
	uint08 isStart;
	uint16 sndCount;
	uint16 rcvCount;
	uint32 dbgTimer;
	uint32 sndTimer;
	uint32 rcvTimer;
	uint08 aut1Flags;
	uint08 aut2Flags;
	uint08 aut1Busys;
	uint08 aut2Busys;
	uint16 scoHandle;
	uint16 aut1Handle;
	uint16 aut2Handle;
}tlkmmi_rdt_t002Dut_t;


void tlkmmi_rdt_t002DutStart(void);
void tlkmmi_rdt_t002DutClose(void);
void tlkmmi_rdt_t002DutTimer(void);
void tlkmmi_rdt_t002DutInput(uint08 msgID, uint08 *pData, uint16 dataLen);




#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T002_DUT_H


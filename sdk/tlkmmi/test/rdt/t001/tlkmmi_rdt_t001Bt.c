/********************************************************************************************************
 * @file	tlkmmi_rdt_t001Bt.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../../tlkmmi_testStdio.h"
#if (TLK_TEST_RDT_ENABLE)
#include "../tlkmmi_rdtStd.h"
#if (TLKMMI_RDT_CASE_T001_ENABLE)
#include "tlkmmi_rdt_t001.h"
#include "tlkmmi_rdt_t001Bt.h"



static tlkmmi_rdt_t001Bt_t sTlkMmiRdtT001Bt = {0};


void tlkmmi_rdt_t001BtStart(void)
{
	sTlkMmiRdtT001Bt.busys = RDT_CASE_T001_BTBUSY_NONE;
}
void tlkmmi_rdt_t001BtClose(void)
{
}



void tlkmmi_rdt_t001BtInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	
}






#endif //#if (TLKMMI_RDT_CASE_T001_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


/********************************************************************************************************
 * @file	tlkmmi_rdt_t001Le.c
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
#include "tlkmmi_rdt_t001Le.h"
#include "../le/tlkmmi_rdt_le.h"



static void tlkmmi_rdt_t001LeConnectCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t001LeEncryptCB(uint16 handle, uint08 status);
static void tlkmmi_rdt_t001LeDisconnCB(uint16 handle, uint08 reason);


void tlkmmi_rdt_t001LeStart(void)
{
	tlkmmi_rdt_leRegConnectCB(tlkmmi_rdt_t001LeConnectCB);
	tlkmmi_rdt_leRegEncryptCB(tlkmmi_rdt_t001LeEncryptCB);
	tlkmmi_rdt_leRegDisconnCB(tlkmmi_rdt_t001LeDisconnCB);
	
	tlkmmi_rdt_leStartAdv();
}
void tlkmmi_rdt_t001LeClose(void)
{
	tlkmmi_rdt_leCloseAdv();
	tlkmmi_rdt_leRegConnectCB(nullptr);
	tlkmmi_rdt_leRegEncryptCB(nullptr);
	tlkmmi_rdt_leRegDisconnCB(nullptr);
}

bool tlkmmi_rdt_t001LeTimer(void)
{
	return true;
}


static void tlkmmi_rdt_t001LeConnectCB(uint16 handle, uint08 status)
{
	
}
static void tlkmmi_rdt_t001LeEncryptCB(uint16 handle, uint08 status)
{
	tlkmmi_rdt_leDisconn(0, 0x08);
}
static void tlkmmi_rdt_t001LeDisconnCB(uint16 handle, uint08 reason)
{
	tlkmmi_rdt_leClsPeerInfo();
	tlkmmi_rdt_leStartAdv();
}


#endif //#if (TLKMMI_RDT_CASE_T001_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


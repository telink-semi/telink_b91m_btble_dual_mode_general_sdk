/********************************************************************************************************
 * @file	tlkmmi_emi.c
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
#include "../tlkmmi_testStdio.h"
#if (TLK_TEST_EMI_ENABLE)
#include "tlkmmi_emi.h"
#include "drivers.h"

static int tlkmmi_emi_init(uint08 procID, uint16 taskID);
static int tlkmmi_emi_start(void);
static int tlkmmi_emi_pause(void);
static int tlkmmi_emi_close(void);
static int tlkmmi_emi_input(uint08 msgID, uint08 *pData, uint16 dataLen);
static void tlkmmi_emi_handler(void);


const tlkmmi_testModinf_t gTlkMmiEmiModinf = 
{
	tlkmmi_emi_init, //.Init
	tlkmmi_emi_start, //.Start
	tlkmmi_emi_pause, //.Pause
	tlkmmi_emi_close, //.Close
	tlkmmi_emi_input, //.Input
	tlkmmi_emi_handler, //Handler
};



static int tlkmmi_emi_init(uint08 procID, uint16 taskID)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_start(void)
{
	emi_start();
	return TLK_ENONE;
}
static int tlkmmi_emi_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_close(void)
{
	return TLK_ENONE;
}
static int tlkmmi_emi_input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static void tlkmmi_emi_handler(void)
{
	emi_loop_server();
}

#endif //#if (TLK_TEST_EMI_ENABLE)

#endif //#if (TLKMMI_TEST_ENABLE)


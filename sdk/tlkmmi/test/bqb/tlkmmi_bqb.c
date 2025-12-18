/********************************************************************************************************
 * @file	tlkmmi_bqb.c
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
#if (TLK_TEST_BQB_ENABLE)
#include "tlkmmi_bqb.h"
#include "drivers.h"
#include "tlkstk/tlkstk_stdio.h"
#include "tlkstk/btble/btble.h"


extern void tlkbt_regPlicIrqClaim(plic_interrupt_claim_callback_t cb);
extern void tlkbt_setWorkMode(u8 workMode);
extern void btc_enterTestMode(void);
extern int  tlkusb_init(uint16 usbID);
extern void tlkdbg_init(void);
extern void tlkusb_handler(void);

static int tlkmmi_bqb_init(uint08 procID, uint16 taskID);
static int tlkmmi_bqb_start(void);
static int tlkmmi_bqb_pause(void);
static int tlkmmi_bqb_close(void);
static int tlkmmi_bqb_input(uint32 msgID, uint08 *pData, uint16 dataLen);
static void tlkmmi_bqb_handler(void);


const tlkmmi_testModinf_t gTlkMmiBqbModinf =
{
	tlkmmi_bqb_init, //.Init
	tlkmmi_bqb_start, //.Start
	tlkmmi_bqb_pause, //.Pause
	tlkmmi_bqb_close, //.Close
	tlkmmi_bqb_input, //.Input
	tlkmmi_bqb_handler, //Handler
};

static int tlkmmi_bqb_init(uint08 procID, uint16 taskID)
{
	return TLK_ENONE;
}

static int tlkmmi_bqb_start(void)
{
	g_plic_preempt_en = 1;
	tlkbt_setWorkMode(0);
	tlkbt_regPlicIrqClaim((plic_interrupt_claim_callback_t)plic_interrupt_claim);

	tlkdbg_init();
	tlkstk_init();
	tlkusb_init(0x120);
	btc_enterTestMode();


	return TLK_ENONE;
}
static int tlkmmi_bqb_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_bqb_close(void)
{
	return TLK_ENONE;
}
static int tlkmmi_bqb_input(uint32 msgID, uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static void tlkmmi_bqb_handler(void)
{
	#if (TLK_DBG_LOG_ENABLE)
	tlkdbg_handler();
	#endif
	#if (TLK_USB_UDB_ENABLE)
	tlkusb_handler();
	#endif
	tlkstk_handler();
}



#endif //#if (TLK_TEST_bqb_ENABLE)

#endif //#if (TLKMMI_TEST_ENABLE)


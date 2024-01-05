/********************************************************************************************************
 * @file	tlkstk.c
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
#include "tlkstk/bt/btc/btc_stdio.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk.h"
#include "tlkstk/hci/bt_hci.h"
#include "tlkstk/ble/ble.h"
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys/tlksys_pm.h"
#endif

extern int      btble_init(void);
extern int      btc_task_pending(void);
extern void     btble_sdk_main_loop(void);
extern void     btc_refresh_rand_ecc_key(void);
extern bool     btc_inqPageBusy(void);
extern uint16_t btc_state(void);


void tlkstk_init(void)
{
	#if(TLK_STK_BT_ENABLE)
	tlkstk_mem_init();
	#endif

	btble_init();

	btc_init();

	blec_init();

#if TLK_STK_LEH_ENABLE
	bleh_init();
#endif
#if TLK_STK_BTH_ENABLE
	bth_init();
#endif
#if TLK_STK_BTP_ENABLE
	btp_init();
#endif
#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendBusyCheckCB(tlkstk_pmIsBusy, "tlkstk");
#endif

}

void tlkstk_handler(void)
{
	btc_refresh_rand_ecc_key();

	btble_sdk_main_loop();

#if TLK_STK_BTH_ENABLE
	bth_handler();
#endif
#if TLK_STK_BTP_ENABLE
	btp_handler();
#endif
}


bool tlkstk_pmIsBusy(void)
{
	return bth_pmIsBusy()||(!tlkbt_hci_c2hFifoIsEmpty())|| btc_inqPageBusy();
}


uint32_t tlkstk_state(void)
{
	if(btc_task_pending()) return 0xf5f5f5f5;

	uint32_t stk_state  =  blc_ll_checkBleTaskIsIdle()|(btc_state()<<16);

 	return stk_state;
}



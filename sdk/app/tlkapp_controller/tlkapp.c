/********************************************************************************************************
 * @file	tlkapp.c
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
#include "tlkstk/tlkstk_stdio.h"
#include "tlkapp_serial.h"
#include "tlkapp_irq.h"
#include "tlkapp.h"
#include "drivers.h"
#include "tlkstk/btble/btble.h"



extern void trng_init(void);
extern void flash_plic_preempt_config(unsigned char preempt_en,unsigned char threshold);

extern void tlkbt_setWorkMode(u8 workMode);
extern void tlkbt_setCtlSecConFeat(u8 support);
extern void tlkbt_regPlicIrqClaim(plic_interrupt_claim_callback_t cb);

extern int tlkdev_init(void);
#if (TLK_CFG_DBG_ENABLE)
extern void tlkdbg_init(void);
extern void tlk_debug_init(void);
#endif
#if (TLK_CFG_USB_ENABLE)
extern int tlkusb_init(uint16 usbID);
extern void tlkusb_handler(void);
#endif

/******************************************************************************
 * Function: tlkapp_init
 * Descript: user initialization when MCU power on or wake_up from deepSleep mode.
 * Params: None.
 * Return: TLK_ENONE is success.
 * Others: None.
*******************************************************************************/
int tlkapp_init(void)
{
  	g_plic_preempt_en = 1;
	flash_plic_preempt_config(1, 1);
	trng_init();
	tlkbt_setWorkMode(0);
	tlkbt_setCtlSecConFeat(1);
	tlkapp_irq_init();
	
	tlkbt_regPlicIrqClaim((plic_interrupt_claim_callback_t)plic_interrupt_claim());
#if (TLK_CFG_DBG_ENABLE)
	tlk_debug_init();
	tlkdbg_init();
#endif
#if (TLK_CFG_USB_ENABLE)
	tlkusb_init(0x120);
#endif
	tlkdev_init();
	tlkstk_init();

	tlkapp_serial_init();

	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkapp_handler
 * Descript: BTBLE SDK main loop.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_handler(void)
{
#if (TLK_CFG_DBG_ENABLE)
	tlkdbg_handler();
#endif
#if (TLK_CFG_USB_ENABLE)
	tlkusb_handler();
#endif
	tlkstk_handler();
	tlkapp_serial_handler();
}



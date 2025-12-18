/********************************************************************************************************
 * @file	tlkmain.c
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
#include "tlkapp_config.h"
#include "tlkapp.h"
#include "drivers.h"


#if (TLK_STK_BT_ENABLE)
extern void btc_ll_system_tick_isr(void);
extern void btc_core_isr(void);
#endif
#if (TLK_CFG_USB_ENABLE)
extern void tlkusb_irqHandler(void);
#endif
#if (TLK_STK_LE_ENABLE)
extern void ble_sdk_rf_irq_handler(void);
extern void ble_ll_system_tick_isr(void);
#endif


/******************************************************************************
 * Function: main
 * Descript: This is main function.
 * Params: None.
 * Return: 0 is success.
 * Others: None.
*******************************************************************************/
int main(void)
{
    blc_pm_select_internal_32k_crystal();

#if(MCU_CORE_TYPE == MCU_CORE_B91)
    sys_init(DCDC_1P4_LDO_1P8,VBAT_MAX_VALUE_GREATER_THAN_3V6);
	tlksys_switch_clock(SYS_CHIP_CLOCK_48M);
#endif
#if(MCU_CORE_TYPE == MCU_CORE_B92)
	sys_init(LDO_1P4_LDO_2P0, VBAT_MAX_VALUE_GREATER_THAN_3V6, GPIO_VOLTAGE_3V3);
	clock_cal_24m_rc();
	/* adjust cap value */
	analog_write_reg8(0x8a, 0x5b); //only used for current chip.
	wd_32k_stop();

	tlksys_switch_clock(SYS_CHIP_CLOCK_48M);

	//	CCLK_96M_HCLK_48M_PCLK_24M;
	clock_32k_init(CLK_32K_RC);
	clock_cal_32k_rc(); //6.68ms
#endif	
	
	ext_gpio_init(1);
    if(tlkapp_init() != TLK_ENONE){
		while(1){}
	}

	#if (TLK_CFG_WDG_ENABLE)
	wd_set_interval_ms(3000);
	wd_start();
	#endif
	
	tlksys_interrupt_enable();

	while(1)
	{
		#if (TLK_CFG_WDG_ENABLE)
		wd_clear();
		#endif
		tlkapp_handler();
	}
    return 0;
} 

/******************************************************************************
 * Function: stimer_irq_handler
 * Descript: System timer interrupt handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
_attribute_retention_code_ 
void stimer_irq_handler(void)
{
	#if (TLK_STK_BT_ENABLE)
    btc_ll_system_tick_isr();
	#endif
	#if (TLK_STK_LE_ENABLE)
    ble_ll_system_tick_isr();
	#endif
//	systimer_clr_irq_status();
}

/******************************************************************************
 * Function: rf_irq_handler
 * Descript: Baseband interrupt handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
_attribute_retention_code_ 
void rf_irq_handler(void)
{
	#if (TLK_STK_LE_ENABLE)
	ble_sdk_rf_irq_handler();
	#endif
}

/******************************************************************************
 * Function: zb_bt_irq_handler
 * Descript: This function for Bt core handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
_attribute_retention_code_ 
void zb_bt_irq_handler(void)
{
	#if (TLK_STK_BT_ENABLE)
    btc_core_isr();
	#endif
}

/******************************************************************************
 * Function: usb_endpoint_irq_handler
 * Descript: This function for audio interrupt handler 
 *           and USB MSC interrupt handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/ 
_attribute_retention_code_ 
void usb_endpoint_irq_handler(void)
{
	#if (TLK_CFG_USB_ENABLE)
	tlkusb_irqHandler();
	#endif
}

void timer0_irq_handler(void)
{
	tlkapi_timer_handler();
}

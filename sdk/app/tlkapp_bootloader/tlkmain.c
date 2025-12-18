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
#include "tlkapp_dfu.h"
#include "drivers.h"
#include "tlkapp_irq.h"
#if (TLKAPP_DFU1_ENABLE)
#include "tlkapp_dfu1.h"
#include "tlkdev/tlkdev.h"
#endif

#if (TLKAPP_DFU1_ENABLE)
extern int tlkcfg_load(void);
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
#if(MCU_CORE_TYPE == MCU_CORE_B91)
	blc_pm_select_internal_32k_crystal();
    sys_init(DCDC_1P4_LDO_1P8,VBAT_MAX_VALUE_GREATER_THAN_3V6);
#endif
#if(MCU_CORE_TYPE == MCU_CORE_B92)
	sys_init(LDO_1P4_LDO_2P0,VBAT_MAX_VALUE_GREATER_THAN_3V6,GPIO_VOLTAGE_3V3);
	wd_32k_stop();
#endif	
    CCLK_96M_HCLK_48M_PCLK_24M;

#if (TLKAPP_DFU1_ENABLE)
#if(MCU_CORE_TYPE == MCU_CORE_B91)
    core_disable_interrupt();
#elif(MCU_CORE_TYPE == MCU_CORE_B92)
    core_interrupt_disable();
#endif
    ext_gpio_init(1);
    tlkapp_irq_init();

    tlkdev_init();
    tlkcfg_load();
	tlkapp_dfu1_init();
	tlkapp_duf1_open();

	tlksys_interrupt_enable();
	while(tlkapp_duf1_isBusy()){
		tlkapp_dfu1_handler();
	}
	tlkapp_duf1_close();

#if(MCU_CORE_TYPE == MCU_CORE_B91)
    core_disable_interrupt();
#elif(MCU_CORE_TYPE == MCU_CORE_B92)
    core_interrupt_disable();
#endif

#endif
	#if 1
	tlkapp_dfu_load();
	TLKAPP_JUMP_TO_APP();
	while(1){ core_reboot(); }
	#else
    tlkapp_init();
	while(1)
	{
		tlkapp_handler();
	}
	#endif
    return 0;
} 


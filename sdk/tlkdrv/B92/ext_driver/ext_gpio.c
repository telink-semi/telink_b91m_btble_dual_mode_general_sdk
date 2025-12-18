/********************************************************************************************************
 * @file     ext_gpio.c
 *
 * @brief    This is the source file for BTBLE SDK
 *
 * @author	 BTBLE GROUP
 * @date         2,2022
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#include "ext_driver/ext_gpio.h"

#if 1
/**
 * @brief     This function set a pin's pull-up/down resistor.
 * @param[in] gpio - the pin needs to set its pull-up/down resistor
 * @param[in] up_down - the type of the pull-up/down resistor
 * @return    none
 */
void gpio_setup_up_down_resistor(gpio_pin_e gpio, gpio_pull_type up_down)
{
	unsigned char r_val = up_down & 0x03;

	unsigned char base_ana_reg = 0x0e + ((gpio >> 8) << 1) + ( (gpio & 0xf0) ? 1 : 0 );  //group = gpio>>8;
	unsigned char shift_num, mask_not;

	if(gpio & 0x11){
		shift_num = 0;
		mask_not = 0xfc;
	}
	else if(gpio & 0x22){
		shift_num = 2;
		mask_not = 0xf3;
	}
	else if(gpio & 0x44){
		shift_num = 4;
		mask_not = 0xcf;
	}
	else if(gpio & 0x88){
		shift_num = 6;
		mask_not = 0x3f;
	}
	else{
		return;
	}

    if(GPIO_DP == gpio){
        //usb_dp_pullup_en (0);
    }

    analog_write_reg8(base_ana_reg, (analog_read_reg8(base_ana_reg) & mask_not) | (r_val << shift_num));
}

#endif


#define PC4_INPUT_ENABLE	0//1  //JTAG 4-WIRE FUNCTION
#define PC5_INPUT_ENABLE	0//1  //JTAG 4-WIRE FUNCTION
#define PC6_INPUT_ENABLE	0//1  //JTAG 4-WIRE FUNCTION
#define PC7_INPUT_ENABLE	0//1  //JTAG 4-WIRE FUNCTION

#define PC4_FUNC	AS_GPIO//AS_TDI //JTAG 4-WIRE FUNCTION
#define PC5_FUNC	AS_GPIO//AS_TDO //JTAG 4-WIRE FUNCTION
#define PC6_FUNC	AS_GPIO//AS_TMS //JTAG 4-WIRE FUNCTION
#define PC7_FUNC	AS_GPIO//AS_TCK //JTAG 4-WIRE FUNCTION

#define PULL_WAKEUP_SRC_PC4	0//GPIO_PIN_PULLUP_10K //JTAG 4-WIRE FUNCTION
#define PULL_WAKEUP_SRC_PC5	0//GPIO_PIN_PULLUP_10K //JTAG 4-WIRE FUNCTION
#define PULL_WAKEUP_SRC_PC6	0//GPIO_PIN_PULLUP_10K //JTAG 4-WIRE FUNCTION
#define PULL_WAKEUP_SRC_PC7	0//GPIO_PIN_PULLDOWN_100K //JTAG 4-WIRE FUNCTION


#include "tlk_config.h"
#include "gpio_default.h"

_attribute_ram_code_sec_
void ext_gpio_init(int anaRes_init_en)
{
	gpio_init(anaRes_init_en);
}




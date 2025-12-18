/********************************************************************************************************
 * @file	tlk_config.c
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
#include "tlk_dbgio.h"
#if (DEBUG_GPIO_ENABLE)
#include "drivers.h"
#endif


void tlk_dbgio_init(void)
{
#if (DEBUG_GPIO_ENABLE)
	#ifdef GPIO_CHN0
		gpio_function_en(GPIO_CHN0);
		gpio_input_en(GPIO_CHN0);
	#endif

	#ifdef GPIO_CHN0
		gpio_function_en(GPIO_CHN0);
		gpio_input_en(GPIO_CHN0);
	#endif

	#ifdef GPIO_CHN1
		gpio_function_en(GPIO_CHN1);
		gpio_input_en(GPIO_CHN1);
	#endif

	#ifdef GPIO_CHN2
		gpio_function_en(GPIO_CHN2);
		gpio_input_en(GPIO_CHN2);
	#endif

	#ifdef GPIO_CHN3
		gpio_function_en(GPIO_CHN3);
		gpio_input_en(GPIO_CHN3);
	#endif

	#ifdef GPIO_CHN4
		gpio_function_en(GPIO_CHN4);
		gpio_input_en(GPIO_CHN4);
	#endif

	#ifdef GPIO_CHN5
		gpio_function_en(GPIO_CHN5);
		gpio_input_en(GPIO_CHN5);
	#endif

	#ifdef GPIO_CHN6
		gpio_function_en(GPIO_CHN6);
		gpio_input_en(GPIO_CHN6);
	#endif

	#ifdef GPIO_CHN7
		gpio_function_en(GPIO_CHN7);
		gpio_input_en(GPIO_CHN7);
	#endif

	#ifdef GPIO_CHN8
		gpio_function_en(GPIO_CHN8);
		gpio_input_en(GPIO_CHN8);
	#endif

	#ifdef GPIO_CHN9
		gpio_function_en(GPIO_CHN9);
		gpio_input_en(GPIO_CHN9);
	#endif

	#ifdef GPIO_CHN10
		gpio_function_en(GPIO_CHN10);
		gpio_input_en(GPIO_CHN10);
	#endif

	#ifdef GPIO_CHN11
		gpio_function_en(GPIO_CHN11);
		gpio_input_en(GPIO_CHN11);
	#endif

	#ifdef GPIO_CHN12
		gpio_function_en(GPIO_CHN12);
		gpio_input_en(GPIO_CHN12);
	#endif

	#ifdef GPIO_CHN13
		gpio_function_en(GPIO_CHN13);
		gpio_input_en(GPIO_CHN13);
	#endif

	#ifdef GPIO_CHN14
		gpio_function_en(GPIO_CHN14);
		gpio_input_en(GPIO_CHN14);
	#endif

	#ifdef GPIO_CHN15
		gpio_function_en(GPIO_CHN15);
		gpio_input_en(GPIO_CHN15);
	#endif
#endif
}






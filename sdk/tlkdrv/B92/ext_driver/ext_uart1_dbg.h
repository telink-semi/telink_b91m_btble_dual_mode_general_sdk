/********************************************************************************************************
 * @file     ext_uart1_dbg.h
 *
 * @brief    This is the header file for BTBLE SDK
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

#pragma once

//for enable or disable the debug function easily, must contain the "user_config.h".
#include "app/app_config.h"

#include "types.h"
#ifndef UART_DEBUG_EN
#define UART_DEBUG_EN			0
#endif


#if (UART_DEBUG_EN)
	void uart1_low_power_init ();
	void uart1_low_power_print (u8 *p, int len);
	void uart1_low_power_print_data (u32 d, int len);
	void uart1_debug_str(u8 *p, int len);
	void uart1_debug_num(unsigned char c) ;
	void uart1_debug_hex(unsigned int w,int len) ;
	void uart1_debug_hex_array(const u8 *p,unsigned char * buffer, int len);
#else
	#define uart1_low_power_init()
	#define uart1_low_power_print(p, len)
	#define uart1_low_power_print_data(d, len)
	#define uart1_debug_str(p, len)
	#define uart1_debug_num(c)
	#define uart1_debug_hex(w, len)
	#define uart1_debug_hex_array(p, buffer, len)
#endif

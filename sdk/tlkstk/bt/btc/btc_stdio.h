/********************************************************************************************************
 * @file	btc_stdio.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef BTC_STDIO_H
#define BTC_STDIO_H



#include "tlkstk/bt/btc/btc_config.h"
#include "tlkstk/bt/btc/btc_define.h"
#include "core/types.h"


extern uint8_t btc_init(void);


extern void btc_ll_sniff_ret_clkrestore(void);
extern void btc_iscan_low_power_enable(uint8_t en);
extern void btc_pscan_low_power_enable(uint8_t en);


#endif //BTC_STDIO_H


/********************************************************************************************************
 * @file	btc_core.h
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
#ifndef BTC_CORE_H
#define BTC_CORE_H


typedef struct{
	uint8_t inq_en;
	uint8_t inq_mode;
	uint8_t inqscan_type;
	uint8_t inqscan_use_fec;

	uint8_t inq_lap[3];
	uint8_t max_link_num;

	uint16_t inqscan_inv;
	uint16_t inqscan_win;

	uint32_t page_to;
	uint32_t ext_page_to;

	uint16_t pagescan_inv;
	uint16_t pagescan_win;

	uint16_t sniff_inv_max;
	uint16_t sniff_inv_min;

	uint32_t classofdevice;

	uint8_t local_bdaddr[6];
	uint8_t pagescan_type;
	uint8_t pagescan_rep_mode;

	uint8_t local_name[32];
	uint8_t scan_en;
	uint8_t page_en;
}btc_stack_param_t;


void btc_core_init(void);


void btc_context_store(void);
void btc_context_restore(void);


void btc_modem_reg_restore(void);
void btc_modem_reg_store(void);

uint8_t btc_core_allocLtAddr(void);
void btc_core_freeLtAddr(uint8_t LtAddr);



#endif //BTC_CORE_H


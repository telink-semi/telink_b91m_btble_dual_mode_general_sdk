/********************************************************************************************************
 * @file     btc_dbg.h
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

#ifndef BTC_DBG_H
#define BTC_DBG_H


#define BT_DIAG_DEBUG_VCD          				 0
//LOOPBACK or not
#define BT_BASEBAND_SIGNAL_LOOPBACK_VCD			(1 && BT_DIAG_DEBUG_VCD)


#define DEBUG_PORT_TX_EN_PIN       GPIO_PB3
#define DEBUG_PORT_RX_EN_PIN       GPIO_PB4
#define DEBUG_PORT_RX_WIN_PIN      GPIO_PB5
#define DEBUG_PORT_RX_SYNC_PIN     GPIO_PB6


/**
 * @brief		to enable some BT base_band RF signal output to GPIO
 * @param[in]	none
 * @return      none
 */
void btc_bb_dbg_init(void);




#endif /* BTC_DBG_H */

/********************************************************************************************************
 * @file	leg_init.h
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
#ifndef LLMS_INIT_H_
#define LLMS_INIT_H_

#include "tlkstk/ble/ble_format.h"
#include "tlkstk/hci/hci_cmd.h"


/**
 * @brief      for user to initialize legacy initiating module
 * 			   notice that only one module can be selected between legacy initiating module and extended initiating module
 * @param	   none
 * @return     none
 */
void 		blc_ll_initLegacyInitiating_module(void);

/**
 * @brief      This function is used to create an ACL connection to a connectable advertiser.
 * @param[in]  scan_interval
 * @param[in]  scan_window
 * @param[in]  initiator_filter_policy
 * @param[in]  adr_type
 * @param[in]  *mac
 * @param[in]  own_adr_type
 * @param[in]  conn_min
 * @param[in]  conn_max
 * @param[in]  conn_latency
 * @param[in]  timeout
 * @param[in]  ce_min
 * @param[in]  ce_max
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_ll_createConnection( scan_inter_t scan_interval, scan_inter_t scan_window, init_fp_t initiator_filter_policy,
										u8 adr_type, 	  u8  *mac, 	   u8 			  own_adr_type,
									   u16 conn_min, 	  u16 conn_max,    u16 			  conn_latency, 				u16 timeout,
									   u16 ce_min, 		  u16 ce_max );


/**
 * @brief      This function is is used to cancel the HCI_LE_Create_Connection or HCI_LE_Extended_Create_Connection commands.
 * @param	   none
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t 	blc_ll_createConnectionCancel (void);







ble_sts_t 	blc_ll_setCreateConnectionTimeout (u32 timeout_ms);

void blc_ll_disableDefaultInitTimeout(void);




#endif /* LLMS_INIT_H_ */

/********************************************************************************************************
 * @file	btc_hci_cmd.h
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
#ifndef BTC_HCI_CMD_H
#define BTC_HCI_CMD_H


#define NUM_OF_ACL_PACKET    4

#define NUM_OF_SCO_PACKET    8

#define MAX_SCO_DATA_PACKET_LENGTH   0x40


typedef int(*BtcHciCmdFunc)(uint8_t *, uint16_t);

typedef uint8_t(*le_hci_cmd_callback_t)(uint16_t, uint8_t *, uint16_t len);
extern _attribute_data_retention_	le_hci_cmd_callback_t 	ble_hci_cmd_cb;
void btc_hci_regBleCmdCallback(le_hci_cmd_callback_t cb);




#endif /* BTC_HCI_CMD_H */


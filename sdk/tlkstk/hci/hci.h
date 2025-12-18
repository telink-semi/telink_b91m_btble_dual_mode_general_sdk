/********************************************************************************************************
 * @file	hci.h
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
#pragma  once

#define HCI_FLAG_SCO_BT_STD                         (1<<21)
#define HCI_FLAG_EVENT_BT_STD						(1<<25)
#define HCI_FLAG_ACL_BT_STD							(1<<27)
#define	HCI_FLAG_ISO_DATE_STD						(1<<29)


#define TLK_MODULE_EVENT_STATE_CHANGE				0x0730
#define TLK_MODULE_EVENT_DATA_RECEIVED				0x0731
#define TLK_MODULE_EVENT_DATA_SEND					0x0732
#define TLK_MODULE_EVENT_BUFF_AVAILABLE				0x0733


// Controller event handler
typedef int(*hci_event_handler_t) (uint32_t h, uint8_t *para, uint16_t n);
extern hci_event_handler_t		hci_event_handler_cb;



int blc_hci_send_event (uint32_t h, uint8_t *para, int n);


void blc_hci_registerControllerEventHandler (hci_event_handler_t  handler);

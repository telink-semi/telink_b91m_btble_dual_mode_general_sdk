/********************************************************************************************************
 * @file	ble.h
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
#ifndef BLE_H_
#define BLE_H_


#include "ble_config.h"
#include "ble_common.h"
#include "ble_format.h"

#include "controller/ble_controller.h"
#include "host/ble_host.h"


#include "service/ota/ota.h"
#include "service/ota/ota_server.h"
#include "service/device_information.h"
#include "service/hids.h"
#include "service/uuid.h"

typedef unsigned int(*ble_getFlashAddr)(unsigned int offset);
void ble_reg_getFlashAddrCB(ble_getFlashAddr ptr);


/***********Customized configuration, Begin **************/
#define  BLE_DELAY_REPORT_DISCONNECT_EV		0   //
/***********Customized configuration, End  ***************/

#ifndef 	ACL_CENTRAL_MAX_NUM
#define 	ACL_CENTRAL_MAX_NUM							0 // ACL central maximum number
#endif

#ifndef 	ACL_PERIPHR_MAX_NUM
#define 	ACL_PERIPHR_MAX_NUM							1 // ACL peripheral maximum number
#endif



#define TLK_LE_PERIPHR_SMP_ENABLE         			1
#define TLK_LE_CENTRAL_SMP_ENABLE        			0



/********************* ACL connection LinkLayer TX & RX data FIFO allocation, Begin ************************************************/
/**
 * @brief	connMaxRxOctets
 * refer to BLE SPEC "4.5.10 Data PDU length management" & "2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RSP"
 * usage limitation:
 * 1. should be in range of 27 ~ 251
 * 2. for CIS peripheral, receive ll_cis_req(36Byte), must be equal to or greater than 36
 */
#define ACL_CONN_MAX_RX_OCTETS			27	//user set value


/**
 * @brief	connMaxTxOctets
 * refer to BLE SPEC: Vol 6, Part B, "4.5.10 Data PDU length management"
 * 					  Vol 6, Part B, "2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RSP"
 *  in this SDK, we separate this value into 2 parts: peripheralMaxTxOctets and centralMaxTxOctets,
 *  for purpose to save some SRAM costed by when peripheral and central use different connMaxTxOctets.
 *
 * usage limitation for ACL_xxx_MAX_TX_OCTETS
 * 1. should be in range of 27 ~ 251
 * 2. for CIS central, send ll_cis_req(36Byte), ACL_CENTRAL_MAX_TX_OCTETS must be equal to or greater than 36
 */
#define ACL_CENTRAL_MAX_TX_OCTETS		27	//user set value
#define ACL_PERIPHR_MAX_TX_OCTETS		27	//user set value



/**
 * @brief	ACL RX buffer size & number
 *  		ACL RX buffer is shared by all connections to hold LinkLayer RF RX data.
 * usage limitation for ACL_RX_FIFO_SIZE:
 * 1. must use CAL_LL_ACL_RX_FIFO_SIZE to calculate, user can not change !!!
 *
 * usage limitation for ACL_RX_FIFO_NUM:
 * 1. must be: 2^n, (power of 2)
 * 2. at least 4; recommended value: 4, 8, 16
 */
#define ACL_RX_FIFO_SIZE				CAL_LL_ACL_RX_FIFO_SIZE(ACL_CONN_MAX_RX_OCTETS)  //user can not change !!!
#define ACL_RX_FIFO_NUM					8	//user set value


/**
 * @brief	ACL TX buffer size & number
 *  		ACL Central TX buffer is shared by all central connections to hold LinkLayer RF TX data.
*			ACL Peripheral TX buffer is shared by all peripheral connections to hold LinkLayer RF TX data.
 * usage limitation for ACL_xxx_TX_FIFO_SIZE:
 * 1. must use CAL_LL_ACL_TX_FIFO_SIZE to calculate, user can not change !!!
 *
 * usage limitation for ACL_xxx_TX_FIFO_NUM:
 * 1. must be: (2^n) + 1, (power of 2, then add 1)
 * 2. for B91m IC: at least 9; recommended value: 9, 17, 33; other value not allowed.
 * 3. for B85m IC: at least 8; recommended value: 8, 16, 32; other value not allowed.
 *
 * only for B91: usage limitation for size * (number - 1)
 * 1. (ACL_xxx_TX_FIFO_SIZE * (ACL_xxx_TX_FIFO_NUM - 1)) must be less than 4096 (4K)
 *    so when ACL TX FIFO size equal to or bigger than 256, ACL TX FIFO number can only be 9(can not use 17 or 33), cause 256*(17-1)=4096
 */
#define ACL_CENTRAL_TX_FIFO_SIZE		CAL_LL_ACL_TX_FIFO_SIZE(ACL_CENTRAL_MAX_TX_OCTETS) //user can not change !!!
#define ACL_CENTRAL_TX_FIFO_NUM			9	//user set value

#define ACL_PERIPHR_TX_FIFO_SIZE		CAL_LL_ACL_TX_FIFO_SIZE(ACL_PERIPHR_MAX_TX_OCTETS) //user can not change !!!
#define ACL_PERIPHR_TX_FIFO_NUM			9   //user set value




extern	u8	app_acl_rx_fifo[];
extern	u8	app_acl_cen_tx_fifo[];
extern	u8	app_acl_per_tx_fifo[];
/******************** ACL connection LinkLayer TX & RX data FIFO allocation, End ***************************************************/





/***************** ACL connection L2CAP RX & TX data Buffer allocation, Begin **************************************/
/**
 * @brief	RX MTU size & L2CAP buffer size
 * RX MTU:
 * refer to BLE SPEC: Vol 3, Part F, "3.2.8 Exchanging MTU size" & "3.4.2 MTU exchange"; Vol 3, Part G, "4.3.1 Exchange MTU"
 * this SDK set ACL Central and Peripheral RX MTU buffer separately to save some SRAM when Central and Peripheral can use different RX MTU.
 *
 * CENTRAL_ATT_RX_MTU & PERIPHR_ATT_RX_MTU
 * 1. must equal to or bigger than 23
 * 2. if support LE Secure Connections, must equal to or bigger than 64
 *
 * CENTRAL_L2CAP_BUFF_SIZE & PERIPHR_L2CAP_BUFF_SIZE
 * 1. must use CAL_L2CAP_BUFF_SIZE to calculate, user can not change !!!
 */
#define CENTRAL_ATT_RX_MTU		  		23	//user set value
#define PERIPHR_ATT_RX_MTU   			23	//user set value


#define	CENTRAL_L2CAP_BUFF_SIZE			CAL_L2CAP_BUFF_SIZE(CENTRAL_ATT_RX_MTU)	//user can not change !!!
#define	PERIPHR_L2CAP_BUFF_SIZE			CAL_L2CAP_BUFF_SIZE(PERIPHR_ATT_RX_MTU)	//user can not change !!!


extern	u8 app_cen_l2cap_rx_buf[]; //ACL Central L2cap RX data buffer

extern	u8 app_per_l2cap_rx_buf[]; //ACL Peripheral L2cap RX data buffer
extern	u8 app_per_l2cap_tx_buf[]; //ACL Peripheral L2cap TX data buffer
/***************** ACL connection L2CAP RX & TX data Buffer allocation, End ****************************************/

typedef enum{
  BLE_EVT_HCI_CMD_ID = 0,
  BLE_EVT_GAP_ID,
  BLE_EVT_GATT_ID,
  BLE_EVT_l2CAP_ID,
  BLE_EVT_MAX,
}ble_evt_id_e;


/*Common BLE Event type*/
typedef struct
{
	ble_evt_id_e header;           /**< Event header. */
  union
  {
	hci_event_handler_t  hci_cmd_evt;
    gap_event_handler_t  gap_evt;
    hci_data_handler_t   l2cap_evt;
    gatt_handler_t    	 gatt_evt;
  } evt;
} ble_evt_t;
/**
 * @brief      for user to initialize ble controller
 * @param	   none
 * @return     none
 */
int blec_init(void);
/**
 * @brief      for user to initialize ble host
 * @param	   none
 * @return     none
 */
int bleh_init(void);

/**@brief  Application's BLE stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
void bleh_event_regCB(ble_evt_t * p_ble_evt);



#endif /* BLE_H_ */

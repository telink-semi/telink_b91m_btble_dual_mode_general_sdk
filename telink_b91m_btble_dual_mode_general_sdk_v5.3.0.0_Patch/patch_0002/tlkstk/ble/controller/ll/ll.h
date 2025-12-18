/********************************************************************************************************
 * @file	ll.h
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
#ifndef LL_H_
#define LL_H_


typedef enum{
	LL_FEATURE_ENABLE	= 1,
	LL_FEATURE_DISABLE  = 0,
}ll_feature_value_t;



/**
 * @brief	irq_handler for BLE stack, process RF interrupt
 * @param	none
 * @return	none
 */
void 		ble_sdk_rf_irq_handler(void);

/**
 * @brief   main_loop for BLE stack, process data and event
 * @param	none
 * @return	none
 */
void 		ble_sdk_main_loop (void);




void 		ble_ll_system_tick_isr(void);




/**
 * @brief      for user to initialize MCU
 * @param	   none
 * @return     none
 */
void 		blc_ll_initBasicMCU (void);



/**
 * @brief      for user to initialize link layer Standby state
 * @param	   none
 * @return     none
 */
void 		blc_ll_initStandby_module (u8 *public_adr);


/**
 * @brief      this function is used to read MAC address
 * @param[in]  *addr -  The address where the read value(MAC address) prepare to write.
 * @return     status, 0x00:  succeed
 * 					   other: failed
 */
ble_sts_t   blc_ll_writeBDAddr(u8 *addr);
ble_sts_t   blc_ll_readBDAddr(u8 *addr);


/**
 * @brief      this function is used to set the LE Random Device Address in the Controller
 * @param[in]  *randomAddr -  Random Device Address
 * @return     status, 0x00:  succeed
 * 					   other: failed
 */
ble_sts_t 	blc_ll_setRandomAddr(u8 *randomAddr);
























ble_sts_t 	blc_ll_isAclhdlInvalid (u16 connHandle);

u8 			blc_llms_isAllRolesNotConnected(void);

ble_sts_t 	blc_ll_encryptedData(u8*key, u8*plaintextData, u8* encryptedTextData);

ble_sts_t   blc_ll_getRandomNumber (u8* randomNumber);

ble_sts_t	blc_hci_le_setHostFeature(u8 bit_number, ll_feature_value_t bit_value);

ble_sts_t	blc_controller_check_appBufferInitialization(void);




/**
 * @brief      this function is used by the Host to specify a channel classification based on its local information,
 *             only the master role is valid.
 * @param[in]  *map - channel map
 * @return     status, 0x00:  succeed
 * 			           other: failed
 */
ble_sts_t 	blc_ll_setHostChannel(u8 * chnMap);


/**
 * @brief      this function checks whether the Bluetooth stack task is IDLE
 * @param	   none
 * @return     status, 0:  idle
 *                     1:  task
 */
u32 		blc_ll_checkBleTaskIsIdle(void);


/**
 *  @brief  Event Parameters for "CONTR_EVT_LE_LEGACY_ADV_TX_EACH_CHANNEL"
 */
typedef struct{
    u8 *    pAdvData;             //point to first ADV data
    u32     advPacketHeader_tick; //ADV packet header Stimer tick
} tlk_contr_evt_advTxEachChn_t;


/**
 * @brief      This function is used to enable or disable RX packet header Stimer tick in ADV report event
 *             attention: it's not standard BLE function, just for some customer's special requirement
 * @param[in]  randomAddr -  Random Device Address
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
void        blc_ll_advReport_setRxPacketTickEnable(int en);


/**
 *  @brief  Event Parameters for TLK defined special ADV report event
            if user enable RX packet header Stimer tick, should this data structure to analyze ADV report event
 */
typedef struct{
    u8      subcode;
    u8      nreport;
    u8      event_type;
    u8      adr_type;
    u8      mac[6];
    u32     rxPktHeader_tick;   //RX packet header Stimer tick
    u8      len;
    u8      data[1];
}hci_tlk_advReportWithRxTickEvt_t;


#endif /* LL_H_ */

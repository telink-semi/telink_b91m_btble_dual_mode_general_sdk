/********************************************************************************************************
 * @file	tlkmdi_le_custom_pair.h
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
#ifndef BLM_PAIR_H_
#define BLM_PAIR_H_

#include "types.h"
#include "app/app_config.h"


/* define pair slave max num,
   if exceed this max num, two methods to process new slave pairing
   method 1: overwrite the oldest one(telink demo use this method)
   method 2: not allow pairing unless unfair happened  */
#define	USER_PAIR_SLAVE_MAX_NUM       4  //telink demo use max 4, you can change this value


typedef struct {
	u8 bond_mark;
	u8 adr_type;
	u8 address[6];
} macAddr_t;


typedef struct {
	u32 bond_flash_idx[USER_PAIR_SLAVE_MAX_NUM];  //mark paired slave mac address in flash
	macAddr_t bond_device[USER_PAIR_SLAVE_MAX_NUM];  //macAddr_t already defined in ble stack
	u8 curNum;
} user_salveMac_t;
extern user_salveMac_t user_tbl_slaveMac;



/*!  Pair parameter manager type */
typedef struct{
	u8 manual_pair;
	u8 mac_type;  //address type
	u8 mac[6];
	u32 pair_tick;
}man_pair_t;

extern man_pair_t blm_manPair;

/**
 * @brief   Pair management initialization for master.
 * @param   none.
 * @return  none.
 */
void user_master_host_pairing_management_init(void);

/**
 * @brief     search mac address in the bond slave mac table:
 *            when slave paired with dongle, add this addr to table
 *            re_poweron slave, dongle will search if this AdvA in slave adv pkt is in this table
 *            if in, it will connect slave directly
 *             this function must in ramcode
 * @param[in]  adr_type   address type
 * @param[in]  adr        Pointer point to address buffer.
 * @return     0:      invalid index
 *             others valid index
 */
int user_tbl_slave_mac_search(u8 adr_type, u8 * adr);

/**
 * @brief     Store bonding info to flash.
 * @param[in] adr_type   address type
 * @param[in] adr        Pointer point to address buffer.
 * @return    none.
 */
int user_tbl_slave_mac_add(u8 adr_type, u8 *adr);


/**
 * @brief      Delete bonding info.
 * @param[in]  adr_type   address type
 * @param[in]  adr        Pointer point to address buffer.
 * @return     1: delete ok
 *             0: no find
 */
int user_tbl_slave_mac_delete_by_adr(u8 adr_type, u8 *adr);

/**
 * @brief      Delete all device bonding info.
 * @param      none.
 * @return     none.
 */
void user_tbl_slave_mac_delete_all(void);





#endif /* APP_PAIR_H_ */

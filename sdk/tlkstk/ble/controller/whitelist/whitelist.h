/********************************************************************************************************
 * @file	whitelist.h
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
#ifndef LL_WHITELIST_H_
#define LL_WHITELIST_H_






#define			MAX_WHITE_LIST_SIZE								4
#define 		MAX_WHITE_IRK_LIST_SIZE          				2





#if ((MCU_CORE_TYPE == MCU_CORE_B91) || (MCU_CORE_TYPE == MCU_CORE_B92))
	#define 	IRK_REVERT_TO_SAVE_AES_TMIE_ENABLE				0
#else
	#define 	IRK_REVERT_TO_SAVE_AES_TMIE_ENABLE				1
#endif



//adv filter policy
#define 		ALLOW_SCAN_WL									BIT(0)
#define 		ALLOW_CONN_WL									BIT(1)



typedef u8 irk_key_t[16];

typedef struct {
	u8 type;
	u8 address[BLE_ADDR_LEN];
	u8 reserved;
} wl_addr_t;

typedef struct {
	wl_addr_t  wl_addr_tbl[MAX_WHITE_LIST_SIZE];
	u8 	wl_addr_tbl_index;
	u8 	wl_irk_tbl_index;
} ll_whiteListTbl_t;


typedef struct {
	u8 type;
	u8 address[BLE_ADDR_LEN];
	u8 reserved;
	u8 irk[16];
} rl_addr_t;

typedef struct {
	rl_addr_t	tbl[MAX_WHITE_IRK_LIST_SIZE];
	u8 			idx;
	u8			en;
} ll_ResolvingListTbl_t;



extern	ll_whiteListTbl_t		ll_whiteList_tbl;
extern	ll_ResolvingListTbl_t	ll_resolvingList_tbl;



ble_sts_t  ll_resolvingList_getSize(u8 *Size);
ble_sts_t  ll_resolvingList_getPeerResolvableAddr (u8 peerIdAddrType, u8* peerIdAddr, u8* peerResolvableAddr); //not available now
ble_sts_t  ll_resolvingList_getLocalResolvableAddr(u8 peerIdAddrType, u8* peerIdAddr, u8* LocalResolvableAddr); //not available now
ble_sts_t  ll_resolvingList_setResolvablePrivateAddrTimer (u16 timeout_s);   //not available now






u8 * ll_searchAddrInWhiteListTbl(u8 type, u8 *addr);
u8 * ll_searchAddrInResolvingListTbl(u8 *addr);  //addr must be RPA
u8 * ll_searchAddr_in_WhiteList_and_ResolvingList(u8 type, u8 *addr);
bool smp_quickResolvPrivateAddr(u8 *key, u8 *addr);








/**
 * @brief      reset whitelist
 * @param[in]  none
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t ll_whiteList_reset(void);


/**
 * @brief      add a device form whitelist
 * @param[in]  type - device mac address type
 * @param[in]  addr - device mac address
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t ll_whiteList_add(u8 type, u8 *addr);


/**
 * @brief      delete a device from whitelist
 * @param[in]  type - device mac address type
 * @param[in]  addr - device mac address
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t ll_whiteList_delete(u8 type, u8 *addr);




/**
 * @brief      get whitelist size
 * @param[out] pointer to size
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t ll_whiteList_getSize(u8 *returnPublicAddrListSize) ;




/**
 * @brief      reset resolvinglist
 * @param[in]  none
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t  ll_resolvingList_reset(void);


/**
 * @brief      add a device to resolvinglist
 * @param[in]  peerIdAddrType - device mac address type
 * @param[in]  peerIdAddr - device mac address
 * @param[in]  peer_irk - peer IRK pointer
 * @param[in]  local_irk - local IRK pointer
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t  ll_resolvingList_add(u8 peerIdAddrType, u8 *peerIdAddr, u8 *peer_irk, u8 *local_irk);


/**
 * @brief      delete a device from resolvinglist
 * @param[in]  peerIdAddrType - device mac address type
 * @param[in]  peerIdAddr - device mac address
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t  ll_resolvingList_delete(u8 peerIdAddrType, u8 *peerIdAddr);


/**
 * @brief      enable resolvinglist resolution
 * @param[in]  resolutionEn - 1: enable; 0:disable
 * @return     Status - 0x00: command succeeded; 0x01-0xFF: command failed
 */
ble_sts_t  ll_resolvingList_setAddrResolutionEnable (u8 resolutionEn);




#endif /* LL_WHITELIST_H_ */

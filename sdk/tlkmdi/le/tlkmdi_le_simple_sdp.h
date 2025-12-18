/********************************************************************************************************
 * @file	tlkmdi_le_simple_sdp.h
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
#ifndef SIMPLE_SDP_H_
#define SIMPLE_SDP_H_

#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "app/app_config.h"
#include "tlkstk/inner/tlkstk_inner.h"

#ifndef BLE_MASTER_SIMPLE_SDP_ENABLE
#define BLE_MASTER_SIMPLE_SDP_ENABLE         		1
#endif



#if (BLE_MASTER_SIMPLE_SDP_ENABLE)


typedef void (*main_service_t) (void);
extern	main_service_t		main_service;

extern	int	master_sdp_pending;
extern	dev_char_info_t  cur_sdp_device;


#define		ATT_DB_UUID16_NUM		20
#define		ATT_DB_UUID128_NUM		8

typedef struct {
	u8	num;
	u8	property;
	u16	handle;
	u16	uuid;
	u16 ref;
} att_db_uuid16_t;			//8-byte


typedef struct {
	u8	num;
	u8	property;
	u16	handle;
	u8	uuid[16];
} att_db_uuid128_t;			//20-byte





#define ATT_BOND_MARK 		0x5A
#define ATT_ERASE_MARK		0x00


typedef struct{
	u8	flag;
	u8	adr_type;
	u8	addr[6];

	u8	rsvd[8];  //very important: 16 byte aligned, to avoid different flash page write for a sequence data

#if (PEER_SLAVE_USE_RPA_EN)
	u8  irk[16];   //TODO: if peer device mac_address is RPA(resolvable private address), IRK will be used
#endif

	u16	char_handle[CHAR_HANDLE_MAX];
}dev_att_t;





u16 blm_att_findHandleOfUuid16 (att_db_uuid16_t *p, u16 uuid, u16 ref);
u16 blm_att_findHandleOfUuid128 (att_db_uuid128_t *p, const u8 * uuid);



/**
 * @brief   SDP handler.
 *          !!! Note: This is a simple SDP processing implemented by telink.
 * @param   none.
 * @return  none.
 */
void 	app_service_discovery (void);

/**
 * @brief       This function is used to register SDP handler.
 * @param[in]   p       - Pointer point to SDP handler.
 * @return      none.
 */
void 	app_register_service (void *p);

/**
 * @brief       This function is used to process ATT packets related to SDP
 * @param[in]   connHandle  - connection handle
 * @param[in]   p           - Pointer point to ATT data buffer.
 * @return      no used
 */
int 	host_att_client_handler (u16 connHandle, u8 *p);

/**
 * @brief       This function is used to register ble stack mainloop function.
 * @param[in]   p           - Pointer point to ble stack mainloop function.
 * @return
 */
int 	host_att_register_idle_func (void *p);


/**
 * @brief     SDP loop
 * @param[in]  none.
 * @return     none.
 */
void 	simple_sdp_loop (void);




/**
 * @brief       Used for add peer device service ATThandle.
 * @param[in]   dev_char_info       - Pointer point to data buffer.
 * @return      0: success
 *              1: failed
 */
int 	dev_char_info_add_peer_att_handle (dev_char_info_t* dev_char_info);

/**
 * @brief       Use for store peer device att handle to flash.
 * @param[in]   dev_char_info    Pointer point to peer device ATT handle info.
 * @return      0: failed
 *             !0: return flash address
 */
int		dev_char_info_store_peer_att_handle(dev_char_info_t* dev_char_info);

/**
 * @brief       Get peer device att handle info by peer address
 * @param[in]   adr_type         address type
 * @param[in]   addr             Pointer point to peer address buffer
 * @param[out]  dev_att          Pointer point to dev_att_t
 * @return      0: failed
 *             !0: return flash address
 */
int		dev_char_info_search_peer_att_handle_by_peer_mac(u8 adr_type, u8* addr, dev_att_t * dev_att);


/**
 * @brief       Delete peer device att handle info by peer address
 * @param[in]   adr_type         address type
 * @param[in]   addr             Pointer point to peer address buffer
 * @return      0: success
 *              1: not find
 */
int		dev_char_info_delete_peer_att_handle_by_peer_mac(u8 addrType, u8 *addr);







#endif

#endif /* SIMPLE_SDP_H_ */

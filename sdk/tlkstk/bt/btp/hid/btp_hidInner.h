/********************************************************************************************************
 * @file	btp_hidInner.h
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
#ifndef BTP_HID_INNER_H
#define BTP_HID_INNER_H

#if (TLKBTP_CFG_HID_ENABLE)


#define BTP_HID_DBG_FLAG      ((TLK_MAJOR_DBGID_BTP << 24) | (TLK_MINOR_DBGID_BTP_HID << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_HID_DBG_SIGN      "[HID]"


#define BTP_HID_CONN_TIMEOUT2 (5000000 / BTP_TIMER_TIMEOUT)


/* HIDP header masks */
#define BTP_HID_HEADER_TRANS_MASK 0xf0
#define BTP_HID_HEADER_PARAM_MASK 0x0f

/* HIDP transaction types */
#define BTP_HID_TRANS_HANDSHAKE    0x00
#define BTP_HID_TRANS_HID_CONTROL  0x10
#define BTP_HID_TRANS_GET_REPORT   0x40
#define BTP_HID_TRANS_SET_REPORT   0x50
#define BTP_HID_TRANS_GET_PROTOCOL 0x60
#define BTP_HID_TRANS_SET_PROTOCOL 0x70
#define BTP_HID_TRANS_GET_IDLE     0x80
#define BTP_HID_TRANS_SET_IDLE     0x90
#define BTP_HID_TRANS_DATA         0xa0
#define BTP_HID_TRANS_DATC         0xb0

/* HIDP control operation parameters */
#define BTP_HID_CTRL_NOP                           0x00
#define BTP_HID_CTRL_HARD_RESET                    0x01
#define BTP_HID_CTRL_SOFT_RESET                    0x02
#define BTP_HID_CTRL_SUSPEND                       0x03
#define BTP_HID_CTRL_EXIT_SUSPEND                  0x04
#define BTP_HID_CTRL_VIRTUAL_CABLE_UNPLUG          0x05


#define BTP_HID_VIRTUAL_CABLE_UNPLUG               0
#define BTP_HID_BOOT_PROTOCOL_MODE                 1
#define BTP_HID_BLUETOOTH_VENDOR_ID                9
#define BTP_HID_WAITING_FOR_RETURN                 10
#define BTP_HID_WAITING_FOR_SEND_ACK               11


#define BTP_HID_REPORT_ID_UNDECLARED               0
#define BTP_HID_REPORT_ID_VALID                    1
#define BTP_HID_REPORT_ID_INVALID                  2

#define BTP_HID_CONTROL_PARAM_SUSPEND              0x03 // Go to reduced power mode.
#define BTP_HID_CONTROL_PARAM_EXIT_SUSPEND         0x04 // Exit reduced power mode.
#define BTP_HID_CONTROL_PARAM_VIRTUAL_CABLE_UNPLUG 0x05

#define BTP_HID_BOOT_MODE_KEYBOARD_ID              1
#define BTP_HID_BOOT_MODE_MOUSE_ID                 2


typedef struct {
    uint08 state;
    uint08 usrID;
    uint16 busys;
    uint16 timeout;
    uint16 ctrScid;
    uint16 irqScid;
    uint16 mtuSize;
    uint16 aclHandle;
    uint16 reportSize;

    uint08 active;
    uint08 errCode;
    uint08 reportID;
    uint08 reportType;

    uint08 rtnMode;
    uint08 fcsIsEn;
    uint08 qosIsEn;
    uint08 reserve01;
    uint16 ctrEhnCtrl;
    uint16 irqEhnCtrl;

    tlkapi_timer_t timer;
} btp_hid_item_t;

typedef struct {
    uint08 rtnMode;
    uint08 qosIsEn;
    uint16 reserve;
    btp_hid_item_t *item;
} btp_hidp_ctrl_t;


int btp_hid_innerInit(uint08 count);

int btp_hid_innerDeinit(void);

int btp_hid_innerGetMemLen(uint08 count);

int btp_hid_enableQos(bool enable);
int btp_hid_enableRtnMode(bool enable, uint08 rtnMode);

void btp_hid_destroy(uint16 aclHandle);


/******************************************************************************
 * Function: HID reset the control block.
 * Descript: Defines reset the special busy hid control block
 * Params:
 *       @pItem[IN]--The hid control block to reset.
 * Return: null.
 *******************************************************************************/
void btp_hid_resetNode(btp_hid_item_t *pItem);

/******************************************************************************
 * Function: HID check the hid Idle control block num interface.
 * Descript: Defines check how many hid idle control block exist.
 * Params:
 * Return: Returning The total number of hid control block.
 *******************************************************************************/
uint08 btp_hid_getIdleCount(void);
uint08 btp_hid_getUsedCount(void);
uint08 btp_hid_getConnCount(void);


/******************************************************************************
 * Function: HID Get the hid idle control block.
 * Descript: Defines get a new hid control block in resource.
 * Params:
 * Return: Returning The new hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hid_getIdleNode(void);

/******************************************************************************
 * Function: HID Get the hidp Busy control block.
 * Descript: Defines get the special busy hid control block
 *           in resource via cid.
 * Params:
 *       @aclHandle[IN]--The aclHandle to search the hid control block.
 * Return: Returning The busy hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hid_getUsedNode(uint16 aclHandle);

/******************************************************************************
 * Function: HID Get the HID Used control block.
 * Descript: Defines get the client or server Used hid control block
 *           in resource via aclhandle and usrid.
 * Params:
 *       @aclHandle[IN]--The aclHandle to search the hid control block.
 * Return: Returning The used hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hid_getAnyUsedNode(void);
btp_hid_item_t *btp_hid_getAnyConnNode(void);
btp_hid_item_t *btp_hid_getAnyUsedNodeByChnID(uint16 aclHandle, uint16 cid);
btp_hid_item_t *btp_hid_getUsedNodeByIntcid(uint16 aclHandle, uint16 scid);


/******************************************************************************
 * Function: HID Get the hidp Used control block.
 * Descript: Defines get the special Connected hid control block
 *           in resource via aclhandle and scid.
 * Params:
 *       @aclHandle[IN]--The aclHandle to  search the hid control block.
 *       @scid[IN]--The scid to search the hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hid_getUsedNodeByScid(uint16 aclHandle, uint16 scid);
btp_hid_item_t *btp_hid_getConnNodeByIntcid(uint16 aclHandle, uint16 scid);


/******************************************************************************
 * Function: HID Get the hidp Connected control block.
 * Descript: Defines get the special Connected hid control block
 *           in resource via cid.
 * Params:
 *       @aclHandle[IN]--The cid to search the hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hid_getConnNode(uint16 aclHandle);
btp_hid_item_t *btp_hid_getConnNodeByScid(uint16 aclHandle, uint16 scid);

/******************************************************************************
 * Function: HID Get the hidp Connected control block.
 * Descript: Defines get the special Connected hid control block
 *           in resource via cid.
 * Params:
 *       @cid[IN]--The cid to search the hid control block.
 * Return: Returning The connected hid control block.
 *******************************************************************************/
btp_hid_item_t *btp_hid_getConnNodeByChnID(uint16 cid);


#endif // TLKBTP_CFG_HID_ENABLE

#endif // BTP_HID_INNER_H

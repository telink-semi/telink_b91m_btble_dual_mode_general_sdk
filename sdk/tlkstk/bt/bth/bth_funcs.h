/********************************************************************************************************
 * @file	bth_funcs.h
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
#ifndef BTH_FUNC_SET_H
#define BTH_FUNC_SET_H

#if (TLK_STK_BTH_ENABLE)

#ifndef BTH_PTS_TEST_L2CAP
#define BTH_PTS_TEST_L2CAP 0
#endif // BTH_PTS_TEST_L2CAP


enum BTH_FUNCID_SET_ENUM {
    BTH_FUNCID_NONE = 0x0000,
    // Comm
    BTH_FUNCID_COMM_START         = 0x0000,
    BTH_FUNCID_COMM_START_SCAN    = 0x01 + BTH_FUNCID_COMM_START,
    BTH_FUNCID_COMM_CLOSE_SCAN    = 0x02 + BTH_FUNCID_COMM_START,
    BTH_FUNCID_COMM_CLS_PEER_INFO = 0x03 + BTH_FUNCID_COMM_START,
    // ACL Start
    BTH_FUNCID_ACL_START           = 0x0100,
    BTH_FUNCID_ACL_CONNECT         = 0x01 + BTH_FUNCID_ACL_START,
    BTH_FUNCID_ACL_DISCONN         = 0x02 + BTH_FUNCID_ACL_START,
    BTH_FUNCID_ACL_DISCONN_BY_ADDR = 0x03 + BTH_FUNCID_ACL_START,
    BTH_FUNCID_ACL_CANCEL_CONNECT  = 0x04 + BTH_FUNCID_ACL_START,
    // SCO Start
    BTH_FUNCID_SCO_START           = 0x0200,
    BTH_FUNCID_SCO_CONNECT         = 0x01 + BTH_FUNCID_SCO_START,
    BTH_FUNCID_SCO_DISCONN         = 0x02 + BTH_FUNCID_SCO_START,
    BTH_FUNCID_SCO_DISCONN_BY_ADDR = 0x03 + BTH_FUNCID_SCO_START,
    // Dev Start
    BTH_FUNCID_DEV_START = 0x0300,
    // HCI-CMD Start
    BTH_FUNCID_CMD_START = 0x0400,
    // HCI-EVT Start
    BTH_FUNCID_EVT_START = 0x0500,
    // L2cap Start
    BTH_FUNCID_L2C_START                = 0x0600,
    BTH_FUNCID_L2C_CONNECT              = 0x01 + BTH_FUNCID_L2C_START,
    BTH_FUNCID_L2C_DISCONN              = 0x02 + BTH_FUNCID_L2C_START,
    BTH_FUNCID_L2C_SEND_DATA            = 0x03 + BTH_FUNCID_L2C_START,
    BTH_FUNCID_L2C_SEND_INFO_REQ        = 0x04 + BTH_FUNCID_L2C_START,
    BTH_FUNCID_L2C_SET_EXT_FEATURE      = 0x05 + BTH_FUNCID_L2C_START,
    BTH_FUNCID_L2C_SET_EXT_FEATURE_BITS = 0x06 + BTH_FUNCID_L2C_START,
    // Signal Start
    BTH_FUNCID_SIG_START = 0x0700,
};


typedef struct {
    uint16 funcID; // Refer to BTH_FUNC_ID_ENUM.
    int (*Enter)(uint08 *pData, uint16 dataLen);
} bth_func_item_t;


int bth_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
void bth_func_setAclHandle(uint16 aclHandle);
void bth_func_setScoHandle(uint16 scoHandle);
void bth_func_setPeerAddr(uint08 peerAddr[6]);


static int bth_func_commStartScan(uint08 *pData, uint16 dataLen);
static int bth_func_commCloseScan(uint08 *pData, uint16 dataLen);
static int bth_func_commClsPeerInfo(uint08 *pData, uint16 dataLen);

static int bth_func_aclConnect(uint08 *pData, uint16 dataLen);
static int bth_func_aclDisconn(uint08 *pData, uint16 dataLen);
static int bth_func_aclConnectCancel(uint08 *pData, uint16 dataLen);
static int bth_func_aclDisconnByAddr(uint08 *pData, uint16 dataLen);

static int bth_func_scoConnect(uint08 *pData, uint16 dataLen);
static int bth_func_scoDisconn(uint08 *pData, uint16 dataLen);
static int bth_func_scoDisconnByAddr(uint08 *pData, uint16 dataLen);

static int bth_func_l2capConnect(uint08 *pData, uint16 dataLen);
static int bth_func_l2capDisconn(uint08 *pData, uint16 dataLen);
static int bth_func_l2capSendData(uint08 *pData, uint16 dataLen);
static int bth_func_l2capSendInfoReq(uint08 *pData, uint16 dataLen);

static int bth_func_l2capSetExtFeature(uint08 *pData, uint16 dataLen);
static int bth_func_l2capSetExtFeatureBits(uint08 *pData, uint16 dataLen);


#endif // #if (TLK_STK_BTH_ENABLE)

#endif // BTH_FUNC_SET_H

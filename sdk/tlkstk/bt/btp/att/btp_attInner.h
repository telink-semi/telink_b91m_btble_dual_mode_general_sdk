/********************************************************************************************************
 * @file	btp_attInner.h
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
#ifndef BTP_ATT_INNER_H
#define BTP_ATT_INNER_H

#if (TLKBTP_CFG_ATT_ENABLE)


#define BTP_ATT_DBG_FLAG ((TLK_MAJOR_DBGID_BTP << 24) | (TLK_MINOR_DBGID_BTP_ATT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_ATT_DBG_SIGN nullptr


typedef struct {
    uint08 state;
    uint08 usrID;
    uint16 chnID;
    uint16 busys;
    uint16 flags;
    uint16 handle;
    uint16 mtuSize;
    uint08 timeout;
    uint08 reserve0;
    uint16 reserve1;
    union {
        struct {
            uint08 opcode;
            uint08 reason;
            uint08 resv000;
            uint08 uuidLen;
            uint16 roffset;
            uint16 resv001;
            uint16 mtuSize;
            uint16 ahandle;
            uint16 shandle; // Att Handle
            uint16 ehandle;
            uint08 attUUID[16];
        } server;
        struct {
            uint32 resever;
        } client;
    } param;
    tlkapi_timer_t timer;
} btp_att_node_t;


typedef struct {
    btp_att_node_t *item;
} btp_att_ctrl_t;


int btp_att_innerInit(uint08 count);

int btp_att_innerDeinit(void);

int btp_att_innerGetMemLen(uint08 count);

void btp_att_destroy(uint16 aclHandle);


void btp_att_resetNode(btp_att_node_t *pItem);

uint08 btp_att_getIdleCount(void);
uint08 btp_att_getUsedCount(void);
uint08 btp_att_getConnCount(void);

btp_att_node_t *btp_att_getIdleNode(void);
btp_att_node_t *btp_att_getBusyNode(uint16 aclHandle);
btp_att_node_t *btp_att_getUsedNode(uint16 aclHandle, uint08 usrID);
btp_att_node_t *btp_att_getConnNode(uint16 aclHandle, uint08 usrID);
btp_att_node_t *btp_att_getUsedNodeByChnID(uint16 aclHandle, uint16 chnID);
btp_att_node_t *btp_att_getAnyUsedNode(uint16 aclHandle);
btp_att_node_t *btp_att_getAnyConnNode(uint16 aclHandle);


#endif // #if (TLKBTP_CFG_ATT_ENABLE)

#endif /* BTP_ATT_INNER_H */

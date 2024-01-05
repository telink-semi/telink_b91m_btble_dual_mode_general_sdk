/********************************************************************************************************
 * @file	btp_ptsL2c.h
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
#ifndef BTP_PTS_L2CAP_H
#define BTP_PTS_L2CAP_H

#if (TLKBTP_CFG_PTSL2C_ENABLE)


#define BTP_PTSL2C_DBG_FLAG      ((TLK_MAJOR_DBGID_BTP << 24) | (TLK_MINOR_DBGID_BTP_PTS << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_PTSL2C_DBG_SIGN      nullptr

#define BTP_PTSL2C_BUFFER_LENGTH 1024

#define BTP_PTSL2C_WAIT_TIMEOUT  (1000 / BTP_TIMER_TIMEOUT_MS)


typedef enum {
    BTP_PTSL2C_BUSY_NONE     = 0x0000,
    BTP_PTSL2C_BUSY_WAIT_ACK = 0x0001,
} BTP_PTSL2C_BUSYS_ENUM;


typedef struct {
    uint08 state;
    uint08 busys;
    uint16 chnID;
    uint16 handle;
    uint16 monTime;
    uint16 monNumb;
    uint16 rtnTime;
    uint16 rtnNumb;
    uint08 rtnMaxTx;
    uint08 rtnTxNum;
    uint16 timeout;
    uint08 rtnMode;
    uint08 fscIsEn;
    uint16 ehnCtrl;
    uint16 resv01;
    tlkapi_timer_t timer;
} btp_ptsl2c_item_t;
typedef struct {
    uint08 enRtn;
    uint08 enFcs;
    uint08 enEfs;
    uint08 rtnMode;
    uint08 fcsType;
    uint08 resv01;
    uint16 resv02;
} btp_ptsl2c_user_t;
typedef struct {
    uint08 buffer[BTP_PTSL2C_BUFFER_LENGTH];
    btp_ptsl2c_item_t *item;
    btp_ptsl2c_user_t *user;
} btp_ptsl2c_ctrl_t;


/******************************************************************************
 * Function: btp_ptsl2c_init
 * Descript: This interface be used by user to initial the ptsl2c resource
 *           of client/server before create a connection between the entity.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_ptsl2c_init(uint08 count);

/******************************************************************************
 * Function: ptsl2c Deinit interface
 * Descript: This interface be used by User to deinitial ptsl2c resource.
 * Params: pPtsL2c: refer to btp_ptsl2c_item_t
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
int btp_ptsl2c_deinit(void);

/******************************************************************************
 * Function: btp_ptsl2c_getMemLen
 * Descript: ptsl2c get the size of the required number of nodes interface.
 * Params:
 * Return: Actual node memory size required(unit: byte).
 *******************************************************************************/
int btp_ptsl2c_getMemLen(uint08 count);

void btp_ptsl2c_enableRtn(bool isEnable, uint08 rtnMode);
void btp_ptsl2c_enableFcs(bool isEnable, uint08 fcsType);
void btp_ptsl2c_enableEfs(bool isEnable);


int btp_ptsl2c_connect(uint16 aclHandle);
int btp_ptsl2c_disconn(uint16 aclHandle);
void btp_ptsl2c_destroy(uint16 aclHandle);

int btp_ptsl2c_sendTestData(uint16 dataLen);


#endif // #if (TLKBTP_CFG_PTSL2C_ENABLE)

#endif /* BTP_PTS_L2CAP_H */

/********************************************************************************************************
 * @file	btp_attSrv.h
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
#ifndef BTP_ATT_SRV_H
#define BTP_ATT_SRV_H

#if (TLKBTP_CFG_ATTSRV_ENABLE)


#define BTP_ATTSRV_SERVICE_NUMB 8

#define BTP_ATTSRV_BUFFER_SIZE  256


typedef enum {
    BTP_ATTSRV_BUSY_NONE               = 0x00,
    BTP_ATTSRV_BUSY_SEND_ERROR_RSP     = 0x01,
    BTP_ATTSRV_BUSY_SEND_FIND_INFO     = 0x02,
    BTP_ATTSRV_BUSY_SEND_FIND_TYPE     = 0x04,
    BTP_ATTSRV_BUSY_SEND_READ_TYPE     = 0x08,
    BTP_ATTSRV_BUSY_SEND_READ_GROUP    = 0x10,
    BTP_ATTSRV_BUSY_SEND_WRITE_RSP     = 0x20,
    BTP_ATTSRV_BUSY_SEND_READ_RSP      = 0x40,
    BTP_ATTSRV_BUSY_SEND_READ_BLOB_RSP = 0x80,
} BTP_ATTSRV_BUSY_ENUM;


int btp_attsrv_init(void);

int btp_attsrv_connect(uint16 aclHandle);
int btp_attsrv_disconn(uint16 aclHandle);

void btp_attsrv_connectEvt(btp_att_node_t *pAtt);
void btp_attsrv_disconnEvt(btp_att_node_t *pAtt);

void btp_attsrv_recvHandler(btp_att_node_t *pAtt, uint08 *pData, uint16 dataLen);

int btp_attsrv_setTable(const btp_attItem_t *pTable, uint16 count);
uint btp_attsrv_getChnID(uint16 aclHandle);


#endif // #if (TLKBTP_CFG_ATTSRV_ENABLE)

#endif /* BTP_ATT_SRV_H */

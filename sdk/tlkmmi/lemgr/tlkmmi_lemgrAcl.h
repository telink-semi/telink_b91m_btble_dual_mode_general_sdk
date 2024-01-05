/********************************************************************************************************
 * @file	tlkmmi_lemgrAcl.h
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
#ifndef TLKMMI_LEMGR_ACL_H
#define TLKMMI_LEMGR_ACL_H

#if (TLKMMI_LEMGR_ENABLE)


#define TLKMMI_LEMGR_OTA_SERVER_ENABLE         0

/*
 * @brief	user use this macro to enable or disable
 * 			workaround solution caused by "(ACL_xxx_TX_FIFO_SIZE * (ACL_xxx_TX_FIFO_NUM - 1)) must be less than 4096 (4K)"
 *          CAL_LL_ACL_TX_FIFO_SIZE(230) = 240, CAL_LL_ACL_TX_FIFO_SIZE(231) = 256
 *          when ACL_xxx_MAX_TX_OCTETS > 230, ACL_xxx_TX_FIFO_SIZE >= 256, ACL_xxx_TX_FIFO_NUM can only be 9
 *          if you want TX FIFO 16 or 32, can enable this macro
 */
#define TLKMMI_LEMGR_WORKAROUND_TX_FIFO_4K_LIMITATION_EN          0




#define TLKMMI_LEMGR_TIMEOUT          200000 //100ms
#define TLKMMI_LEMGR_TIMEOUT_MS       200

#define TLKMMI_LEMGR_ADV_TIMEOUT_MIN       500 //Unit:ms
#define TLKMMI_LEMGR_ADV_TIMEOUT_DEF       60000
#define TLKMMI_LEMGR_ADV_TIMEOUT_MAX       180000



typedef struct{
	uint16 connHandle;
	uint08 connAddr[6];
	uint16 timeout;
	uint08 isStart;
	uint32 advTime;
	tlkapi_timer_t timer;
}tlkmmi_lemgr_acl_t;

int tlkmmi_lemgr_aclInit(void);

uint16 tlkmmi_lemgr_aclGetSlaveHandle(void);

int tlkmmi_lemgr_createconnect(uint08 *pData, uint08 dataLen);

int tlkmmi_lemgr_disconn(uint16 handle, uint08 *pPeerAddr);

int tlkmmi_lemgr_closeAdv(void);

int tlkmmi_lemgr_startAdv(uint32 timeout, uint08 advType);

int tlkmmi_lemgr_startScan(void);

int tlkmmi_lemgr_closeScan(void);
#endif //TLKMMI_LEMGR_ENABLE

#endif //TLKMMI_LEMGR_ACL_H


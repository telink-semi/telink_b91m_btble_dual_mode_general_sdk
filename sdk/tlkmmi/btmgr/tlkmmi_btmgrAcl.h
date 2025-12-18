/********************************************************************************************************
 * @file	tlkmmi_btmgrAcl.h
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
#ifndef TLKMMI_BTMGR_ACL_H
#define TLKMMI_BTMGR_ACL_H

#if (TLKMMI_BTMGR_BTACL_ENABLE)


#define TLKMMI_BTMGR_TIMEOUT          200000 //100ms
#define TLKMMI_BTMGR_TIMEOUT_MS       200
#define TLKMMI_BTMGR_WAIT_TIMEOUT     (5000000/TLKMMI_BTMGR_TIMEOUT)

#define TLKMMI_BTMGR_INQ_RSSI         0x9C //-100


typedef enum{
	TLKMMI_BTMGR_REASON_FAIL    = 0x80,
	TLKMMI_BTMGR_REASON_TIMEOUT = 0x81,
}TLKMMI_BTMGR_REASON_ENUM;

typedef enum{
	TLKMMI_BTMGR_BUSY_NONE = 0x00,
	TLKMMI_BTMGR_BUSY_WAIT_REC  = 0x01,
	TLKMMI_BTMGR_BUSY_WAIT_INQ  = 0x02,
	TLKMMI_BTMGR_BUSY_OPEN_INQ  = 0x04,
	TLKMMI_BTMGR_BUSY_WAIT_DEV  = 0x08,
	TLKMMI_BTMGR_BUSY_OPEN_CONN = 0x10,
	TLKMMI_BTMGR_BUSY_WAIT_CONN = 0x20,
}TLKMMI_BTMGR_BUSY_ENUM;


typedef struct{
	uint08 busys;
	uint08 flags;
	uint16 timeout;
	uint32 devClass;
	uint16 connTime;
	uint08 btaddr[6];
	tlkapi_timer_t timer;
}tlkmmi_btmgr_acl_t;

/******************************************************************************
 * Function: tlkmmi_btmgr_aclInit
 * Descript: Handle bt ACL initial including register acl callback to listening 
 *           each acl status changing, and reset the acl manager resource. 
 * Params:
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_btmgr_aclInit(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_aclIsBusy
 * Descript: verify acl is busy. 
 * Params:
 * Return: true is busy, false is idle.
 * Others: None.
*******************************************************************************/
bool tlkmmi_btmgr_aclIsBusy(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_connect
 * Descript: Connect the acl link and set a timeout to handle acl connect timeout. 
 * Params:
 *         @btaddr[IN]--The BT address.
 *         @timeout[IN]--The timeout value. Unit:ms.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_btmgr_connect(uint08 btaddr[6], uint32 timeout);

/******************************************************************************
 * Function: tlkmmi_btmgr_disconn
 * Descript: Disconnect the acl link which specify by bt handle. 
 * Params:
 *        @handle[IN]--The acl handle.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_btmgr_disconn(uint16 handle);

/******************************************************************************
 * Function: tlkmmi_btmgr_disconnByAddr
 * Descript: Disconnect the acl link which specify by bt address. 
 * Params:
 *        @handle[IN]--The Bt address.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmmi_btmgr_disconnByAddr(uint08 btaddr[6]);

/******************************************************************************
 * Function: tlkmdi_btmgr_regAclConnectCB
 * Descript: Register a acl connect callback.. 
 * Params:
 *        @connCB[IN]--The callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
extern void tlkmdi_btmgr_regAclConnectCB(TlkMmiBtMgrAclConnectCallback connCB);

/******************************************************************************
 * Function: tlkmdi_btmgr_regAclDisconnCB
 * Descript: Register the acl link disconnect callback. 
 * Params:
 *        @discCB[IN]--The Acl Disconnect callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
extern void tlkmdi_btmgr_regAclDisconnCB(TlkMmiBtMgrAclDisconnCallback discCB);

/******************************************************************************
 * Function: tlkmdi_btmgr_regProfileConnectCB
 * Descript: Register a profile connect callback.. 
 * Params:
 *        @connCB[IN]--The callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
extern void tlkmdi_btmgr_regProfileConnectCB(TlkMmiBtMgrProfileConnectCallback connCB);

/******************************************************************************
 * Function: tlkmdi_btmgr_regProfileDisconnCB
 * Descript: Register the profile disconnect callback. 
 * Params:
 *        @discCB[IN]--The Acl Disconnect callback function.
 * Return: TLK_ENONE is success, others value is failure.
 * Others: None.
*******************************************************************************/
extern void tlkmdi_btmgr_regProfileDisconnCB(TlkMmiBtMgrProfileDisconnCallback discCB);


#endif //#if (TLKMMI_BTMGR_BTACL_ENABLE)

#endif //TLKMMI_BTMGR_ACL_H


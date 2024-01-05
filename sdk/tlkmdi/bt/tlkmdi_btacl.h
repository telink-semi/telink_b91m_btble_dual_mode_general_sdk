/********************************************************************************************************
 * @file	tlkmdi_btacl.h
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
#ifndef TLKMDI_BTACL_H
#define TLKMDI_BTACL_H

#if (TLK_MDI_BTACL_ENABLE)

/******************************************************************************
 * File: tlkmdi_btacl
 * Auth: telink
 * Func: This file is used to manage the logics related to connections, 
 *       including ACL and Profile connections.
*******************************************************************************/

#define TLKMDI_BTACL_TIMEOUT             200000 //us
#define TLKMDI_BTACL_TIMEOUT_MS          200
#define TLKMDI_BTACL_CONN_DEF_TIMEOUT    (20000000/TLKMDI_BTACL_TIMEOUT)
#define TLKMDI_BTACL_CONN_MIN_TIMEOUT    (10000000/TLKMDI_BTACL_TIMEOUT) //6S
#define TLKMDI_BTACL_CONN_MAX_TIMEOUT    (40000000/TLKMDI_BTACL_TIMEOUT) //40S -- Spec specifies that the maximum value is 40.9s
#define TLKMDI_BTACL_DISC_DEF_TIMEOUT    (8000000/TLKMDI_BTACL_TIMEOUT)
#define TLKMDI_BTACL_DISC_MIN_TIMEOUT    (5000000/TLKMDI_BTACL_TIMEOUT)
#define TLKMDI_BTACL_DISC_MAX_TIMEOUT    (30000000/TLKMDI_BTACL_TIMEOUT)

#define TLKMDI_BTACL_IDLE_DEF_TIMEOUT    (10000000/TLKMDI_BTACL_TIMEOUT)
#define TLKMDI_BTACL_IDLE_DEF_TIMEOUT1   (1500000/TLKMDI_BTACL_TIMEOUT)

#define TLKMDI_BTACL_PROFILE_CONN_TIMEOUT     (15000000/TLKMDI_BTACL_TIMEOUT)
#define TLKMDI_BTACL_PROFILE_DISC_TIMEOUT     (10000000/TLKMDI_BTACL_TIMEOUT)


#define TLKMDI_BTACL_ITEM_NUMB           TLK_STK_BTACl_NUMB
#define TLKMDI_BTACL_PROF_NUMB           8

#define TLKMDI_BTACL_PEER_ANDROID_DEV        0
#define TLKMDI_BTACL_PEER_IOS_DEV            1


typedef enum{
	TLKMDI_BTACL_STAGE_NONE = 0,
	TLKMDI_BTACL_CONNING_STAGE_NONE = 0,
	TLKMDI_BTACL_CONNING_STAGE_OPEN,
	TLKMDI_BTACL_CONNING_STAGE_WAIT,

	TLKMDI_BTACL_DISCING_STAGE_NONE = 0,
	TLKMDI_BTACL_DISCING_STAGE_PROFILE,
	TLKMDI_BTACL_DISCING_STAGE_MIDDLE,
	TLKMDI_BTACL_DISCING_STAGE_ACL,
	TLKMDI_BTACL_DISCING_STAGE_WAIT,
}TLKMDI_BTACL_STAGE_ENUM;


typedef enum{
	TLKMDI_BTACL_ATTR_NONE = 0x00,
	TLKMDI_BTACL_ATTR_REQUEST = 0x01,
	TLKMDI_BTACL_ATTR_CONNECT = 0x02,
	TLKMDI_BTACL_ATTR_ENCRYPT = 0x04,
}TLKMDI_BTACL_ATTRS_ENUM;
typedef enum{
	TLKMDI_BTACL_BUSY_NONE = 0x00,
	TLKMDI_BTACL_BUSY_CONN_PROF  = 0x02,
	TLKMDI_BTACL_BUSY_DISC_PROF  = 0x04,
	TLKMDI_BTACL_WAIT_DISC_PROF  = 0x08,
	TLKMDI_BTACL_BUSY_DISC_RFC   = 0x01,
	TLKMDI_BTACL_WAIT_DISC_RFC   = 0x02,
	TLKMDI_BTACL_BUSY_DISC_ACL   = 0x80,
	TLKMDI_BTACL_WAIT_DISC_ACL   = 0x40,
	TLKMDI_BTACL_WAIT_CANCEL     = 0x20,
}TLKMDI_BTACL_BUSYS_ENUM;
typedef enum{
	TLKMDI_BTACL_FLAG_NONE = 0x00,
		
}TLKMDI_BTACL_FLAGS_ENUM;


typedef void(*TlkMdiBtAclConnCallback)(uint16 handle, uint08 status, uint08 *pBtAddr);
typedef void(*TlkMdiBtAclDiscCallback)(uint16 handle, uint08 reason, uint08 *pBtAddr);
typedef void(*TlkMdiBtAclCrypCallback)(uint16 handle, uint08 status, uint08 *pBtAddr);
typedef void(*TlkMdiBtAclProfConnCallback)(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr);
typedef void(*TlkMdiBtAclProfDiscCallback)(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr);

typedef struct{
	uint08 state;
	uint08 ptype; //Profile Type, Refer to BTP_PTYPE_ENUM.
	uint08 usrID;
	uint08 cwait; //Connect Wait Time. ()
	uint16 delay; //
}tlkmdi_btacl_prof_t;
typedef struct{
	uint08 state;
	uint08 stage;
	uint08 busys;
	uint08 attrs;
	uint08 flags;
	uint08 profIsConn;
	uint16 handle;
	uint16 timeout;
	uint16 connFlag; //Connect Flag
	uint16 idleTime;
	uint32 devClass;
	uint08 active;
	uint08 hfChannel;
	uint08 agChannel;
	uint08 sppChannel;
	uint08 iapChannel;
	uint08 pbapChannel;
	uint08 btaddr[6];
	tlkapi_timer_t timer;
	tlkmdi_btacl_prof_t prof[TLKMDI_BTACL_PROF_NUMB];
}tlkmdi_btacl_item_t;
typedef struct{
	tlkmdi_btacl_item_t item[TLKMDI_BTACL_ITEM_NUMB];
}tlkmdi_btacl_ctrl_t;


/******************************************************************************
 * Function: tlkmdi_btacl_init
 * Descript: Initial the ACL control block and register the callback, including
 *           the acl connection callback and some profile connection callback.
 * Params:
 * Return: Return TLK_ENONE is success/other value is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_init(void);

/******************************************************************************
 * Function: tlkmdi_btacl_isActive
 * Descript: Check the acl status is connected.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is active/false is de-active.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isActive(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_btacl_isHaveRfc
 * Descript: Check rfcomm whether used or not.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isHaveRfc(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_btacl_isFindHfp
 * Descript: Check HFP whether used or not.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isFindHfp(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_btacl_isFindSpp
 * Descript: Check SPP whether used or not.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isFindSpp(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_btacl_isFindPbap
 * Descript: Check Pbap whether used or not.
 * Params:
 *        @handle[IN]--The acl handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isFindPbap(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_btacl_cancel
 * Descript: Cancel the acl link setup procedure.
 * Params:
 *        @pBtAddr[IN]--The bt address.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_cancel(uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmdi_btacl_connect
 * Descript: Create a connection setup procedure.
 * Params:
 *        @pBtAddr[IN]--The bt address.
 *        @devClass[IN]--The Device type.
 *        @timeout[IN]--The timeout value. Unit:ms, Range:3000~3600000.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_connect(uint08 *pBtAddr, uint32 devClass, uint32 timeout); //timeout: unit-ms

/******************************************************************************
 * Function: tlkmdi_btacl_disconn
 * Descript: DisConnect the connection.
 * Params:
 *        @handle[IN]--The ACL handle.
 *        @reaosn[IN]--The disconnect reason.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_disconn(uint16 handle, uint08 reason);


/******************************************************************************
 * Function: tlkmdi_btacl_appendProf
 * Descript: Add a new profile to a acl link.
 * Params:
 *        @handle[IN]--The ACL handle.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id.
 *        @delayMs[IN]--The delay time.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_appendProf(uint16 handle, uint08 ptype, uint08 usrID, uint16 delayMs);

/******************************************************************************
 * Function: tlkmdi_btacl_removeProf
 * Descript: remove a profile from the acl link.
 * Params:
 *        @handle[IN]--The ACL handle.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_removeProf(uint16 handle, uint08 ptype, uint08 usrID);

/******************************************************************************
 * Function: tlkmdi_btacl_getRole
 * Descript: Get the role of acl link.
 * Params:
 *        @devClass[IN]--The device class.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_getRole(uint32 devClass);

/******************************************************************************
 * Function: tlkmdi_btacl_regConnectCB/tlkmdi_btacl_regDisconnCB
 *           /tlkmdi_btacl_regProfileConnectCB/tlkmdi_btacl_regProfileDisconnCB
 * Descript: Register acl link callback.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btacl_regConnectCB(TlkMdiBtAclConnCallback callback);
void tlkmdi_btacl_regEncryptCB(TlkMdiBtAclDiscCallback callback);
void tlkmdi_btacl_regDisconnCB(TlkMdiBtAclDiscCallback callback);
void tlkmdi_btacl_regProfileConnectCB(TlkMdiBtAclProfConnCallback callback);
void tlkmdi_btacl_regProfileDisconnCB(TlkMdiBtAclProfDiscCallback callback);

/******************************************************************************
 * Function: tlkmdi_btacl_getIdleCount
 * Descript: Get the idle acl link count.
 * Params: None.
 * Return: The count of idle acl link.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_btacl_getIdleCount(void);

/******************************************************************************
 * Function: tlkmdi_btacl_getUsedCount
 * Descript: Get the used acl link count.
 * Params: None.
 * Return: The count of used acl link.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_btacl_getUsedCount(void);

/******************************************************************************
 * Function: tlkmdi_btacl_getConnCount
 * Descript: Get the connected acl link count.
 * Params: None.
 * Return: The count of connected acl link.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_btacl_getConnCount(void);

/******************************************************************************
 * Function: tlkmdi_btacl_resetItem
 * Descript: Reset the acl control block.
 * Params: @pItem[IN]--The acl control block.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btacl_resetItem(tlkmdi_btacl_item_t *pItem);

/******************************************************************************
 * Function: tlkmdi_btacl_getIdleItem
 * Descript: Get a idle acl control block.
 * Params: None.
 * Return: The idle acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getIdleItem(void);

/******************************************************************************
 * Function: tlkmdi_btacl_getInitItem
 * Descript: Get a init acl control block.
 * Params: None.
 * Return: The init acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getInitItem(void);

/******************************************************************************
 * Function: tlkmdi_btacl_getBusyItem
 * Descript: Get a Connecting acl control block.
 * Params: None.
 * Return: The connecting acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getBusyItem(void);

/******************************************************************************
 * Function: tlkmdi_btacl_getUsedItem
 * Descript: Get a Used acl control block.
 * Params: None.
 * Return: The used acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getUsedItem(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_btacl_getConnItem
 * Descript: Get a Connected acl control block.
 * Params: None.
 * Return: The Connected acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getConnItem(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_btacl_getConnItemByIndex
 * Descript: Get a Connected acl control block.
 * Params: @Index[IN]--The index of connected acl control block.
 * Return: The Connected acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getConnItemByIndex(uint08 index);

/******************************************************************************
 * Function: tlkmdi_btacl_searchUsedItem
 * Descript: Find a Used acl control block.
 * Params: @pBtAddr[IN]--The Bt address of acl.
 * Return: The Used acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_searchUsedItem(uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmdi_btacl_searchUsedItem
 * Descript: Find a Connected acl control block.
 * Params: @pBtAddr[IN]--The Bt address of acl.
 * Return: The Connected acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_searchConnItem(uint08 *pBtAddr);



#endif //#if (TLK_MDI_BTACL_ENABLE)

#endif //TLKMDI_BTACL_H


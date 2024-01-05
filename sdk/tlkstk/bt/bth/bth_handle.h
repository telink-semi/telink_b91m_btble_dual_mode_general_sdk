/********************************************************************************************************
 * @file	bth_handle.h
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
#ifndef BTH_HANDLE_H
#define BTH_HANDLE_H


typedef struct {
    uint08 state;
    uint08 attrs;
    uint16 busys;
    uint16 flags;
    uint16 timeout;
    uint32 connTime;
    uint16 aclHandle;
    uint16 pageTime;
    uint16 rswTimer;      // negetive rsw
    uint16 authSecrTimer; // negetive auth
    uint16 exitTimer;     // Exit Sniffer wait timer in disconnect acl.
    uint16 connTimer;

    uint08 sniffBusys; // Refer BTH_ACL_SNIFF_BUSYS_ENUM.
    uint08 sniffReqCount;
    uint16 sniffReqTimer;
    uint16 unsniffReqTimer;
    uint16 leaveSniffTimer;

    uint08 reason;
    uint08 othBusys; // Other Busys
    uint08 btaddr[6];

    uint32 devClass;
    uint08 curRole;
    uint08 airMode;
    uint08 reserved;
    uint08 positive;   // 1:positive connect to the remote or 0: negative be connected
    uint08 switchCnt;  // role_switch_req_cnt
    uint08 scanMode;   // page_scan_repetition_mode
    uint16 clkOffs;
    uint32 extFeature; // Peer

    uint08 curMode;
    uint08 initRole;
    uint08 curPolicy;
    uint08 setPolicy;

    uint08 pinCode[4];
    uint08 linkKey[16];
    uint08 infoRsp[16];
    uint08 devName[32];

    tlkapi_timer_t timer;
} bth_handle_t, bth_acl_handle_t;


typedef struct {
    uint08 state;
    uint08 busys;
    uint08 reason;
    uint08 airMode;
    uint08 timeout;
    uint08 positive;
    uint08 linkType; // BTH_LINK_TYPE_ESCO or BTH_LINK_TYPE_SCO
    uint08 resv01;
    uint16 aclHandle;
    uint16 scoHandle;
    tlkapi_timer_t timer;
} bth_sco_handle_t;


/******************************************************************************
 * Function: bth_handle_init
 * Descript: Initial the acl handle control block and sco handle control block.
 * Params:
 * 		@aclNumb[IN]--refer to TLK_STK_BTACl_NUMB
 * 		@scoNumb[IN]--refer to TLK_STK_BTSCO_NUMB
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *         If others value is returned means the initial process fail.
 *******************************************************************************/
int bth_handle_init(uint08 aclNumb, uint08 scoNumb);

/******************************************************************************
 * Function: bth_handle_deinit
 * Descript: deinitial the acl handle control block and sco handle control block.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
int bth_handle_deinit(void);

/******************************************************************************
 * Function: bth_handle_getMemLen
 * Descript: get the size of the required number of nodes interface.
 * Params:
 * 		@aclNumb[IN]--refer to TLK_STK_BTACl_NUMB
 * 		@scoNumb[IN]--refer to TLK_STK_BTSCO_NUMB
 * Return: success: node size.
 *         -TLK_EPARAM means the get memory length fail.
 *******************************************************************************/
int bth_handle_getMemLen(uint08 aclNumb, uint08 scoNumb);


/******************************************************************************
 * Function: bth_handle_getIdleAclCount
 * Descript: Get the number of idle acl handle.
 * Params: None.
 * Return: The number of idle acl handle.
 *******************************************************************************/
uint08 bth_handle_getIdleAclCount(void);

/******************************************************************************
 * Function: bth_handle_getUsedAclCount
 * Descript: Get the number of used acl handle.
 * Params: None.
 * Return: The number of used acl handle.
 *******************************************************************************/
uint08 bth_handle_getUsedAclCount(void);

/******************************************************************************
 * Function: bth_handle_getConnAclCount
 * Descript: Get the number of Connected acl handle.
 * Params: None.
 * Return: The number of connected acl handle.
 *******************************************************************************/
uint08 bth_handle_getConnAclCount(void);

/******************************************************************************
 * Function: bth_handle_getDiscAclCount
 * Descript: Get the number of DisConnected acl handle.
 * Params: None.
 * Return: The number of connected acl handle.
 *******************************************************************************/
uint08 bth_handle_getDiscAclCount(void);

/******************************************************************************
 * Function: bth_handle_getIdleScoCount
 * Descript: Get the number of idle sco handle.
 * Params: None.
 * Return: The number of idle sco handle.
 *******************************************************************************/
uint08 bth_handle_getIdleScoCount(void);

/******************************************************************************
 * Function: bth_handle_getUsedScoCount
 * Descript: Get the number of used sco handle.
 * Params: None.
 * Return: The number of used sco handle.
 *******************************************************************************/
uint08 bth_handle_getUsedScoCount(void);

/******************************************************************************
 * Function: bth_handle_getConnScoCount
 * Descript: Get the number of connected sco handle.
 * Params: None.
 * Return: The number of connected sco handle.
 *******************************************************************************/
uint08 bth_handle_getConnScoCount(void);

/******************************************************************************
 * Function: bth_handle_getDiscScoCount
 * Descript: Get the number of disconnected sco handle.
 * Params: None.
 * Return: The number of disconnected sco handle.
 *******************************************************************************/
uint08 bth_handle_getDiscScoCount(void);

/******************************************************************************
 * Function: bth_handle_getAclHandle
 * Descript: Get the acl handle via bt address.
 * Params: None.
 * Return: The acl handle.
 *******************************************************************************/
uint16 bth_handle_getAclHandle(uint08 *pBtAddr);

/******************************************************************************
 * Function: bth_handle_getBtAddr
 * Descript: Get the Bt address.
 * Params: @aclHandle[IN]--The acl link handle.
 * Return: The bt address.
 *******************************************************************************/
uint08 *bth_handle_getBtAddr(uint16 aclHandle);

/******************************************************************************
 * Function: bth_handle_resetAcl
 * Descript: reset the acl handle and release the resource.
 * Params:
 *        @pHandle[IN]--The acl link handle.
 * Return: None.
 *******************************************************************************/
void bth_handle_resetAcl(bth_acl_handle_t *pHandle);

/******************************************************************************
 * Function: bth_handle_resetSco
 * Descript: reset the sco handle and release the resource.
 * Params:
 *        @pHandle[IN]--The acl link handle.
 * Return: None.
 *******************************************************************************/
void bth_handle_resetSco(bth_sco_handle_t *pHandle);


/******************************************************************************
 * Function: Get the idle acl handle item or get used or connected item
 *           via acl handle or bt address
 * Descript: get the acl handle item.
 * Params:
 * Return: acl Handle item.
 *******************************************************************************/
bth_acl_handle_t *bth_handle_getIdleAcl(void);
bth_acl_handle_t *bth_handle_getFirstConnAcl(void);
bth_acl_handle_t *bth_handle_getUsedAcl(uint16 aclHandle);
bth_acl_handle_t *bth_handle_getConnAcl(uint16 aclHandle);
bth_acl_handle_t *bth_handle_getDiscAcl(uint16 aclHandle);
bth_acl_handle_t *bth_device_getConnAclByType(uint08 devType);
bth_acl_handle_t *bth_handle_searchUsedAcl(uint08 *pBtAddr);
bth_acl_handle_t *bth_handle_searchConnAcl(uint08 *pBtAddr);

/******************************************************************************
 * Function: Get the idle sco handle item or get used or connected item
 *           via sco handle or acl handle.
 * Descript: get the sco handle item.
 * Params:
 * Return: sco Handle item.
 *******************************************************************************/
bth_sco_handle_t *bth_handle_getIdleSco(void);
bth_sco_handle_t *bth_handle_getUsedSco(uint16 scoHandle);
bth_sco_handle_t *bth_handle_getConnSco(uint16 scoHandle);
bth_sco_handle_t *bth_handle_getDiscSco(uint16 scoHandle);
bth_sco_handle_t *bth_handle_searchUsedSco(uint16 aclHandle);
bth_sco_handle_t *bth_handle_searchConnSco(uint16 aclHandle);
bth_sco_handle_t *bth_handle_searchBusySco(uint16 aclHandle);
bth_sco_handle_t *bth_handle_findUsedSco(uint16 aclHandle, uint08 offset);
bth_sco_handle_t *bth_handle_findConnSco(uint16 aclHandle, uint08 offset);
uint16 bth_handle_getConnScoHandle(uint16 aclHandle);


#endif // BTH_HANDLE_H

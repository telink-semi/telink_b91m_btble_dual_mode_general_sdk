/********************************************************************************************************
 * @file	tlkmmi_btmgr.h
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
#ifndef TLKMMI_BTMGR_H
#define TLKMMI_BTMGR_H

#if (TLKMMI_BTMGR_ENABLE)


#define TLKMMI_BTMGR_BTREC_ENABLE     (1 && TLK_MDI_BTREC_ENABLE)
#define TLKMMI_BTMGR_BTINQ_ENABLE     (1 && TLK_MDI_BTINQ_ENABLE)
#define TLKMMI_BTMGR_BTACL_ENABLE     (1 && TLK_MDI_BTACL_ENABLE)


#define TLKMMI_BTMGR_DBG_FLAG         ((TLK_MAJOR_DBGID_MMI_BTMGR << 24) | (TLK_MINOR_DBGID_MMI_BTMGR << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_BTMGR_DBG_SIGN         "[MMI]"

#define TLKMMI_BTMGR_MAJOR_SERVICE_CLASS	(BTH_COD_SERVICE_CLASS_AUDIO | BTH_COD_SERVICE_CLASS_RENDER)
#define TLKMMI_BTMGR_MAJOR_DEVICE_CLASS		(BTH_COD_MAJOR_CLASS_AUDIO)
#define TLKMMI_BTMGR_MINOR_DEVICE_CLASS		(BTH_COD_AUDIO_WEARABLE_HEADSET)

#define TLKMMI_BTMGR_DEVICE_CLASS     (TLKMMI_BTMGR_MAJOR_SERVICE_CLASS | TLKMMI_BTMGR_MAJOR_DEVICE_CLASS | TLKMMI_BTMGR_MINOR_DEVICE_CLASS)//0x240404//0x5a020c
#define TLKMMI_BTMGR_BDADDR_DEF       {0x2c, 0xc0, 0xc3, 0x6f, 0xbb, 0x31}
#define TLKMMI_BTMGR_BTNAME_DEF       "TLK_Smart_Watch"



typedef void(*TlkMmiBtMgrAclConnectCallback)(uint16 handle, uint08 status, uint08 *pBtAddr);
typedef void(*TlkMmiBtMgrAclDisconnCallback)(uint16 handle, uint08 reason, uint08 *pBtAddr);

typedef void(*TlkMmiBtMgrProfileConnectCallback)(uint16 handle, uint08 status, uint08 ptype, uint08 usrID);
typedef void(*TlkMmiBtMgrProfileDisconnCallback)(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID);


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


#if (TLKMMI_BTMGR_BTREC_ENABLE)

/******************************************************************************
 * Function: tlkmmi_btmgr_recStart
 * Descript: Trigger start BT page. 
 * Params:
 *         @pDevAddr[IN]--The device bt address.
 *         @devClass[IN]--The device class type.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_btmgr_recStart(uint08 *pDevAddr, uint32 devClass, bool enPage, bool enScan);

/******************************************************************************
 * Function: tlkmmi_btmgr_recClose
 * Descript: Trigger stop BT page. 
 * Params: None.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_btmgr_recClose(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_recIsBusy
 * Descript: Check the BT is busy. 
 * Params: None.
 * Return: Return true is busy, false is idle.
 * Others: None.
*******************************************************************************/
extern bool tlkmmi_btmgr_recIsBusy(void);
extern bool tlkmmi_btmgr_recIsPage(void);
extern bool tlkmmi_btmgr_recIsScan(void);

extern uint08 *tlkmmi_btmgr_recPageAddr(void);

#endif //#if (TLKMMI_BTMGR_BTREC_ENABLE)


#endif 

#endif //TLKMMI_BTMGR_H


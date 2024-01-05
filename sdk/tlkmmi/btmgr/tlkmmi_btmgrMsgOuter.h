/********************************************************************************************************
 * @file	tlkmmi_btmgrMsgOuter.h
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
#ifndef TLKMMI_BTMGR_MSG_OUTER_H
#define TLKMMI_BTMGR_MSG_OUTER_H

#if (TLKMMI_BTMGR_ENABLE)



int tlkmmi_btmgr_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmmi_btmgr_ptypeToCtype
 * Descript: convert the ptype to ctype. 
 * Params:
 *        @ptype[IN]--The profile type
 *        @usrID[IN]--The user id(Client or Server).
 * Return: The communication type(AG/HFP/A2DPSNK/A2DPSRC..).
 * Others: None.
*******************************************************************************/
uint08 tlkmmi_btmgr_ptypeToCtype(uint08 ptype, uint08 usrID);

/******************************************************************************
 * Function: tlkmmi_btmgr_sendAclConnectEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendAclConnectEvt(uint16 handle, uint08 status, uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmmi_btmgr_sendAclDisconnEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendAclDisconnEvt(uint16 handle, uint08 reason, uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmmi_btmgr_sendProfConnectEvt
 * Descript: Send profile connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id(Client/Server).
 *        @pBtAddr[IN]--The Bt address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendProfConnectEvt(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmmi_btmgr_sendProfDisconnEvt
 * Descript: Send profle disconnect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id(Client/Server).
 *        @pBtAddr[IN]--The Bt address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendProfDisconnEvt(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr);



#endif //#if (TLKMMI_BTMGR_ENABLE)

#endif //TLKMMI_BTMGR_MSG_OUTER_H


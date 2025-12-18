/********************************************************************************************************
 * @file	tlkmmi_lemgrMsgOuter.h
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
#ifndef TLKMMI_LEMGR_MSG_OUTER_H
#define TLKMMI_LEMGR_MSG_OUTER_H

#if (TLKMMI_LEMGR_ENABLE)



int tlkmmi_lemgr_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmmi_lemgr_sendAclConnectEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_lemgr_sendAclConnectEvt(uint16 handle, uint08 status,
	uint08 peerRole, uint08 peerType, uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmmi_lemgr_sendAclDisconnEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_lemgr_sendAclDisconnEvt(uint16 handle, uint08 reason, uint08 *pBtAddr);


void tlkmmi_lemgr_sendAclAdvReportEvt(uint08 pData_byte,uint08 *pData,s8 rssi);
void tlkmmi_lemgr_sendAdvStartEvt(void);
void tlkmmi_lemgr_sendAdvCompleteEvt(void);

void tlkmmi_lemgr_sendScanStartEvt(void);
void tlkmmi_lemgr_sendScanCompleteEvt(void);


#endif //TLKMMI_LEMGR_ENABLE

#endif //TLKMMI_LEMGR_MSG_OUTER_H


/********************************************************************************************************
 * @file	btp_browsingSend.h
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
#ifndef BTP_BROWSING_SEND_H
#define BTP_BROWSING_SEND_H


void btp_browsing_sendGeneralRejectProc(btp_browsing_item_t *pBrowsing);


int btp_browsing_sendCmd(uint16 aclHandle, uint08 trid, uint08 pduID, uint08 *pParam, uint16 paramLen);
int btp_browsing_sendRsp(uint16 aclHandle, uint08 trid, uint08 pduID, uint08 *pParam, uint16 paramLen);

int btp_browsing_sendGeneralReject(uint16 aclHandle, uint08 errCode);
int btp_browsing_setChangePathCmd(uint16 aclHandle, uint16 uidCounter, uint08 direct, uint08 *pUID);
int btp_browsing_setBrowsedPlayerCmd(uint16 aclHandle, uint16 playerID);
int btp_browsing_setBrowsedPlayerRsp(uint16 aclHandle, uint08 status);
int btp_browsing_sendGetFolderItemsCmd(uint16 aclHandle,
                                       uint08 scope,
                                       uint32 startItem,
                                       uint32 endItem,
                                       uint08 attCount,
                                       uint32 *pAttList);
int btp_browsing_sendGetItemAttrCmd(uint16 aclHandle,
                                    uint08 scope,
                                    uint08 *pUids,
                                    uint08 uidCounter,
                                    uint08 attCount,
                                    uint32 *pAttList);
int btp_browsing_sendGetTotalNumbOfItemsCmd(uint16 aclHandle, uint08 scope);
int btp_browsing_sendGetTotalNumbOfItemsRsp(uint16 aclHandle, uint08 status, uint16 uidCounter, uint32 number);


#endif // BTP_BROWSING_SEND_H

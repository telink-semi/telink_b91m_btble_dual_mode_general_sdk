/********************************************************************************************************
 * @file	btp_browsingRecv.h
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
#ifndef BTP_BROWSING_RECV_H
#define BTP_BROWSING_RECV_H


void btp_browsing_recvCmdProc(btp_browsing_item_t *pBrowsing, uint08 trid, uint08 pduID, uint08 *pData, uint16 dataLen);
void btp_browsing_recvRspProc(btp_browsing_item_t *pBrowsing, uint08 trid, uint08 pduID, uint08 *pData, uint16 dataLen);


#endif // BTP_BROWSING_RECV_H

/********************************************************************************************************
 * @file	tlkcpc1_send.h
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
#ifndef TLKCPC1_SEND_H
#define TLKCPC1_SEND_H

#if (TLK_CFG_COMM_ENABLE)




int tlkcpc1_send_init(tlkcpc1_send_ctrl_t *pCtrl);

void tlkcpc1_send_setBuffer(uint08 *pBuffer, uint16 buffLen);

void tlkcpc1_send_regCB(TlkCpcSendCB sendCB);
void tlkcpc1_send_handler(void);

int tlkcpc1_send_cmd(uint08 mType, uint08 cmdID, uint08 *pData, uint08 dataLen);
int tlkcpc1_send_rsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkcpc1_send_evt(uint08 mType, uint08 evtID, uint08 *pData, uint08 dataLen);
int tlkcpc1_send_dat(uint08 datID, uint32 numb, uint08 *pData, uint16 dataLen);
int tlkcpc1_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);



#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKCPC1_SEND_H


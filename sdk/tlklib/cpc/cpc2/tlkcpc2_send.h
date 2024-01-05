/********************************************************************************************************
 * @file	tlkmdi_atsSend.h
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
#ifndef TLKCPC2_SEND_H
#define TLKCPC2_SEND_H

#if (TLK_CFG_COMM_ENABLE)



int tlkcpc2_send_init(void);

void tlkcpc2_send_setBuffer(uint08 *pBuffer, uint16 buffLen);

void tlkcpc2_send_handler(void);

int tlkcpc2_send_packet(bool isEnExt, uint08 ptype, uint08 *pHead, uint08 headLen, uint08 *pBody, uint16 bodyLen, bool isForce);

int tlkcpc2_sendCmdPacket(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen, bool enFlow);
int tlkcpc2_sendRspPacket(uint08 mtype, uint16 msgID, uint08 status, uint08 reason, uint08 *pData, uint16 dataLen, bool enFlow);
int tlkcpc2_sendEvtPacket(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen, bool enFlow);
int tlkcpc2_sendDatPacket(uint08 datID, uint32 number, uint08 *pData, uint16 dataLen);
int tlkcpc2_sendFcsPacket(uint08 bitP, uint08 bitF, uint08 bitR, uint08 recvNumb, uint08 sendNumb);

int tlkcpc2_sendResetPacket(uint08 resetCode); //resetCode refer TLKCPC2_RST_CODE_ENUM
int tlkcpc2_sendResetReqPacket(void);
int tlkcpc2_sendResetAckPacket(void);
int tlkcpc2_sendResetCfmPacket(void);
int tlkcpc2_sendResetDonePacket(void);

int tlkcpc2_packet_escape(uint08 *pData, uint16 dataLen, uint16 maxLen);
int tlkcpc2_packet_revise(uint08 *pData, uint16 dataLen);


#endif //#if (TLK_TEST_ATS_ENABLE)

#endif //TLKCPC2_SEND_H


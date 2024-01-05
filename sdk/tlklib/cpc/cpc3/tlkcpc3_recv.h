/********************************************************************************************************
 * @file	tlkcpc3_recv.h
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
#ifndef TLKCPC3_RECV_H
#define TLKCPC3_RECV_H

#if (TLK_CFG_COMM_ENABLE)


typedef enum{
	TLKCPC3_RECV_MSTATE_HEAD = 0,
	TLKCPC3_RECV_MSTATE_BODY,
	TLKCPC3_RECV_MSTATE_READY,
}TLKCPC3_RECV_MSTATE_ENUM;



int tlkcpc3_recv_init(tlkcpc3_recv_ctrl_t *pCtrl);

void tlkcpc3_recv_reset(void);

void tlkcpc3_recv_regCB(TlkCpcRecvCB recvCB);

void tlkcpc3_recv_setBuffer(uint08 *pBuffer, uint16 buffLen);

void tlkcpc3_recv_handler(uint08 *pData, uint16 dataLen);



#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKCPC3_RECV_H


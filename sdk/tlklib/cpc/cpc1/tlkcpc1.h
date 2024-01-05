/********************************************************************************************************
 * @file	tlkcpc1.h
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
#ifndef TLKCPC1_H
#define TLKCPC1_H

#if (TLK_CFG_COMM_ENABLE)



#define TLKCPC1_DBG_FLAG           ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_COMM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKCPC1_DBG_SIGN           "[COMM]"


#define TLKCPC1_FRM_EXTLEN         8
#define TLKCPC1_CMDHEAD_LENS       4
#define TLKCPC1_DATHEAD_LENS       5


#define TLKCPC1_HEAD_SIGN          0x7F
#define TLKCPC1_TAIL_SIGN          0x80

#define TLKCPC1_HEAD_SIGN0         TLKCPC1_HEAD_SIGN
#define TLKCPC1_HEAD_SIGN1         TLKCPC1_HEAD_SIGN
#define TLKCPC1_TAIL_SIGN0         TLKCPC1_TAIL_SIGN
#define TLKCPC1_TAIL_SIGN1         TLKCPC1_TAIL_SIGN


typedef struct{
	uint08 sendNumb;
	uint08 recvNumb;
	uint08 makeState;
	uint08 reserved;
	uint32 busyTimer;
	uint16 makeLens;
	uint16 recvLens;
	uint16 buffLen;
	uint08 *pBuffer;
	TlkCpcRecvCB recvCB;
}tlkcpc1_recv_ctrl_t;

typedef struct{
	uint08 sendNumb;
	uint08 reserved;
	uint16 sendLens;
	uint16 buffLen;
	uint08 *pBuffer;
	TlkCpcSendCB sendCB;
}tlkcpc1_send_ctrl_t;

typedef struct{
	tlkcpc1_send_ctrl_t sendCtrl;
	tlkcpc1_recv_ctrl_t recvCtrl;
}tlkcpc1_ctrl_t;



#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKCPC1_H


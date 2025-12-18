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
#ifndef TLKCPC3_H
#define TLKCPC3_H

#if (TLK_CFG_COMM_ENABLE)



#define TLKCPC3_DBG_FLAG           ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_COMM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKCPC3_DBG_SIGN           "[COMM]"


#define TLKCPC3_FRM_EXTLEN         8
#define TLKCPC3_CMDHEAD_LENS       4
#define TLKCPC3_DATHEAD_LENS       5


#define TLKCPC3_FRAME_HEAD_SIGN         0xAC
#define TLKCPC3_FRAME_TAIL_SIGN         0xCA
#define TLKCPC3_ESCAPE_CHARS            0xDC //Must not TLKCPC1_HEAD_SIGN or TLKPRT_COMM_HEAD_TAIL
#define TLKCPC3_ESCAPE_CHARS_RAW        0x01 //eg: DC -> DC 01
#define TLKCPC3_ESCAPE_CHARS_HEAD       0x02 //eg: AC -> DC 02
#define TLKCPC3_ESCAPE_CHARS_TAIL       0x03 //eg: CA -> DC 03


typedef struct{
	uint32 sendLens;
	uint32 buffLens;
	uint08 *pBuffer;
	TlkCpcSendCB sendCB;
}tlkcpc3_send_ctrl_t;
typedef struct{
	uint08 sendNumb;
	uint08 recvNumb;
	uint08 mstate;
	uint08 reserved;
	uint32 busyTimer;
	uint16 makeLens;
	uint16 recvLens;
	uint16 buffLen;
	uint08 *pBuffer;
	TlkCpcRecvCB recvCB;
}tlkcpc3_recv_ctrl_t;


typedef struct{
	tlkcpc3_send_ctrl_t sendCtrl;
	tlkcpc3_recv_ctrl_t recvCtrl;
}tlkcpc3_ctrl_t;



#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKCPC3_H


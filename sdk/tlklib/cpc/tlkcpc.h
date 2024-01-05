/********************************************************************************************************
 * @file	tlkcpc.h
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
#ifndef TLKCPC_H
#define TLKCPC_H

#if (TLK_CFG_COMM_ENABLE)


//CPC: Communication Protocol Control

typedef int(*TlkCpcSendCB)(uint08 *pData, uint16 dataLen);
typedef int(*TlkCpcRecvCB)(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen);
typedef int(*TlkCpcResetCB)(void);

	
typedef enum{
	TLKCPC_OPCODE_NONE = 0,
	TLKCPC_OPCODE_REG_SENDCB,
	TLKCPC_OPCODE_REG_REG_RECEIVE_CB,
	TLKCPC_OPCODE_REG_REG_RESET_REQUEST_CB,
	TLKCPC_OPCODE_REG_REG_RESET_COMPLETE_CB,
	TLKCPC_OPCODE_SET_RECV_BUFFER,
	TLKCPC_OPCODE_SET_SEND_BUFFER,
	TLKCPC_OPCODE_ENABLE_AUTHEN,
	TLKCPC_OPCODE_SET_AUTH_STATUE,
	TLKCPC_OPCODE_GET_BUFFER_SIZE,
	TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE,
}TLKCPC_OPCODE_ENUM;

/******************************************************************************
 * Descript: TLKMDI_COMM_AUTH_STATUS_ENUM is bound to TLKCPC_AUTH_STATUS_ENUM,
 *           and needs to be modified accordingly
 *****************************************************************************/
typedef enum{
    TLKCPC_AUTH_STATUS_NONE    = 0,
    TLKCPC_AUTH_STATUS_SUCCESS = 1,
    TLKCPC_AUTH_STATUS_FAILURE = 2,
    TLKCPC_AUTH_STATUS_RUNNING = 3,
}TLKCPC_AUTH_STATUS_ENUM;



typedef struct{
	int(*Init)(uint08 procID);
	void(*Deinit)(void);
	void(*Reset)(void);
	int(*IdleNumb)(void);
	int(*RecvProc)(uint08 *pData, uint16 dataLen);
	int(*SendProc)(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
	void(*Process)(void);
}tlkcpc_infs_t;




#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKCPC_H


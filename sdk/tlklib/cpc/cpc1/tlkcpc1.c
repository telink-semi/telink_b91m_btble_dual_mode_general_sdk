/********************************************************************************************************
 * @file	tlkcpc1.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_CFG_COMM_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkalg/digest/crc/tlkalg_crc.h"
#include "../tlkcpc.h"
#include "tlkcpc1.h"
#include "tlkcpc1_recv.h"
#include "tlkcpc1_send.h"


static int tlkcpc1_init(uint08 procID);
static void tlkcpc1_deinit(void);
static void tlkcpc1_reset(void);
static int tlkcpc1_recvProc(uint08 *pData, uint16 dataLen);
static int tlkcpc1_sendProc(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen);
static int tlkcpc1_handler(uint16 opcode, uint32 param0, uint32 param1);
static int tlkcpc1_recvRegSendCbDeal(uint32 param0, uint32 param1);
static int tlkcpc1_recvRegRecvCbDeal(uint32 param0, uint32 param1);
static int tlkcpc1_recvSetRecvBufferCbDeal(uint32 param0, uint32 param1);
static int tlkcpc1_recvSetSendBufferDeal(uint32 param0, uint32 param1);
static int tlkcpc1_recvGetBufferSizeDeal(void);


const tlkcpc_infs_t gcTlkCpc1Infs = {
	tlkcpc1_init, //Init
	tlkcpc1_deinit, //Deinit
	tlkcpc1_reset, //Reset
	nullptr, //IdleNumb
	tlkcpc1_recvProc, //RecvProc
	tlkcpc1_sendProc, //SendProc
	tlkcpc1_handler, //Handler
	nullptr, //Process
};
static tlkcpc1_ctrl_t *spTlkCpc1Ctrl;

static int tlkcpc1_init(uint08 procID)
{
	spTlkCpc1Ctrl = (tlkcpc1_ctrl_t*)tlkapi_malloc(sizeof(tlkcpc1_ctrl_t));
	if(spTlkCpc1Ctrl == nullptr) return -TLK_ENOMEM;

	tmemset(spTlkCpc1Ctrl, 0, sizeof(tlkcpc1_ctrl_t));
	tlkcpc1_recv_init(&spTlkCpc1Ctrl->recvCtrl);
	tlkcpc1_send_init(&spTlkCpc1Ctrl->sendCtrl);
	
	return TLK_ENONE;
}
static void tlkcpc1_deinit(void)
{
	if(spTlkCpc1Ctrl != nullptr){
		tlkapi_free(spTlkCpc1Ctrl);
		spTlkCpc1Ctrl = nullptr;
	}
}
static void tlkcpc1_reset(void)
{
	tlkcpc1_recv_reset();
}
static int tlkcpc1_recvProc(uint08 *pData, uint16 dataLen)
{
	tlkcpc1_recv_handler(pData, dataLen);
	return TLK_ENONE;
}
static int tlkcpc1_sendProc(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen)
{
	uint08 ptype;

	ptype = param0 & 0xFF;
	if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT){
		uint08 mtype;
		uint08 msgID;
		mtype = (param0 & 0xFF00) >> 8;
		msgID = (param0 & 0xFF0000) >> 16;
		if(ptype == TLKPRT_COMM_PTYPE_CMD){
			return tlkcpc1_send_cmd(mtype, msgID, pData, dataLen);
		}else{
			return tlkcpc1_send_evt(mtype, msgID, pData, dataLen);
		}
	}else if(ptype == TLKPRT_COMM_PTYPE_RSP){
		uint08 mtype;
		uint08 msgID;
		uint08 status;
		uint08 reason;
		mtype = (param0 & 0xFF00) >> 8;
		msgID = (param0 & 0xFF0000) >> 16;
		status = param1 & 0xFF;
		reason = (param1 & 0xFF00) >> 8;
		return tlkcpc1_send_rsp(mtype, msgID, status, reason, pData, dataLen);
	}else if(ptype == TLKPRT_COMM_PTYPE_DAT){
		uint08 datID;
		uint32 number;
		datID = (param0 & 0xFF00) >> 8;
		number = (param1 & 0x0FFFFF);
		return tlkcpc1_send_dat(datID, number, pData, dataLen);
	}else{
		return -TLK_ENOSUPPORT;
	}
}
static int tlkcpc1_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKCPC_OPCODE_REG_SENDCB){
		return tlkcpc1_recvRegSendCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_REG_REG_RECEIVE_CB){
		return tlkcpc1_recvRegRecvCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_SET_RECV_BUFFER){
		return tlkcpc1_recvSetRecvBufferCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_SET_SEND_BUFFER){
		return tlkcpc1_recvSetSendBufferDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_GET_BUFFER_SIZE){
		return tlkcpc1_recvGetBufferSizeDeal();
	}else if(opcode == TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE){
		return tlkcpc1_recvGetBufferSizeDeal();
	}else{
		return -TLK_ENOSUPPORT;
	}
}

static int tlkcpc1_recvRegSendCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc1_send_regCB((TlkCpcSendCB)param0);
	return TLK_ENONE;
}
static int tlkcpc1_recvRegRecvCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc1_recv_regCB((TlkCpcRecvCB)param0);
	return TLK_ENONE;
}
static int tlkcpc1_recvSetRecvBufferCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc1_recv_setBuffer((uint08*)param0, param1);
	return TLK_ENONE;
}
static int tlkcpc1_recvSetSendBufferDeal(uint32 param0, uint32 param1)
{
	tlkcpc1_send_setBuffer((uint08*)param0, param1);
	return TLK_ENONE;
}

static int tlkcpc1_recvGetBufferSizeDeal(void)
{
	if(spTlkCpc1Ctrl->sendCtrl.buffLen < 13) return 0;
	return (spTlkCpc1Ctrl->sendCtrl.buffLen-13);
}



#endif //#if (TLK_CFG_COMM_ENABLE)


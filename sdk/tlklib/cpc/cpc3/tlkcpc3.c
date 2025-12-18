/********************************************************************************************************
 * @file	tlkcpc3.c
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
#include "tlkcpc3.h"
#include "tlkcpc3_recv.h"
#include "tlkcpc3_send.h"


static int tlkcpc3_init(uint08 procID);
static void tlkcpc3_deinit(void);
static void tlkcpc3_reset(void);
static int tlkcpc3_recvProc(uint08 *pData, uint16 dataLen);
static int tlkcpc3_sendProc(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen);
static int tlkcpc3_handler(uint16 opcode, uint32 param0, uint32 param1);
static int tlkcpc3_recvRegSendCbDeal(uint32 param0, uint32 param1);
static int tlkcpc3_recvRegRecvCbDeal(uint32 param0, uint32 param1);
static int tlkcpc3_recvSetRecvBufferDeal(uint32 param0, uint32 param1);
static int tlkcpc3_recvSetSendBufferDeal(uint32 param0, uint32 param1);
static int tlkcpc3_recvGetBufferSizeDeal(void);



const tlkcpc_infs_t gcTlkCpc3Infs = {
	tlkcpc3_init, //Init
	tlkcpc3_deinit, //Deinit
	tlkcpc3_reset, //Reset
	nullptr, //IdleNumb
	tlkcpc3_recvProc, //RecvProc
	tlkcpc3_sendProc, //SendProc
	tlkcpc3_handler, //Handler
	nullptr, //Process
};
static tlkcpc3_ctrl_t *spTlkCpc3Ctrl;


static int tlkcpc3_init(uint08 procID)
{
	spTlkCpc3Ctrl = (tlkcpc3_ctrl_t*)tlkapi_malloc(sizeof(tlkcpc3_ctrl_t));
	if(spTlkCpc3Ctrl == nullptr) return -TLK_ENOMEM;

	tmemset(spTlkCpc3Ctrl, 0, sizeof(tlkcpc3_ctrl_t));
	tlkcpc3_recv_init(&spTlkCpc3Ctrl->recvCtrl);
	tlkcpc3_send_init(&spTlkCpc3Ctrl->sendCtrl);
	
	return TLK_ENONE;
}
static void tlkcpc3_deinit(void)
{
	if(spTlkCpc3Ctrl != nullptr){
		tlkapi_free(spTlkCpc3Ctrl);
		spTlkCpc3Ctrl = nullptr;
	}
}

static void tlkcpc3_reset(void)
{
	tlkcpc3_recv_reset();
}
static int tlkcpc3_recvProc(uint08 *pData, uint16 dataLen)
{
	tlkcpc3_recv_handler(pData, dataLen);
	return TLK_ENONE;
}
static int tlkcpc3_sendProc(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen)
{
	uint08 ptype;

	ptype = param0 & 0xFF;
	if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT){
		uint08 mtype;
		uint16 msgID;
		mtype = (param0 & 0xFF00) >> 8;
		msgID = (param0 & 0x0FFF0000) >> 16;
		if(ptype == TLKPRT_COMM_PTYPE_CMD){
			return tlkcpc3_send_cmd(mtype, msgID, pData, dataLen);
		}else{
			return tlkcpc3_send_evt(mtype, msgID, pData, dataLen);
		}
	}else if(ptype == TLKPRT_COMM_PTYPE_RSP){
		uint08 mtype;
		uint16 msgID;
		uint08 status;
		uint08 reason;
		mtype = (param0 & 0xFF00) >> 8;
		msgID = (param0 & 0x0FFF0000) >> 16;
		status = param1 & 0xFF;
		reason = (param1 & 0xFF00) >> 8;
		return tlkcpc3_send_rsp(mtype, msgID, status, reason, pData, dataLen);
	}else if(ptype == TLKPRT_COMM_PTYPE_DAT){
		uint08 datID;
		uint32 number;
		datID = (param0 & 0xFF00) >> 8;
		number = (param1 & 0x0FFFFF);
		return tlkcpc3_send_dat(datID, number, pData, dataLen);
	}else{
		return -TLK_ENOSUPPORT;
	}
}
static int tlkcpc3_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKCPC_OPCODE_REG_SENDCB){
		return tlkcpc3_recvRegSendCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_REG_REG_RECEIVE_CB){
		return tlkcpc3_recvRegRecvCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_SET_RECV_BUFFER){
		return tlkcpc3_recvSetRecvBufferDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_SET_SEND_BUFFER){
		return tlkcpc3_recvSetSendBufferDeal(param0, param1);
	}else if(TLKCPC_OPCODE_GET_BUFFER_SIZE){
		return tlkcpc3_recvGetBufferSizeDeal();
	}else if(TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE){
		return tlkcpc3_recvGetBufferSizeDeal();
	}else{
		return -TLK_ENOSUPPORT;
	}
}

static int tlkcpc3_recvRegSendCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc3_send_regCB((TlkCpcSendCB)param0);
	return TLK_ENONE;
}
static int tlkcpc3_recvRegRecvCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc3_recv_regCB((TlkCpcRecvCB)param0);
	return TLK_ENONE;
}
static int tlkcpc3_recvSetRecvBufferDeal(uint32 param0, uint32 param1)
{
	tlkcpc3_recv_setBuffer((uint08*)param0, param1);
	return TLK_ENONE;
}
static int tlkcpc3_recvSetSendBufferDeal(uint32 param0, uint32 param1)
{
	tlkcpc3_send_setBuffer((uint08*)param0, param1);
	return TLK_ENONE;
}

static int tlkcpc3_recvGetBufferSizeDeal(void)
{
	if(spTlkCpc3Ctrl->sendCtrl.buffLens < 15) return 0;
	return (spTlkCpc3Ctrl->sendCtrl.buffLens-11-4);
}


#endif //#if (TLK_CFG_COMM_ENABLE)


/********************************************************************************************************
 * @file	tlkmdi_ats.c
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
#include "tlksys/prt/tlkpto_comm.h"
#include "../tlkcpc.h"
#include "tlkcpc2.h"
#include "tlkcpc2_ctrl.h"
#include "tlkcpc2_pool.h"
#include "tlkcpc2_recv.h"
#include "tlkcpc2_send.h"

extern void tlkcpc2_recv_regCB(TlkCpcRecvCB recvCB);
extern void tlkcpc2_send_regCB(TlkCpcSendCB sendCB);

static int tlkcpc2_init(uint08 procID);
static void tlkcpc2_deinit(void);
static void tlkcpc2_reset(void);
static int tlkcpc2_idleNumb(void);
static int tlkcpc2_recvProc(uint08 *pData, uint16 dataLen);
static int tlkcpc2_sendProc(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen);
static int tlkcpc2_handler(uint16 opcode, uint32 param0, uint32 param1);
static void tlkcpc2_process(void);
static int tlkcpc2_recvRegSendCbDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvRegRecvCbDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvRegResetRequestCbDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvRegResetCompleteCbDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvSetRecvBufferDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvSetSendBufferDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvEnableAuthenDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvSetAuthenStatusDeal(uint32 param0, uint32 param1);
static int tlkcpc2_recvGetBufferSizeDeal(void);
static int tlkcpc2_recvGetDatBufferSizeDeal(void);


const tlkcpc_infs_t gcTlkCpc2Infs = {
	tlkcpc2_init, //Init
	tlkcpc2_deinit, //Deinit
	tlkcpc2_reset, //Reset
	tlkcpc2_idleNumb, //IdleNumb
	tlkcpc2_recvProc, //RecvProc
	tlkcpc2_sendProc, //SendProc
	tlkcpc2_handler, //Handler
	tlkcpc2_process, //Process
};
tlkcpc2_ctrl_t *spTlkCpc2Ctrl = nullptr;


static int tlkcpc2_init(uint08 procID)
{
	spTlkCpc2Ctrl = (tlkcpc2_ctrl_t*)tlkapi_malloc(sizeof(tlkcpc2_ctrl_t));
	if(spTlkCpc2Ctrl == nullptr) return -TLK_ENOMEM;

	tmemset(spTlkCpc2Ctrl, 0, sizeof(tlkcpc2_ctrl_t));
	tlkcpc2_ctrl_init();
	
	return TLK_ENONE;
}
static void tlkcpc2_deinit(void)
{
	if(spTlkCpc2Ctrl != nullptr){
		tlkapi_free(spTlkCpc2Ctrl);
		spTlkCpc2Ctrl = nullptr;
	}
}
static void tlkcpc2_reset(void)
{
	tlkcpc2_pool_reset(true);
	tlkcpc2_ctrl_reset();
}

static int tlkcpc2_idleNumb(void)
{
	return tlkcpc2_pool_getIdleNumb();	
}
static int tlkcpc2_recvProc(uint08 *pData, uint16 dataLen)
{
	tlkcpc2_recv_handler(pData, dataLen);
	return TLK_ENONE;
}
static int tlkcpc2_sendProc(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen)
{
	uint08 ptype;

	ptype = param0 & 0xFF;
	if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT){
		uint08 enFlow;
		uint08 mtype;
		uint16 msgID;
		mtype = (param0 & 0xFF00) >> 8;
		msgID = (param0 & 0x0FFF0000) >> 16;
		if((param0 & 0x80000000) == 0) enFlow = true;
		else enFlow = false;
		if(ptype == TLKPRT_COMM_PTYPE_CMD){
			return tlkcpc2_sendCmdPacket(mtype, msgID, pData, dataLen, enFlow);
		}else{
			return tlkcpc2_sendEvtPacket(mtype, msgID, pData, dataLen, enFlow);
		}
	}else if(ptype == TLKPRT_COMM_PTYPE_RSP){
		uint08 enFlow;
		uint08 mtype;
		uint16 msgID;
		uint08 status;
		uint08 reason;
		if((param0 & 0x80000000) == 0) enFlow = true;
		else enFlow = false;
		mtype = (param0 & 0xFF00) >> 8;
		msgID = (param0 & 0x0FFF0000) >> 16;
		status = param1 & 0xFF;
		reason = (param1 & 0xFF00) >> 8;
		return tlkcpc2_sendRspPacket(mtype, msgID, status, reason, pData, dataLen, enFlow);
	}else if(ptype == TLKPRT_COMM_PTYPE_DAT){
		uint08 datID;
		uint32 number;
		datID = (param0 & 0xFF00) >> 8;
		number = (param1 & 0x0FFFFF);
		return tlkcpc2_sendDatPacket(datID, number, pData, dataLen);
	}else{
		return -TLK_ENOSUPPORT;
	}
}
static int tlkcpc2_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKCPC_OPCODE_REG_SENDCB){
		return tlkcpc2_recvRegSendCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_REG_REG_RECEIVE_CB){
		return tlkcpc2_recvRegRecvCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_REG_REG_RESET_REQUEST_CB){
		return tlkcpc2_recvRegResetRequestCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_REG_REG_RESET_COMPLETE_CB){
		return tlkcpc2_recvRegResetCompleteCbDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_SET_RECV_BUFFER){
		return tlkcpc2_recvSetRecvBufferDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_SET_SEND_BUFFER){
		return tlkcpc2_recvSetSendBufferDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_ENABLE_AUTHEN){
		return tlkcpc2_recvEnableAuthenDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_SET_AUTH_STATUE){
		return tlkcpc2_recvSetAuthenStatusDeal(param0, param1);
	}else if(opcode == TLKCPC_OPCODE_GET_BUFFER_SIZE){
		return tlkcpc2_recvGetBufferSizeDeal();
	}else if(opcode == TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE){
		return tlkcpc2_recvGetDatBufferSizeDeal();
	}else{
		return -TLK_ENOSUPPORT;
	}
}
static void tlkcpc2_process(void)
{
	tlkcpc2_ctrl_handler();
	tlkcpc2_send_handler();
}

static int tlkcpc2_recvRegSendCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_send_regCB((TlkCpcSendCB)param0);
	return TLK_ENONE;
}
static int tlkcpc2_recvRegRecvCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_recv_regCB((TlkCpcRecvCB)param0);
	return TLK_ENONE;
}
static int tlkcpc2_recvRegResetRequestCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_regResetRequestCB((TlkCpcResetCB)param0);
	return TLK_ENONE;
}
static int tlkcpc2_recvRegResetCompleteCbDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_regResetCompleteCB((TlkCpcResetCB)param0);
	return TLK_ENONE;
}
static int tlkcpc2_recvSetRecvBufferDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_recv_setBuffer((uint08*)param0, param1);
	return TLK_ENONE;
}
static int tlkcpc2_recvSetSendBufferDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_send_setBuffer((uint08*)param0, param1);
	return TLK_ENONE;
}
static int tlkcpc2_recvEnableAuthenDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_enable_authen(param0);
	return TLK_ENONE;
}
static int tlkcpc2_recvSetAuthenStatusDeal(uint32 param0, uint32 param1)
{
	tlkcpc2_set_authenStatus(param0);
	return TLK_ENONE;
}

static int tlkcpc2_recvGetBufferSizeDeal(void)
{
	if(TLKCPC2_SEND_BUFF_SIZE < 15) return 0;
	return (TLKCPC2_SEND_BUFF_SIZE-11-4);
}static int tlkcpc2_recvGetDatBufferSizeDeal(void)
{
	if(spTlkCpc2Ctrl->sendCtrl.buffLen < 15) return 0;
	return (spTlkCpc2Ctrl->sendCtrl.buffLen-11-4);
}




#endif //#if (TLK_TEST_ATS_ENABLE)


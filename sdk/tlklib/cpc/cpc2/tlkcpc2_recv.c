/********************************************************************************************************
 * @file	tlkmdi_atsRecv.c
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
#include "tlkalg/digest/crc/tlkalg_crc.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "../tlkcpc.h"
#include "tlkcpc2.h"
#include "tlkcpc2_ctrl.h"
#include "tlkcpc2_pool.h"
#include "tlkcpc2_recv.h"
#include "tlkcpc2_send.h"
#include "drivers.h"


static void tlkcpc2_recvResetStreamDeal(uint08 rstCode);
static void tlkcpc2_recv_fcsPacketDeal(uint08 *pData, uint16 dataLen);
static void tlkcpc2_recv_othPacketDeal(bool isHasExt, uint08 ptype, uint08 *pData, uint16 dataLen);


extern tlkcpc2_ctrl_t *spTlkCpc2Ctrl;


int tlkcpc2_recv_init(void)
{
	if(spTlkCpc2Ctrl == nullptr) return -TLK_EPARAM;
	
	spTlkCpc2Ctrl->recvCtrl.pBuffer = spTlkCpc2Ctrl->recvCtrl.recvBuff;
	spTlkCpc2Ctrl->recvCtrl.buffLen = TLKCPC2_RECV_CACHE_SIZE;
	
	return TLK_ENONE;
}

void tlkcpc2_recv_regCB(TlkCpcRecvCB recvCB)
{
	if(spTlkCpc2Ctrl == nullptr) return;
	spTlkCpc2Ctrl->recvCtrl.recvCB = recvCB;
}

void tlkcpc2_recv_setBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(spTlkCpc2Ctrl == nullptr) return;
	
	if(pBuffer == nullptr || buffLen < TLKCPC2_RECV_CACHE_SIZE){
		spTlkCpc2Ctrl->recvCtrl.pBuffer = spTlkCpc2Ctrl->recvCtrl.recvBuff;
		spTlkCpc2Ctrl->recvCtrl.buffLen = TLKCPC2_RECV_CACHE_SIZE;
	}else{
		spTlkCpc2Ctrl->recvCtrl.pBuffer = pBuffer;
		spTlkCpc2Ctrl->recvCtrl.buffLen = buffLen;
	}
}

void tlkcpc2_regResetRequestCB(TlkCpcResetCB resetCB)
{
	if(spTlkCpc2Ctrl == nullptr) return;
	spTlkCpc2Ctrl->recvCtrl.resetRequestCB = resetCB;
}
void tlkcpc2_regResetCompleteCB(TlkCpcResetCB resetCB)
{
	if(spTlkCpc2Ctrl == nullptr) return;
	spTlkCpc2Ctrl->recvCtrl.resetCompleteCB = resetCB;
}


void tlkcpc2_recv_reset(void)
{
	spTlkCpc2Ctrl->recvCtrl.mstate = TLKCPC2_RECV_MSTATE_HEAD;
	spTlkCpc2Ctrl->recvCtrl.recvLens = 0;
}

void tlkcpc2_recv_handler(uint08 *pData, uint16 dataLen)
{
	int ret;
	int index;
	uint16 tempVar0;
	uint16 tempVar1;

	if(spTlkCpc2Ctrl == nullptr || spTlkCpc2Ctrl->recvCtrl.recvCB == nullptr){
		return;
	} 
	
//	tlkapi_array(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recv_handler 01: ", pData, dataLen);
	for(index=0; index<dataLen; index++){
		if(pData[index] == TLKCPC2_FRAME_HEAD_SIGN){
			spTlkCpc2Ctrl->recvCtrl.mstate = TLKCPC2_RECV_MSTATE_BODY;
			spTlkCpc2Ctrl->recvCtrl.recvLens = 0;
		}else if(pData[index] == TLKCPC2_FRAME_TAIL_SIGN){
			if(spTlkCpc2Ctrl->recvCtrl.mstate == TLKCPC2_RECV_MSTATE_BODY && spTlkCpc2Ctrl->recvCtrl.recvLens >= 6){
				spTlkCpc2Ctrl->recvCtrl.mstate = TLKCPC2_RECV_MSTATE_READY;
			}else{
				spTlkCpc2Ctrl->recvCtrl.mstate = TLKCPC2_RECV_MSTATE_BODY;
				spTlkCpc2Ctrl->recvCtrl.recvLens = 0;
			}
		}else if(spTlkCpc2Ctrl->recvCtrl.mstate == TLKCPC2_RECV_MSTATE_BODY){
			if(spTlkCpc2Ctrl->recvCtrl.recvLens < spTlkCpc2Ctrl->recvCtrl.buffLen){
				spTlkCpc2Ctrl->recvCtrl.pBuffer[spTlkCpc2Ctrl->recvCtrl.recvLens++] = pData[index];
			}else{
				spTlkCpc2Ctrl->recvCtrl.mstate = TLKCPC2_RECV_MSTATE_HEAD;
				spTlkCpc2Ctrl->recvCtrl.recvLens = 0;
			}
		}
		if(spTlkCpc2Ctrl->recvCtrl.mstate != TLKCPC2_RECV_MSTATE_READY) continue;
		//Check whether escaping is required 
		spTlkCpc2Ctrl->recvCtrl.mstate = TLKCPC2_RECV_MSTATE_HEAD;
		ret = tlkcpc2_packet_revise(spTlkCpc2Ctrl->recvCtrl.pBuffer, spTlkCpc2Ctrl->recvCtrl.recvLens);
		if(ret < 0) continue;
		spTlkCpc2Ctrl->recvCtrl.recvLens = ret;
		
		//Check CRC is correct
		tempVar0 = spTlkCpc2Ctrl->recvCtrl.pBuffer[spTlkCpc2Ctrl->recvCtrl.recvLens-1];
		tempVar0 <<= 8;
		tempVar0 |= spTlkCpc2Ctrl->recvCtrl.pBuffer[spTlkCpc2Ctrl->recvCtrl.recvLens-2];
		tempVar1 = tlkalg_crc16_calc(spTlkCpc2Ctrl->recvCtrl.pBuffer, spTlkCpc2Ctrl->recvCtrl.recvLens-2);
		if(tempVar0 != tempVar1){
//			tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recv_handler 05: CRC Error 0x%x 0x%x ", 
//				tempVar0, tempVar1);
			continue;
		}
		//
		tempVar0 = (spTlkCpc2Ctrl->recvCtrl.pBuffer[1] & 0xF0) >> 4;
		tempVar1 = ((spTlkCpc2Ctrl->recvCtrl.pBuffer[1] & 0x07) << 8) | spTlkCpc2Ctrl->recvCtrl.pBuffer[0];
		if(tempVar1 < 4 || tempVar1 != spTlkCpc2Ctrl->recvCtrl.recvLens) continue;
		if(tempVar0 == TLKPRT_COMM_PTYPE_FCS){
			tlkcpc2_recv_fcsPacketDeal(spTlkCpc2Ctrl->recvCtrl.pBuffer+2, tempVar1-4);
		}else{
			if((spTlkCpc2Ctrl->recvCtrl.pBuffer[1] & 0x08) != 0){
				tlkcpc2_recv_othPacketDeal(true, tempVar0, spTlkCpc2Ctrl->recvCtrl.pBuffer+2, tempVar1-4);
			}else if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_DONE){
				tlkcpc2_recv_othPacketDeal(false, tempVar0, spTlkCpc2Ctrl->recvCtrl.pBuffer+2, tempVar1-4);
			}else if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_NONE){
				tlkcpc2_sendResetReqPacket();
			}
		}
	}
}

static void tlkcpc2_recv_fcsPacketDeal(uint08 *pData, uint16 dataLen)
{
	bool isStall;
	uint16 fcsFlag;
	uint08 rstCode;
	uint08 recvNumb;
	uint08 sendNumb;
	
//	tlkapi_array(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recvDataCB: ", pData, dataLen);
	if(dataLen < 4){
		tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recvDataCB: error length!");
		return;
	}
	
	fcsFlag = ((uint16)pData[1] << 8) | pData[0];
	recvNumb = pData[2];
	sendNumb = pData[3];

	rstCode = (fcsFlag & 0x00F0) >> 4;
	if(rstCode != 0){
		tlkcpc2_recvResetStreamDeal(rstCode);
		return;
	}
	if(spTlkCpc2Ctrl->resetStat != TLKCPC2_RESET_STATUS_DONE){
		if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_NONE){
			tlkcpc2_sendResetReqPacket();
		}
		return;
	}
	
	if(tlkcpc2_pool_removeCacheItem((recvNumb - 1) & 0xFF)){
		if(tlkcpc2_pool_getCacheNumb() == 0) spTlkCpc2Ctrl->syncTimer = 0;
		else spTlkCpc2Ctrl->syncTimer = clock_time()|1;
	}
	if((fcsFlag & TLKCPC2_FCS_FLAG_R) != 0){
		tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "TLKCPC2_FCS_FLAG_R: recvNumb[%d]", recvNumb);
		if(!tlkcpc2_pool_activeCacheItem(recvNumb) && !tlkcpc2_pool_isInSendList(recvNumb)){
			tlkcpc2_abort_handler(TLKCPC2_ABORT_EVTID_RESTORE);
			return;
		}
	}
	if(spTlkCpc2Ctrl->recvNumb != sendNumb){
		isStall = true;
	}else{
		isStall = false;
		spTlkCpc2Ctrl->stallTimer = 0;
	}
	if((fcsFlag & TLKCPC2_FCS_FLAG_P) != 0 || (isStall && spTlkCpc2Ctrl->stallTimer == 0)){
		//The peer end needs to respond to a handshake request in a timely manner
		uint08 bitR = 0;
		if(isStall){
			bitR = 1;
			spTlkCpc2Ctrl->stallTimer = clock_time() | 1;
			spTlkCpc2Ctrl->pollTimer = clock_time() | 1;
		}
		tlkcpc2_sendFcsPacket(0, 1, bitR, spTlkCpc2Ctrl->recvNumb, spTlkCpc2Ctrl->sendNumb);
	}
	if(tlkcpc2_pool_getSendNumb() != 0)
	{
		tlkcpc2_send_handler();
	}
}

static void tlkcpc2_recv_othPacketDeal(bool isHasExt, uint08 ptype, uint08 *pData, uint16 dataLen)
{
	if(isHasExt)
	{
		uint08 recvNumb = pData[0];
		uint08 sendNumb = pData[1];
		if(dataLen < 2){
			tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN,
				"tlkcpc2_recv_othPacketDeal: format error for extended attribute");
			return;
		}
		if(tlkcpc2_pool_removeCacheItem((recvNumb - 1) & 0xFF)){
			if(tlkcpc2_pool_getCacheNumb() == 0) spTlkCpc2Ctrl->syncTimer = 0;
			else spTlkCpc2Ctrl->syncTimer = clock_time()|1;
		}
		if(sendNumb != spTlkCpc2Ctrl->recvNumb){
			if(spTlkCpc2Ctrl->stallTimer == 0){
				spTlkCpc2Ctrl->stallTimer = clock_time() | 1;
				tlkcpc2_sendFcsPacket(0, 0, 1, spTlkCpc2Ctrl->recvNumb, spTlkCpc2Ctrl->sendNumb);
			}
			spTlkCpc2Ctrl->pollTimer = clock_time()|1;
			return;
		}
		
		spTlkCpc2Ctrl->recvNumb = (sendNumb + 1) & 0xFF;
		spTlkCpc2Ctrl->stallTimer = 0;
		pData += 2;
		dataLen -= 2;
	}
	if(spTlkCpc2Ctrl->authState == TLKCPC2_AUTH_STATUS_FAILURE){
		return;
	}
	
	if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT || ptype == TLKPRT_COMM_PTYPE_RSP)
	{
		uint08 mtype;
		uint16 msgID;
		uint16 length;
		
		mtype = pData[0];
		msgID = ((uint16)(pData[3] & 0xF0) << 4) | pData[1];
		length = ((uint16)(pData[3] & 0x0F) << 8) | pData[2];
		if((ptype == TLKPRT_COMM_PTYPE_RSP && length < 2) || 4+length > dataLen){
			tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN,
				"tlkcpc2_recv_othPacketDeal Length Error: frmLen-%d lens-%d",
				dataLen, length);
			return;
		}
		if(mtype == 0) mtype = TLKPRT_COMM_MTYPE_AUTHEN;
		if(spTlkCpc2Ctrl->authState == TLKCPC2_AUTH_STATUS_RUNNING && mtype != TLKPRT_COMM_MTYPE_AUTHEN){
			return;
		}
		
		if(ptype == TLKPRT_COMM_PTYPE_RSP){
			spTlkCpc2Ctrl->recvCtrl.recvCB(((uint32)msgID<<16)|((uint32)mtype<<8)|ptype, ((uint16)pData[5]<<8)|pData[4], pData+6, length-2);
		}else{
			spTlkCpc2Ctrl->recvCtrl.recvCB(((uint32)msgID<<16)|((uint32)mtype<<8)|ptype, 0, pData+4, length);
		}
	}
	else if(ptype == TLKPRT_COMM_PTYPE_DAT)
	{
		uint32 numb;
		uint16 lens;
		uint08 datID;

		if(spTlkCpc2Ctrl->authState != TLKCPC2_AUTH_STATUS_NONE && spTlkCpc2Ctrl->authState != TLKCPC2_AUTH_STATUS_SUCCESS
			&& ptype == TLKPRT_COMM_PTYPE_CMD){
			return;
		}
		datID = pData[0];
		numb  = (((uint32)pData[2]) << 8) | pData[1] | (((uint32)pData[4] & 0xF0) << 12);
		lens  = (((uint16)pData[4] & 0x0F) << 8) | pData[3];
		if(5+lens > dataLen){
			tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recv_othPacketDeal Length Error: frmLen-%d lens-%d",
				dataLen, lens);
			return;
		}
		spTlkCpc2Ctrl->recvCtrl.recvCB(((uint32)datID<<8)|ptype, numb, pData+5, lens);
	}
}



static void tlkcpc2_recvResetStreamDeal(uint08 rstCode)
{
	if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_NONE || spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_DONE){
		if(rstCode == TLKCPC2_RST_CODE_REQ){
			tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recvResetStreamDeal: TLKCPC2_RESET_STATUS_NONE");
			tlkcpc2_ctrl_reset();
			if(spTlkCpc2Ctrl->recvCtrl.resetRequestCB != nullptr){
				spTlkCpc2Ctrl->recvCtrl.resetRequestCB();
			}
			tlkcpc2_sendResetAckPacket();
			spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_WAIT_CFM;
		}
	}else if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_WAIT_REQ){
		if(rstCode == TLKCPC2_RST_CODE_REQ){
			tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recvResetStreamDeal: TLKCPC2_RESET_STATUS_WAIT_REQ");
			tlkcpc2_sendResetAckPacket();
			spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_WAIT_CFM;
		}
	}else if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_WAIT_CFM || spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_DONE){
		if(rstCode == TLKCPC2_RST_CODE_CFM){
			uint08 status = spTlkCpc2Ctrl->resetStat;
			tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_recvResetStreamDeal: TLKCPC2_RESET_STATUS_WAIT_CFM");
			
			tlkcpc2_sendResetDonePacket();
			
			spTlkCpc2Ctrl->resetTimer = 0;
			spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_DONE;
			if(spTlkCpc2Ctrl->authState != TLKCPC2_AUTH_STATUS_NONE){
				spTlkCpc2Ctrl->authState = TLKCPC2_AUTH_STATUS_RUNNING;
				spTlkCpc2Ctrl->authTimer = clock_time() | 1;
			}else{
				tlkcpc2_pool_activeCopyItem();
			}
			if(status == TLKCPC2_RESET_STATUS_WAIT_CFM && spTlkCpc2Ctrl->recvCtrl.resetCompleteCB != nullptr){
				spTlkCpc2Ctrl->recvCtrl.resetCompleteCB();
			}
		}
	}
}



#endif //#if (TLK_TEST_ATS_ENABLE)


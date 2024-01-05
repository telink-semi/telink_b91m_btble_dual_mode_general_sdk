/********************************************************************************************************
 * @file	tlkcpc1_recv.c
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
#include "tlkalg/digest/crc/tlkalg_crc.h"
#include "../tlkcpc.h"
#include "tlkcpc1.h"
#include "tlkcpc1_recv.h"
#include "drivers.h"


static void tlkcpc1_recv_makeFrame(uint08 rbyte);

static tlkcpc1_recv_ctrl_t *spTlkCpc1RecvCtrl;


int tlkcpc1_recv_init(tlkcpc1_recv_ctrl_t *pCtrl)
{
	spTlkCpc1RecvCtrl = pCtrl;
	if(pCtrl == nullptr) return -TLK_EPARAM;
	
	spTlkCpc1RecvCtrl->pBuffer = nullptr;
	spTlkCpc1RecvCtrl->buffLen = 0;
	return TLK_ENONE;
}

void tlkcpc1_recv_regCB(TlkCpcRecvCB recvCB)
{
	if(spTlkCpc1RecvCtrl == nullptr) return;
	spTlkCpc1RecvCtrl->recvCB = recvCB;
}

void tlkcpc1_recv_reset(void)
{
	if(spTlkCpc1RecvCtrl == nullptr) return;
	spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_HEAD;
	spTlkCpc1RecvCtrl->recvLens = 0;
	spTlkCpc1RecvCtrl->busyTimer = 0;
}
void tlkcpc1_recv_setBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(spTlkCpc1RecvCtrl == nullptr) return;
	spTlkCpc1RecvCtrl->pBuffer = pBuffer;
	spTlkCpc1RecvCtrl->buffLen = buffLen;
}

void tlkcpc1_recv_handler(uint08 *pData, uint16 dataLen)
{
	uint08 mtype;
	uint08 ptype;
	uint16 index;
	uint16 rawCrc;
	uint16 calCrc;

//	tlkapi_array(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, "=== recv", pData, dataLen);

	if(spTlkCpc1RecvCtrl == nullptr || spTlkCpc1RecvCtrl->recvCB == nullptr){
		return;
	} 
	for(index=0; index<dataLen; index++){
		tlkcpc1_recv_makeFrame(pData[index]);
		if(spTlkCpc1RecvCtrl->makeState != TLKCPC1_RECV_MSTATE_READY) continue;

//		spTlkCpc1RecvCtrl->makeLens = 0;
//		spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_HEAD;
		rawCrc = ((uint16)(spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens-3])<<8)
			| (spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens-4]);
		calCrc = tlkalg_crc16_calc(spTlkCpc1RecvCtrl->pBuffer+2, spTlkCpc1RecvCtrl->recvLens-6);
		if(rawCrc != calCrc){
			tlkapi_error(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, "Serial CRC Error: rawCrc[%x] - calCrc[%x]", rawCrc, calCrc);
			continue;
		}
		ptype = (spTlkCpc1RecvCtrl->pBuffer[3] & 0xF0) >> 4;
		
		if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT){
			uint08 numb;
			uint08 lens;
			uint08 msgID;
			mtype = spTlkCpc1RecvCtrl->pBuffer[4];
			msgID = spTlkCpc1RecvCtrl->pBuffer[5];
			numb  = spTlkCpc1RecvCtrl->pBuffer[6];
			lens  = spTlkCpc1RecvCtrl->pBuffer[7];
			if(spTlkCpc1RecvCtrl->recvLens < 4+TLKCPC1_FRM_EXTLEN || spTlkCpc1RecvCtrl->recvLens < 4+lens+TLKCPC1_FRM_EXTLEN){
				tlkapi_error(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, 
					"Recv CmdOrRspOrEvtPkt Length Error: frmLen-%d lens-%d",
					spTlkCpc1RecvCtrl->recvLens, lens);
				return;
			}
			spTlkCpc1RecvCtrl->recvCB(((uint32)msgID<<16)|((uint32)mtype<<8)|ptype, 0, spTlkCpc1RecvCtrl->pBuffer+8, lens);
			spTlkCpc1RecvCtrl->recvNumb = numb;
		}
		else if(ptype == TLKPRT_COMM_PTYPE_DAT){
			uint32 numb;
			uint16 lens;
			uint08 datID;
			datID = spTlkCpc1RecvCtrl->pBuffer[4];
			numb  = (((uint32)spTlkCpc1RecvCtrl->pBuffer[6]) << 8) | spTlkCpc1RecvCtrl->pBuffer[5]
				| (((uint32)spTlkCpc1RecvCtrl->pBuffer[8] & 0xF0) << 12);
			lens  = (((uint16)spTlkCpc1RecvCtrl->pBuffer[8] & 0x0F) << 8) | spTlkCpc1RecvCtrl->pBuffer[7];
			if(spTlkCpc1RecvCtrl->recvLens < 4+TLKCPC1_FRM_EXTLEN || spTlkCpc1RecvCtrl->recvLens < 4+lens+TLKCPC1_FRM_EXTLEN){
				tlkapi_error(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, "Recv DatPkt Length Error: frmLen-%d lens-%d", spTlkCpc1RecvCtrl->recvLens, lens);
				return;
			}
			spTlkCpc1RecvCtrl->recvCB(((uint32)datID<<8)|ptype, numb, spTlkCpc1RecvCtrl->pBuffer+9, lens);
//			if(datID <= TLKMDI_COMM_DATA_CHANNEL_MAX && sTlkMdiCommDatCB[datID] != nullptr){
//				sTlkMdiCommDatCB[datID](datID, numb, spTlkCpc1RecvCtrl->recvFrame+9, lens);
//			}else{
//				tlkapi_error(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, "Recv DatPkt Unexpected: not used - datID[%d]", datID);
//			}
		}
		else{
			tlkapi_error(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, "Recv Error PktType: ptype-%d", ptype);
		}
		spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_HEAD;
		spTlkCpc1RecvCtrl->makeLens = 0;
	}	
}

static void tlkcpc1_recv_makeFrame(uint08 rbyte)
{
	if(spTlkCpc1RecvCtrl->makeState != TLKCPC1_RECV_MSTATE_BODY && spTlkCpc1RecvCtrl->busyTimer != 0){
		spTlkCpc1RecvCtrl->busyTimer = 0;
	}else if(spTlkCpc1RecvCtrl->makeState == TLKCPC1_RECV_MSTATE_BODY){
		if(spTlkCpc1RecvCtrl->busyTimer == 0){
			spTlkCpc1RecvCtrl->busyTimer = clock_time() | 1;
		}else if(spTlkCpc1RecvCtrl->busyTimer != 0 && clock_time_exceed(spTlkCpc1RecvCtrl->busyTimer, 500000)){
			spTlkCpc1RecvCtrl->busyTimer = 0;
			spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_HEAD;
			spTlkCpc1RecvCtrl->makeLens = 0;
		}
	}	
	if(spTlkCpc1RecvCtrl->makeState == TLKCPC1_RECV_MSTATE_READY){
		spTlkCpc1RecvCtrl->busyTimer = 0;
		spTlkCpc1RecvCtrl->makeLens = 0;
		spTlkCpc1RecvCtrl->makeState  = TLKCPC1_RECV_MSTATE_HEAD;
	}
	if(spTlkCpc1RecvCtrl->makeState == TLKCPC1_RECV_MSTATE_HEAD){
		if(spTlkCpc1RecvCtrl->makeLens == 0 && rbyte == TLKCPC1_HEAD_SIGN0){
			spTlkCpc1RecvCtrl->makeLens ++;
		}else if(spTlkCpc1RecvCtrl->makeLens == 1 && rbyte == TLKCPC1_HEAD_SIGN1){
			spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_ATTR;
			spTlkCpc1RecvCtrl->makeLens = 0;
			spTlkCpc1RecvCtrl->recvLens = 0;
			spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens++] = TLKCPC1_HEAD_SIGN0;
			spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens++] = TLKCPC1_HEAD_SIGN1;
		}else{
			spTlkCpc1RecvCtrl->makeLens = 0;
		}
	}else if(spTlkCpc1RecvCtrl->makeState == TLKCPC1_RECV_MSTATE_ATTR){
		spTlkCpc1RecvCtrl->makeLens ++;
		spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens++] = rbyte;
		if(spTlkCpc1RecvCtrl->makeLens < 2) return;
		spTlkCpc1RecvCtrl->makeLens = (((uint16)(spTlkCpc1RecvCtrl->pBuffer[3] & 0x0F))<<8)|spTlkCpc1RecvCtrl->pBuffer[2];
		if(spTlkCpc1RecvCtrl->makeLens+4 > spTlkCpc1RecvCtrl->buffLen || spTlkCpc1RecvCtrl->makeLens < 4){
			spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_HEAD;
			spTlkCpc1RecvCtrl->makeLens = 0;
		}else if(spTlkCpc1RecvCtrl->makeLens == 4){
			spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_CHECK;
			spTlkCpc1RecvCtrl->makeLens = 2;
		}else{
			spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_BODY;
			spTlkCpc1RecvCtrl->makeLens -= 4;
		}
	}else if(spTlkCpc1RecvCtrl->makeState == TLKCPC1_RECV_MSTATE_BODY){
		spTlkCpc1RecvCtrl->makeLens --;
		spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens++] = rbyte;
		if(spTlkCpc1RecvCtrl->makeLens != 0) return;
		spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_CHECK;
		spTlkCpc1RecvCtrl->makeLens = 0;
	}else if(spTlkCpc1RecvCtrl->makeState == TLKCPC1_RECV_MSTATE_CHECK){
		spTlkCpc1RecvCtrl->makeLens ++;
		spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens++] = rbyte;
		if(spTlkCpc1RecvCtrl->makeLens < 2) return;
		spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_TAIL;
		spTlkCpc1RecvCtrl->makeLens = 0;
	}else if(spTlkCpc1RecvCtrl->makeState == TLKCPC1_RECV_MSTATE_TAIL){
		if(spTlkCpc1RecvCtrl->makeLens == 0 && rbyte == TLKCPC1_TAIL_SIGN0){
			spTlkCpc1RecvCtrl->makeLens ++;
		}else if(spTlkCpc1RecvCtrl->makeLens == 1 && rbyte == TLKCPC1_TAIL_SIGN1){
			spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens++] = TLKCPC1_TAIL_SIGN0;
			spTlkCpc1RecvCtrl->pBuffer[spTlkCpc1RecvCtrl->recvLens++] = TLKCPC1_TAIL_SIGN1;
			spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_READY;
			spTlkCpc1RecvCtrl->makeLens = 0;
		}else{
			spTlkCpc1RecvCtrl->makeState = TLKCPC1_RECV_MSTATE_HEAD;
			spTlkCpc1RecvCtrl->makeLens = 0;
		}
	}
}





#endif //#if (TLK_CFG_COMM_ENABLE)


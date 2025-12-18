/********************************************************************************************************
 * @file	tlkcpc3_recv.c
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
#include "tlkcpc3.h"
#include "tlkcpc3_recv.h"
#include "drivers.h"


static void tlkcpc3_recv_normalPacketDeal(uint08 ptype, uint08 *pData, uint16 dataLen);

static tlkcpc3_recv_ctrl_t *spTlkCpc3RecvCtrl;


int tlkcpc3_recv_init(tlkcpc3_recv_ctrl_t *pCtrl)
{
	spTlkCpc3RecvCtrl = pCtrl;
	if(pCtrl == nullptr) return -TLK_EFAIL;
	
	spTlkCpc3RecvCtrl->pBuffer = nullptr;
	spTlkCpc3RecvCtrl->buffLen = 0;
	
	return TLK_ENONE;
}

void tlkcpc3_recv_regCB(TlkCpcRecvCB recvCB)
{
	if(spTlkCpc3RecvCtrl == nullptr) return;
	
	spTlkCpc3RecvCtrl->recvCB = recvCB;
}

void tlkcpc3_recv_reset(void)
{
	if(spTlkCpc3RecvCtrl == nullptr) return;
	spTlkCpc3RecvCtrl->mstate = TLKCPC3_RECV_MSTATE_HEAD;
	spTlkCpc3RecvCtrl->recvLens = 0;
	spTlkCpc3RecvCtrl->busyTimer = 0;
}
void tlkcpc3_recv_setBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(spTlkCpc3RecvCtrl == nullptr) return;
	spTlkCpc3RecvCtrl->pBuffer = pBuffer;
	spTlkCpc3RecvCtrl->buffLen = buffLen;
}

void tlkcpc3_recv_handler(uint08 *pData, uint16 dataLen)
{
	int indexI;
	int indexJ;
	uint16 tempVar0;
	uint16 tempVar1;

	if(spTlkCpc3RecvCtrl == nullptr || spTlkCpc3RecvCtrl->recvCB == nullptr) return;

//	tlkapi_array(TLKCPC3_DBG_FLAG, TLKCPC3_DBG_SIGN, "tlkcpc2_recv_handler 01: ", pData, dataLen);
	for(indexI=0; indexI<dataLen; indexI++){
		if(pData[indexI] == TLKCPC3_FRAME_HEAD_SIGN){
			spTlkCpc3RecvCtrl->mstate = TLKCPC3_RECV_MSTATE_BODY;
			spTlkCpc3RecvCtrl->recvLens = 0;
		}else if(pData[indexI] == TLKCPC3_FRAME_TAIL_SIGN){
			if(spTlkCpc3RecvCtrl->mstate == TLKCPC3_RECV_MSTATE_BODY && spTlkCpc3RecvCtrl->recvLens >= 6){
				spTlkCpc3RecvCtrl->mstate = TLKCPC3_RECV_MSTATE_READY;
			}else{
				spTlkCpc3RecvCtrl->mstate = TLKCPC3_RECV_MSTATE_BODY;
				spTlkCpc3RecvCtrl->recvLens = 0;
			}
		}else if(spTlkCpc3RecvCtrl->mstate == TLKCPC3_RECV_MSTATE_BODY){
			if(spTlkCpc3RecvCtrl->recvLens < spTlkCpc3RecvCtrl->buffLen){
				spTlkCpc3RecvCtrl->pBuffer[spTlkCpc3RecvCtrl->recvLens++] = pData[indexI];
			}else{
				spTlkCpc3RecvCtrl->mstate = TLKCPC3_RECV_MSTATE_HEAD;
				spTlkCpc3RecvCtrl->recvLens = 0;
			}
		}
		if(spTlkCpc3RecvCtrl->mstate != TLKCPC3_RECV_MSTATE_READY) continue;

		//Check whether escaping is required 
		spTlkCpc3RecvCtrl->mstate = TLKCPC3_RECV_MSTATE_HEAD;
		for(indexJ=0; indexJ<spTlkCpc3RecvCtrl->recvLens; indexJ++){
			if(spTlkCpc3RecvCtrl->pBuffer[indexJ] == TLKCPC3_ESCAPE_CHARS) break;
		}
		if(indexJ != spTlkCpc3RecvCtrl->recvLens){ //Escape the data
			int offset = 0;
			for(indexJ=0; indexJ<spTlkCpc3RecvCtrl->recvLens; indexJ++){
				if(spTlkCpc3RecvCtrl->pBuffer[indexJ] != TLKCPC3_ESCAPE_CHARS){
					spTlkCpc3RecvCtrl->pBuffer[offset++] = spTlkCpc3RecvCtrl->pBuffer[indexJ];
				}else{
					if(indexJ+1 >= spTlkCpc3RecvCtrl->recvLens) break;
					indexJ ++;
					if(spTlkCpc3RecvCtrl->pBuffer[indexJ] == TLKCPC3_ESCAPE_CHARS_RAW){
						spTlkCpc3RecvCtrl->pBuffer[offset++] = TLKCPC3_ESCAPE_CHARS;
					}else if(spTlkCpc3RecvCtrl->pBuffer[indexJ] == TLKCPC3_ESCAPE_CHARS_HEAD){
						spTlkCpc3RecvCtrl->pBuffer[offset++] = TLKCPC3_FRAME_HEAD_SIGN;
					}else if(spTlkCpc3RecvCtrl->pBuffer[indexJ] == TLKCPC3_ESCAPE_CHARS_TAIL){
						spTlkCpc3RecvCtrl->pBuffer[offset++] = TLKCPC3_FRAME_TAIL_SIGN;
					}else{
						break;
					}
				}
			}
			if(indexJ != spTlkCpc3RecvCtrl->recvLens){
				tlkapi_error(0xFFFFFFFF, TLKCPC3_DBG_SIGN, "tlkcpc2_recv_handler 07:  escaping failure");
				continue; //escaping failure
			} 
			spTlkCpc3RecvCtrl->recvLens = offset;
		}

		//Check CRC is correct
		tempVar0 = spTlkCpc3RecvCtrl->pBuffer[spTlkCpc3RecvCtrl->recvLens-1];
		tempVar0 <<= 8;
		tempVar0 |= spTlkCpc3RecvCtrl->pBuffer[spTlkCpc3RecvCtrl->recvLens-2];
		tempVar1 = tlkalg_crc16_calc(spTlkCpc3RecvCtrl->pBuffer, spTlkCpc3RecvCtrl->recvLens-2);
		if(tempVar0 != tempVar1){
//			tlkapi_error(TLKCPC3_DBG_FLAG, TLKCPC3_DBG_SIGN, "tlkcpc2_recv_handler 05: CRC Error 0x%x 0x%x ", 
//				tempVar0, tempVar1);
			tlkapi_error(0xFFFFFFFF, TLKCPC3_DBG_SIGN, "tlkcpc2_recv_handler 05: CRC Error 0x%x 0x%x ", 
				tempVar0, tempVar1);
			continue;
		}

		//
		tempVar0 = (spTlkCpc3RecvCtrl->pBuffer[1] & 0xF0) >> 4;
		tempVar1 = ((spTlkCpc3RecvCtrl->pBuffer[1] & 0x0F) << 8) | spTlkCpc3RecvCtrl->pBuffer[0];
		if(tempVar1 < 4 || tempVar1 != spTlkCpc3RecvCtrl->recvLens){
			continue;
		} 

		if(tempVar0 == TLKPRT_COMM_PTYPE_FCS) continue;
		
		if((spTlkCpc3RecvCtrl->pBuffer[1] & 0x08) == 0){
			tlkcpc3_recv_normalPacketDeal(tempVar0, spTlkCpc3RecvCtrl->pBuffer+2, tempVar1-4);
		}else if(tempVar1 >= 6){
			tlkcpc3_recv_normalPacketDeal(tempVar0, spTlkCpc3RecvCtrl->pBuffer+4, tempVar1-6);
		}
	}
}


static void tlkcpc3_recv_normalPacketDeal(uint08 ptype, uint08 *pData, uint16 dataLen)
{
	if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT){
//		uint08 numb;
		uint16 lens;
		uint16 msgID;
		uint08 mtype;
		mtype = pData[0];
		msgID = pData[1] | ((pData[3] & 0xF0) << 8);
		lens  = pData[2] | ((pData[3] & 0x0F) << 8);

		if(4+lens > dataLen){
			tlkapi_error(TLKCPC3_DBG_FLAG, TLKCPC3_DBG_SIGN,
				"tlkcpc2_recv_normalPacketDeal Length Error: frmLen-%d lens-%d",
				dataLen, lens);
			return;
		}
		spTlkCpc3RecvCtrl->recvCB(((uint32)msgID<<16)|((uint32)mtype<<8)|ptype, 0, pData+4, lens);
//		spTlkCpc3RecvCtrl->recvNumb = numb;
	}else if(ptype == TLKPRT_COMM_PTYPE_DAT){
		uint32 numb;
		uint16 lens;
		uint08 datID;
		datID = pData[0];
		numb  = (((uint32)pData[2]) << 8) | pData[1] | (((uint32)pData[4] & 0xF0) << 12);
		lens  = (((uint16)pData[4] & 0x0F) << 8) | pData[3];
		if(5+lens > dataLen){
			tlkapi_error(TLKCPC3_DBG_FLAG, TLKCPC3_DBG_SIGN, "tlkcpc2_recv_normalPacketDeal Length Error: frmLen-%d lens-%d",
				dataLen, lens);
			return;
		}
//		tlkapi_array(0xFFFFFFFF, TLKCPC3_DBG_SIGN, "RecvCpc:", pData, dataLen);
		spTlkCpc3RecvCtrl->recvCB(((uint32)datID<<8)|ptype, numb, pData+5, lens);
	}
}




#endif //#if (TLK_CFG_COMM_ENABLE)


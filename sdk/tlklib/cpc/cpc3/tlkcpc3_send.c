/********************************************************************************************************
 * @file	tlkcpc3_send.c
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
#include "tlkcpc3_send.h"

static int tlkcpc3_send_makeFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);


static tlkcpc3_send_ctrl_t *spTlkCpc3SendCtrl;


int tlkcpc3_send_init(tlkcpc3_send_ctrl_t *pCtrl)
{
	spTlkCpc3SendCtrl = pCtrl;
	if(pCtrl == nullptr) return -TLK_EPARAM;

	spTlkCpc3SendCtrl->buffLens = 0;
	spTlkCpc3SendCtrl->pBuffer  = nullptr;
		
	return TLK_ENONE;
}

void tlkcpc3_send_regCB(TlkCpcSendCB sendCB)
{
	if(spTlkCpc3SendCtrl == nullptr) return;
	spTlkCpc3SendCtrl->sendCB = sendCB;
}
void tlkcpc3_send_handler(void)
{
	
}

void tlkcpc3_send_setBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(spTlkCpc3SendCtrl == nullptr) return;
	spTlkCpc3SendCtrl->buffLens = buffLen;
	spTlkCpc3SendCtrl->pBuffer  = pBuffer;
}


/******************************************************************************
 * Function: tlkcpc3_send_XXX.
 * Descript: send the command or Response or Event or data.
 * Params:
 *        @mType[IN]--The message type.
 *        @cmdID[IN]--which command will be to send.
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 *        @status[IN]--The status.
 *        @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkcpc3_send_cmd(uint08 mtype, uint16 msgID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 headLen;
	uint08 head[4];

	if(spTlkCpc3SendCtrl == nullptr || spTlkCpc3SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	
	headLen = 0;
	head[headLen++] = (mtype & 0xFF);
	head[headLen++] = (msgID & 0xFF);
	head[headLen++] = (dataLen & 0xFF);
	head[headLen++] = ((dataLen & 0x0F00) >> 8) | ((msgID & 0xF00) >> 4);
	ret = tlkcpc3_send_makeFrame(TLKPRT_COMM_PTYPE_CMD, head, 4, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc3SendCtrl->sendCB(spTlkCpc3SendCtrl->pBuffer, spTlkCpc3SendCtrl->sendLens);
//		tlkapi_trace(TLKCPC3_DBG_FLAG, TLKCPC3_DBG_SIGN, "send 01:");
	}
	return ret;
}
int tlkcpc3_send_rsp(uint08 mtype, uint16 msgID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 headLen;
	uint08 head[8];
	
	if(spTlkCpc3SendCtrl == nullptr || spTlkCpc3SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}

	headLen = 0;
	head[headLen++] = (mtype & 0xFF);
	head[headLen++] = (msgID & 0xFF);
	head[headLen++] = ((2+dataLen) & 0xFF);
	head[headLen++] = (((2+dataLen) & 0x0F00) >> 8) | ((msgID & 0xF00) >> 4);
	head[headLen++] = status;
	head[headLen++] = reason;
	ret = TLK_ENONE;
	ret = tlkcpc3_send_makeFrame(TLKPRT_COMM_PTYPE_RSP, head, 6, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc3SendCtrl->sendCB(spTlkCpc3SendCtrl->pBuffer, spTlkCpc3SendCtrl->sendLens);
//		tlkapi_array(0xFFFFFFFF, TLKCPC3_DBG_SIGN, "send 01:", spTlkCpc3SendCtrl->sendFrame, spTlkCpc3SendCtrl->sendLens);
	}
	return ret;
}
int tlkcpc3_send_evt(uint08 mtype, uint16 msgID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 headLen;
	uint08 head[4];

	if(spTlkCpc3SendCtrl == nullptr || spTlkCpc3SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	
	headLen = 0;
	head[headLen++] = (mtype & 0xFF);
	head[headLen++] = (msgID & 0xFF);
	head[headLen++] = (dataLen & 0xFF);
	head[headLen++] = ((dataLen & 0x0F00) >> 8) | ((msgID& 0xF00) >> 4);
	ret = tlkcpc3_send_makeFrame(TLKPRT_COMM_PTYPE_EVT, head, 4, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc3SendCtrl->sendCB(spTlkCpc3SendCtrl->pBuffer, spTlkCpc3SendCtrl->sendLens);
	}
	return ret;
}

int tlkcpc3_send_dat(uint08 datID, uint32 numb, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint08 head[8];

	if(spTlkCpc3SendCtrl == nullptr || spTlkCpc3SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	if(pData == nullptr || dataLen == 0 || dataLen > 256) return -TLK_EPARAM;

	head[0] = datID; //DID
	head[1] = (numb & 0x00FF); //Num
	head[2] = (numb & 0xFF00) >> 8;
	head[3] = (dataLen & 0x00FF); //Lens
	head[4] = ((dataLen & 0x0F00) >> 8) | ((numb & 0x0F0000) >> 12); 
	ret = tlkcpc3_send_makeFrame(TLKPRT_COMM_PTYPE_DAT, head, 5, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc3SendCtrl->sendCB(spTlkCpc3SendCtrl->pBuffer, spTlkCpc3SendCtrl->sendLens);
	}
	
	return ret;
}

int tlkcpc3_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	int ret;
	
	if(spTlkCpc3SendCtrl == nullptr || spTlkCpc3SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	
	ret = tlkcpc3_send_makeFrame(pktType, pHead, headLen, pBody, bodyLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc3SendCtrl->sendCB(spTlkCpc3SendCtrl->pBuffer, spTlkCpc3SendCtrl->sendLens);
	}
	return ret;
}

static int tlkcpc3_send_makeFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	int index;
	int count;
	uint08 *pFrame;
	uint16 frmLen;
	uint16 tempVar;

	if(headLen+bodyLen == 0){
		return -TLK_EPARAM;
	}
	if(headLen+bodyLen+6 > spTlkCpc3SendCtrl->buffLens){
		return -TLK_EOVERFLOW;
	}

	frmLen = 0;
	pFrame = spTlkCpc3SendCtrl->pBuffer;

	frmLen = 0;
	pFrame[frmLen++] = TLKCPC3_FRAME_HEAD_SIGN;
	pFrame[frmLen++] = (headLen+bodyLen+4) & 0xFF;
	pFrame[frmLen++] = (((headLen+bodyLen+4) & 0x0F00)>>8) | ((pktType & 0x0F) << 4);
	if(headLen != 0){
		memcpy(pFrame+frmLen, pHead, headLen);
		frmLen += headLen;
	}
	if(bodyLen != 0){
		memcpy(pFrame+frmLen, pBody, bodyLen);
		frmLen += bodyLen;
	}
	tempVar = tlkalg_crc16_calc(pFrame+1, frmLen-1);
	pFrame[frmLen++] = tempVar & 0xFF;
	pFrame[frmLen++] = (tempVar & 0xFF00) >> 8;
	
	count = 0;
	for(index=1; index<frmLen; index++){
		if(pFrame[index] == TLKCPC3_FRAME_HEAD_SIGN || pFrame[index] == TLKCPC3_ESCAPE_CHARS
			 || pFrame[index] == TLKCPC3_FRAME_TAIL_SIGN){
			count++;
		} 
	}
	if(count != 0){
		if(frmLen+count+1 > spTlkCpc3SendCtrl->buffLens){
			return -TLK_EOVERFLOW;
		}
		int offset = frmLen+count;
		for(index=frmLen-1; index>=1; index--){
			if(pFrame[index] == TLKCPC3_FRAME_HEAD_SIGN){
				pFrame[--offset] = TLKCPC3_ESCAPE_CHARS_HEAD;
				pFrame[--offset] = TLKCPC3_ESCAPE_CHARS;
			}else if(pFrame[index] == TLKCPC3_FRAME_TAIL_SIGN){
				pFrame[--offset] = TLKCPC3_ESCAPE_CHARS_TAIL;
				pFrame[--offset] = TLKCPC3_ESCAPE_CHARS;
			}else if(pFrame[index] == TLKCPC3_ESCAPE_CHARS){
				pFrame[--offset] = TLKCPC3_ESCAPE_CHARS_RAW;
				pFrame[--offset] = TLKCPC3_ESCAPE_CHARS;
			}else{
				pFrame[--offset] = pFrame[index];
			}
		}
		if(offset != 1){
			tlkapi_error(TLKCPC3_DBG_FLAG, TLKCPC3_DBG_SIGN, "tlkcpc2_send_data: send failure [%d]!", frmLen);
			return -TLK_EFAULT;
		}
		frmLen += count;
	}
	pFrame[frmLen++] = TLKCPC3_FRAME_TAIL_SIGN;

	spTlkCpc3SendCtrl->sendLens = frmLen;
	
	return TLK_ENONE;
}




#endif //#if (TLK_CFG_COMM_ENABLE)


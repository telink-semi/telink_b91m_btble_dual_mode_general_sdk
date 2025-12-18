/********************************************************************************************************
 * @file	tlkcpc1_send.c
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
#include "tlkcpc1_send.h"

static int tlkcpc1_send_makeFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen);


static tlkcpc1_send_ctrl_t *spTlkCpc1SendCtrl;


int tlkcpc1_send_init(tlkcpc1_send_ctrl_t *pCtrl)
{
	spTlkCpc1SendCtrl = pCtrl;
	if(pCtrl == nullptr) return -TLK_EPARAM;

	spTlkCpc1SendCtrl->buffLen = 0;
	spTlkCpc1SendCtrl->pBuffer = nullptr;
	
	return TLK_ENONE;
}

void tlkcpc1_send_regCB(TlkCpcSendCB sendCB)
{
	if(spTlkCpc1SendCtrl == nullptr) return;
	spTlkCpc1SendCtrl->sendCB = sendCB;
}
void tlkcpc1_send_handler(void)
{
	
}

void tlkcpc1_send_setBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(spTlkCpc1SendCtrl == nullptr) return;
	spTlkCpc1SendCtrl->buffLen = buffLen;
	spTlkCpc1SendCtrl->pBuffer = pBuffer;
}


/******************************************************************************
 * Function: tlkcpc1_send_XXX.
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
int tlkcpc1_send_cmd(uint08 mType, uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[4];

	if(spTlkCpc1SendCtrl == nullptr || spTlkCpc1SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}

	head[0] = mType; //Mtype
	head[1] = cmdID; //MsgID
	head[2] = spTlkCpc1SendCtrl->sendNumb; //Numb
	head[3] = dataLen; //Lens
	ret = tlkcpc1_send_makeFrame(TLKPRT_COMM_PTYPE_CMD, head, 4, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc1SendCtrl->sendCB(spTlkCpc1SendCtrl->pBuffer, spTlkCpc1SendCtrl->sendLens);
//		tlkapi_trace(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, "send 01:");
	}
	if(ret == TLK_ENONE) spTlkCpc1SendCtrl->sendNumb++;
	return ret;
}
int tlkcpc1_send_rsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[6];

	if(spTlkCpc1SendCtrl == nullptr || spTlkCpc1SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}

	head[0] = mType; //Mtype
	head[1] = cmdID; //MsgID
	head[2] = spTlkCpc1SendCtrl->sendNumb; //Numb
	head[3] = 2+dataLen; //Lens
	head[4] = status;
	head[5] = reason;
	ret = tlkcpc1_send_makeFrame(TLKPRT_COMM_PTYPE_RSP, head, 6, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc1SendCtrl->sendCB(spTlkCpc1SendCtrl->pBuffer, spTlkCpc1SendCtrl->sendLens);
//		tlkapi_array(0xFFFFFFFF, TLKCPC1_DBG_SIGN, "send 01:", spTlkCpc1SendCtrl->sendFrame, spTlkCpc1SendCtrl->sendLens);
	}
	if(ret == TLK_ENONE) spTlkCpc1SendCtrl->sendNumb++;
	return ret;
}
int tlkcpc1_send_evt(uint08 mType, uint08 evtID, uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 head[4];

	if(spTlkCpc1SendCtrl == nullptr || spTlkCpc1SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	
	head[0] = mType; //Mtype
	head[1] = evtID; //MsgID
	head[2] = spTlkCpc1SendCtrl->sendNumb; //Numb
	head[3] = dataLen; //Lens
	ret = tlkcpc1_send_makeFrame(TLKPRT_COMM_PTYPE_EVT, head, 4, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc1SendCtrl->sendCB(spTlkCpc1SendCtrl->pBuffer, spTlkCpc1SendCtrl->sendLens);
	}
	if(ret == TLK_ENONE) spTlkCpc1SendCtrl->sendNumb++;
	return ret;
}

int tlkcpc1_send_dat(uint08 datID, uint32 numb, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint08 head[8];

	if(spTlkCpc1SendCtrl == nullptr || spTlkCpc1SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	if(pData == nullptr || dataLen == 0 || dataLen > 256) return -TLK_EPARAM;

	head[0] = datID; //DID
	head[1] = (numb & 0x00FF); //Num
	head[2] = (numb & 0xFF00) >> 8;
	head[3] = (dataLen & 0x00FF); //Lens
	head[4] = (dataLen & 0x0F00) >> 8;
	head[4] |= (numb & 0x0F0000) >> 12;
	ret = tlkcpc1_send_makeFrame(TLKPRT_COMM_PTYPE_DAT, head, 5, pData, dataLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc1SendCtrl->sendCB(spTlkCpc1SendCtrl->pBuffer, spTlkCpc1SendCtrl->sendLens);
	}
	
	return ret;
}

int tlkcpc1_send(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	int ret;
	
	if(spTlkCpc1SendCtrl == nullptr || spTlkCpc1SendCtrl->sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	
	ret = tlkcpc1_send_makeFrame(pktType, pHead, headLen, pBody, bodyLen);
	if(ret == TLK_ENONE){
		ret = spTlkCpc1SendCtrl->sendCB(spTlkCpc1SendCtrl->pBuffer, spTlkCpc1SendCtrl->sendLens);
	}
	return ret;
}



static int tlkcpc1_send_makeFrame(uint08 pktType, uint08 *pHead, uint16 headLen, uint08 *pBody, uint16 bodyLen)
{
	uint16 tempVar;
	uint08 *pFrame;
	uint16 frmLen;
	
//	tlkapi_trace(TLKCPC1_DBG_FLAG, TLKCPC1_DBG_SIGN, "send 01:");
	if(headLen == 0 && pHead != nullptr) pHead = nullptr;
	if(bodyLen == 0 && pBody != nullptr) pBody = nullptr;
	if((headLen+bodyLen) == 0) return -TLK_EPARAM;
	if(headLen+bodyLen+TLKCPC1_FRM_EXTLEN > spTlkCpc1SendCtrl->buffLen){
		return -TLK_ELENGTH;
	}
	
	pFrame = spTlkCpc1SendCtrl->pBuffer;
	
	frmLen = 0;
	pFrame[frmLen++] = TLKCPC1_HEAD_SIGN0;
	pFrame[frmLen++] = TLKCPC1_HEAD_SIGN1;
	pFrame[frmLen++] = 0x00; //Length[0~7]
	pFrame[frmLen++] = (pktType & 0x0F) << 4; //Length[8~11]+pktType[0~4]
	if(headLen != 0){
		tmemcpy(pFrame+frmLen, pHead, headLen);
		frmLen += headLen;
	}
	if(bodyLen != 0){
		tmemcpy(pFrame+frmLen, pBody, bodyLen);
		frmLen += bodyLen;
	}
	pFrame[2] |= (frmLen & 0x00FF);
	pFrame[3] |= (frmLen & 0x0F00) >> 8;
	tempVar = tlkalg_crc16_calc(pFrame+2, frmLen-2);
	pFrame[frmLen++] = (tempVar & 0x00FF);
	pFrame[frmLen++] = (tempVar & 0xFF00) >> 8;
	pFrame[frmLen++] = TLKCPC1_TAIL_SIGN0;
	pFrame[frmLen++] = TLKCPC1_TAIL_SIGN1;
	spTlkCpc1SendCtrl->sendLens = frmLen;
	
	return TLK_ENONE;
}




#endif //#if (TLK_CFG_COMM_ENABLE)


/********************************************************************************************************
 * @file	tlkmdi_atsSend.c
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


extern tlkcpc2_ctrl_t *spTlkCpc2Ctrl;


int tlkcpc2_send_init(void)
{
	spTlkCpc2Ctrl->sendCtrl.buffLen = 0;
	spTlkCpc2Ctrl->sendCtrl.pBuffer = nullptr;
	return TLK_ENONE;
}

void tlkcpc2_send_regCB(TlkCpcSendCB sendCB)
{
	if(spTlkCpc2Ctrl == nullptr) return;
	spTlkCpc2Ctrl->sendCtrl.sendCB = sendCB;
}
void tlkcpc2_send_setBuffer(uint08 *pBuffer, uint16 buffLen)
{
	if(spTlkCpc2Ctrl == nullptr) return;
	spTlkCpc2Ctrl->sendCtrl.buffLen = buffLen;
	spTlkCpc2Ctrl->sendCtrl.pBuffer = pBuffer;
}

void tlkcpc2_send_handler(void)
{
	int ret;
	tlkcpc2_pool_item_t *pItem;

	if(spTlkCpc2Ctrl == nullptr || spTlkCpc2Ctrl->sendCtrl.sendCB == nullptr){
		return;
	}
	if(tlkcpc2_pool_getSendNumb() == 0) return;
	while(true){
		pItem = tlkcpc2_pool_takeSendItem();
		if(pItem == nullptr) break;
	
		ret = spTlkCpc2Ctrl->sendCtrl.sendCB(pItem->itemData, pItem->dataSize);
		if(ret != TLK_ENONE){
			tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_send_handler: send failure [%d]!", pItem->dataSize);
			tlkcpc2_pool_pushSendItem(pItem, true);
			break;
		}

//		tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_send_handler: send success [%d]!", pItem->sendNumb);
		if(pItem->ctrlTran == 1){
//			tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_send_handler: send success [%d]!", pItem->sendNumb);
			tlkcpc2_pool_pushCacheItem(pItem, false);
			spTlkCpc2Ctrl->pollTimer = clock_time()|1;
			if(spTlkCpc2Ctrl->syncTimer == 0) spTlkCpc2Ctrl->syncTimer = clock_time()|1;
		}else{
			tlkcpc2_pool_pushIdleItem(pItem, false);
		}
	}
}

/******************************************************************************
 * Function: tlkcpc2_packet_escape
 * Descript: Escaping a frame of data.
 * Params: None.
 * Return: 
 *****************************************************************************/
int tlkcpc2_packet_escape(uint08 *pData, uint16 dataLen, uint16 maxLen)
{
	int index;
	int count;
	int offset;
	
	count = 0;
	for(index=0; index<dataLen; index++){
		if(pData[index] == TLKCPC2_FRAME_HEAD_SIGN || pData[index] == TLKCPC2_ESCAPE_CHARS
			 || pData[index] == TLKCPC2_FRAME_TAIL_SIGN){
			count++;
		} 
	}
	if(count == 0) return dataLen;
	if(dataLen+count+1 > maxLen) return -TLK_EOVERFLOW;

	offset = dataLen+count;
	for(index=dataLen-1; index>=0; index--){
		if(pData[index] == TLKCPC2_FRAME_HEAD_SIGN){
			pData[--offset] = TLKCPC2_ESCAPE_CHARS_HEAD;
			pData[--offset] = TLKCPC2_ESCAPE_CHARS;
		}else if(pData[index] == TLKCPC2_ESCAPE_CHARS){
			pData[--offset] = TLKCPC2_ESCAPE_CHARS_RAW;
			pData[--offset] = TLKCPC2_ESCAPE_CHARS;
		}else if(pData[index] == TLKCPC2_FRAME_TAIL_SIGN){
			pData[--offset] = TLKCPC2_ESCAPE_CHARS_TAIL;
			pData[--offset] = TLKCPC2_ESCAPE_CHARS;
		}else{
			pData[--offset] = pData[index];
		}
	}
	if(offset != 0) return -TLK_EFAULT;
	return (dataLen+count);
}

/******************************************************************************
 * Function: tlkcpc2_packet_revise
 * Descript: Escaping a frame of data.
 * Params: None.
 * Return: 
 *****************************************************************************/
int tlkcpc2_packet_revise(uint08 *pData, uint16 dataLen)
{
	int index;
	int offset;
	
	for(index=0; index<dataLen; index++){
		if(pData[index] == TLKCPC2_ESCAPE_CHARS) break;
	}
	if(index == dataLen) return dataLen;
	
	offset = 0;
	for(index=0; index<dataLen; index++){
		if(pData[index] != TLKCPC2_ESCAPE_CHARS){
			pData[offset++] = pData[index];
		}else{
			if(index+1 >= dataLen) break;
			index ++;
			if(pData[index] == TLKCPC2_ESCAPE_CHARS_RAW){
				pData[offset++] = TLKCPC2_ESCAPE_CHARS;
			}else if(pData[index] == TLKCPC2_ESCAPE_CHARS_HEAD){
				pData[offset++] = TLKCPC2_FRAME_HEAD_SIGN;
			}else if(pData[index] == TLKCPC2_ESCAPE_CHARS_TAIL){
				pData[offset++] = TLKCPC2_FRAME_TAIL_SIGN;
			}else{
				break;
			}
		}
	}
	if(index != dataLen) return -TLK_EFAIL; //escaping failure
	return offset;
}

int tlkcpc2_send_packet(bool isEnExt, uint08 ptype, uint08 *pHead, uint08 headLen, uint08 *pBody, uint16 bodyLen, bool isForce)
{
	int ret;
	bool isSucc;
	uint16 frmAttr;
	uint16 tempVar;
	tlkcpc2_pool_item_t *pItem;

	if(spTlkCpc2Ctrl == nullptr || spTlkCpc2Ctrl->sendCtrl.sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	if(headLen+bodyLen == 0){
		return -TLK_EPARAM;
	}
	if(isEnExt){
		if(headLen+bodyLen+8 > TLKCPC2_POOL_ITEM_SIZE){
			return -TLK_EOVERFLOW;
		}
	}else{
		if(headLen+bodyLen+6 > TLKCPC2_POOL_ITEM_SIZE){
			return -TLK_EOVERFLOW;
		}
	}
	
	pItem = tlkcpc2_pool_takeIdleItem();
	if(pItem == nullptr) return -TLK_EQUOTA;

	if(isEnExt) frmAttr = ((ptype & 0x0F) << 12) | ((headLen+bodyLen+6) & 0x07FF);
	else frmAttr = ((ptype & 0x0F) << 12) | ((headLen+bodyLen+4) & 0x07FF);

	if(isEnExt) frmAttr |= 0x0800;
	
	pItem->dataSize = 0;
	pItem->itemData[pItem->dataSize++] = TLKCPC2_FRAME_HEAD_SIGN;
	pItem->itemData[pItem->dataSize++] = (frmAttr & 0xFF);
	pItem->itemData[pItem->dataSize++] = (frmAttr & 0xFF00) >> 8;
	if(!isEnExt){
		pItem->ctrlTran = 0;
	}else{
		pItem->ctrlTran = 1;
		pItem->sendNumb = spTlkCpc2Ctrl->sendNumb;
		pItem->itemData[pItem->dataSize++] = spTlkCpc2Ctrl->recvNumb;
		pItem->itemData[pItem->dataSize++] = spTlkCpc2Ctrl->sendNumb;
	}
	if(headLen != 0){
		memcpy(pItem->itemData+pItem->dataSize, pHead, headLen);
		pItem->dataSize += headLen;
	}
	if(bodyLen != 0){
		memcpy(pItem->itemData+pItem->dataSize, pBody, bodyLen);
		pItem->dataSize += bodyLen;
	}
	tempVar = tlkalg_crc16_calc(pItem->itemData+1, pItem->dataSize-1);
	pItem->itemData[pItem->dataSize++] = tempVar & 0xFF;
	pItem->itemData[pItem->dataSize++] = (tempVar & 0xFF00) >> 8;
	ret = tlkcpc2_packet_escape(pItem->itemData+1, pItem->dataSize-1, TLKCPC2_POOL_ITEM_SIZE-2);
	if(ret < 0){
		tlkcpc2_pool_pushIdleItem(pItem, true);
		return ret;
	}
	
	pItem->dataSize = ret+1;
	pItem->itemData[pItem->dataSize++] = TLKCPC2_FRAME_TAIL_SIGN;
//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_send_packet 01: %d %d %d %d", pItem->itemNumb,
//		tlkcpc2_pool_getIdleNumb(), tlkcpc2_pool_getSendNumb(), tlkcpc2_pool_getCacheNumb());

	if(pItem->ctrlTran == 0 || isForce || ((spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_DONE) && 
		(spTlkCpc2Ctrl->authState == TLKCPC2_AUTH_STATUS_NONE || spTlkCpc2Ctrl->authState == TLKCPC2_AUTH_STATUS_SUCCESS))){
		isSucc = tlkcpc2_pool_pushSendItem(pItem, false);
		if(isSucc && pItem->ctrlTran == 1) spTlkCpc2Ctrl->sendNumb ++;
		if(isSucc) tlkcpc2_send_handler();
	}else{
		isSucc = tlkcpc2_pool_pushCopyItem(pItem, false);
	}
	if(!isSucc) tlkcpc2_pool_pushIdleItem(pItem, true);
	
	return TLK_ENONE;
}

int tlkcpc2_sendCmdPacket(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen, bool enFlow)
{
	uint08 headLen;
	uint08 head[4];
	bool isForce;

	if(mtype == TLKPRT_COMM_MTYPE_AUTHEN) mtype = TLKPRT_COMM_MTYPE_NONE;

	isForce = (mtype == TLKPRT_COMM_MTYPE_NONE) ? true : false;

	headLen = 0;
	head[headLen++] = (mtype & 0xFF);
	head[headLen++] = (msgID & 0xFF);
	head[headLen++] = (dataLen & 0xFF);
	head[headLen++] = ((dataLen & 0x0F00) >> 8) | ((msgID & 0xF00) >> 4);
	return tlkcpc2_send_packet(enFlow, TLKPRT_COMM_PTYPE_CMD, head, headLen, pData, dataLen, isForce);
}
int tlkcpc2_sendRspPacket(uint08 mtype, uint16 msgID, uint08 status, uint08 reason, uint08 *pData, uint16 dataLen, bool enFlow)
{
	uint08 headLen;
	uint08 head[8];
	bool isForce;
	
	if(mtype == TLKPRT_COMM_MTYPE_AUTHEN) mtype = TLKPRT_COMM_MTYPE_NONE;
	
	isForce = (mtype == TLKPRT_COMM_MTYPE_NONE) ? true : false;
//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_sendRspPacket: %d %d %d", msgType, msgID, dataLen);

	headLen = 0;
	head[headLen++] = (mtype & 0xFF);
	head[headLen++] = (msgID & 0xFF);
	head[headLen++] = (2+dataLen & 0xFF);
	head[headLen++] = ((2+dataLen & 0x0F00) >> 8) | ((msgID & 0xF00) >> 4);
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlkcpc2_send_packet(enFlow, TLKPRT_COMM_PTYPE_RSP, head, headLen, pData, dataLen, isForce);
}
int tlkcpc2_sendEvtPacket(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen, bool enFlow)
{
	uint08 headLen;
	uint08 head[4];
	bool isForce;

	if(mtype == TLKPRT_COMM_MTYPE_AUTHEN) mtype = TLKPRT_COMM_MTYPE_NONE;

	isForce = (mtype == TLKPRT_COMM_MTYPE_NONE) ? true : false;

	headLen = 0;
	head[headLen++] = (mtype & 0xFF);
	head[headLen++] = (msgID & 0xFF);
	head[headLen++] = (dataLen & 0xFF);
	head[headLen++] = ((dataLen & 0x0F00) >> 8) | ((msgID & 0xF00) >> 4);
	return tlkcpc2_send_packet(enFlow, TLKPRT_COMM_PTYPE_EVT, head, headLen, pData, dataLen, isForce);
}
int tlkcpc2_sendDatPacket(uint08 datID, uint32 number, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint16 frmAttr;
	uint16 tempVar;
	uint16 sendLen;

	if(pData == nullptr || dataLen == 0) return -TLK_EPARAM;
	if(spTlkCpc2Ctrl == nullptr || spTlkCpc2Ctrl->sendCtrl.sendCB == nullptr){
		return -TLK_ENOREADY;
	}
	
	if(tlkcpc2_pool_getIdleNumb() < 3) return -TLK_EBUSY;
					
	if(5+dataLen+6 > spTlkCpc2Ctrl->sendCtrl.buffLen){
		return -TLK_EOVERFLOW;
	}
	
	frmAttr = ((TLKPRT_COMM_PTYPE_DAT & 0x0F) << 12) | ((5+dataLen+4) & 0x07FF);
	
	sendLen = 0;
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = TLKCPC2_FRAME_HEAD_SIGN;
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = (frmAttr & 0xFF);
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = (frmAttr & 0xFF00) >> 8;
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = datID; //DID
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = (number & 0x00FF); //Num
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = (number & 0xFF00) >> 8;
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = (dataLen & 0x00FF); //Lens
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = ((dataLen & 0x0F00) >> 8) | ((number & 0x0F0000) >> 12);
	tmemcpy(spTlkCpc2Ctrl->sendCtrl.pBuffer+sendLen, pData, dataLen);
	sendLen += dataLen;
	tempVar = tlkalg_crc16_calc(spTlkCpc2Ctrl->sendCtrl.pBuffer+1, sendLen-1);
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = tempVar & 0xFF;
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = (tempVar & 0xFF00) >> 8;
	ret = tlkcpc2_packet_escape(spTlkCpc2Ctrl->sendCtrl.pBuffer+1, sendLen-1, spTlkCpc2Ctrl->sendCtrl.buffLen-2);
	if(ret < 0) return ret;
	
	sendLen = ret+1;
	spTlkCpc2Ctrl->sendCtrl.pBuffer[sendLen++] = TLKCPC2_FRAME_TAIL_SIGN;

	return spTlkCpc2Ctrl->sendCtrl.sendCB(spTlkCpc2Ctrl->sendCtrl.pBuffer, sendLen);
}
int tlkcpc2_sendFcsPacket(uint08 bitP, uint08 bitF, uint08 bitR, uint08 recvNumb, uint08 sendNumb)
{
	uint16 fcsFlag;
	uint16 fcsAttr;
	uint08 headLen;
	uint08 head[4];

	fcsFlag = 0;
	fcsAttr = 0;
	if(bitP != 0) fcsFlag |= TLKCPC2_FCS_FLAG_P;
	if(bitF != 0) fcsFlag |= TLKCPC2_FCS_FLAG_F;
	if(bitR != 0) fcsFlag |= TLKCPC2_FCS_FLAG_R;
	fcsAttr = (sendNumb << 8) | (recvNumb & 0xFF);

	headLen = 0;
	head[headLen++] = (fcsFlag & 0xFF);
	head[headLen++] = (fcsFlag & 0xFF00) >> 8;
	head[headLen++] = (fcsAttr & 0xFF);
	head[headLen++] = (fcsAttr & 0xFF00) >> 8;
	return tlkcpc2_send_packet(false, TLKPRT_COMM_PTYPE_FCS, head, headLen, nullptr, 0, true);
}
int tlkcpc2_sendResetPacket(uint08 resetCode)
{
	uint08 headLen;
	uint16 fscFlag;
	uint16 fcsAttr;
	uint08 header[4];
	
	fscFlag = ((resetCode & 0x0F) << 4 ) | TLKCPC2_FCS_FLAG_P;
	fcsAttr = 0;
	
	headLen = 0;
	header[headLen++] = (fscFlag & 0xFF);
	header[headLen++] = (fscFlag & 0xFF00) >> 8;
	header[headLen++] = (fcsAttr & 0xFF);
	header[headLen++] = (fcsAttr & 0xFF00) >> 8;
	return tlkcpc2_send_packet(false, TLKPRT_COMM_PTYPE_FCS, header, headLen, nullptr, 0, true);
}

int tlkcpc2_sendResetReqPacket(void)
{
//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_sendResetReqPacket");
	return tlkcpc2_sendResetPacket(TLKCPC2_RST_CODE_REQ);
}
int tlkcpc2_sendResetAckPacket(void)
{
//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_sendResetAckPacket");
	return tlkcpc2_sendResetPacket(TLKCPC2_RST_CODE_ACK);
}
int tlkcpc2_sendResetCfmPacket(void)
{
//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_sendResetCfmPacket");
	return tlkcpc2_sendResetPacket(TLKCPC2_RST_CODE_CFM);
}
int tlkcpc2_sendResetDonePacket(void)
{
//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_sendResetDonePacket");
	return tlkcpc2_sendResetPacket(TLKCPC2_RST_CODE_CFM_ACK);
}



#endif //#if (TLK_TEST_ATS_ENABLE)


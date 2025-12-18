/********************************************************************************************************
 * @file	tlkmdi_atsPool.c
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
#include "tlkcpc2_send.h"


extern tlkcpc2_ctrl_t *spTlkCpc2Ctrl;


int tlkcpc2_pool_init(void)
{
	tlkcpc2_pool_reset(true);
		
	return TLK_ENONE;
}


void tlkcpc2_pool_reset(bool isForce)
{
	int index;
	tlkcpc2_pool_item_t *pItem;

	if(isForce){
		spTlkCpc2Ctrl->poolCtrl.idleNumb = 0;
		spTlkCpc2Ctrl->poolCtrl.copyNumb = 0;
		spTlkCpc2Ctrl->poolCtrl.sendNumb = 0;
		spTlkCpc2Ctrl->poolCtrl.cacheNumb = 0;
		for(index=0; index<TLKCPC2_POOL_ITEM_NUMB; index++){
			pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[index];
			pItem->ctrlTran = 0;
			pItem->dataSize = 0;
			pItem->sendNumb = 0;
			pItem->itemNumb = index;
			spTlkCpc2Ctrl->poolCtrl.idleList[spTlkCpc2Ctrl->poolCtrl.idleNumb++] = index;
		}
		return;
	}

	tlkcpc2_pool_backupCopyItem();
}

int tlkcpc2_pool_getIdleNumb(void)
{
	return spTlkCpc2Ctrl->poolCtrl.idleNumb;
}
int tlkcpc2_pool_getSendNumb(void)
{
	return spTlkCpc2Ctrl->poolCtrl.sendNumb;
}
int tlkcpc2_pool_getCacheNumb(void)
{
	return spTlkCpc2Ctrl->poolCtrl.cacheNumb;
}

tlkcpc2_pool_item_t *tlkcpc2_pool_takeIdleItem(void)
{
	return tlkcpc2_pool_takeItem(spTlkCpc2Ctrl->poolCtrl.idleList, &spTlkCpc2Ctrl->poolCtrl.idleNumb);
}
tlkcpc2_pool_item_t *tlkcpc2_pool_takeSendItem(void)
{
	return tlkcpc2_pool_takeItem(spTlkCpc2Ctrl->poolCtrl.sendList, &spTlkCpc2Ctrl->poolCtrl.sendNumb);
}
tlkcpc2_pool_item_t *tlkcpc2_pool_takeCacheItem(void)
{
	return tlkcpc2_pool_takeItem(spTlkCpc2Ctrl->poolCtrl.cacheList, &spTlkCpc2Ctrl->poolCtrl.cacheNumb);
}

bool tlkcpc2_pool_cacheItem(tlkcpc2_pool_item_t *pItem)
{
	return tlkcpc2_pool_pushCacheItem(pItem, false);
}
bool tlkcpc2_pool_removeCacheItem(uint08 sendNumb)
{
	int index;
	int count;
	tlkcpc2_pool_item_t *pItem;

//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_removeCacheItem: sendNumb[%d]", sendNumb);
	for(index=0; index<spTlkCpc2Ctrl->poolCtrl.cacheNumb; index++){
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.cacheList[index]];
		if(pItem->sendNumb == sendNumb) break;
	}
	if(index == spTlkCpc2Ctrl->poolCtrl.cacheNumb) return false;

//	tlkcpc2_pool_printCacheList();
	
	count = index+1;
	for(index=0; index<count; index++){
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.cacheList[count-index-1]];
		if(!tlkcpc2_pool_pushIdleItem(pItem, false)) break;
	}
	if(index != count) return false;
	spTlkCpc2Ctrl->poolCtrl.cacheNumb -= count;
	if(spTlkCpc2Ctrl->poolCtrl.cacheNumb != 0){
		tmemcpy(&spTlkCpc2Ctrl->poolCtrl.cacheList[0], &spTlkCpc2Ctrl->poolCtrl.cacheList[index], spTlkCpc2Ctrl->poolCtrl.cacheNumb);
	}

//	tlkcpc2_pool_printCacheList();
//	tlkcpc2_pool_printIdleList();
	
	return true;
}
bool tlkcpc2_pool_activeCacheItem(uint08 sendNumb)
{
	int index;
	int count;
	tlkcpc2_pool_item_t *pItem;

//	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_activeCacheItem: sendNumb[%d]", sendNumb);
//	tlkapi_array(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "restoreBackupItem 01: ", 
//		spTlkCpc2Ctrl->poolCtrl.cacheList, spTlkCpc2Ctrl->poolCtrl.cacheNumb);
	for(index=0; index<spTlkCpc2Ctrl->poolCtrl.cacheNumb; index++){
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.cacheList[index]];
		if(pItem->sendNumb == sendNumb) break;
	}
	if(index == spTlkCpc2Ctrl->poolCtrl.cacheNumb) return false;

//	tlkcpc2_pool_printCacheList();
	
	//Remove received packet.
	if(index != 0){
		count = index;
		for(index=0; index<count; index++){
			pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.cacheList[count-index-1]];
			if(!tlkcpc2_pool_pushIdleItem(pItem, false)) break;
		}
		if(index != count) return false;
		spTlkCpc2Ctrl->poolCtrl.cacheNumb -= count;
		if(spTlkCpc2Ctrl->poolCtrl.cacheNumb != 0){
			tmemcpy(&spTlkCpc2Ctrl->poolCtrl.cacheList[0], &spTlkCpc2Ctrl->poolCtrl.cacheList[index], spTlkCpc2Ctrl->poolCtrl.cacheNumb);
		}
	}
	//Restore remaining packet
	count = spTlkCpc2Ctrl->poolCtrl.cacheNumb;
	for(index=0; index<count; index++){
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.cacheList[count-index-1]];
		if(!tlkcpc2_pool_pushSendItem(pItem, true)) break;
	}
	if(index != count) return false;

	spTlkCpc2Ctrl->poolCtrl.cacheNumb = 0;
	
//	tlkcpc2_pool_printSendList();
	
	return true;
}


/******************************************************************************
 * Function: tlkcpc2_pool_backupCopyItem
 * Descript: Back up the sent data to the sent copy and wait until the system 
 *           is ready before sending.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice:
 *     1.Once the system is ready, "tlkcpc2_pool_activeCopyItem()" needs to be
 *       called to resume sending.
 *****************************************************************************/
bool tlkcpc2_pool_backupCopyItem(void)
{
	int ret;
	int index;
	int count;
	uint08 ptype;
	uint08 mtype;
	tlkcpc2_pool_item_t *pItem;
	
	if(spTlkCpc2Ctrl->poolCtrl.cacheNumb != 0){
		count = spTlkCpc2Ctrl->poolCtrl.cacheNumb;
		for(index=0; index<count; index++){
			pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.cacheList[index]];
			ret = tlkcpc2_packet_revise(pItem->itemData+1, pItem->dataSize-2);
			if(ret < 0){
				tlkcpc2_pool_pushIdleItem(pItem, false);
				continue;
			}
			pItem->dataSize = ret+1;
			pItem->itemData[pItem->dataSize++] = TLKCPC2_FRAME_TAIL_SIGN;
			ptype = (pItem->itemData[2] & 0xF0) >> 4;
			mtype = pItem->itemData[5];
			if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT || ptype == TLKPRT_COMM_PTYPE_RSP){
				if(mtype == TLKPRT_COMM_MTYPE_NONE || mtype == TLKPRT_COMM_MTYPE_AUTHEN){
					tlkcpc2_pool_pushIdleItem(pItem, false);
					continue;
				}
			}
			if(!tlkcpc2_pool_pushCopyItem(pItem, false)){
				tlkcpc2_pool_pushIdleItem(pItem, false);
			}
		}
		spTlkCpc2Ctrl->poolCtrl.cacheNumb = 0;
	}
	if(spTlkCpc2Ctrl->poolCtrl.sendNumb != 0){
		count = spTlkCpc2Ctrl->poolCtrl.sendNumb;
		for(index=0; index<count; index++){
			pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.sendList[index]];
			if(pItem->ctrlTran == 0){
				tlkcpc2_pool_pushIdleItem(pItem, false);
				continue;
			}
			ret = tlkcpc2_packet_revise(pItem->itemData+1, pItem->dataSize-2);
			if(ret < 0){
				tlkcpc2_pool_pushIdleItem(pItem, false);
				continue;
			}
			pItem->dataSize = ret+1;
			pItem->itemData[pItem->dataSize++] = TLKCPC2_FRAME_TAIL_SIGN;
			ptype = (pItem->itemData[2] & 0xF0) >> 4;
			mtype = pItem->itemData[5];
			if(ptype == TLKPRT_COMM_PTYPE_CMD || ptype == TLKPRT_COMM_PTYPE_EVT || ptype == TLKPRT_COMM_PTYPE_RSP){
				if(mtype == TLKPRT_COMM_MTYPE_NONE || mtype == TLKPRT_COMM_MTYPE_AUTHEN){
					tlkcpc2_pool_pushIdleItem(pItem, false);
					continue;
				}
			}
			if(pItem->ctrlTran == 0 || !tlkcpc2_pool_pushCopyItem(pItem, false)){
				tlkcpc2_pool_pushIdleItem(pItem, false);
			}
		}
		spTlkCpc2Ctrl->poolCtrl.sendNumb = 0;
	}
	
	return true;
}

/******************************************************************************
 * Function: tlkcpc2_pool_activeCopyItem
 * Descript: Initialize all devices used in the system.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice:
 *     1.The device that performs this operation first backs up its own device 
 *       in the system device copy.
 *     2.Only devices in the system device copy can be mounted by calling 
 *       "tlkdev_mount()".
 *****************************************************************************/
bool tlkcpc2_pool_activeCopyItem(void)
{
	int ret;
	int index;
	int count;	
	uint16 tempVar;
	tlkcpc2_pool_item_t *pItem;

	count = spTlkCpc2Ctrl->poolCtrl.copyNumb;
	for(index=0; index<count; index++){
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.copyList[index]];
		pItem->sendNumb = spTlkCpc2Ctrl->sendNumb;
		pItem->itemData[3] = spTlkCpc2Ctrl->recvNumb;
		pItem->itemData[4] = spTlkCpc2Ctrl->sendNumb;
		pItem->dataSize -= 3;
		tempVar = tlkalg_crc16_calc(pItem->itemData+1, pItem->dataSize-1);
		pItem->itemData[pItem->dataSize++] = tempVar & 0xFF;
		pItem->itemData[pItem->dataSize++] = (tempVar & 0xFF00) >> 8;
		ret = tlkcpc2_packet_escape(pItem->itemData+1, pItem->dataSize-1, TLKCPC2_POOL_ITEM_SIZE);
		if(ret < 0){
			tlkcpc2_pool_pushIdleItem(pItem, false);
			continue;
		}
		pItem->dataSize = ret+1;
		pItem->itemData[pItem->dataSize++] = TLKCPC2_FRAME_TAIL_SIGN;
		spTlkCpc2Ctrl->sendNumb ++;
		if(!tlkcpc2_pool_pushSendItem(pItem, false)){
			tlkcpc2_pool_pushIdleItem(pItem, false);
		}
	}
	spTlkCpc2Ctrl->poolCtrl.copyNumb = 0;
	return true;
}


bool tlkcpc2_pool_pushIdleItem(tlkcpc2_pool_item_t *pItem, bool isFirst)
{
	if(!tlkcpc2_pool_pushItem(spTlkCpc2Ctrl->poolCtrl.idleList, &spTlkCpc2Ctrl->poolCtrl.idleNumb, 
		pItem->itemNumb, isFirst))
	{
		tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_pushIdleItem: failure !");
		return false;
	}
	return true;
}
bool tlkcpc2_pool_pushCopyItem(tlkcpc2_pool_item_t *pItem, bool isFirst)
{
	if(spTlkCpc2Ctrl->poolCtrl.idleNumb < 3) return false;
	if(!tlkcpc2_pool_pushItem(spTlkCpc2Ctrl->poolCtrl.copyList, &spTlkCpc2Ctrl->poolCtrl.copyNumb,
		pItem->itemNumb, isFirst))
	{
		tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_pushCopyItem: failure !");
		return false;
	}
	return true;
}
bool tlkcpc2_pool_pushSendItem(tlkcpc2_pool_item_t *pItem, bool isFirst)
{
	if(!tlkcpc2_pool_pushItem(spTlkCpc2Ctrl->poolCtrl.sendList, &spTlkCpc2Ctrl->poolCtrl.sendNumb,
		pItem->itemNumb, isFirst))
	{
		tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_pushSendItem: failure !");
		return false;
	}
	return true;
}
bool tlkcpc2_pool_pushCacheItem(tlkcpc2_pool_item_t *pItem, bool isFirst)
{
	if(!tlkcpc2_pool_pushItem(spTlkCpc2Ctrl->poolCtrl.cacheList, &spTlkCpc2Ctrl->poolCtrl.cacheNumb,
		pItem->itemNumb, isFirst))
	{
		tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_pushCacheItem: failure !");
		return false;
	}
	return true;
}

bool tlkcpc2_pool_isInSendList(uint08 sendNumb)
{
	int index;
	tlkcpc2_pool_item_t *pItem;

	for(index=0; index<spTlkCpc2Ctrl->poolCtrl.sendNumb; index++){
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.sendList[index]];
		if(pItem->sendNumb == sendNumb) break;
	}
	if(index == spTlkCpc2Ctrl->poolCtrl.sendNumb) return false;
	return true;
}



bool tlkcpc2_pool_isHaveItem(uint08 *pList, uint08 numb, uint08 item)
{
	int index;

	for(index=0; index<numb; index++){
		if(pList[index] == item) break;
	}
	if(index == numb) return false;
	return true;
}
bool tlkcpc2_pool_pushItem(uint08 *pList, uint08 *pNumb, uint08 item, bool isFirst)
{
	int index;
	uint08 numb;

	numb = *pNumb;
	if(tlkcpc2_pool_isHaveItem(pList, numb, item)){
		tlkapi_error(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_pushItem: repeat push !");
//		tlkapi_array(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_pushItem: ", pList, *pNumb);
		return false;
	}
	if(numb >= TLKCPC2_POOL_ITEM_NUMB){
		tlkapi_fatal(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_pool_pushItem: fatal !!!");
		return false;
	}

	if(*pNumb != 0 && isFirst){
		for(index=numb; index>0; index--){
			pList[index] = pList[index-1];
		}
		pList[0] = item;
	}else{
		pList[numb] = item;
	}
	*pNumb = numb+1;
	
	return true;
}
tlkcpc2_pool_item_t *tlkcpc2_pool_takeItem(uint08 *pList, uint08 *pNumb)
{
	uint08 numb;
	tlkcpc2_pool_item_t *pItem;

	numb = *pNumb;
	if(numb == 0) return nullptr;
	pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[pList[0]];
	numb --;
	if(numb != 0) tmemcpy(&pList[0], &pList[1], numb);
	*pNumb = numb;
	return pItem;
}


void tlkcpc2_pool_printIdleList(void)
{
	int index;
	tlkcpc2_pool_item_t *pItem;
	uint08 buffer[TLKCPC2_POOL_ITEM_NUMB*2+2];

	if(spTlkCpc2Ctrl->poolCtrl.idleNumb == 0)
	{
		tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "IdleList: is empty");
		return;
	}
	for(index=0; index<spTlkCpc2Ctrl->poolCtrl.idleNumb; index++)
	{
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.idleList[index]];
		buffer[index] = pItem->sendNumb;
		buffer[2+spTlkCpc2Ctrl->poolCtrl.idleNumb+index] = pItem->itemNumb;
	}
	buffer[spTlkCpc2Ctrl->poolCtrl.idleNumb] = 0xFF;
	buffer[spTlkCpc2Ctrl->poolCtrl.idleNumb+1] = 0xFF;
	tlkapi_array(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "IdleList:", buffer, spTlkCpc2Ctrl->poolCtrl.idleNumb*2+2);
}
void tlkcpc2_pool_printSendList(void)
{
	int index;
	tlkcpc2_pool_item_t *pItem;
	uint08 buffer[TLKCPC2_POOL_ITEM_NUMB*2+2];

	if(spTlkCpc2Ctrl->poolCtrl.sendNumb == 0)
	{
		tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "SendList: is empty");
		return;
	}
	for(index=0; index<spTlkCpc2Ctrl->poolCtrl.sendNumb; index++)
	{
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.sendList[index]];
		buffer[index] = pItem->sendNumb;
		buffer[2+spTlkCpc2Ctrl->poolCtrl.sendNumb+index] = pItem->itemNumb;
	}
	buffer[spTlkCpc2Ctrl->poolCtrl.sendNumb] = 0xFF;
	buffer[spTlkCpc2Ctrl->poolCtrl.sendNumb+1] = 0xFF;
	tlkapi_array(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "SendList:", buffer, spTlkCpc2Ctrl->poolCtrl.sendNumb*2+2);
}
void tlkcpc2_pool_printCacheList(void)
{
	int index;
	tlkcpc2_pool_item_t *pItem;
	uint08 buffer[TLKCPC2_POOL_ITEM_NUMB*2+2];

	if(spTlkCpc2Ctrl->poolCtrl.cacheNumb == 0)
	{
		tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "CacheList: is empty");
		return;
	}
	for(index=0; index<spTlkCpc2Ctrl->poolCtrl.cacheNumb; index++)
	{
		pItem = &spTlkCpc2Ctrl->poolCtrl.itemList[spTlkCpc2Ctrl->poolCtrl.cacheList[index]];
		buffer[index] = pItem->sendNumb;
		buffer[2+spTlkCpc2Ctrl->poolCtrl.cacheNumb+index] = pItem->itemNumb;
	}
	buffer[spTlkCpc2Ctrl->poolCtrl.cacheNumb] = 0xFF;
	buffer[spTlkCpc2Ctrl->poolCtrl.cacheNumb+1] = 0xFF;
	tlkapi_array(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "CacheList:", buffer, spTlkCpc2Ctrl->poolCtrl.cacheNumb*2+2);
}


#endif //#if (TLK_TEST_ATS_ENABLE)


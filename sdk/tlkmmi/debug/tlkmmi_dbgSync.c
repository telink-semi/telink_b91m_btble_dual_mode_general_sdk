/********************************************************************************************************
 * @file	tlkmmi_dbgSync.c
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
#if (TLKMMI_DEBUG_ENABLE)
#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkdev/tlkdev.h"
#include "tlkmmi_dbg.h"
#include "tlkmmi_dbgAdapt.h"
#include "tlkmmi_dbgSync.h"
#include "tlkmmi_dbgMsgInner.h"


static bool tlkmmi_debug_syncTimer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmmi_debug_syncListInfo(void);
static void tlkmmi_debug_syncItemInfo(void);


static tlkmmi_debug_sync_t sTlkMmiDebugSync;


int tlkmmi_debug_syncInit(void)
{	
	tmemset(&sTlkMmiDebugSync, 0, sizeof(tlkmmi_debug_sync_t));
	
	tlkmmi_debug_adaptInitTimer(&sTlkMmiDebugSync.timer, tlkmmi_debug_syncTimer, 0, TLKMMI_DEBUG_SYNC_TIMEOUT);
	
	return TLK_ENONE;
}

void tlkmmi_debug_syncReset(void)
{
	sTlkMmiDebugSync.syncBusys = TLKMMI_DEBUG_SYNC_BUSY_NONE;
	sTlkMmiDebugSync.syncFlags = TLKMMI_DEBUG_SYNC_FLAG_NONE;
	sTlkMmiDebugSync.listSerial = 0;
	sTlkMmiDebugSync.listNumber = 0;
	sTlkMmiDebugSync.itemSerial = 0;
	sTlkMmiDebugSync.itemNumber = 0;
	sTlkMmiDebugSync.syncListID = 0;
	tlkmmi_debug_adaptRemoveTimer(&sTlkMmiDebugSync.timer);
}



int tlkmmi_debug_syncList(bool isVCD, bool syncItem)
{
	tlkmmi_debug_syncReset();
	
	if(isVCD) sTlkMmiDebugSync.syncFlags |= TLKMMI_DEBUG_SYNC_FLAG_VCD;
	else sTlkMmiDebugSync.syncFlags |= TLKMMI_DEBUG_SYNC_FLAG_LOG;
		
	sTlkMmiDebugSync.listNumber = tlk_debug_getListNumb();
	sTlkMmiDebugSync.syncBusys |= TLKMMI_DEBUG_SYNC_BUSY_LIST;
	if(syncItem){
		sTlkMmiDebugSync.syncFlags |= TLKMMI_DEBUG_SYNC_FLAG_ITEM;
		sTlkMmiDebugSync.itemNumber = tlk_debug_getItemNumb(0);
		sTlkMmiDebugSync.itemSerial = 0;
		sTlkMmiDebugSync.syncListID = 0;
		sTlkMmiDebugSync.syncBusys |= TLKMMI_DEBUG_SYNC_BUSY_ITEM;
	}
	tlkmmi_debug_adaptInsertTimer(&sTlkMmiDebugSync.timer);
	
	return TLK_ENONE;
}

int tlkmmi_debug_syncItem(bool isVCD, uint16 listID)
{
	tlkmmi_debug_syncReset();

	if(listID != 0xFFFF){
		if(listID >= tlk_debug_getListNumb()) return -TLK_EPARAM;
		sTlkMmiDebugSync.syncListID = listID;
		sTlkMmiDebugSync.syncFlags |= TLKMMI_DEBUG_SYNC_FLAG_SINGLE;
	}
	
	if(isVCD) sTlkMmiDebugSync.syncFlags |= TLKMMI_DEBUG_SYNC_FLAG_VCD;
	else sTlkMmiDebugSync.syncFlags |= TLKMMI_DEBUG_SYNC_FLAG_LOG;
	sTlkMmiDebugSync.syncBusys |= TLKMMI_DEBUG_SYNC_BUSY_ITEM;

	sTlkMmiDebugSync.listNumber = tlk_debug_getListNumb();
	sTlkMmiDebugSync.itemNumber = tlk_debug_getItemNumb(sTlkMmiDebugSync.syncListID);
	sTlkMmiDebugSync.itemSerial = 0;

	tlkmmi_debug_adaptInsertTimer(&sTlkMmiDebugSync.timer);
	
	return TLK_ENONE;
}


static bool tlkmmi_debug_syncTimer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if((sTlkMmiDebugSync.syncBusys & TLKMMI_DEBUG_SYNC_BUSY_LIST) != 0){
		tlkmmi_debug_syncListInfo();
	}else if((sTlkMmiDebugSync.syncBusys & TLKMMI_DEBUG_SYNC_BUSY_ITEM) != 0){
		tlkmmi_debug_syncItemInfo();
	}else{
		return false;
	}
	return true;
}

static void tlkmmi_debug_syncListInfo(void)
{
	int ret;
	uint16 lcount;
	uint16 nameLen;
	uint16 buffLen;
	uint08 buffer[TLKMMI_DEBUG_SEND_BUFFLEN];
	const char *pName;

	if(sTlkMmiDebugSync.listSerial >= sTlkMmiDebugSync.listNumber){
		sTlkMmiDebugSync.syncBusys &= ~TLKMMI_DEBUG_SYNC_BUSY_LIST;
		return;
	}

	lcount = 0;
	buffLen = 0;
	if((sTlkMmiDebugSync.syncFlags & TLKMMI_DEBUG_SYNC_FLAG_VCD) != 0){
		buffer[buffLen++] = 0x01; //VCD
	}else{
		buffer[buffLen++] = 0x00; //LOG
	}
	buffer[buffLen++] = lcount;
	buffer[buffLen++] = (sTlkMmiDebugSync.listNumber & 0xFF);
	buffer[buffLen++] = (sTlkMmiDebugSync.listNumber & 0xFF00) >> 8;
	while(sTlkMmiDebugSync.listSerial+lcount < sTlkMmiDebugSync.listNumber && buffLen+6 <= TLKMMI_DEBUG_SEND_BUFFLEN){
		buffer[buffLen++] = ((sTlkMmiDebugSync.listSerial+lcount) & 0xFF);
		buffer[buffLen++] = ((sTlkMmiDebugSync.listSerial+lcount) & 0xFF00) >> 8;
		buffer[buffLen++] = tlk_debug_getItemNumb(sTlkMmiDebugSync.listSerial+lcount);
		buffer[buffLen++] = tlk_debug_getListFlag(sTlkMmiDebugSync.listSerial+lcount);
		buffer[buffLen++] = tlk_debug_logListIsEnable(sTlkMmiDebugSync.listSerial+lcount);
		pName = tlk_debug_getListName(sTlkMmiDebugSync.listSerial+lcount);
		if(pName == nullptr) nameLen = 0;
		else nameLen = tstrlen(pName);
		if(lcount != 0 && buffLen+nameLen+1 > TLKMMI_DEBUG_SEND_BUFFLEN) break;
		if(buffLen+nameLen+1 > TLKMMI_DEBUG_SEND_BUFFLEN) nameLen = TLKMMI_DEBUG_SEND_BUFFLEN-buffLen-1;
		buffer[buffLen++] = nameLen;
		if(nameLen != 0){
			tmemcpy(buffer+buffLen, pName, nameLen);
			buffLen += nameLen;
		}
		lcount ++;
	}
	if(lcount == 0){
		sTlkMmiDebugSync.syncBusys &= ~TLKMMI_DEBUG_SYNC_BUSY_LIST;
		return;
	}

	tlkapi_trace(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_syncListInfo: number[%d], serial[%d], lcount[%d]", 
		sTlkMmiDebugSync.listNumber, sTlkMmiDebugSync.listSerial, lcount);

	buffer[1] = lcount;
	ret = tlkmmi_debug_sendCommEvt(TLKPRT_COMM_EVTID_DBG_LIST_REPORT, buffer, buffLen);
	if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
		sTlkMmiDebugSync.listSerial += lcount;
		
	}
}
static void tlkmmi_debug_syncItemInfo(void)
{
	int ret;
	uint16 icount;
	uint16 nameLen;
	uint16 buffLen;
	uint08 buffer[TLKMMI_DEBUG_SEND_BUFFLEN];
	const char *pName;

	icount = 0;
	buffLen = 0;
	if((sTlkMmiDebugSync.syncFlags & TLKMMI_DEBUG_SYNC_FLAG_VCD) != 0){
		buffer[buffLen++] = 0x01; //VCD
	}else{
		buffer[buffLen++] = 0x00; //LOG
	}
	buffer[buffLen++] = icount;
	buffer[buffLen++] = sTlkMmiDebugSync.itemNumber;
	buffer[buffLen++] = (sTlkMmiDebugSync.syncListID & 0xFF);
	buffer[buffLen++] = (sTlkMmiDebugSync.syncListID & 0xFF00) >> 8;
	while(sTlkMmiDebugSync.itemSerial+icount < sTlkMmiDebugSync.itemNumber && buffLen+4 <= TLKMMI_DEBUG_SEND_BUFFLEN){
		buffer[buffLen++] = ((sTlkMmiDebugSync.itemSerial+icount) & 0xFF);
		buffer[buffLen++] = tlk_debug_getItemFlag(sTlkMmiDebugSync.syncListID, sTlkMmiDebugSync.itemSerial+icount);
		buffer[buffLen++] = tlk_debug_logItemIsEnable(sTlkMmiDebugSync.syncListID, sTlkMmiDebugSync.itemSerial+icount);
		pName = tlk_debug_getItemName(sTlkMmiDebugSync.syncListID, sTlkMmiDebugSync.itemSerial+icount);
		if(pName == nullptr) nameLen = 0;
		else nameLen = tstrlen(pName);
		if(icount != 0 && buffLen+nameLen+1 > TLKMMI_DEBUG_SEND_BUFFLEN) break;
		if(buffLen+nameLen+1 > TLKMMI_DEBUG_SEND_BUFFLEN) nameLen = TLKMMI_DEBUG_SEND_BUFFLEN-buffLen-1;
		buffer[buffLen++] = nameLen;
		if(nameLen != 0){
			tmemcpy(buffer+buffLen, pName, nameLen);
			buffLen += nameLen;
		}
		icount ++;
	}

	tlkapi_trace(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_syncItemInfo: listID[%d], itemNumb[%d], itemSerial[%d], icount[%d]", 
		sTlkMmiDebugSync.syncListID, sTlkMmiDebugSync.itemNumber, sTlkMmiDebugSync.itemSerial, icount);

	buffer[1] = icount;
	ret = tlkmmi_debug_sendCommEvt(TLKPRT_COMM_EVTID_DBG_ITEM_REPORT, buffer, buffLen);
	if(ret == TLK_ENONE || ret == -TLK_ENOSUPPORT){
		sTlkMmiDebugSync.itemSerial += icount;
		if(sTlkMmiDebugSync.itemSerial >= sTlkMmiDebugSync.itemNumber){
			if((sTlkMmiDebugSync.syncFlags & TLKMMI_DEBUG_SYNC_FLAG_SINGLE) != 0){
				sTlkMmiDebugSync.syncBusys &= ~TLKMMI_DEBUG_SYNC_BUSY_ITEM;
				return;
			}
			sTlkMmiDebugSync.syncListID ++;
			if(sTlkMmiDebugSync.syncListID >= sTlkMmiDebugSync.listNumber){
				sTlkMmiDebugSync.syncBusys &= ~TLKMMI_DEBUG_SYNC_BUSY_ITEM;
				return;
			}
			sTlkMmiDebugSync.itemNumber = tlk_debug_getItemNumb(sTlkMmiDebugSync.syncListID);
			sTlkMmiDebugSync.itemSerial = 0;
		}
	}
}




#endif //#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)
#endif //#if (TLKMMI_DEBUG_ENABLE)

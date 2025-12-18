/********************************************************************************************************
 * @file	tlkmdi_btinq.c
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
#if (TLK_MDI_BTINQ_ENABLE)
#include "tlkmdi/bt/tlkmdi_btadapt.h"
#include "tlkmdi/bt/tlkmdi_btinq.h"

#include "tlkstk/bt/bth/bth_stdio.h"

#include "tlkstk/hci/hci_cmd.h"


#define TLKMDI_BTINQ_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_INQ << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTINQ_DBG_SIGN       "[MDI]"


static int tlkmdi_btinq_resultEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btinq_completeEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btinq_getNameCompleteEvt(uint08 *pData, uint16 dataLen);
static void tlkmdi_btinq_reportDevice(tlkmdi_btinq_item_t *pItem);
static tlkmdi_btinq_item_t *tlkmdi_btinq_getReportItem(void);

static void tlkmdi_btinq_inquiryProcs(void);
static void tlkmdi_btinq_getNameProcs(void);
static void tlkmdi_btinq_closingProcs(void);


static bool tlkmdi_btinq_timer(tlkapi_timer_t *pTimer, uint32 userArg);


static TlkMmiBtInqReportCallBack sTlkMmiBtInqReportCB;
static TlkMmiBtInqCompleteCallBack sTlkMmiBtInqCompleteCB;

static tlkmdi_btinq_ctrl_t sTlkMdiBtInqCtrl;



int tlkmdi_btinq_init(void)
{
	tmemset(&sTlkMdiBtInqCtrl, 0, sizeof(tlkmdi_btinq_ctrl_t));

	sTlkMmiBtInqReportCB = nullptr;
	sTlkMmiBtInqCompleteCB = nullptr;

	tlkmdi_btadapt_initTimer(&sTlkMdiBtInqCtrl.timer, tlkmdi_btinq_timer, NULL, TLKMDI_BTINQ_TIMEOUT);

	bth_event_regCB(BTH_EVTID_INQUIRY_RESULT,   tlkmdi_btinq_resultEvt);
	bth_event_regCB(BTH_EVTID_INQUIRY_COMPLETE, tlkmdi_btinq_completeEvt);
	bth_event_regCB(BTH_EVTID_GETNAME_COMPLETE, tlkmdi_btinq_getNameCompleteEvt);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btinq_isBusy
 * Descript: Check exist a Inquiry .
 * Params: None.
 * Return: Return true is busy/false is idle.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btinq_isBusy(void)
{
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_IDLE) return false;
	else return true;
}

/******************************************************************************
 * Function: tlkmdi_btinq_start
 * Descript: Begins to retrieve the surrounding BT devices.
 * Params:
 *     @inqType[IN]--The inquiry type.
 *     @rssiThd[IN]--The rssi of signal.
 *     @maxNumb[IN]--The report devices number.
 *     @inqWind[IN]--Window for a single query. (unit:s, inqWind <= 30s )
 *     @inqNumb[IN]--The number of Query Windows. 
 *     @isGetName[IN]--whether get Bt device name or not.
 *     @isSave[IN]--Whether to save the queried BT device to the query list.
 * Return: Return TLK_ENONE is success,other value is false.
 * Others: 
 *     If isSave=FALSE, then GetName also needs to be FALSE. This means that 
 *     the system does not get names for devices that do not need to be saved.
 *     Applicable to connecting a device of an unknown device type.
*******************************************************************************/
int  tlkmdi_btinq_start(uint08 inqType, uint08 rssiThd, uint08 maxNumb,
     uint08 inqWind, uint08 inqNumb, bool isGetName, bool isSave)
{
	uint08 stage;

	if(!isSave && isGetName) return false;
	if(inqWind < 3) inqWind = 3;
	else if(inqWind > 60) inqWind = 60;
	if(inqNumb == 0) inqNumb = 1;
	if(inqNumb > 100) inqNumb = 100;
	if(!isGetName){
		inqWind *= inqNumb;
		inqNumb = 1;
	}
	if(inqWind > 100) inqWind = 100;
	if(maxNumb > TLKMDI_BTINQ_ITEM_NUMB) maxNumb = TLKMDI_BTINQ_ITEM_NUMB;

	tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_start: inqType-%d, rssiThd-%d, maxNumb-%d, inqWind-%d",
			inqType, rssiThd, maxNumb, inqWind);

	if(maxNumb == 0) maxNumb = TLKMDI_BTINQ_ITEM_NUMB;

	sTlkMdiBtInqCtrl.inqType = inqType;
	sTlkMdiBtInqCtrl.curNumb = 0;
	sTlkMdiBtInqCtrl.nameIdx = 0;
	sTlkMdiBtInqCtrl.inqNumb = inqNumb;
	sTlkMdiBtInqCtrl.inqWind = ((uint32)inqWind*1000)/TLKMDI_BTINQ_TIMEOUT_MS;
	sTlkMdiBtInqCtrl.maxNumb = maxNumb;

	sTlkMdiBtInqCtrl.saveDev = isSave;
	sTlkMdiBtInqCtrl.rssiThd = rssiThd;
	sTlkMdiBtInqCtrl.getName = isGetName;

	stage = TLKMDI_BTINQ_STAGE_NONE;
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_INQUIRY){
		if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL){
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL;
		}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_DOING){
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_CLOSE;
		}else{
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0;
		}
	}else if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_GETNAME){
		if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_DOING
			|| sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT0
			|| sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_CLOSE){
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL;
		}else{
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0;
		}
	}else if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_CLOSING){
		if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_GETNAME){
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL;
		}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_INQUIRY){
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_CLOSE;
		}else{
			stage = TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0;
		}
	}
	if(stage == TLKMDI_BTINQ_STAGE_NONE) stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
	sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_INQUIRY;
	sTlkMdiBtInqCtrl.stage = stage;
	if(stage == TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0) sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_SWITCH_TIMEOUT;
		
	tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_start ...");
	
	tlkmdi_btadapt_insertTimer(&sTlkMdiBtInqCtrl.timer);

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btinq_close
 * Descript: stop a Inquiry.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_close(void)
{
	uint08 stage;
	
	tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_close: state-%d", sTlkMdiBtInqCtrl.state);
	
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_IDLE) return;
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_CLOSING) return;

	stage = TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER;
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_INQUIRY){
		if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL || sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_CLOSE){
			stage = TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_GETNAME;
		}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0){
			stage = TLKMDI_BTINQ_CLOSING_STAGE_WAIT_GETNAME;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
		}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_DOING){
			stage = TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_INQUIRY;
		}else{
			stage = TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER;
		}
	}else if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_GETNAME){
		if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_CLOSE || sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT0){
			stage = TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_GETNAME;
		}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT1){
			stage = TLKMDI_BTINQ_CLOSING_STAGE_WAIT_GETNAME;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
		}else{
			stage = TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER;
		}
	}
	
	sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_CLOSING;
	sTlkMdiBtInqCtrl.stage = stage;

	sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
	if(stage == TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER){
		sTlkMdiBtInqCtrl.timeout = 0;
		if(sTlkMmiBtInqCompleteCB != nullptr) sTlkMmiBtInqCompleteCB();
		sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_IDLE;
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_STAGE_NONE;
		tlkmdi_btadapt_removeTimer(&sTlkMdiBtInqCtrl.timer);
	}
	
	tlkmdi_btinq_printList();
}


/******************************************************************************
 * Function: tlkmdi_btinq_reset
 * Descript: reset the Inquiry control block.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_reset(void)
{
	tlkmdi_btadapt_removeTimer(&sTlkMdiBtInqCtrl.timer);
	tmemset(&sTlkMdiBtInqCtrl, 0, sizeof(tlkmdi_btinq_ctrl_t));
}

/******************************************************************************
 * Function: tlkmdi_btinq_regCallback
 * Descript: Register the callback.
 * Params: 
 *        @reportCB[IN]--The report callback function.
 *        @completeCB[IN]--Inquiry complete callback function.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_regCallback(TlkMmiBtInqReportCallBack reportCB, TlkMmiBtInqCompleteCallBack completeCB)
{
	sTlkMmiBtInqReportCB = reportCB;
	sTlkMmiBtInqCompleteCB = completeCB;
}

/******************************************************************************
 * Function: tlkmdi_btinq_cleanItems
 * Descript: Clear the inquiry number.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_cleanItems(void)
{
	sTlkMdiBtInqCtrl.curNumb = 0;
}

/******************************************************************************
 * Function: tlkmdi_btinq_getItemCount
 * Descript: Get the idle inquiry count.
 * Params: None.
 * Return: The number of idle count.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btinq_getItemCount(void)
{
	return sTlkMdiBtInqCtrl.curNumb;
}

/******************************************************************************
 * Function: tlkmdi_btinq_getItemCount
 * Descript: Get the idle inquiry count.
 * Params: None.
 * Return: The number of idle count.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btinq_getReadyItemCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<sTlkMdiBtInqCtrl.curNumb; index++){
		if(sTlkMdiBtInqCtrl.item[index].nameLen != 0) count ++;
	}
	return count;
}


/******************************************************************************
 * Function: tlkmdi_btinq_getItemIndex
 * Descript: Get the inquiry item index.
 * Params: 
 *        @pAddr[IN]--The bt address.
 * Return: The index of Item.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btinq_getItemIndex(uint08 *pAddr)
{
	uint08 index;
	for(index=0; index<sTlkMdiBtInqCtrl.curNumb; index++){
		if(tmemcmp(sTlkMdiBtInqCtrl.item[index].btaddr, pAddr, 6) == 0) break;
	}
	if(index == sTlkMdiBtInqCtrl.curNumb) return -1;
	return index;
}

/******************************************************************************
 * Function: tlkmdi_btinq_getItem
 * Descript: Get the inquiry item by index.
 * Params: 
 *        @index[IN]--The index of inquiry control block.
 * Return: The Inquiry control block.
 * Others: None.
*******************************************************************************/
tlkmdi_btinq_item_t *tlkmdi_btinq_getItem(uint08 index)
{
	if(index >= TLKMDI_BTINQ_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtInqCtrl.item[index];
}

/******************************************************************************
 * Function: tlkmdi_btinq_getIdleItem
 * Descript: Get the Idle inquiry item.
 * Params: None.
 * Return: The Idle Inquiry control block.
 * Others: None.
*******************************************************************************/
tlkmdi_btinq_item_t *tlkmdi_btinq_getIdleItem(void)
{
	tlkmdi_btinq_item_t *pItem;

	if(sTlkMdiBtInqCtrl.curNumb >= sTlkMdiBtInqCtrl.maxNumb) return nullptr;
	
	pItem = &sTlkMdiBtInqCtrl.item[sTlkMdiBtInqCtrl.curNumb];
	tmemset(pItem, 0, sizeof(tlkmdi_btinq_item_t));
	
	sTlkMdiBtInqCtrl.curNumb ++;
	
	return pItem;
}

/******************************************************************************
 * Function: tlkmdi_btinq_getUsedItem
 * Descript: Get the Used inquiry item.
 * Params: None.
 * Return: The Used Inquiry control block.
 * Others: None.
*******************************************************************************/
tlkmdi_btinq_item_t *tlkmdi_btinq_getUsedItem(uint08 *pAddr)
{
	uint08 index;
	for(index=0; index<sTlkMdiBtInqCtrl.curNumb; index++){
		if(tmemcmp(pAddr, sTlkMdiBtInqCtrl.item[index].btaddr, 6) == 0) break;
	}
	if(index == sTlkMdiBtInqCtrl.curNumb) return nullptr;
	return &sTlkMdiBtInqCtrl.item[index];
}
static tlkmdi_btinq_item_t *tlkmdi_btinq_getReportItem(void)
{
	uint08 index;
	for(index=0; index<sTlkMdiBtInqCtrl.curNumb; index++){
		if(sTlkMdiBtInqCtrl.item[index].state == TLKMDI_BTINQ_ITEM_STATE_WAIT) break;
	}
	if(index == sTlkMdiBtInqCtrl.curNumb) return nullptr;
	return &sTlkMdiBtInqCtrl.item[index];
}



void tlkmdi_btinq_printList(void)
{
#if (TLK_DBG_LOG_ENABLE)
    int i, k;
	uint08 buffLen;
    char buffer[128];

    tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "inquiry device list<----------------------------");	  
    for(i=0; i<sTlkMdiBtInqCtrl.curNumb; i++){
    	tlkmdi_btinq_item_t *pItem = &sTlkMdiBtInqCtrl.item[i];
        buffLen = tlkapi_sprintf(buffer, "[INQ]->DEVICE %02x %02x %02x %02x %02x %02x, RSSI:%d, NAME:",
                    sTlkMdiBtInqCtrl.item[i].btaddr[0],
                    sTlkMdiBtInqCtrl.item[i].btaddr[1],
                    sTlkMdiBtInqCtrl.item[i].btaddr[2],
                    sTlkMdiBtInqCtrl.item[i].btaddr[3],
                    sTlkMdiBtInqCtrl.item[i].btaddr[4],
                    sTlkMdiBtInqCtrl.item[i].btaddr[5],                    
					(int)sTlkMdiBtInqCtrl.item[i].rssi);
        for(k=0; k < pItem->nameLen && buffLen < 120; k++){
            buffLen += tlkapi_sprintf(buffer+buffLen, "%c", sTlkMdiBtInqCtrl.item[i].btname[k]);
        }
		buffer[buffLen] = 0;
        tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, buffer, 0, 0);	  
    }
    tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "---------------------------->list end");
#endif
}


static bool tlkmdi_btinq_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkmdi_btinq_item_t *pItem = nullptr;
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_IDLE) return false;

//	tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_timer: timeout-%d", sTlkMdiBtInqCtrl.runTimeout);
	do{
		pItem = tlkmdi_btinq_getReportItem();
		if(pItem != nullptr){
			tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_getReportItem: devClass-0x%x", pItem->devClass);
			tlkmdi_btinq_reportDevice(pItem);
		}
	}while(pItem != nullptr);
	
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_INQUIRY){
		tlkmdi_btinq_inquiryProcs();
	}else if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_GETNAME){
		tlkmdi_btinq_getNameProcs();
	}else if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_CLOSING){
		tlkmdi_btinq_closingProcs();
	}
	
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_IDLE){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_timer: over [timer arrive]");
		return false;
	}else{
		return true;
	}
}

static void tlkmdi_btinq_inquiryProcs(void)
{
	if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL){
		tlkmdi_btinq_item_t *pItem = tlkmdi_btinq_getItem(sTlkMdiBtInqCtrl.nameIdx);
		if(pItem == nullptr){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
		}else if(bth_hci_sendRemoteNameReqCancelCmd(pItem->btaddr) == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0){
		if(sTlkMdiBtInqCtrl.timeout != 0) sTlkMdiBtInqCtrl.timeout --;
		if(sTlkMdiBtInqCtrl.timeout == 0){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
			sTlkMdiBtInqCtrl.timeout = sTlkMdiBtInqCtrl.inqWind;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_START){
		if(sTlkMdiBtInqCtrl.inqNumb == 0 || sTlkMdiBtInqCtrl.curNumb >= sTlkMdiBtInqCtrl.maxNumb){
			sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_IDLE;
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_STAGE_NONE;
			sTlkMdiBtInqCtrl.timeout = 0;
		}else if(bth_hci_sendInquiryCmd(INQUIRY_LENGTH_30S72, 100) == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_DOING;
			sTlkMdiBtInqCtrl.timeout = sTlkMdiBtInqCtrl.inqWind;
			if(sTlkMdiBtInqCtrl.inqNumb != 0) sTlkMdiBtInqCtrl.inqNumb --;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_DOING){
		if(sTlkMdiBtInqCtrl.timeout != 0) sTlkMdiBtInqCtrl.timeout --;
		if(sTlkMdiBtInqCtrl.timeout != 0) return;
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_CLOSE;
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_CLOSE){
		if(bth_hci_sendInquiryCancelCmd() == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_WAIT1;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_WAIT1){
		if(sTlkMdiBtInqCtrl.timeout != 0) sTlkMdiBtInqCtrl.timeout --;		
		if(sTlkMdiBtInqCtrl.timeout != 0) return;
		if(!sTlkMdiBtInqCtrl.getName){
			sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_IDLE;
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_STAGE_NONE;
			sTlkMdiBtInqCtrl.timeout = 0;
		}else if(sTlkMdiBtInqCtrl.curNumb != 0 && tlkmdi_btinq_getReadyItemCount() != sTlkMdiBtInqCtrl.curNumb){
			sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_GETNAME;
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_START;
			sTlkMdiBtInqCtrl.nameIdx = 0;
			tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_inquiryProcs: Switch to GetName");
		}else{
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
			tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_inquiryProcs: Return to inquiry");
		}
	}else{
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_inquiryProcs: Error Stage - %d", sTlkMdiBtInqCtrl.stage);
	}
}
static void tlkmdi_btinq_getNameProcs(void)
{
	if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_START){
		if(sTlkMdiBtInqCtrl.curNumb == 0 || tlkmdi_btinq_getReadyItemCount() == sTlkMdiBtInqCtrl.curNumb){
			sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_INQUIRY;
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
		}else{
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_DOING;
			sTlkMdiBtInqCtrl.nameIdx = sTlkMdiBtInqCtrl.curNumb-1;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_DOING){
		uint08 index;
		tlkmdi_btinq_item_t *pItem = nullptr;
		for(index=sTlkMdiBtInqCtrl.nameIdx; index<0x80; index--){
			if(sTlkMdiBtInqCtrl.item[index].nameLen == 0) break;
		}
		if(index < sTlkMdiBtInqCtrl.curNumb) pItem = &sTlkMdiBtInqCtrl.item[index];
		if(pItem == nullptr){
			sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_INQUIRY;
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
			sTlkMdiBtInqCtrl.nameIdx = sTlkMdiBtInqCtrl.curNumb;
			tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_getNameProcs: Switch to Inquiry [No Idle Device]");
		}else if(bth_hci_sendRemoteNameReqCmd(pItem->btaddr, pItem->smode, pItem->clkOff) == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_WAIT0;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_GETNAME_TIMEOUT;
			sTlkMdiBtInqCtrl.nameIdx = index;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT0){
		if(sTlkMdiBtInqCtrl.timeout != 0) sTlkMdiBtInqCtrl.timeout --;
		if(sTlkMdiBtInqCtrl.timeout != 0) return;
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_CLOSE;
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_CLOSE){
		tlkmdi_btinq_item_t *pItem = tlkmdi_btinq_getItem(sTlkMdiBtInqCtrl.nameIdx);
		if(pItem == nullptr){
			sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_INQUIRY;
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
		}else if(bth_hci_sendRemoteNameReqCancelCmd(pItem->btaddr) == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_WAIT1;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
			if(sTlkMdiBtInqCtrl.nameIdx != 0) sTlkMdiBtInqCtrl.nameIdx--;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT1){
		if(sTlkMdiBtInqCtrl.timeout != 0) sTlkMdiBtInqCtrl.timeout --;
		if(sTlkMdiBtInqCtrl.timeout != 0) return;
		if(sTlkMdiBtInqCtrl.nameIdx != 0 && sTlkMdiBtInqCtrl.nameIdx < sTlkMdiBtInqCtrl.curNumb){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_DOING;
		}else{
			sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_INQUIRY;
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_DOING;
			tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_getNameProcs: Switch to Inquiry [Device Get Over]");
		}
	}else{
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_getNameProcs: Error Stage - %d", sTlkMdiBtInqCtrl.stage);
	}
}
static void tlkmdi_btinq_closingProcs(void)
{
	if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_GETNAME){
		tlkmdi_btinq_item_t *pItem = tlkmdi_btinq_getItem(sTlkMdiBtInqCtrl.nameIdx);
		if(pItem == nullptr){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_INQUIRY;
		}else if(bth_hci_sendRemoteNameReqCancelCmd(pItem->btaddr) == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_CLOSING_STAGE_WAIT_GETNAME;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_WAIT_GETNAME){
		if(sTlkMdiBtInqCtrl.timeout != 0) sTlkMdiBtInqCtrl.timeout --;
		if(sTlkMdiBtInqCtrl.timeout == 0){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_INQUIRY){
		if(bth_hci_sendInquiryCancelCmd() == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_CLOSING_STAGE_WAIT_INQUIRY;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_WAIT_INQUIRY){
		if(sTlkMdiBtInqCtrl.timeout != 0) sTlkMdiBtInqCtrl.timeout --;
		if(sTlkMdiBtInqCtrl.timeout == 0){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER;
		}
	}else if(sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_closingProcs: Inquiry Over!");
		if(sTlkMmiBtInqCompleteCB != nullptr) sTlkMmiBtInqCompleteCB();
		sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_IDLE;
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_STAGE_NONE;
	}else{
//		tlkmdi_btinq_reset();
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_closingProcs: Error Stage - %d", sTlkMdiBtInqCtrl.stage);
	}
}


static int tlkmdi_btinq_resultEvt(uint08 *pData, uint16 dataLen)
{
	uint08 dtype;
	//uint32 majorDType;
	tlkmdi_btinq_item_t *pItem;
	bth_inquiryResultEvt_t *pEvt;

	if(sTlkMdiBtInqCtrl.state != TLKMDI_BTINQ_STATE_INQUIRY || sTlkMdiBtInqCtrl.stage != TLKMDI_BTINQ_INQUIRY_STAGE_DOING){
		tlkapi_error(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_resultEvt: State Error");
		return TLK_ENONE;
	}
	
	pEvt = (bth_inquiryResultEvt_t*)pData;
	pItem = tlkmdi_btinq_getUsedItem(pEvt->mac);
	if(pItem != nullptr && (pItem->nameLen != 0 || pEvt->nameLen == 0)){
		return TLK_ENONE;
	}
	if(pItem == nullptr && (sint08)sTlkMdiBtInqCtrl.rssiThd > (sint08)pEvt->rssi){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_resultEvt: Rssi is Low  %d %d", sTlkMdiBtInqCtrl.rssiThd, pEvt->rssi);
		return TLK_ENONE;
	}
	if(pEvt->dtype == 0){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_resultEvt: error devClass - 0x%x", pEvt->dtype);
		return TLK_ENONE;
	}
	
	dtype = bth_devClassToDevType(pEvt->dtype);
	if(dtype == TLKMDI_BTINQ_DTYPE_PC){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_callback[DType]: PC Device");
	}else if(dtype == TLKMDI_BTINQ_DTYPE_PHONE){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_callback[DType]: Phone Device");
	}else if(dtype == TLKMDI_BTINQ_DTYPE_HEADSET){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_callback[DType]: Headset eDevice");
	}else{
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_callback[DType]: Not Supported Device");
		return TLK_ENONE;
	}
	if(sTlkMdiBtInqCtrl.inqType != 0 && sTlkMdiBtInqCtrl.inqType != dtype){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "Does not match the filter type: %d %d", sTlkMdiBtInqCtrl.inqType, dtype);
		return TLK_ENONE;
	}

	if(!sTlkMdiBtInqCtrl.saveDev){
		tlkmdi_btinq_item_t item;
		item.rssi = pEvt->rssi;
		item.smode = pEvt->smode; //Scan Mode
		item.dtype = dtype;
		item.clkOff = pEvt->clock;
		item.devClass = pEvt->dtype;
		if(pEvt->nameLen <= TLKMDI_BTINQ_NAME_LENS) item.nameLen = pEvt->nameLen;
		else item.nameLen = TLKMDI_BTINQ_NAME_LENS; //Name Length
		tmemcpy(item.btaddr, pEvt->mac, 6);
		if(item.nameLen != 0){
			tmemcpy(item.btname, pEvt->pName, item.nameLen);
		}
		item.btname[item.nameLen] = 0;
		tlkmdi_btinq_reportDevice(&item);
		return TLK_ENONE;
	}
	
	if(pItem == nullptr) pItem = tlkmdi_btinq_getIdleItem();
	if(pItem == nullptr){
		tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "Inquiry Device Is Full!");
		return TLK_ENONE;
	}
	
	pItem->rssi = pEvt->rssi;
	pItem->smode = pEvt->smode; //Scan Mode
	pItem->dtype = dtype;
	pItem->clkOff = pEvt->clock;
	pItem->devClass = pEvt->dtype;
	if(pEvt->nameLen <= TLKMDI_BTINQ_NAME_LENS) pItem->nameLen = pEvt->nameLen;
	else pItem->nameLen = TLKMDI_BTINQ_NAME_LENS; //Name Length
	tmemcpy(pItem->btaddr, pEvt->mac, 6);
	if(pItem->nameLen != 0){
		tmemcpy(pItem->btname, pEvt->pName, pItem->nameLen);
	}
	pItem->btname[pItem->nameLen] = 0;

	if(pItem->nameLen != 0 || !sTlkMdiBtInqCtrl.getName){
		tlkmdi_btinq_reportDevice(pItem);
	}
	if(sTlkMdiBtInqCtrl.curNumb >= sTlkMdiBtInqCtrl.maxNumb){
		if(bth_hci_sendInquiryCancelCmd() == TLK_ENONE){
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_WAIT1;
			sTlkMdiBtInqCtrl.timeout = TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT;
		}else{
			sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_CLOSE;
		}
	}
			
	return TLK_ENONE;
}
static int tlkmdi_btinq_completeEvt(uint08 *pData, uint16 dataLen)
{
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_IDLE) return -TLK_ESTATUS;
	tlkapi_trace(TLKMDI_BTINQ_DBG_FLAG, TLKMDI_BTINQ_DBG_SIGN, "tlkmdi_btinq_completeEvt: state-%d, stage-%d", sTlkMdiBtInqCtrl.state, sTlkMdiBtInqCtrl.stage);
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_CLOSING
		&& (sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_WAIT_INQUIRY
		|| sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER)){
		sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_IDLE;
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_STAGE_NONE;
		sTlkMdiBtInqCtrl.timeout = 0;
		tlkmdi_btadapt_removeTimer(&sTlkMdiBtInqCtrl.timer);
		if(sTlkMmiBtInqCompleteCB != nullptr) sTlkMmiBtInqCompleteCB();
		tlkmdi_btinq_printList();
		return TLK_ENONE;
	}
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_GETNAME && sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT1){
		sTlkMdiBtInqCtrl.timeout = 0;
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_DOING;
	}
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_INQUIRY || !sTlkMdiBtInqCtrl.getName){
		sTlkMdiBtInqCtrl.state = TLKMDI_BTINQ_STATE_INQUIRY;
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
	}
//	tlkmdi_adapt_insertTimer(sTlkMdiBtInqCtrl.timer);
	return TLK_ENONE;
}
static int tlkmdi_btinq_getNameCompleteEvt(uint08 *pData, uint16 dataLen)
{
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_CLOSING && sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_CLOSING_STAGE_WAIT_GETNAME){
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER;
		sTlkMdiBtInqCtrl.timeout = 0;
	}
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_GETNAME && sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT1){
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_DOING;
		sTlkMdiBtInqCtrl.timeout = 0;
	}
	if(sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_INQUIRY && sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL){
		sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_INQUIRY_STAGE_START;
		sTlkMdiBtInqCtrl.timeout = 0;
	}
	if(sTlkMdiBtInqCtrl.getName && sTlkMdiBtInqCtrl.state == TLKMDI_BTINQ_STATE_GETNAME 
		&& sTlkMdiBtInqCtrl.stage == TLKMDI_BTINQ_GETNAME_STAGE_WAIT1){
		if(dataLen != 0 && pData != nullptr && dataLen >= 7 && pData[0] == 0x00){
			tlkmdi_btinq_item_t *pItem = tlkmdi_btinq_getUsedItem(pData+1);
			if(pItem != nullptr && pItem->nameLen == 0){
				pItem->nameLen = dataLen-7;
				if(pItem->nameLen > TLKMDI_BTINQ_NAME_LENS) pItem->nameLen = TLKMDI_BTINQ_NAME_LENS;
				if(pItem->nameLen != 0){
					tmemcpy(pItem->btname, pData+7, pItem->nameLen);
				}
				pItem->btname[pItem->nameLen] = 0;
				tlkmdi_btinq_reportDevice(pItem);
				sTlkMdiBtInqCtrl.stage = TLKMDI_BTINQ_GETNAME_STAGE_DOING;
			}
			if(pItem != nullptr && sTlkMdiBtInqCtrl.nameIdx != 0) sTlkMdiBtInqCtrl.nameIdx --;
		}
	}
	return TLK_ENONE;
}
static void tlkmdi_btinq_reportDevice(tlkmdi_btinq_item_t *pItem)
{
	if(sTlkMmiBtInqReportCB == nullptr){
		pItem->state = TLKMDI_BTINQ_ITEM_STATE_OVER;
	}else{
		if(sTlkMmiBtInqReportCB(pItem->devClass, pItem->rssi, pItem->nameLen, pItem->btaddr, pItem->btname) == TLK_ENONE){
			pItem->state = TLKMDI_BTINQ_ITEM_STATE_OVER;
		}else{
			pItem->state = TLKMDI_BTINQ_ITEM_STATE_WAIT;
		}
	}
}



#endif //#if (TLK_MDI_BTINQ_ENABLE)


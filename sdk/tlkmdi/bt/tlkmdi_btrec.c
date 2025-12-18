/********************************************************************************************************
 * @file	tlkmdi_btrec.c
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
#if (TLK_MDI_BTREC_ENABLE)
#include "tlkmdi/bt/tlkmdi_btadapt.h"
#include "tlkdev/sys/tlkdev_serial.h"
#include "tlkmdi/bt/tlkmdi_btacl.h"
#include "tlkmdi/bt/tlkmdi_btrec.h"

#include "tlkstk/bt/bth/bth_stdio.h"

#include "tlkstk/hci/hci_cmd.h"


extern uint8_t btc_pscan_setSchParam(pagescan_inr_t interval, pagescan_win_t window);
extern uint8_t btc_iscan_setSchParam(inqscan_inr_t interval, inqscan_win_t window);

#define TLKMDI_BTREC_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_REC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTREC_DBG_SIGN       "[MDI]"


static bool tlkmdi_btrec_timer(tlkapi_timer_t *pTimer, uint32 userArg);


static tlkmdi_btrec_t sTlkMdiBtRecCtrl;
static TlkMdiBtRecOverCallback sTlkMdiBtRecOverCB;

/******************************************************************************
 * Function: tlkmdi_btrec_init
 * Descript: Trigger the reconnect initial.
 * Params: None.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_init(void)
{
	sTlkMdiBtRecOverCB = nullptr;
	tmemset(&sTlkMdiBtRecCtrl, 0, sizeof(tlkmdi_btrec_t));
	tlkmdi_btrec_reset();
	tlkmdi_btadapt_initTimer(&sTlkMdiBtRecCtrl.timer, tlkmdi_btrec_timer, (uint32)&sTlkMdiBtRecCtrl, TLKMDI_BTREC_TIMEOUT);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btrec_reset
 * Descript: Trigger reset the reconnect Control Block.
 * Params: None.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_reset(void)
{
	uint32 pageTimeout;
	
	if(sTlkMdiBtRecCtrl.state != TLKMDI_BTREC_STATE_IDLE) return -TLK_ESTATUS;
	
	sTlkMdiBtRecCtrl.state = TLKMDI_BTREC_STATE_IDLE;
	sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_NONE;
	sTlkMdiBtRecCtrl.actMode = TLKMDI_BTREC_ACTIVE_MODE_BOTH;
	sTlkMdiBtRecCtrl.keepTime = TLKMDI_BTREC_KEEP_TIME_DEF;
	sTlkMdiBtRecCtrl.keepMode = TLKMDI_BTREC_KEEP_MODE_BOTH_SCAN;
	sTlkMdiBtRecCtrl.pageTime = TLKMDI_BTREC_PAGE_TIME_DEF;
	sTlkMdiBtRecCtrl.pageWait = TLKMDI_BTREC_PAGE_WAIT;
	sTlkMdiBtRecCtrl.pageCount = TLKMDI_BTREC_PAGE_COUNT_DEF;
	sTlkMdiBtRecCtrl.scanTime = TLKMDI_BTREC_SCAN_TIME_DEF;
	sTlkMdiBtRecCtrl.scanStep = TLKMDI_BTREC_SCAN_STEP_DEF;
	sTlkMdiBtRecCtrl.scanWait = TLKMDI_BTREC_SCAN_WAIT;
	
	pageTimeout = TLKMDI_BTREC_PAGE_TIME_DEF*TLKMDI_BTREC_TIMEOUT;
	sTlkMdiBtRecCtrl.ptimeout = pageTimeout/625;
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btrec_start
 * Descript: Trigger start to reconnect.
 * Params:
 *        @pageAddr[IN]--The peer address.
 *        @devClass[IN]--The device type.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_start(uint08 *pPageAddr, uint32 devClass, bool enPage, bool enScan)
{
	if(enPage && (pPageAddr == nullptr || devClass == 0)){
		tlkapi_error(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_start: failure - param");
		return -TLK_EPARAM;
	}
	
	if(sTlkMdiBtRecCtrl.state != TLKMDI_BTREC_STATE_IDLE && (sTlkMdiBtRecCtrl.actMode & TLKMDI_BTREC_ACTIVE_MODE_PAGE) != 0){
		tlkmdi_btacl_cancel(sTlkMdiBtRecCtrl.pageAddr);
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_start: prepare handle - cancel connect");
	}
	
	if(enPage) sTlkMdiBtRecCtrl.actMode |= TLKMDI_BTREC_ACTIVE_MODE_PAGE;
	else sTlkMdiBtRecCtrl.actMode &= ~TLKMDI_BTREC_ACTIVE_MODE_PAGE;
	if(enScan) sTlkMdiBtRecCtrl.actMode |= TLKMDI_BTREC_ACTIVE_MODE_SCAN;
	else sTlkMdiBtRecCtrl.actMode &= ~TLKMDI_BTREC_ACTIVE_MODE_SCAN;

	if(pPageAddr != nullptr){
		tmemcpy(sTlkMdiBtRecCtrl.pageAddr, pPageAddr, 6);
		sTlkMdiBtRecCtrl.devClass = devClass;
		sTlkMdiBtRecCtrl.tempCount = sTlkMdiBtRecCtrl.pageCount;
	}
	sTlkMdiBtRecCtrl.state = TLKMDI_BTREC_STATE_INIT;
	sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_INIT_CLOSE_SCAN;
	
	sTlkMdiBtRecCtrl.timeout = 0;
	tlkmdi_btadapt_insertTimer(&sTlkMdiBtRecCtrl.timer);
	tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_start: stage-%d", sTlkMdiBtRecCtrl.stage);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btrec_close
 * Descript: Trigger stop reconnect.
 * Params: None.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_close(void)
{
	uint08 closeFlags;
	
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_IDLE){
		return -TLK_ESTATUS;
	}

	closeFlags = 0;	
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_PAGE){
		if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_PAGE_WAIT0
			|| sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_PAGE_CLOSE){
			closeFlags = 1; //CLOSE_PAGE -- TLKMDI_BTREC_STAGE_KEEP_CLOSE_PAGE
		}else if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_PAGE_WAIT1){
			closeFlags = 3; //Wait -- TLKMDI_BTREC_STAGE_KEEP_WAIT
		}else{
			closeFlags = 0; //idle -- TLKMDI_BTREC_STAGE_KEEP_START
		}
	}else if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_SCAN){
		if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_SCAN_WAIT0
			|| sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_SCAN_CLOSE){
			closeFlags = 2; //CLOSE_SCAN -- TLKMDI_BTREC_STAGE_KEEP_CLOSE_SCAN
		}else if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_SCAN_WAIT1){
			closeFlags = 3; //Wait -- TLKMDI_BTREC_STAGE_KEEP_WAIT
		}else{
			closeFlags = 0; //idle -- TLKMDI_BTREC_STAGE_KEEP_START
		}
	}else if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_KEEP){
		if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_KEEP_CLOSE_PAGE){
			closeFlags = 1; //CLOSE_PAGE -- TLKMDI_BTREC_STAGE_KEEP_CLOSE_PAGE
		}else if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_KEEP_CLOSE_SCAN){
			closeFlags = 2; //CLOSE_SCAN -- TLKMDI_BTREC_STAGE_KEEP_CLOSE_SCAN
		}else if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_KEEP_WAIT){
			closeFlags = 3; //Wait -- TLKMDI_BTREC_STAGE_KEEP_WAIT
		}else if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_KEEP_START){
			closeFlags = 0; //idle -- TLKMDI_BTREC_STAGE_KEEP_START
		}else{
			closeFlags = 2; //CLOSE_SCAN -- TLKMDI_BTREC_STAGE_KEEP_CLOSE_SCAN
		}
	}else if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_STOP){
		if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_STOP_PAGE){
			closeFlags = 1; //CLOSE_PAGE -- TLKMDI_BTREC_STAGE_STOP_PAGE
		}else if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_STOP_SCAN){
			closeFlags = 2; //CLOSE_SCAN -- TLKMDI_BTREC_STAGE_STOP_SCAN
		}else if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_STOP_WAIT){
			closeFlags = 3; //idle -- TLKMDI_BTREC_STAGE_KEEP_START
		}else{
			closeFlags = 0; //idle -- TLKMDI_BTREC_STAGE_KEEP_START
		}
	}else{
		closeFlags = 0; //idle -- TLKMDI_BTREC_STAGE_KEEP_START
	}

	sTlkMdiBtRecCtrl.state = TLKMDI_BTREC_STATE_STOP;
	sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_NONE;
	if(closeFlags == 1){
		sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_STOP_PAGE;
	}else if(closeFlags == 2){
		sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_STOP_SCAN;
	}else if(closeFlags == 3){
		sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_STOP_WAIT;
	}else{
		sTlkMdiBtRecCtrl.state = TLKMDI_BTREC_STATE_IDLE;
	}
	if(sTlkMdiBtRecCtrl.state != TLKMDI_BTREC_STATE_IDLE){
		tlkmdi_btadapt_insertTimer(&sTlkMdiBtRecCtrl.timer);
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_close: success - busy stage-%d", sTlkMdiBtRecCtrl.stage);
		return -TLK_EBUSY;
	}else{
		// Stopped
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_close: reconnect is closed");
		return TLK_ENONE;
	}
}

/******************************************************************************
 * Function: tlkmdi_btrec_regCB
 * Descript: Register the reconnect result callback.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btrec_regCB(TlkMdiBtRecOverCallback overCB)
{
	sTlkMdiBtRecOverCB = overCB;
}

/******************************************************************************
 * Function: tlkmdi_btrec_isInBusy
 * Descript: Check the reconnect status.
 * Params: None.
 * Return: true /false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btrec_isInBusy(void)
{
	if(sTlkMdiBtRecCtrl.state != TLKMDI_BTREC_STATE_IDLE) return true;
	else return false;
}

bool tlkmdi_btrec_isInPage(void)
{
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_PAGE) return true;
	return false;
}
bool tlkmdi_btrec_isInScan(void)
{
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_SCAN) return true;
	return false;
}
bool tlkmdi_btrec_isInKeep(void)
{
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_KEEP) return true;
	return false;
}
bool tlkmdi_btrec_isInStop(void)
{
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_STOP) return true;
	return false;
}

/******************************************************************************
 * Function: tlkmdi_btrec_getPageAddr
 * Descript: Get the page device address under reconnecting.
 * Params: None.
 * Return: BT address.
 * Others: None.
*******************************************************************************/
uint08 *tlkmdi_btrec_getPageAddr(void)
{
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_IDLE
		|| (sTlkMdiBtRecCtrl.actMode & TLKMDI_BTREC_ACTIVE_MODE_PAGE) == 0
		|| sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_KEEP
		|| sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_STOP){
		return nullptr;
	}
	return sTlkMdiBtRecCtrl.pageAddr;
}


/******************************************************************************
 * Function: Set the scan parameter.
 * Descript: Set the reconnect parameter.
 * Params: None.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_setKeepMode(TLKMDI_BTREC_KEEP_MODE_ENUM keepMode, uint16 keepTime) //Unit:s
{
	uint08 mode;
	
	if(keepMode == TLKMDI_BTREC_KEEP_MODE_INQUIRY_SCAN){
		mode = INQUIRY_SCAN_ENABLE;
	}else if(keepMode == TLKMDI_BTREC_KEEP_MODE_PAGE_SCAN){
		mode = PAGE_SCAN_ENABLE;
	}else if(keepMode == TLKMDI_BTREC_KEEP_MODE_BOTH_SCAN){
		mode = BOTH_SCAN_ENABLE;
	}else{
		mode = NO_SCAN_ENABLE;
	}
	sTlkMdiBtRecCtrl.keepTime = ((uint32)keepTime*1000)/TLKMDI_BTREC_TIMEOUT_MS;	
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_KEEP || sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_KEEP_RUN){
		sTlkMdiBtRecCtrl.timeout = sTlkMdiBtRecCtrl.keepTime;
	}
	if(sTlkMdiBtRecCtrl.keepMode == mode) return TLK_ENONE;

	sTlkMdiBtRecCtrl.keepMode = mode;
	if(sTlkMdiBtRecCtrl.state == TLKMDI_BTREC_STATE_KEEP){
		if(sTlkMdiBtRecCtrl.stage == TLKMDI_BTREC_STAGE_KEEP_RUN){
			sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_KEEP_CLOSE_SCAN;
		}
	}
	if(sTlkMdiBtRecCtrl.state != TLKMDI_BTREC_STATE_IDLE){
		tlkmdi_btadapt_insertTimer(&sTlkMdiBtRecCtrl.timer);
	}
	return TLK_ENONE;
}
int tlkmdi_btrec_setPageParam(uint16 pageTime, uint08 pageCount)
{
	uint32 pageTimeout;
	
	if(sTlkMdiBtRecCtrl.state != TLKMDI_BTREC_STATE_IDLE) return -TLK_ESTATUS;
	
	pageTime = pageTime/TLKMDI_BTREC_TIMEOUT_MS;
	if(pageTime == 0) pageTime = TLKMDI_BTREC_PAGE_TIME_DEF;
	else if(pageTime < TLKMDI_BTREC_PAGE_TIME_MIN) pageTime = TLKMDI_BTREC_PAGE_TIME_MIN;
	else if(pageTime > TLKMDI_BTREC_PAGE_TIME_MAX) pageTime = TLKMDI_BTREC_PAGE_TIME_MAX;
	pageTimeout = pageTime*TLKMDI_BTREC_TIMEOUT_MS;
	sTlkMdiBtRecCtrl.pageTime = pageTime;
	sTlkMdiBtRecCtrl.ptimeout = pageTimeout/625;
	sTlkMdiBtRecCtrl.pageCount = pageCount;
	
	return TLK_ENONE;
}
int tlkmdi_btrec_setScanParam(uint16 scanTime, uint16 scanStep, bool enInqScan, bool enPageScan)
{
	if(!enInqScan && !enPageScan) return -TLK_EPARAM;
	if(sTlkMdiBtRecCtrl.state != TLKMDI_BTREC_STATE_IDLE) return -TLK_ESTATUS;

	scanTime = scanTime/TLKMDI_BTREC_TIMEOUT_MS;
	scanStep = scanStep/TLKMDI_BTREC_TIMEOUT_MS;
	if(scanTime == 0) scanTime = TLKMDI_BTREC_SCAN_TIME_DEF;
	else if(scanTime < TLKMDI_BTREC_SCAN_TIME_MIN) scanTime = TLKMDI_BTREC_SCAN_TIME_MIN;
	else if(scanTime > TLKMDI_BTREC_SCAN_TIME_MAX) scanTime = TLKMDI_BTREC_SCAN_TIME_MAX;
	if(scanStep > TLKMDI_BTREC_SCAN_STEP_MAX) scanStep = TLKMDI_BTREC_SCAN_STEP_MAX;
	sTlkMdiBtRecCtrl.scanStep = scanStep;
	sTlkMdiBtRecCtrl.scanTime = scanTime;
	sTlkMdiBtRecCtrl.scanMode = 0;
	if(enInqScan)  sTlkMdiBtRecCtrl.scanMode |= INQUIRY_SCAN_ENABLE;
	if(enPageScan) sTlkMdiBtRecCtrl.scanMode |= PAGE_SCAN_ENABLE;
	
	return TLK_ENONE;
}

int tlkmdi_btrec_connectCancel(uint08 btAddr[6])
{
	if(tmemcmp(sTlkMdiBtRecCtrl.pageAddr, btAddr, 6) != 0){
		tlkapi_error(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_connectCancel: no device");
		return -TLK_ENOOBJECT;
	}
	if(tlkmdi_btrec_isInPage() || (tlkmdi_btrec_isInScan() && (sTlkMdiBtRecCtrl.actMode & TLKMDI_BTREC_ACTIVE_MODE_PAGE) != 0)){
		if(tlkmdi_btrec_isInPage()){
			sTlkMdiBtRecCtrl.stage = TLKMDI_BTREC_STAGE_PAGE_WAIT1;
			sTlkMdiBtRecCtrl.timeout = 0;
			tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_connectCancel: restart");
		}else{
			tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_connectCancel: continue");
		}
		return TLK_ENONE;
	}else{
		tlkapi_error(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN,
			"tlkmdi_btrec_connectCancel: error status [%d]", sTlkMdiBtRecCtrl.state);
		return -TLK_ESTATUS;
	}
}

static void tlkmdi_btrec_initProc(tlkmdi_btrec_t *pCtrl);
static void tlkmdi_btrec_pageProc(tlkmdi_btrec_t *pCtrl);
static void tlkmdi_btrec_scanProc(tlkmdi_btrec_t *pCtrl);
static void tlkmdi_btrec_keepProc(tlkmdi_btrec_t *pCtrl);
static void tlkmdi_btrec_stopProc(tlkmdi_btrec_t *pCtrl);
static bool tlkmdi_btrec_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkmdi_btrec_t *pCtrl = (tlkmdi_btrec_t*)userArg;
	
	if(pCtrl->state == TLKMDI_BTREC_STATE_INIT){
		tlkmdi_btrec_initProc(pCtrl);
	}else if(pCtrl->state == TLKMDI_BTREC_STATE_PAGE){
		tlkmdi_btrec_pageProc(pCtrl);
	}else if(pCtrl->state == TLKMDI_BTREC_STATE_SCAN){
		tlkmdi_btrec_scanProc(pCtrl);
	}else if(pCtrl->state == TLKMDI_BTREC_STATE_KEEP){
		tlkmdi_btrec_keepProc(pCtrl);
	}else if(pCtrl->state == TLKMDI_BTREC_STATE_STOP){
		tlkmdi_btrec_stopProc(pCtrl);
	}else{
		tlkapi_error(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_timer: error state %d", pCtrl->state);
	}
	
	if(pCtrl->state == TLKMDI_BTREC_STATE_IDLE){
		if(sTlkMdiBtRecOverCB != nullptr){
			sTlkMdiBtRecOverCB();
		}
	}
	
	if(pCtrl->state != TLKMDI_BTREC_STATE_IDLE) return true;
	return false;
}

static void tlkmdi_btrec_initProc(tlkmdi_btrec_t *pCtrl)
{
	if(pCtrl->stage == TLKMDI_BTREC_STAGE_INIT_CLOSE_SCAN){
		if(bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_DISABLE) == TLK_ENONE){
			pCtrl->timeout = TLKMDI_BTREC_INIT_WAIT;
			pCtrl->stage = TLKMDI_BTREC_STAGE_INIT_WAIT;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_INIT_WAIT){
		if(pCtrl->timeout != 0) pCtrl->timeout --;
		if(pCtrl->timeout == 0){
			if((pCtrl->actMode & TLKMDI_BTREC_ACTIVE_MODE_PAGE) != 0){
				pCtrl->state = TLKMDI_BTREC_STATE_PAGE;
				pCtrl->stage = TLKMDI_BTREC_STAGE_PAGE_START;
				pCtrl->timeout = pCtrl->pageTime;
				tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_initProc: switch to page-start");
			}else if((pCtrl->actMode & TLKMDI_BTREC_ACTIVE_MODE_SCAN) != 0){
				pCtrl->state = TLKMDI_BTREC_STATE_SCAN;
				pCtrl->stage = TLKMDI_BTREC_STAGE_SCAN_START;
				pCtrl->timeout = pCtrl->scanTime;
				tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_initProc: switch to scan-start");
			}else{
				pCtrl->state = TLKMDI_BTREC_STATE_KEEP;
				pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_START;
				tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_initProc: switch to keep-start");
			}
		}
	}
	else{
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_initProc: error stage - %d", pCtrl->stage);
	}
}
static void tlkmdi_btrec_pageProc(tlkmdi_btrec_t *pCtrl)
{
	if(pCtrl->stage == TLKMDI_BTREC_STAGE_PAGE_START){
		if(bth_hci_sendWritePageTimeoutCmd(pCtrl->ptimeout) == TLK_ENONE){
			pCtrl->stage = TLKMDI_BTREC_STAGE_PAGE_WAIT0;
			pCtrl->timeout = pCtrl->pageTime;
			#if (TLK_MDI_BTACL_ENABLE)
			int ret = tlkmdi_btacl_connect(pCtrl->pageAddr, pCtrl->devClass, pCtrl->pageTime*TLKMDI_BTREC_TIMEOUT_MS);
			if(ret != TLK_ENONE && ret != -TLK_EBUSY){
				pCtrl->state = TLKMDI_BTREC_STATE_KEEP;
				pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_START;
				tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_pageProc: force switch to keep-start");
			}
			#endif
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_PAGE_WAIT0){
		if(pCtrl->timeout != 0) pCtrl->timeout --;
		if(pCtrl->timeout == 0){
			pCtrl->stage = TLKMDI_BTREC_STAGE_PAGE_CLOSE;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_PAGE_CLOSE){
		#if (TLK_MDI_BTACL_ENABLE)
		tlkmdi_btacl_cancel(pCtrl->pageAddr);
		#endif
		pCtrl->stage = TLKMDI_BTREC_STAGE_PAGE_WAIT1;
		pCtrl->timeout = pCtrl->pageWait;
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_PAGE_WAIT1){
		if(pCtrl->timeout != 0) pCtrl->timeout --;
		if(pCtrl->timeout == 0){
			if(pCtrl->tempCount == 0 || (--pCtrl->tempCount) != 0){
				if((pCtrl->actMode & TLKMDI_BTREC_ACTIVE_MODE_SCAN) != 0){
					tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_pageProc: switch to scan-start");
					pCtrl->state = TLKMDI_BTREC_STATE_SCAN;
					pCtrl->stage = TLKMDI_BTREC_STAGE_SCAN_START;
				}else{
					tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_pageProc: switch to scan-page");
					pCtrl->state = TLKMDI_BTREC_STATE_PAGE;
					pCtrl->stage = TLKMDI_BTREC_STAGE_PAGE_START;
				}
			}else{
				pCtrl->state = TLKMDI_BTREC_STATE_KEEP;
				pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_START;
				tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_pageProc: switch to keep-start");
			}
		}
	}
	else{
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_pageProc: error stage - %d", pCtrl->stage);
	}
}
static void tlkmdi_btrec_scanProc(tlkmdi_btrec_t *pCtrl)
{
	if(pCtrl->stage == TLKMDI_BTREC_STAGE_SCAN_START){
		if(bth_hci_sendWriteScanEnableCmd(sTlkMdiBtRecCtrl.scanMode) == TLK_ENONE){
			btc_pscan_setSchParam(PAGESCAN_INR_143MS125, 32);
			btc_iscan_setSchParam(INQSCAN_INR_143MS125, 32);
			pCtrl->stage = TLKMDI_BTREC_STAGE_SCAN_WAIT0;
			pCtrl->timeout = pCtrl->scanTime+pCtrl->scanStep*pCtrl->scanCount;
			pCtrl->scanCount ++;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_SCAN_WAIT0){
		if(pCtrl->timeout != 0) pCtrl->timeout --;
		if(pCtrl->timeout == 0){
			pCtrl->stage = TLKMDI_BTREC_STAGE_SCAN_CLOSE;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_SCAN_CLOSE){
		if(bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_DISABLE) == TLK_ENONE){
			pCtrl->stage = TLKMDI_BTREC_STAGE_SCAN_WAIT1;
			pCtrl->timeout = pCtrl->scanWait;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_SCAN_WAIT1){
		if(pCtrl->timeout != 0) pCtrl->timeout --;
		if(pCtrl->timeout == 0){
			if((pCtrl->actMode & TLKMDI_BTREC_ACTIVE_MODE_PAGE) == 0){
				pCtrl->state = TLKMDI_BTREC_STATE_KEEP;
				pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_START;
				tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_scanProc: switch to keep-start");
			}else{
				pCtrl->state = TLKMDI_BTREC_STATE_PAGE;
				pCtrl->stage = TLKMDI_BTREC_STAGE_PAGE_START;
				tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_scanProc: switch to page-start");
			}
		}
	}else{
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_scanProc: error stage - %d", pCtrl->stage);
	}
}
static void tlkmdi_btrec_keepProc(tlkmdi_btrec_t *pCtrl)
{
	if(pCtrl->stage == TLKMDI_BTREC_STAGE_KEEP_CLOSE_PAGE){
		#if (TLK_MDI_BTACL_ENABLE)
		tlkmdi_btacl_cancel(pCtrl->pageAddr);
		#endif
		pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_WAIT;
		pCtrl->timeout = TLKMDI_BTREC_KEEP_WAIT1;
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_KEEP_CLOSE_SCAN){
		if(bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_DISABLE) == TLK_ENONE){
			pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_WAIT;
			pCtrl->timeout = TLKMDI_BTREC_KEEP_WAIT;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_KEEP_WAIT){
		if(pCtrl->timeout != 0) pCtrl->timeout --;
		if(pCtrl->timeout == 0){
			pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_START;
			pCtrl->timeout = pCtrl->keepTime;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_KEEP_START){
		if(pCtrl->keepMode == NO_SCAN_ENABLE){
			// Stopped
			pCtrl->state = TLKMDI_BTREC_STATE_IDLE;
			pCtrl->stage = TLKMDI_BTREC_STAGE_NONE;
			tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_keepProc: reconnect is closed");
		}else{
			btc_pscan_setSchParam(PAGESCAN_INR_143MS125, 32);
			btc_iscan_setSchParam(INQSCAN_INR_143MS125, 32);
			if(bth_hci_sendWriteScanEnableCmd(pCtrl->keepMode) == TLK_ENONE){
				pCtrl->stage = TLKMDI_BTREC_STAGE_KEEP_RUN;
				pCtrl->timeout = pCtrl->keepTime;
			}
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_KEEP_RUN){
		if(pCtrl->timeout != 0){
			pCtrl->timeout --;
			if(pCtrl->timeout == 0){
				pCtrl->state = TLKMDI_BTREC_STATE_STOP;
				pCtrl->stage = TLKMDI_BTREC_STAGE_STOP_SCAN;
			}
		}
	}else{
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_keepProc: error stage - %d", pCtrl->stage);
	}
}
static void tlkmdi_btrec_stopProc(tlkmdi_btrec_t *pCtrl)
{
	if(pCtrl->stage == TLKMDI_BTREC_STAGE_STOP_PAGE){
		tlkmdi_btacl_cancel(pCtrl->pageAddr);
		pCtrl->stage = TLKMDI_BTREC_STAGE_STOP_WAIT;
		pCtrl->timeout = TLKMDI_BTREC_STOP_WAIT;
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_STOP_SCAN){
//		tlkmdi_btacl_cancel(pCtrl->pageAddr);
		if(bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_DISABLE) == TLK_ENONE){
			pCtrl->stage = TLKMDI_BTREC_STAGE_STOP_WAIT;
			pCtrl->timeout = TLKMDI_BTREC_STOP_WAIT;
		}
	}else if(pCtrl->stage == TLKMDI_BTREC_STAGE_STOP_WAIT){
		if(pCtrl->timeout != 0) pCtrl->timeout--;
		if(pCtrl->timeout == 0){
			// Stopped
			pCtrl->state = TLKMDI_BTREC_STATE_IDLE;
			pCtrl->stage = TLKMDI_BTREC_STAGE_NONE;
			tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_stopProc: reconnect is closed");
		}
	}else{
		tlkapi_trace(TLKMDI_BTREC_DBG_FLAG, TLKMDI_BTREC_DBG_SIGN, "tlkmdi_btrec_stopProc: error stage - %d", pCtrl->stage);
	}	
}



#endif //#if (TLK_MDI_BTREC_ENABLE)


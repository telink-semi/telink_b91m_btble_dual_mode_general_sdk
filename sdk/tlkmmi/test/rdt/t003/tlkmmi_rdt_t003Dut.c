/********************************************************************************************************
 * @file	tlkmmi_rdt_t003Dut.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../../tlkmmi_testStdio.h"
#if (TLK_TEST_RDT_ENABLE)
#include "../tlkmmi_rdtStd.h"
#if (TLKMMI_RDT_CASE_T003_ENABLE)
#include "../bt/tlkmmi_rdt_bt.h"
#include "tlkmmi_rdt_t003.h"
#include "tlkmmi_rdt_t003Dut.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"
#include "tlkstk/hci/hci_cmd.h"
#include "drivers.h"

extern int rand(void);

static bool tlkmmi_rdt_t003DutQueue(tlkapi_queue_t *pQueue, uint32 userArg);
static int tlkmmi_rdt_t003DutAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003DutAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003DutAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003DutAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003DutPrfConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003DutPrfDisconnEvt(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003DutSppDataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003DutSetSendStartDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003DutSetSendStopDeal(uint08 *pData, uint16 dataLen);


static tlkmmi_rdt_t003DutSend_t sTlkMmiRdtT003DutSend1 = { //Send Ctrl For AUT1
	false, //Enable: 
	TLKMMI_RDT_T003_METHOD_UNFIX_LENGTH_UNFIX_INTERVAL, //method:
	0, //number: 0-Send Always
	256, //minLength:
	TLKMMI_RDT_T003_SEND_BUFF_LEN, //maxLength:
	0, //minIntval: Unit-ms
	30, //maxIntval: Unit-ms
};
static tlkmmi_rdt_t003DutSend_t sTlkMmiRdtT003DutSend2 = { //Send Ctrl For AUT2
	false, //Enable: 
	TLKMMI_RDT_T003_METHOD_UNFIX_LENGTH_UNFIX_INTERVAL, //method:
	0, //number: 0-Send Always
	256, //minLength:
	TLKMMI_RDT_T003_SEND_BUFF_LEN, //maxLength:
	0, //minIntval: 
	30, //maxIntval: 
};
static tlkmmi_rdt_t003Dut_t sTlkMmiRdtT003Dut;



void tlkmmi_rdt_t003DutStart(void)
{
	uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutStart");

	sTlkMmiRdtT003Dut.isStart = true;
	sTlkMmiRdtT003Dut.dbgTimer = 0;
	sTlkMmiRdtT003Dut.aut1.handle = 0;
	sTlkMmiRdtT003Dut.aut2.handle = 0;
	sTlkMmiRdtT003Dut.aut1.flags  = TLKMMI_RDT_T003_FLAG_NONE;
	sTlkMmiRdtT003Dut.aut1.busys  = TLKMMI_RDT_T003_BUSY_NONE;
	sTlkMmiRdtT003Dut.aut2.flags  = TLKMMI_RDT_T003_FLAG_NONE;
	sTlkMmiRdtT003Dut.aut2.busys  = TLKMMI_RDT_T003_BUSY_NONE;
	sTlkMmiRdtT003Dut.aut1.timer  = 0;
	sTlkMmiRdtT003Dut.aut2.timer  = 0;
	
	bth_hci_sendWriteLocalNameCmd(TLKMMI_RDT_T003_BTNAME_DUT);
	bth_hci_sendSetBtAddrCmd(dutAddr);
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_RDT_T003_BTDEV_CLASS_DUT);
	bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_t003DutAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_t003DutAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_t003DutAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_t003DutAclDisconnEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  tlkmmi_rdt_t003DutPrfConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  tlkmmi_rdt_t003DutPrfDisconnEvt);
	btp_spp_regDataCB(tlkmmi_rdt_t003DutSppDataCB);

	tlkmmi_test_adaptInitQueue(&sTlkMmiRdtT003Dut.queue, tlkmmi_rdt_t003DutQueue, nullptr);
	bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
}
void tlkmmi_rdt_t003DutClose(void)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutClose");
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  nullptr);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  nullptr);

	sTlkMmiRdtT003Dut.isStart = false;
	if(sTlkMmiRdtT003Dut.aut1.handle != 0){
		bth_acl_disconn(sTlkMmiRdtT003Dut.aut1.handle, 0x00);
		sTlkMmiRdtT003Dut.aut1.handle = 0;
		sTlkMmiRdtT003Dut.aut1.busys = TLKMMI_RDT_T003_BUSY_NONE;
		sTlkMmiRdtT003Dut.aut1.flags  = TLKMMI_RDT_T003_FLAG_NONE;
	}
	if(sTlkMmiRdtT003Dut.aut2.handle != 0){
		bth_acl_disconn(sTlkMmiRdtT003Dut.aut2.handle, 0x00);
		sTlkMmiRdtT003Dut.aut2.handle = 0;
		sTlkMmiRdtT003Dut.aut2.busys = TLKMMI_RDT_T003_BUSY_NONE;
		sTlkMmiRdtT003Dut.aut2.flags  = TLKMMI_RDT_T003_FLAG_NONE;
	}
	bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_DISABLE);
}
void tlkmmi_rdt_t003DutTimer(void)
{
	if(!sTlkMmiRdtT003Dut.isStart) return;
	if(sTlkMmiRdtT003Dut.dbgTimer != 0 && !clock_time_exceed(sTlkMmiRdtT003Dut.dbgTimer, 3000000)){
		return;
	}
	sTlkMmiRdtT003Dut.dbgTimer = clock_time()|1;
	if(sTlkMmiRdtT003Dut.aut1.handle != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"DUT-Flags1[0x%02x],DUT->AUT1(0x%08x), AUT1->DUT(0x%08x), Fail[0x%08x]",
			sTlkMmiRdtT003Dut.aut1.flags, sTlkMmiRdtT003Dut.aut1.sendNumb,
			sTlkMmiRdtT003Dut.aut1.recvNumb, sTlkMmiRdtT003Dut.aut1.sendFail);
		
	}
	if(sTlkMmiRdtT003Dut.aut2.handle != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"DUT-Flags2[0x%02x],DUT->AUT2(0x%08x), AUT2->DUT(0x%08x), Fail[0x%08x]",
			sTlkMmiRdtT003Dut.aut2.flags, sTlkMmiRdtT003Dut.aut2.sendNumb, 
			sTlkMmiRdtT003Dut.aut2.recvNumb, sTlkMmiRdtT003Dut.aut2.sendFail);
	}
}
void tlkmmi_rdt_t003DutInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKMMI_RDT_T003_MSGID_SET_SEND_START){
		tlkmmi_rdt_t003DutSetSendStartDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_T003_MSGID_SET_SEND_STOP){
		tlkmmi_rdt_t003DutSetSendStopDeal(pData, dataLen);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutInput: unknown msg [0x%x]", msgID);
	}
}


static void tlkmmi_rdt_t003DutSetSendStartDeal(uint08 *pData, uint16 dataLen)
{
	uint08 srcRole;
	uint08 dstRole;
	uint08 method;
	uint16 number;
	uint16 minLength;
	uint16 maxLength;
	uint32 minIntval;
	uint32 maxIntval;
	tlkmmi_rdt_t003DutSend_t *pSend;
	tlkmmi_rdt_t003DutUnit_t *pUnit;

	if(dataLen < 13){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutSetSendStartDeal: error length [%d]", dataLen);
		return;
	}

	srcRole = pData[0];
	dstRole = pData[1];
	if(srcRole != TLKMMI_RDT_ROLE_DUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutSetSendStartDeal: SrcRole Error [%d]", srcRole);
		return;
	}
	if(dstRole != TLKMMI_RDT_ROLE_AUT1 && dstRole != TLKMMI_RDT_ROLE_AUT2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutSetSendStartDeal: DstRole Error [%d]", srcRole);
		return;
	}
	
	method = pData[2];
	ARRAY_TO_UINT16L(pData, 3, number);
	ARRAY_TO_UINT16L(pData, 5, minLength);
	ARRAY_TO_UINT16L(pData, 7, maxLength);
	ARRAY_TO_UINT16L(pData, 9, minIntval);
	ARRAY_TO_UINT16L(pData, 11, maxIntval);
	if(maxIntval > TLKMMI_RDT_T003_SEND_BUFF_LEN){
		maxLength = TLKMMI_RDT_T003_SEND_BUFF_LEN;
	}
	if(method > 3){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN,
			"tlkmmi_rdt_t003DutSetSendStartDeal: method Error [%d]", method);
		return;
	}
	if(maxLength < minLength){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN,
			"tlkmmi_rdt_t003DutSetSendStartDeal: Length Error [%d-%d]", 
			minLength, maxLength);
		return;
	}
	if(maxIntval < minIntval){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t003DutSetSendStartDeal: Interval Error [%d-%d]",
			minIntval, maxIntval);
		return;
	}

	if(dstRole == TLKMMI_RDT_ROLE_AUT1){
		pSend = &sTlkMmiRdtT003DutSend1;
		pUnit = &sTlkMmiRdtT003Dut.aut1;
	}else{
		pSend = &sTlkMmiRdtT003DutSend2;
		pUnit = &sTlkMmiRdtT003Dut.aut2;
	}

	pSend->enable = true;
	pSend->method = method;
	pSend->number = number;
	pSend->minLength = minLength;
	pSend->maxLength = maxLength;
	pSend->minIntval = minIntval;
	pSend->maxIntval = maxIntval;
	if((pUnit->flags & TLKMMI_RDT_T003_FLAG_SPP) != 0){
		pUnit->flags |= TLKMMI_RDT_T003_FLAG_SEND;
		pUnit->timer = 0;
		pUnit->intval = (pSend->minIntval+pSend->maxIntval)*1000/2;
		pUnit->length = (pSend->minLength+pSend->maxLength)/2;
		tlkmmi_test_adaptAppendQueue(&sTlkMmiRdtT003Dut.queue);
	}
}
static void tlkmmi_rdt_t003DutSetSendStopDeal(uint08 *pData, uint16 dataLen)
{
	uint08 srcRole;
	uint08 dstRole;
	tlkmmi_rdt_t003DutSend_t *pSend;
	tlkmmi_rdt_t003DutUnit_t *pUnit;

	if(dataLen < 2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutSetSendStopDeal: error length [%d]", dataLen);
		return;
	}

	srcRole = pData[0];
	dstRole = pData[1];
	if(srcRole != TLKMMI_RDT_ROLE_DUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutSetSendStopDeal: SrcRole Error [%d]", srcRole);
		return;
	}
	if(dstRole != TLKMMI_RDT_ROLE_AUT1 && dstRole != TLKMMI_RDT_ROLE_AUT2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutSetSendStopDeal: DstRole Error [%d]", srcRole);
		return;
	}
	

	if(dstRole == TLKMMI_RDT_ROLE_AUT1){
		pSend = &sTlkMmiRdtT003DutSend1;
		pUnit = &sTlkMmiRdtT003Dut.aut1;
	}else{
		pSend = &sTlkMmiRdtT003DutSend2;
		pUnit = &sTlkMmiRdtT003Dut.aut2;
	}

	pSend->enable = false;
	pUnit->flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
}
static void tlkmmi_rdt_t003DutUnitSendDeal(tlkmmi_rdt_t003DutUnit_t *pUnit, tlkmmi_rdt_t003DutSend_t *pSend)
{
	if((pUnit->flags & TLKMMI_RDT_T003_FLAG_SEND) == 0 || !pSend->enable) return;
	if(pUnit->timer == 0 || pUnit->intval == 0 || clock_time_exceed(pUnit->timer, pUnit->intval)){
		if(pUnit->intval != 0) pUnit->timer = clock_time()|1;
		if(tlkmmi_rdt_t003SendSppData(pUnit->handle, pUnit->length, pUnit->sendNumb+1) == TLK_ENONE){
			pUnit->sendNumb ++;
		}else{
			pUnit->sendFail ++;
		}
	}
	if((pSend->method & TLKMMI_RDT_T003_METHOD_UNFIX_LENGTH_MASK) != 0){
		uint16 interval = pSend->maxLength-pSend->minLength;
		if(interval != 0){
			pUnit->length = pSend->minLength+rand()%(interval+1);
		}
	}
	if((pSend->method & TLKMMI_RDT_T003_METHOD_UNFIX_INTERVAL_MASK) != 0){
		uint16 interval = pSend->maxIntval-pSend->minIntval;
		if(interval != 0){
			pUnit->intval = pSend->minIntval+rand()%(interval+1);
		}
	}
	if(pSend->number != 0 && pUnit->sendNumb >= pSend->number){
		pUnit->flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
	}
}

static bool tlkmmi_rdt_t003DutQueue(tlkapi_queue_t *pQueue, uint32 userArg)
{
	if(sTlkMmiRdtT003Dut.aut1.handle == 0 && sTlkMmiRdtT003Dut.aut2.handle == 0) return false;
	if((sTlkMmiRdtT003Dut.aut1.flags & TLKMMI_RDT_T003_FLAG_SEND) == 0
		&& (sTlkMmiRdtT003Dut.aut2.flags & TLKMMI_RDT_T003_FLAG_SEND) == 0){
		return false;
	}
	if((sTlkMmiRdtT003Dut.aut1.flags & TLKMMI_RDT_T003_FLAG_SEND) != 0){
		tlkmmi_rdt_t003DutUnitSendDeal(&sTlkMmiRdtT003Dut.aut1, &sTlkMmiRdtT003DutSend1);
	}
	if((sTlkMmiRdtT003Dut.aut2.flags & TLKMMI_RDT_T003_FLAG_SEND) != 0){
		tlkmmi_rdt_t003DutUnitSendDeal(&sTlkMmiRdtT003Dut.aut2, &sTlkMmiRdtT003DutSend2);
	}
	return true;
}


static int tlkmmi_rdt_t003DutAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T003_BTADDR_AUT;
	uint08 aut2Addt[6] = TLKMMI_RDT_T003_BTADDR_AUT2;
	bth_aclConnRequestEvt_t *pEvt;

	pEvt = (bth_aclConnRequestEvt_t*)pData;
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) != 0 && tmemcmp(pEvt->peerMac, aut2Addt, 6) != 0){
		return -TLK_EFAIL;
	}
	
	bth_acl_setInitRole(pEvt->peerMac, BTH_ROLE_NOT_SET);
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003DutAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T003_BTADDR_AUT;
	uint08 aut2Addt[6] = TLKMMI_RDT_T003_BTADDR_AUT2;
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
		return TLK_ENONE;
	}
	if(!sTlkMmiRdtT003Dut.isStart){
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) != 0 && tmemcmp(pEvt->peerMac, aut2Addt, 6) != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutAclConnectEvt: Unexpected");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
		sTlkMmiRdtT003Dut.aut1.handle = pEvt->handle;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutAclConnectEvt: AUT1");
	}
	if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
		sTlkMmiRdtT003Dut.aut2.handle = pEvt->handle;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutAclConnectEvt: AUT2");
	}
	if(sTlkMmiRdtT003Dut.aut1.handle == 0 || sTlkMmiRdtT003Dut.aut2.handle == 0){
		bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003DutAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;
	
	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(sTlkMmiRdtT003Dut.aut1.handle == pEvt->handle){
		sTlkMmiRdtT003Dut.aut1.flags |= TLKMMI_RDT_T003_FLAG_CRYPT;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutAclEncryptEvt: AUT1");
	}
	if(sTlkMmiRdtT003Dut.aut2.handle == pEvt->handle){
		sTlkMmiRdtT003Dut.aut2.flags |= TLKMMI_RDT_T003_FLAG_CRYPT;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutAclEncryptEvt: AUT2");
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003DutAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T003_BTADDR_AUT;
	uint08 aut2Addt[6] = TLKMMI_RDT_T003_BTADDR_AUT2;
	bth_aclDiscCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
		sTlkMmiRdtT003Dut.aut1.handle = 0;
		sTlkMmiRdtT003Dut.aut1.busys = TLKMMI_RDT_T003_BUSY_NONE;
		sTlkMmiRdtT003Dut.aut1.flags = TLKMMI_RDT_T003_FLAG_NONE;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutAclDisconnEvt: AUT1");
	}
	if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
		sTlkMmiRdtT003Dut.aut2.handle = 0;
		sTlkMmiRdtT003Dut.aut2.busys = TLKMMI_RDT_T003_BUSY_NONE;
		sTlkMmiRdtT003Dut.aut2.flags = TLKMMI_RDT_T003_FLAG_NONE;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutAclDisconnEvt: AUT2");
	}
		
	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);
	if(sTlkMmiRdtT003Dut.aut1.handle == 0 || sTlkMmiRdtT003Dut.aut2.handle == 0){
		if(sTlkMmiRdtT003Dut.isStart){
			bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
		}
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003DutPrfConnectEvt(uint08 *pData, uint16 dataLen)
{
	btp_connectEvt_t *pEvt;

	pEvt = (btp_connectEvt_t*)pData;
	if(pEvt->ptype == BTP_PTYPE_SPP){
		if(sTlkMmiRdtT003Dut.aut1.handle == pEvt->handle){
			sTlkMmiRdtT003Dut.aut1.sendFail = 0;
			sTlkMmiRdtT003Dut.aut1.recvNumb = 0;
			sTlkMmiRdtT003Dut.aut1.sendNumb = 0;
			sTlkMmiRdtT003Dut.aut1.flags |= TLKMMI_RDT_T003_FLAG_SPP;
			if(sTlkMmiRdtT003DutSend1.enable){
				sTlkMmiRdtT003Dut.aut1.flags |= TLKMMI_RDT_T003_FLAG_SEND;
				tlkmmi_test_adaptAppendQueue(&sTlkMmiRdtT003Dut.queue);
			}
			sTlkMmiRdtT003Dut.aut1.timer = 0;
			sTlkMmiRdtT003Dut.aut1.intval = (sTlkMmiRdtT003DutSend1.minIntval+sTlkMmiRdtT003DutSend1.maxIntval)*1000/2;
			sTlkMmiRdtT003Dut.aut1.length = (sTlkMmiRdtT003DutSend1.minLength+sTlkMmiRdtT003DutSend1.maxLength)/2;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutPrfConnectEvt: AUT1 - SPP");
		}
		if(sTlkMmiRdtT003Dut.aut2.handle == pEvt->handle){
			sTlkMmiRdtT003Dut.aut2.sendFail = 0;
			sTlkMmiRdtT003Dut.aut2.recvNumb = 0;
			sTlkMmiRdtT003Dut.aut2.sendNumb = 0;
			sTlkMmiRdtT003Dut.aut2.flags |= TLKMMI_RDT_T003_FLAG_SPP;
			if(sTlkMmiRdtT003DutSend2.enable){
				sTlkMmiRdtT003Dut.aut2.flags |= TLKMMI_RDT_T003_FLAG_SEND;
				tlkmmi_test_adaptAppendQueue(&sTlkMmiRdtT003Dut.queue);
			}
			sTlkMmiRdtT003Dut.aut2.timer = 0;
			sTlkMmiRdtT003Dut.aut2.intval = (sTlkMmiRdtT003DutSend2.minIntval+sTlkMmiRdtT003DutSend2.maxIntval)*1000/2;
			sTlkMmiRdtT003Dut.aut2.length = (sTlkMmiRdtT003DutSend2.minLength+sTlkMmiRdtT003DutSend2.maxLength)/2;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003DutPrfConnectEvt: AUT2 - SPP");
		}
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003DutPrfDisconnEvt(uint08 *pData, uint16 dataLen)
{
	btp_disconnEvt_t *pEvt;

	pEvt = (btp_disconnEvt_t*)pData;
	if(pEvt->ptype == BTP_PTYPE_SPP){
		if(sTlkMmiRdtT003Dut.aut1.handle == pEvt->handle){
			sTlkMmiRdtT003Dut.aut1.flags &= ~TLKMMI_RDT_T003_FLAG_SPP;
			sTlkMmiRdtT003Dut.aut1.flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003DutPrfDisconnEvt: AUT1 - SPP [0x%x-0x%x]",
				sTlkMmiRdtT003Dut.aut1.recvNumb, sTlkMmiRdtT003Dut.aut1.sendNumb);
		}
		if(sTlkMmiRdtT003Dut.aut2.handle == pEvt->handle){
			sTlkMmiRdtT003Dut.aut2.flags &= ~TLKMMI_RDT_T003_FLAG_SPP;
			sTlkMmiRdtT003Dut.aut2.flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003DutPrfDisconnEvt: AUT2 - SPP [0x%x-0x%x]",
				sTlkMmiRdtT003Dut.aut2.recvNumb, sTlkMmiRdtT003Dut.aut2.sendNumb);
		}
	}
	return TLK_ENONE;
}
static void tlkmmi_rdt_t003DutSppDataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
	uint32 serial;

	if(dataLen < 4) return;

	ARRAY_TO_UINT32L(pData, 0, serial);
	if(sTlkMmiRdtT003Dut.aut1.handle == aclHandle){
		if(sTlkMmiRdtT003Dut.aut1.recvNumb+1 != serial){
			tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003DutSppDataCB[AUT1]: Stall Packet !!! exp[0x%x]-rcv[0x%x]",
				sTlkMmiRdtT003Dut.aut1.recvNumb+1, serial);
		}
		sTlkMmiRdtT003Dut.aut1.recvNumb = serial;
	}
	if(sTlkMmiRdtT003Dut.aut2.handle == aclHandle){
		if(sTlkMmiRdtT003Dut.aut2.recvNumb+1 != serial){
			tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003DutSppDataCB[AUT2]: Stall Packet !!! exp[0x%x]-rcv[0x%x]",
				sTlkMmiRdtT003Dut.aut2.recvNumb+1, serial);
		}
		sTlkMmiRdtT003Dut.aut2.recvNumb = serial;
	}
}





#endif //#if (TLKMMI_RDT_CASE_T003_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


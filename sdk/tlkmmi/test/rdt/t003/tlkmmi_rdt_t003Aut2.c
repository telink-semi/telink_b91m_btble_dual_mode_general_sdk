/********************************************************************************************************
 * @file	tlkmmi_rdt_t003Aut2.c
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
#include "tlkmmi_rdt_t003Aut2.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"
#include "tlkstk/bt/btp/rfcomm/btp_rfcomm.h"
#include "drivers.h"

extern int rand(void);
static bool tlkmmi_rdt_t003Aut2Queue(tlkapi_queue_t *pQueue, uint32 userArg);
static int tlkmmi_rdt_t003Aut2AclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003Aut2AclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003Aut2AclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003Aut2AclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003Aut2PrfConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003Aut2PrfDisconnEvt(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003Aut2SppDataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003Aut2SetSendStartDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003Aut2SetSendStopDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003Aut2SetSwitchParamDeal(uint08 *pData, uint16 dataLen);


static tlkmmi_rdt_t003Aut2Send_t sTlkMmiRdtT003Aut2Send = { //Send Ctrl For AUT1
	false, //Enable: 
	TLKMMI_RDT_T003_METHOD_UNFIX_LENGTH_UNFIX_INTERVAL, //method:
	0, //number: 0-Send Always
	64, //minLength:
	TLKMMI_RDT_T003_SEND_BUFF_LEN, //maxLength:
	0, //minIntval: Unit-ms
	30, //maxIntval: Unit-ms
};

static tlkmmi_rdt_t003Aut2_t sTlkMmiRdtT003Aut2;



void tlkmmi_rdt_t003Aut2Start(void)
{
	uint08 autAddr[6] = TLKMMI_RDT_T003_BTADDR_AUT;
	uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;

	sTlkMmiRdtT003Aut2.isStart = true;
	sTlkMmiRdtT003Aut2.dbgTimer = 0;
	sTlkMmiRdtT003Aut2.connIntvl = 0;
	sTlkMmiRdtT003Aut2.connTimer = 0;
	sTlkMmiRdtT003Aut2.dut.handle = 0;
	sTlkMmiRdtT003Aut2.dut.flags  = TLKMMI_RDT_T003_FLAG_NONE;
	sTlkMmiRdtT003Aut2.dut.busys  = TLKMMI_RDT_T003_BUSY_NONE;
	sTlkMmiRdtT003Aut2.dut.timer  = 0;
		
	bth_hci_sendWriteLocalNameCmd(TLKMMI_RDT_T003_BTNAME_AUT);
	bth_hci_sendSetBtAddrCmd(autAddr);
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_RDT_T003_BTDEV_CLASS_AUT);
	bth_hci_sendWriteSimplePairingModeCmd(1);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();

	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_t003Aut2AclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_t003Aut2AclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_t003Aut2AclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_t003Aut2AclDisconnEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  tlkmmi_rdt_t003Aut2PrfConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  tlkmmi_rdt_t003Aut2PrfDisconnEvt);
	
	btp_spp_regDataCB(tlkmmi_rdt_t003Aut2SppDataCB);

	tlkmmi_test_adaptInitQueue(&sTlkMmiRdtT003Aut2.queue, tlkmmi_rdt_t003Aut2Queue, nullptr);
	bth_acl_connect(dutAddr, TLKMMI_RDT_T003_BTDEV_CLASS_DUT, BTH_ROLE_SLAVE, 40000);
}
void tlkmmi_rdt_t003Aut2Close(void)
{
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  nullptr);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  nullptr);

	btp_spp_regDataCB(nullptr);

	sTlkMmiRdtT003Aut2.isStart = false;
	if(sTlkMmiRdtT003Aut2.dut.handle == 0){
		uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;
		bth_acl_connectCancel(dutAddr);
	}else{
		bth_acl_disconn(sTlkMmiRdtT003Aut2.dut.handle, 0x00);
	}
	sTlkMmiRdtT003Aut2.dut.handle = 0;
}
void tlkmmi_rdt_t003Aut2Timer(void)
{
	if(sTlkMmiRdtT003Aut2.isStart && sTlkMmiRdtT003Aut2.dut.handle != 0
		&& sTlkMmiRdtT003Aut2.connIntvl != 0 && sTlkMmiRdtT003Aut2.connTimer != 0){
		if(clock_time_exceed(sTlkMmiRdtT003Aut2.connTimer, sTlkMmiRdtT003Aut2.connIntvl)){
			bth_acl_disconn(sTlkMmiRdtT003Aut2.dut.handle, 0x00);
			sTlkMmiRdtT003Aut2.connTimer = 0;
		}
	}
	if(!sTlkMmiRdtT003Aut2.isStart) return;
	if(sTlkMmiRdtT003Aut2.dbgTimer != 0 && !clock_time_exceed(sTlkMmiRdtT003Aut2.dbgTimer, 3000000)){
		return;
	}
	sTlkMmiRdtT003Aut2.dbgTimer = clock_time()|1;
	if(sTlkMmiRdtT003Aut2.dut.handle != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"AUT2-Flags[0x%02x],AUT2->DUT(0x%08x), DUT->AUT2(0x%08x), Fail[0x%08x]",
			sTlkMmiRdtT003Aut2.dut.flags, sTlkMmiRdtT003Aut2.dut.sendNumb,
			sTlkMmiRdtT003Aut2.dut.recvNumb, sTlkMmiRdtT003Aut2.dut.sendFail);
	}
}
void tlkmmi_rdt_t003Aut2Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKMMI_RDT_T003_MSGID_SET_SEND_START){
		tlkmmi_rdt_t003Aut2SetSendStartDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_T003_MSGID_SET_SEND_STOP){
		tlkmmi_rdt_t003Aut2SetSendStopDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_T003_MSGID_SET_SWITCH_PARAM){
		tlkmmi_rdt_t003Aut2SetSwitchParamDeal(pData, dataLen);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2Input: unknown msg [0x%x]", msgID);
	}
}

static void tlkmmi_rdt_t003Aut2SetSendStartDeal(uint08 *pData, uint16 dataLen)
{
	uint08 srcRole;
	uint08 dstRole;
	uint08 method;
	uint16 number;
	uint16 minLength;
	uint16 maxLength;
	uint32 minIntval;
	uint32 maxIntval;
	tlkmmi_rdt_t003Aut2Send_t *pSend;
	tlkmmi_rdt_t003Aut2Unit_t *pUnit;

	if(dataLen < 13){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2SetSendStartDeal: error length [%d]", dataLen);
		return;
	}

	srcRole = pData[0];
	dstRole = pData[1];
	if(srcRole != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2SetSendStartDeal: SrcRole Error [%d]", srcRole);
		return;
	}
	if(dstRole != TLKMMI_RDT_ROLE_DUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2SetSendStartDeal: DstRole Error [%d]", srcRole);
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
			"tlkmmi_rdt_t003Aut2SetSendStartDeal: method Error [%d]", method);
		return;
	}
	if(maxLength < minLength){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN,
			"tlkmmi_rdt_t003Aut2SetSendStartDeal: Length Error [%d-%d]", 
			minLength, maxLength);
		return;
	}
	if(maxIntval < minIntval){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t003Aut2SetSendStartDeal: Interval Error [%d-%d]",
			minIntval, maxIntval);
		return;
	}

	pSend = &sTlkMmiRdtT003Aut2Send;
	pUnit = &sTlkMmiRdtT003Aut2.dut;

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
		tlkmmi_test_adaptAppendQueue(&sTlkMmiRdtT003Aut2.queue);
	}
}
static void tlkmmi_rdt_t003Aut2SetSendStopDeal(uint08 *pData, uint16 dataLen)
{
	uint08 srcRole;
	uint08 dstRole;
	tlkmmi_rdt_t003Aut2Send_t *pSend;
	tlkmmi_rdt_t003Aut2Unit_t *pUnit;

	if(dataLen < 2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2SetSendStopDeal: error length [%d]", dataLen);
		return;
	}

	srcRole = pData[0];
	dstRole = pData[1];
	if(srcRole != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2SetSendStopDeal: SrcRole Error [%d]", srcRole);
		return;
	}
	if(dstRole != TLKMMI_RDT_ROLE_DUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2SetSendStopDeal: DstRole Error [%d]", srcRole);
		return;
	}
	
	pSend = &sTlkMmiRdtT003Aut2Send;
	pUnit = &sTlkMmiRdtT003Aut2.dut;

	pSend->enable = false;
	pUnit->flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
}
static void tlkmmi_rdt_t003Aut2SetSwitchParamDeal(uint08 *pData, uint16 dataLen)
{
	if(dataLen < 3){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2SetSendStopDeal: error length [%d]", dataLen);
		return;
	}

	if(!sTlkMmiRdtT003Aut2.isStart) return;
	sTlkMmiRdtT003Aut2.connTimer = 0;
	if(pData[0] == 0x00){
		sTlkMmiRdtT003Aut2.connIntvl = 0;
	}else{
		sTlkMmiRdtT003Aut2.connIntvl = (((uint16)pData[2] << 8) | pData[1])*1000;
	}
	if(sTlkMmiRdtT003Aut2.connIntvl != 0 && (sTlkMmiRdtT003Aut2.dut.flags & TLKMMI_RDT_T003_FLAG_CRYPT) != 0){
		sTlkMmiRdtT003Aut2.connTimer = clock_time()|1;
	}
}

static void tlkmmi_rdt_t003Aut2UnitSendDeal(tlkmmi_rdt_t003Aut2Unit_t *pUnit, tlkmmi_rdt_t003Aut2Send_t *pSend)
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

static bool tlkmmi_rdt_t003Aut2Queue(tlkapi_queue_t *pQueue, uint32 userArg)
{
	if(sTlkMmiRdtT003Aut2.dut.handle == 0) return false;
	if((sTlkMmiRdtT003Aut2.dut.flags & TLKMMI_RDT_T003_FLAG_SEND) == 0){
		return false;
	}
	if((sTlkMmiRdtT003Aut2.dut.flags & TLKMMI_RDT_T003_FLAG_SEND) != 0){
		tlkmmi_rdt_t003Aut2UnitSendDeal(&sTlkMmiRdtT003Aut2.dut, &sTlkMmiRdtT003Aut2Send);
	}
	return true;
}

static int tlkmmi_rdt_t003Aut2AclRequestEvt(uint08 *pData, uint16 dataLen)
{
	return -TLK_EFAIL;
}
static int tlkmmi_rdt_t003Aut2AclConnectEvt(uint08 *pData, uint16 dataLen)
{
	uint08 dutAddt[6] = TLKMMI_RDT_T003_BTADDR_DUT;
	bth_aclConnCompleteEvt_t *pEvt;

	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		if(sTlkMmiRdtT003Aut2.dut.handle == 0 && sTlkMmiRdtT003Aut2.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T003_BTDEV_CLASS_DUT, BTH_ROLE_SLAVE, 40000);
		}
		return TLK_ENONE;
	}
	
	if(tmemcmp(pEvt->peerMac, dutAddt, 6) != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2AclConnectEvt: Unexpected");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2AclConnectEvt");

	sTlkMmiRdtT003Aut2.connTimer = 0;
	sTlkMmiRdtT003Aut2.dut.handle = pEvt->handle;
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003Aut2AclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;

	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(!sTlkMmiRdtT003Aut2.isStart){
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	if(sTlkMmiRdtT003Aut2.dut.handle == pEvt->handle){
		btp_rfcomm_connect(pEvt->handle);
		if(sTlkMmiRdtT003Aut2.connIntvl != 0){
			sTlkMmiRdtT003Aut2.connTimer = clock_time()|1;
		}
		sTlkMmiRdtT003Aut2.dut.flags |= TLKMMI_RDT_T003_FLAG_CRYPT;
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003Aut2AclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclDiscCompleteEvt_t *pEvt;

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2AclDisconnEvt");

	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);
	if(sTlkMmiRdtT003Aut2.dut.handle == pEvt->handle){
		sTlkMmiRdtT003Aut2.dut.handle = 0;
		sTlkMmiRdtT003Aut2.connTimer = 0;
		sTlkMmiRdtT003Aut2.dut.flags = TLKMMI_RDT_T003_FLAG_NONE;
		sTlkMmiRdtT003Aut2.dut.busys = TLKMMI_RDT_T003_BUSY_NONE;
		if(sTlkMmiRdtT003Aut2.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T003_BTDEV_CLASS_DUT, BTH_ROLE_SLAVE, 40000);
		}
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003Aut2PrfConnectEvt(uint08 *pData, uint16 dataLen)
{
	btp_connectEvt_t *pEvt;

	pEvt = (btp_connectEvt_t*)pData;
	if(pEvt->ptype == BTP_PTYPE_RFC){
		btp_spp_connect(pEvt->handle, TLKBT_CFG_SPP_RFC_CHANNEL);
	}
	if(pEvt->ptype == BTP_PTYPE_SPP){
		if(sTlkMmiRdtT003Aut2.dut.handle == pEvt->handle){
			sTlkMmiRdtT003Aut2.dut.sendFail = 0;
			sTlkMmiRdtT003Aut2.dut.recvNumb = 0;
			sTlkMmiRdtT003Aut2.dut.sendNumb = 0;
			sTlkMmiRdtT003Aut2.dut.flags |= TLKMMI_RDT_T003_FLAG_SPP;
			if(sTlkMmiRdtT003Aut2Send.enable){
				sTlkMmiRdtT003Aut2.dut.flags |= TLKMMI_RDT_T003_FLAG_SEND;
				tlkmmi_test_adaptAppendQueue(&sTlkMmiRdtT003Aut2.queue);
			}
			sTlkMmiRdtT003Aut2.dut.timer = 0;
			sTlkMmiRdtT003Aut2.dut.intval = (sTlkMmiRdtT003Aut2Send.minIntval+sTlkMmiRdtT003Aut2Send.maxIntval)*1000/2;
			sTlkMmiRdtT003Aut2.dut.length = (sTlkMmiRdtT003Aut2Send.minLength+sTlkMmiRdtT003Aut2Send.maxLength)/2;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Aut2PrfConnectEvt: AUT1 - SPP");
		}
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003Aut2PrfDisconnEvt(uint08 *pData, uint16 dataLen)
{
	btp_disconnEvt_t *pEvt;

	pEvt = (btp_disconnEvt_t*)pData;
	if(pEvt->ptype == BTP_PTYPE_SPP){
		if(sTlkMmiRdtT003Aut2.dut.handle == pEvt->handle){
			sTlkMmiRdtT003Aut2.dut.flags &= ~TLKMMI_RDT_T003_FLAG_SPP;
			sTlkMmiRdtT003Aut2.dut.flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003Aut2PrfDisconnEvt: AUT1 - SPP [0x%x-0x%x]",
				sTlkMmiRdtT003Aut2.dut.recvNumb, sTlkMmiRdtT003Aut2.dut.sendNumb);
		}
	}
	return TLK_ENONE;
}
static void tlkmmi_rdt_t003Aut2SppDataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
	uint32 serial;

	if(dataLen < 4) return;

	ARRAY_TO_UINT32L(pData, 0, serial);
	if(sTlkMmiRdtT003Aut2.dut.handle == aclHandle){
		if(sTlkMmiRdtT003Aut2.dut.recvNumb+1 != serial){
			tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003Aut2SppDataCB[AUT1]: Stall Packet !!! exp[0x%x]-rcv[0x%x]",
				sTlkMmiRdtT003Aut2.dut.recvNumb+1, serial);
		}
		sTlkMmiRdtT003Aut2.dut.recvNumb = serial;
	}
}



#endif //#if (TLKMMI_RDT_CASE_T003_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


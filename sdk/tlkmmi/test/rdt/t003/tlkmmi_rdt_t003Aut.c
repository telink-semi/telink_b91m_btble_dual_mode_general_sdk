/********************************************************************************************************
 * @file	tlkmmi_rdt_t003Aut.c
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
#include "tlkmmi_rdt_t003Aut.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"
#include "tlkstk/bt/btp/rfcomm/btp_rfcomm.h"
#include "drivers.h"

extern int rand(void);
static bool tlkmmi_rdt_t003AutQueue(tlkapi_queue_t *pQueue, uint32 userArg);
static int tlkmmi_rdt_t003AutAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003AutAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003AutAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003AutAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003AutPrfConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t003AutPrfDisconnEvt(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003AutSppDataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003AutSetSendStartDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t003AutSetSendStopDeal(uint08 *pData, uint16 dataLen);


static tlkmmi_rdt_t003AutSend_t sTlkMmiRdtT003AutSend = { //Send Ctrl For AUT1
	false, //Enable: 
	TLKMMI_RDT_T003_METHOD_UNFIX_LENGTH_UNFIX_INTERVAL, //method:
	0, //number: 0-Send Always
	64, //minLength:
	TLKMMI_RDT_T003_SEND_BUFF_LEN, //maxLength:
	0, //minIntval: Unit-ms
	30, //maxIntval: Unit-ms
};
static tlkmmi_rdt_t003Aut_t sTlkMmiRdtT003Aut;



void tlkmmi_rdt_t003AutStart(void)
{
	uint08 autAddr[6] = TLKMMI_RDT_T003_BTADDR_AUT;
	uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;

	sTlkMmiRdtT003Aut.isStart = true;
	sTlkMmiRdtT003Aut.dbgTimer = 0;
	sTlkMmiRdtT003Aut.dut.handle = 0;
	sTlkMmiRdtT003Aut.dut.flags  = TLKMMI_RDT_T003_FLAG_NONE;
	sTlkMmiRdtT003Aut.dut.busys  = TLKMMI_RDT_T003_BUSY_NONE;
	sTlkMmiRdtT003Aut.dut.timer  = 0;
		
	bth_hci_sendWriteLocalNameCmd(TLKMMI_RDT_T003_BTNAME_AUT);
	bth_hci_sendSetBtAddrCmd(autAddr);
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_RDT_T003_BTDEV_CLASS_AUT);
	bth_hci_sendWriteSimplePairingModeCmd(1);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();

	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_t003AutAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_t003AutAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_t003AutAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_t003AutAclDisconnEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  tlkmmi_rdt_t003AutPrfConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  tlkmmi_rdt_t003AutPrfDisconnEvt);
	
	btp_spp_regDataCB(tlkmmi_rdt_t003AutSppDataCB);

	tlkmmi_test_adaptInitQueue(&sTlkMmiRdtT003Aut.queue, tlkmmi_rdt_t003AutQueue, nullptr);
	bth_acl_connect(dutAddr, TLKMMI_RDT_T003_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
}
void tlkmmi_rdt_t003AutClose(void)
{
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  nullptr);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  nullptr);

	btp_spp_regDataCB(nullptr);

	sTlkMmiRdtT003Aut.isStart = false;
	if(sTlkMmiRdtT003Aut.dut.handle == 0){
		uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;
		bth_acl_connectCancel(dutAddr);
	}else{
		bth_acl_disconn(sTlkMmiRdtT003Aut.dut.handle, 0x00);
	}
	sTlkMmiRdtT003Aut.dut.handle = 0;
}
void tlkmmi_rdt_t003AutTimer(void)
{
	if(!sTlkMmiRdtT003Aut.isStart) return;
	if(sTlkMmiRdtT003Aut.dbgTimer != 0 && !clock_time_exceed(sTlkMmiRdtT003Aut.dbgTimer, 3000000)){
		return;
	}
	sTlkMmiRdtT003Aut.dbgTimer = clock_time()|1;
	if(sTlkMmiRdtT003Aut.dut.handle != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"AUT-Flags[0x%02x],AUT->DUT(0x%08x), DUT->AUT(0x%08x), Fail[0x%08x]",
			sTlkMmiRdtT003Aut.dut.flags, sTlkMmiRdtT003Aut.dut.sendNumb,
			sTlkMmiRdtT003Aut.dut.recvNumb, sTlkMmiRdtT003Aut.dut.sendFail);
	}
}
void tlkmmi_rdt_t003AutInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKMMI_RDT_T003_MSGID_SET_SEND_START){
		tlkmmi_rdt_t003AutSetSendStartDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_T003_MSGID_SET_SEND_STOP){
		tlkmmi_rdt_t003AutSetSendStopDeal(pData, dataLen);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutInput: unknown msg [0x%x]", msgID);
	}
}

static void tlkmmi_rdt_t003AutSetSendStartDeal(uint08 *pData, uint16 dataLen)
{
	uint08 srcRole;
	uint08 dstRole;
	uint08 method;
	uint16 number;
	uint16 minLength;
	uint16 maxLength;
	uint32 minIntval;
	uint32 maxIntval;
	tlkmmi_rdt_t003AutSend_t *pSend;
	tlkmmi_rdt_t003AutUnit_t *pUnit;

	if(dataLen < 13){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutSetSendStartDeal: error length [%d]", dataLen);
		return;
	}

	srcRole = pData[0];
	dstRole = pData[1];
	if(srcRole != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutSetSendStartDeal: SrcRole Error [%d]", srcRole);
		return;
	}
	if(dstRole != TLKMMI_RDT_ROLE_DUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutSetSendStartDeal: DstRole Error [%d]", srcRole);
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
			"tlkmmi_rdt_t003AutSetSendStartDeal: method Error [%d]", method);
		return;
	}
	if(maxLength < minLength){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN,
			"tlkmmi_rdt_t003AutSetSendStartDeal: Length Error [%d-%d]", 
			minLength, maxLength);
		return;
	}
	if(maxIntval < minIntval){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t003AutSetSendStartDeal: Interval Error [%d-%d]",
			minIntval, maxIntval);
		return;
	}

	pSend = &sTlkMmiRdtT003AutSend;
	pUnit = &sTlkMmiRdtT003Aut.dut;

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
		tlkmmi_test_adaptAppendQueue(&sTlkMmiRdtT003Aut.queue);
	}
}
static void tlkmmi_rdt_t003AutSetSendStopDeal(uint08 *pData, uint16 dataLen)
{
	uint08 srcRole;
	uint08 dstRole;
	tlkmmi_rdt_t003AutSend_t *pSend;
	tlkmmi_rdt_t003AutUnit_t *pUnit;

	if(dataLen < 2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutSetSendStopDeal: error length [%d]", dataLen);
		return;
	}

	srcRole = pData[0];
	dstRole = pData[1];
	if(srcRole != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutSetSendStopDeal: SrcRole Error [%d]", srcRole);
		return;
	}
	if(dstRole != TLKMMI_RDT_ROLE_DUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutSetSendStopDeal: DstRole Error [%d]", srcRole);
		return;
	}
	
	pSend = &sTlkMmiRdtT003AutSend;
	pUnit = &sTlkMmiRdtT003Aut.dut;

	pSend->enable = false;
	pUnit->flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
}

static void tlkmmi_rdt_t003AutUnitSendDeal(tlkmmi_rdt_t003AutUnit_t *pUnit, tlkmmi_rdt_t003AutSend_t *pSend)
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

static bool tlkmmi_rdt_t003AutQueue(tlkapi_queue_t *pQueue, uint32 userArg)
{
	if(sTlkMmiRdtT003Aut.dut.handle == 0) return false;
	if((sTlkMmiRdtT003Aut.dut.flags & TLKMMI_RDT_T003_FLAG_SEND) == 0){
		return false;
	}
	if((sTlkMmiRdtT003Aut.dut.flags & TLKMMI_RDT_T003_FLAG_SEND) != 0){
		tlkmmi_rdt_t003AutUnitSendDeal(&sTlkMmiRdtT003Aut.dut, &sTlkMmiRdtT003AutSend);
	}
	return true;
}

static int tlkmmi_rdt_t003AutAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	return -TLK_EFAIL;
}
static int tlkmmi_rdt_t003AutAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	uint08 dutAddt[6] = TLKMMI_RDT_T003_BTADDR_DUT;
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		if(sTlkMmiRdtT003Aut.dut.handle == 0 && sTlkMmiRdtT003Aut.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T003_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
		}
		return TLK_ENONE;
	}
		
	if(tmemcmp(pEvt->peerMac, dutAddt, 6) != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutAclConnectEvt: Unexpected");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutAclConnectEvt");
	sTlkMmiRdtT003Aut.dut.handle = pEvt->handle;
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003AutAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;
	
	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(!sTlkMmiRdtT003Aut.isStart){
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutAclEncryptEvt: [%x-%x]",
		sTlkMmiRdtT003Aut.dut.handle, pEvt->handle);
	
	if(sTlkMmiRdtT003Aut.dut.handle == pEvt->handle){
		btp_rfcomm_connect(pEvt->handle);
	}
		
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003AutAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclDiscCompleteEvt_t *pEvt;

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutAclDisconnEvt");

	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);
	if(sTlkMmiRdtT003Aut.dut.handle == pEvt->handle){
		sTlkMmiRdtT003Aut.dut.handle = 0;
		sTlkMmiRdtT003Aut.dut.flags = TLKMMI_RDT_T003_FLAG_NONE;
		sTlkMmiRdtT003Aut.dut.busys = TLKMMI_RDT_T003_BUSY_NONE;
		if(sTlkMmiRdtT003Aut.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T003_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T003_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
		}
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003AutPrfConnectEvt(uint08 *pData, uint16 dataLen)
{
	btp_connectEvt_t *pEvt;

	pEvt = (btp_connectEvt_t*)pData;
	if(pEvt->ptype == BTP_PTYPE_RFC){
		btp_spp_connect(pEvt->handle, TLKBT_CFG_SPP_RFC_CHANNEL);
	}
	if(pEvt->ptype == BTP_PTYPE_SPP){
		if(sTlkMmiRdtT003Aut.dut.handle == pEvt->handle){
			sTlkMmiRdtT003Aut.dut.sendFail = 0;
			sTlkMmiRdtT003Aut.dut.recvNumb = 0;
			sTlkMmiRdtT003Aut.dut.sendNumb = 0;
			sTlkMmiRdtT003Aut.dut.flags |= TLKMMI_RDT_T003_FLAG_SPP;
			if(sTlkMmiRdtT003AutSend.enable){
				sTlkMmiRdtT003Aut.dut.flags |= TLKMMI_RDT_T003_FLAG_SEND;
				tlkmmi_test_adaptAppendQueue(&sTlkMmiRdtT003Aut.queue);
			}
			sTlkMmiRdtT003Aut.dut.timer = 0;
			sTlkMmiRdtT003Aut.dut.intval = (sTlkMmiRdtT003AutSend.minIntval+sTlkMmiRdtT003AutSend.maxIntval)*1000/2;
			sTlkMmiRdtT003Aut.dut.length = (sTlkMmiRdtT003AutSend.minLength+sTlkMmiRdtT003AutSend.maxLength)/2;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003AutPrfConnectEvt: AUT1 - SPP");
		}
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003AutPrfDisconnEvt(uint08 *pData, uint16 dataLen)
{
	btp_disconnEvt_t *pEvt;

	pEvt = (btp_disconnEvt_t*)pData;
	if(pEvt->ptype == BTP_PTYPE_SPP){
		if(sTlkMmiRdtT003Aut.dut.handle == pEvt->handle){
			sTlkMmiRdtT003Aut.dut.flags &= ~TLKMMI_RDT_T003_FLAG_SPP;
			sTlkMmiRdtT003Aut.dut.flags &= ~TLKMMI_RDT_T003_FLAG_SEND;
			tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003AutPrfDisconnEvt: AUT1 - SPP [0x%x-0x%x]",
				sTlkMmiRdtT003Aut.dut.recvNumb, sTlkMmiRdtT003Aut.dut.sendNumb);
		}
	}
	return TLK_ENONE;
}
static void tlkmmi_rdt_t003AutSppDataCB(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
	uint32 serial;

	if(dataLen < 4) return;

	ARRAY_TO_UINT32L(pData, 0, serial);
	if(sTlkMmiRdtT003Aut.dut.handle == aclHandle){
		if(sTlkMmiRdtT003Aut.dut.recvNumb+1 != serial){
			tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
				"tlkmmi_rdt_t003AutSppDataCB[AUT1]: Stall Packet !!! exp[0x%x]-rcv[0x%x]",
				sTlkMmiRdtT003Aut.dut.recvNumb+1, serial);
		}
		sTlkMmiRdtT003Aut.dut.recvNumb = serial;
	}
}




#endif //#if (TLKMMI_RDT_CASE_T003_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


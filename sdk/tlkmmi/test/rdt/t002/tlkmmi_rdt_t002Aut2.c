/********************************************************************************************************
 * @file	tlkmmi_rdt_t002Aut2.c
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
#if (TLKMMI_RDT_CASE_T002_ENABLE)
#include "tlkmmi_rdt_t002.h"
#include "tlkmmi_rdt_t002Aut2.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_sco.h"
#include "drivers.h"

extern int rand(void);
static int tlkmmi_rdt_t002Aut2AclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002Aut2AclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002Aut2AclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002Aut2AclDisconnEvt(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t002Aut2SetAclSwitchDeal(uint08 *pData, uint16 dataLen);

static tlkmmi_rdt_t002Aut2Conn_t sTlkMmiRdtT002Aut2Acl = {
	true, //.enable-
	0x4E20, //.minIntval-20s
	0xEA60, //.maxIntval-60s
};
static tlkmmi_rdt_t002Aut2_t sTlkMmiRdtT002Aut2 = {0};


void tlkmmi_rdt_t002Aut2Start(void)
{
	uint08 autAddr[6] = TLKMMI_RDT_T002_BTADDR_AUT2;
	uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2Start");
	
	sTlkMmiRdtT002Aut2.isStart  = true;
	sTlkMmiRdtT002Aut2.dbgTimer = 0;
	sTlkMmiRdtT002Aut2.acl.timer = 0;
	sTlkMmiRdtT002Aut2.acl.flags = TLKMMI_RDT_T002_FLAG_NONE;
	sTlkMmiRdtT002Aut2.acl.flags = TLKMMI_RDT_T002_FLAG_NONE;
	sTlkMmiRdtT002Aut2.acl.handle = 0;
	
	sTlkMmiRdtT002Aut2.dbgTimer = clock_time() | 1;
	
	bth_hci_sendWriteLocalNameCmd(TLKMMI_RDT_T002_BTNAME_AUT);
	bth_hci_sendSetBtAddrCmd(autAddr);
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_RDT_T002_BTDEV_CLASS_AUT);
	bth_hci_sendWriteSimplePairingModeCmd(1);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_t002Aut2AclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_t002Aut2AclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_t002Aut2AclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_t002Aut2AclDisconnEvt);
		
	sTlkMmiRdtT002Aut2.acl.intval = (sTlkMmiRdtT002Aut2Acl.minIntval+sTlkMmiRdtT002Aut2Acl.maxIntval)*1000/2;
	bth_acl_connect(dutAddr, TLKMMI_RDT_T002_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
}
void tlkmmi_rdt_t002Aut2Close(void)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2Close");
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  nullptr);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, nullptr);
	
	sTlkMmiRdtT002Aut2.isStart = false;
	if(sTlkMmiRdtT002Aut2.acl.handle == 0){
		uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
		bth_acl_connectCancel(dutAddr);
	}else{
		bth_acl_disconn(sTlkMmiRdtT002Aut2.acl.handle, 0x00);
	}
	sTlkMmiRdtT002Aut2.acl.handle = 0;
}
void tlkmmi_rdt_t002Aut2Timer(void)
{
	if(!sTlkMmiRdtT002Aut2.isStart) return;
	if(sTlkMmiRdtT002Aut2.dbgTimer != 0 && clock_time_exceed(sTlkMmiRdtT002Aut2.dbgTimer, 3000000)){
		sTlkMmiRdtT002Aut2.dbgTimer = clock_time()|1;
//		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
//			"tlkmmi_rdt_t002Aut2Timer: Send[0x%04x],Recv[0x%04x]",
//			sTlkMmiRdtT002Aut2.sndCount, sTlkMmiRdtT002Aut2.rcvCount);
	}
	if(sTlkMmiRdtT002Aut2Acl.enable && sTlkMmiRdtT002Aut2.acl.timer != 0 
		&& clock_time_exceed(sTlkMmiRdtT002Aut2.acl.timer, sTlkMmiRdtT002Aut2.acl.intval)){
		sTlkMmiRdtT002Aut2.acl.timer = clock_time() | 1;
		bth_acl_disconn(sTlkMmiRdtT002Aut2.acl.handle, 0x00);
	}
}
void tlkmmi_rdt_t002Aut2Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(!sTlkMmiRdtT002Aut2Acl.enable) return;
	if(msgID == TLKMMI_RDT_T002_MSGID_SET_ACL_SWITCH){
		tlkmmi_rdt_t002Aut2SetAclSwitchDeal(pData, dataLen);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2Input: unknown msg [0x%x]", msgID);
	}
}

static void tlkmmi_rdt_t002Aut2SetAclSwitchDeal(uint08 *pData, uint16 dataLen)
{
	uint32 minIntval;
	uint32 maxIntval;

	if(dataLen < 5){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2SetAclSwitchDeal: error length [%d]", dataLen);
		return;
	}

	if(pData[0] == 0x00){
		sTlkMmiRdtT002Aut2Acl.enable = false;
		return;
	}
	
	ARRAY_TO_UINT16L(pData, 1, minIntval);
	ARRAY_TO_UINT16L(pData, 3, maxIntval);
	if(maxIntval < minIntval){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t002Aut2SetAclSwitchDeal: Interval Error [%d-%d]",
			minIntval, maxIntval);
		return;
	}
	
	sTlkMmiRdtT002Aut2Acl.enable = true;
	sTlkMmiRdtT002Aut2Acl.minIntval = minIntval;
	sTlkMmiRdtT002Aut2Acl.maxIntval = maxIntval;
	sTlkMmiRdtT002Aut2.acl.intval = (sTlkMmiRdtT002Aut2Acl.minIntval+sTlkMmiRdtT002Aut2Acl.maxIntval)*1000/2;
	if(sTlkMmiRdtT002Aut2.acl.handle != 0){
		sTlkMmiRdtT002Aut2.acl.timer = clock_time() | 1;
	}
}

static int tlkmmi_rdt_t002Aut2AclRequestEvt(uint08 *pData, uint16 dataLen)
{
	return -TLK_EFAIL;
}
static int tlkmmi_rdt_t002Aut2AclConnectEvt(uint08 *pData, uint16 dataLen)
{
	uint08 dutAddt[6] = TLKMMI_RDT_T002_BTADDR_DUT;
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		if(sTlkMmiRdtT002Aut2.acl.handle == 0 && sTlkMmiRdtT002Aut2.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T002_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
		}
		return TLK_ENONE;
	}
	
	if(!sTlkMmiRdtT002Aut2.isStart){
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	if(tmemcmp(pEvt->peerMac, dutAddt, 6) != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2AclConnectEvt: Unexpected");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}

	sTlkMmiRdtT002Aut2.acl.handle = pEvt->handle;
	sTlkMmiRdtT002Aut2.acl.timer  = 0;
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2AclConnectEvt: DUT");
	if(sTlkMmiRdtT002Aut2Acl.enable){
		uint16 interval;
		sTlkMmiRdtT002Aut2.acl.timer = clock_time() | 1;
		interval = sTlkMmiRdtT002Aut2Acl.maxIntval-sTlkMmiRdtT002Aut2Acl.minIntval;
		if(interval != 0){
			interval = rand()%(interval+1);
			sTlkMmiRdtT002Aut2.acl.intval = (sTlkMmiRdtT002Aut2Acl.minIntval+interval)*1000;
		}
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Aut2AclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;
	
	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(sTlkMmiRdtT002Aut2.acl.handle == pEvt->handle){
		sTlkMmiRdtT002Aut2.acl.flags |= TLKMMI_RDT_T002_FLAG_CRYPT;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2AclEncryptEvt: DUT");
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Aut2AclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclDiscCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);
	
	if(sTlkMmiRdtT002Aut2.acl.handle == pEvt->handle){
		sTlkMmiRdtT002Aut2.acl.timer = 0;
		sTlkMmiRdtT002Aut2.acl.handle = 0;
		sTlkMmiRdtT002Aut2.acl.busys = TLKMMI_RDT_T002_BUSY_NONE;
		sTlkMmiRdtT002Aut2.acl.flags = TLKMMI_RDT_T002_FLAG_NONE;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2AclDisconnEvt: DUT");
		if(sTlkMmiRdtT002Aut2.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T002_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
		}
	}
		
	return TLK_ENONE;
}





#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


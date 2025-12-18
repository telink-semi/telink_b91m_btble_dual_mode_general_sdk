/********************************************************************************************************
 * @file	tlkmmi_rdt_t002Aut.c
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
#include "tlkmmi_rdt_t002Aut.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_sco.h"
#include "drivers.h"

extern int rand(void);
extern void btc_sco_regDataCB(void *prx, void *ptx);
static int tlkmmi_rdt_t002AutAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002AutAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002AutAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002AutAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002AutScoConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002AutScoDisconnEvt(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t002AutAddSpkFrame(uint08 id, uint08 *pData, int dataLen);
static void tlkmmi_rdt_t002AutGetMicFrame(uint08 id, uint08 *pBuff, int buffLen);
static void tlkmmi_rdt_t002AutSetAclSwitchDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t002AutSetScoSwitchDeal(uint08 *pData, uint16 dataLen);


static tlkmmi_rdt_t002AutConn_t sTlkMmiRdtT002AutAcl = {
	false, //.enable-
	0x4E20, //.minIntval-20s
	0xEA60, //.maxIntval-60s
};
static tlkmmi_rdt_t002AutConn_t sTlkMmiRdtT002AutSco = {
	true, //.enable-
	0x0BB8, //.minIntval-1.5s
	0x1388, //.maxIntval-5s
};
static tlkmmi_rdt_t002Aut_t sTlkMmiRdtT002Aut = {0};


void tlkmmi_rdt_t002AutStart(void)
{
	uint08 autAddr[6] = TLKMMI_RDT_T002_BTADDR_AUT;
	uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutStart");
	
	sTlkMmiRdtT002Aut.isStart  = true;
	sTlkMmiRdtT002Aut.sndCount = 0;
	sTlkMmiRdtT002Aut.rcvCount = 0;
	sTlkMmiRdtT002Aut.dbgTimer = 0;
	sTlkMmiRdtT002Aut.sndTimer = 0;
	sTlkMmiRdtT002Aut.rcvTimer = 0;
	sTlkMmiRdtT002Aut.acl.timer = 0;
	sTlkMmiRdtT002Aut.sco.timer = 0;
	sTlkMmiRdtT002Aut.acl.flags = TLKMMI_RDT_T002_FLAG_NONE;
	sTlkMmiRdtT002Aut.acl.flags = TLKMMI_RDT_T002_FLAG_NONE;
	sTlkMmiRdtT002Aut.acl.handle = 0;
	sTlkMmiRdtT002Aut.sco.handle = 0;

	sTlkMmiRdtT002Aut.dbgTimer = clock_time() | 1;
	
	bth_hci_sendWriteLocalNameCmd(TLKMMI_RDT_T002_BTNAME_AUT);
	bth_hci_sendSetBtAddrCmd(autAddr);
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_RDT_T002_BTDEV_CLASS_AUT);
	bth_hci_sendWriteSimplePairingModeCmd(1);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_t002AutAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_t002AutAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_t002AutAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_t002AutAclDisconnEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmmi_rdt_t002AutScoConnectEvt);
	bth_event_regCB(BTH_EVTID_SCODISC_COMPLETE, tlkmmi_rdt_t002AutScoDisconnEvt);
	
	btc_sco_regDataCB(tlkmmi_rdt_t002AutAddSpkFrame, tlkmmi_rdt_t002AutGetMicFrame);
	
	sTlkMmiRdtT002Aut.acl.intval = (sTlkMmiRdtT002AutAcl.minIntval+sTlkMmiRdtT002AutAcl.maxIntval)*1000/2;
	sTlkMmiRdtT002Aut.sco.intval = (sTlkMmiRdtT002AutSco.minIntval+sTlkMmiRdtT002AutSco.maxIntval)*1000/2;
	bth_acl_connect(dutAddr, TLKMMI_RDT_T002_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
}
void tlkmmi_rdt_t002AutClose(void)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutClose");
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  nullptr);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_SCODISC_COMPLETE, nullptr);
	btc_sco_regDataCB(nullptr, nullptr);
	
	sTlkMmiRdtT002Aut.isStart = false;
	if(sTlkMmiRdtT002Aut.acl.handle == 0){
		uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
		bth_acl_connectCancel(dutAddr);
	}else{
		bth_acl_disconn(sTlkMmiRdtT002Aut.acl.handle, 0x00);
	}
	sTlkMmiRdtT002Aut.acl.handle = 0;
}
void tlkmmi_rdt_t002AutTimer(void)
{
	if(!sTlkMmiRdtT002Aut.isStart) return;
	if(sTlkMmiRdtT002Aut.dbgTimer != 0 && clock_time_exceed(sTlkMmiRdtT002Aut.dbgTimer, 3000000)){
		sTlkMmiRdtT002Aut.dbgTimer = clock_time()|1;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t002AutTimer: handle[0x%x],Send[0x%04x],Recv[0x%04x]",
			sTlkMmiRdtT002Aut.acl.handle, sTlkMmiRdtT002Aut.sndCount, sTlkMmiRdtT002Aut.rcvCount);
	}
	if(sTlkMmiRdtT002AutAcl.enable && sTlkMmiRdtT002Aut.acl.timer != 0 
		&& clock_time_exceed(sTlkMmiRdtT002Aut.acl.timer, sTlkMmiRdtT002Aut.acl.intval)){
		sTlkMmiRdtT002Aut.acl.timer = clock_time() | 1;
		bth_acl_disconn(sTlkMmiRdtT002Aut.acl.handle, 0x00);
	}
	if(sTlkMmiRdtT002AutSco.enable && sTlkMmiRdtT002Aut.acl.handle != 0 && sTlkMmiRdtT002Aut.sco.timer != 0 
		&& clock_time_exceed(sTlkMmiRdtT002Aut.sco.timer, sTlkMmiRdtT002Aut.sco.intval)){
		sTlkMmiRdtT002Aut.sco.timer = clock_time() | 1;
		if(sTlkMmiRdtT002Aut.sco.handle != 0){
			bth_sco_disconn(sTlkMmiRdtT002Aut.sco.handle, 0x13);
		}else{
			bth_sco_connect(sTlkMmiRdtT002Aut.acl.handle, BTH_SCO_LINK_TYPE_ESCO, BTH_SCO_AIRMODE_CVSD);
		}
	}
	if(sTlkMmiRdtT002Aut.sco.handle == 0) return;
	if(sTlkMmiRdtT002Aut.sndTimer != 0 && clock_time_exceed(sTlkMmiRdtT002Aut.sndTimer, 500000)){
		sTlkMmiRdtT002Aut.sndTimer = clock_time()|1;
		tlkapi_fatal(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t002AutTimer: SCO Send Stall !!! - Send[0x%04x],Recv[0x%04x]",
			sTlkMmiRdtT002Aut.sndCount, sTlkMmiRdtT002Aut.rcvCount);
	}
	if(sTlkMmiRdtT002Aut.rcvTimer != 0 && clock_time_exceed(sTlkMmiRdtT002Aut.rcvTimer, 500000)){
		sTlkMmiRdtT002Aut.rcvTimer = clock_time()|1;
		tlkapi_fatal(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t002AutTimer: SCO Recv Stall !!! - Send[0x%04x],Recv[0x%04x]",
			sTlkMmiRdtT002Aut.sndCount, sTlkMmiRdtT002Aut.rcvCount);
	}
}
void tlkmmi_rdt_t002AutInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(!sTlkMmiRdtT002AutAcl.enable) return;
	if(msgID == TLKMMI_RDT_T002_MSGID_SET_ACL_SWITCH){
		tlkmmi_rdt_t002AutSetAclSwitchDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_T002_MSGID_SET_SCO_SWITCH){
		tlkmmi_rdt_t002AutSetScoSwitchDeal(pData, dataLen);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2Input: unknown msg [0x%x]", msgID);
	}
}

static void tlkmmi_rdt_t002AutSetAclSwitchDeal(uint08 *pData, uint16 dataLen)
{
	uint32 minIntval;
	uint32 maxIntval;

	if(dataLen < 5){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2SetAclSwitchDeal: error length [%d]", dataLen);
		return;
	}

	if(pData[0] == 0x00){
		sTlkMmiRdtT002AutAcl.enable = false;
		sTlkMmiRdtT002Aut.acl.timer = 0;
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
	
	sTlkMmiRdtT002AutAcl.enable = true;
	sTlkMmiRdtT002AutAcl.minIntval = minIntval;
	sTlkMmiRdtT002AutAcl.maxIntval = maxIntval;
	sTlkMmiRdtT002Aut.acl.intval = (sTlkMmiRdtT002AutAcl.minIntval+sTlkMmiRdtT002AutAcl.maxIntval)*1000/2;
	if(sTlkMmiRdtT002Aut.acl.handle != 0){
		sTlkMmiRdtT002Aut.acl.timer = clock_time() | 1;
	}
}
static void tlkmmi_rdt_t002AutSetScoSwitchDeal(uint08 *pData, uint16 dataLen)
{
	uint32 minIntval;
	uint32 maxIntval;

	if(dataLen < 5){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Aut2SetAclSwitchDeal: error length [%d]", dataLen);
		return;
	}

	if(pData[0] == 0x00){
		sTlkMmiRdtT002AutSco.enable = false;
		sTlkMmiRdtT002Aut.sco.timer = 0;
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
	
	sTlkMmiRdtT002AutSco.enable = true;
	sTlkMmiRdtT002AutSco.minIntval = minIntval;
	sTlkMmiRdtT002AutSco.maxIntval = maxIntval;
	sTlkMmiRdtT002Aut.sco.intval = (sTlkMmiRdtT002AutSco.minIntval+sTlkMmiRdtT002AutSco.maxIntval)*1000/2;
	if(sTlkMmiRdtT002Aut.sco.handle != 0){
		sTlkMmiRdtT002Aut.sco.timer = clock_time() | 1;
	}
}



static int tlkmmi_rdt_t002AutAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	return -TLK_EFAIL;
}
static int tlkmmi_rdt_t002AutAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	uint08 dutAddt[6] = TLKMMI_RDT_T002_BTADDR_DUT;
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		if(sTlkMmiRdtT002Aut.acl.handle == 0 && sTlkMmiRdtT002Aut.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T002_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
		}
		return TLK_ENONE;
	}
	
	if(!sTlkMmiRdtT002Aut.isStart){
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	if(tmemcmp(pEvt->peerMac, dutAddt, 6) != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutAclConnectEvt: Unexpected");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}

	sTlkMmiRdtT002Aut.acl.handle = pEvt->handle;
	sTlkMmiRdtT002Aut.acl.timer  = 0;
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutAclConnectEvt: DUT");
	if(sTlkMmiRdtT002AutAcl.enable){
		uint16 interval;
		sTlkMmiRdtT002Aut.acl.timer = clock_time() | 1;
		interval = sTlkMmiRdtT002AutAcl.maxIntval-sTlkMmiRdtT002AutAcl.minIntval;
		if(interval != 0){
			interval = rand()%(interval+1);
			sTlkMmiRdtT002Aut.acl.intval = (sTlkMmiRdtT002AutAcl.minIntval+interval)*1000;
		}
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002AutAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;
	
	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(sTlkMmiRdtT002Aut.acl.handle == pEvt->handle){
		sTlkMmiRdtT002Aut.acl.flags |= TLKMMI_RDT_T002_FLAG_CRYPT;
		if(sTlkMmiRdtT002AutSco.enable){
			bth_sco_connect(sTlkMmiRdtT002Aut.acl.handle, BTH_SCO_LINK_TYPE_ESCO, BTH_SCO_AIRMODE_CVSD);
		}
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutAclEncryptEvt: DUT");
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002AutAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclDiscCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);
	
	if(sTlkMmiRdtT002Aut.acl.handle == pEvt->handle){
		sTlkMmiRdtT002Aut.acl.timer = 0;
		sTlkMmiRdtT002Aut.sco.timer = 0;
		sTlkMmiRdtT002Aut.acl.handle = 0;
		sTlkMmiRdtT002Aut.sco.handle = 0;
		sTlkMmiRdtT002Aut.sndTimer = 0;
		sTlkMmiRdtT002Aut.rcvTimer = 0;
		sTlkMmiRdtT002Aut.acl.busys = TLKMMI_RDT_T002_BUSY_NONE;
		sTlkMmiRdtT002Aut.acl.flags = TLKMMI_RDT_T002_FLAG_NONE;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutAclDisconnEvt: DUT");
		if(sTlkMmiRdtT002Aut.isStart){
			uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
			bth_acl_connect(dutAddr, TLKMMI_RDT_T002_BTDEV_CLASS_DUT, BTH_ROLE_MASTER, 40000);
		}
	}
		
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002AutScoConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnCompleteEvt_t *pEvt;

	pEvt = (bth_scoConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		if(sTlkMmiRdtT002AutSco.enable){
			bth_sco_connect(sTlkMmiRdtT002Aut.acl.handle, BTH_SCO_LINK_TYPE_ESCO, BTH_SCO_AIRMODE_CVSD);
		}
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutScoConnectEvt: ConnFail");
		return -TLK_EFAIL;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutScoConnectEvt");
	sTlkMmiRdtT002Aut.sco.handle = pEvt->scoHandle;
	sTlkMmiRdtT002Aut.sndTimer = clock_time() | 1;
	sTlkMmiRdtT002Aut.rcvTimer = clock_time() | 1;
	sTlkMmiRdtT002Aut.sndCount = 0;
	sTlkMmiRdtT002Aut.rcvCount = 0;
	if(sTlkMmiRdtT002AutSco.enable){
		uint16 interval;
		sTlkMmiRdtT002Aut.sco.timer = clock_time() | 1;
		interval = sTlkMmiRdtT002AutSco.maxIntval-sTlkMmiRdtT002AutSco.minIntval;
		if(interval != 0){
			interval = rand()%(interval+1);
			sTlkMmiRdtT002Aut.sco.intval = (sTlkMmiRdtT002AutSco.minIntval+interval)*1000;
		}
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002AutScoDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoDiscCompleteEvt_t *pEvt;

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002AutScoDisconnEvt");
	pEvt = (bth_scoDiscCompleteEvt_t*)pData;
	if(sTlkMmiRdtT002Aut.sco.handle == pEvt->scoHandle){
		sTlkMmiRdtT002Aut.sco.handle = 0;
		sTlkMmiRdtT002Aut.sco.timer = 0;
		sTlkMmiRdtT002Aut.sndTimer = 0;
		sTlkMmiRdtT002Aut.rcvTimer = 0;
		if(sTlkMmiRdtT002AutSco.enable){
			bth_sco_connect(sTlkMmiRdtT002Aut.acl.handle, BTH_SCO_LINK_TYPE_ESCO, BTH_SCO_AIRMODE_CVSD);
		}
	}
	
	return TLK_ENONE;
}

_attribute_bt_ram_code_
static void tlkmmi_rdt_t002AutAddSpkFrame(uint08 id, uint08 *pData, int dataLen)
{
	sTlkMmiRdtT002Aut.rcvCount ++;
	sTlkMmiRdtT002Aut.rcvTimer = clock_time() | 1;
}
_attribute_bt_ram_code_
static void tlkmmi_rdt_t002AutGetMicFrame(uint08 id, uint08 *pBuff, int buffLen)
{
	sTlkMmiRdtT002Aut.sndCount ++;
	sTlkMmiRdtT002Aut.sndTimer = clock_time() | 1;
}





#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


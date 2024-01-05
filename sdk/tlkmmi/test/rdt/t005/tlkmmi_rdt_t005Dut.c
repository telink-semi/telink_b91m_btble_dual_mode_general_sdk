/********************************************************************************************************
 * @file	tlkmmi_rdt_t005Dut.c
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
#if (TLKMMI_RDT_CASE_T005_ENABLE)
#include "../bt/tlkmmi_rdt_bt.h"
#include "tlkmmi_rdt_t005.h"
#include "tlkmmi_rdt_t005Dut.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"
#include "tlkstk/bt/btp/rfcomm/btp_rfcomm.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"
#include "tlkstk/bt/btp/hid/btp_hid.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlkstk/hci/hci_cmd.h"
#include "drivers.h"
#include "tlksys/tlksys_stdio.h"


extern int rand(void);

static int tlkmmi_rdt_t005DutAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t005DutAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t005DutAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t005DutAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t005DutPrfChannelEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t005DutPrfConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t005DutPrfDisconnEvt(uint08 *pData, uint16 dataLen);

static void tlkmmi_rdt_t005SendAclConnectEvt(uint16 handle, uint08 status, uint08 *pBtAddr);
static void tlkmmi_rdt_t005SendAclDisconnEvt(uint16 handle, uint08 reason, uint08 *pBtAddr);
static void tlkmmi_rdt_t005SendProfConnectEvt(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr);
static void tlkmmi_rdt_t005SendProfDisconnEvt(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr);

static uint08 sTlkMmiRdtT005WaitCount = 0;
static uint08 sTlkMmiRdtT005WaitPtype[TLKMMI_RDT_T005_WAIT_MAX_NUMB];
static uint08 sTlkMmiRdtT005WaitUsrID[TLKMMI_RDT_T005_WAIT_MAX_NUMB];



static tlkmmi_rdt_t005Dut_t sTlkMmiRdtT005Dut;



void tlkmmi_rdt_t005DutStart(void)
{
	uint08 dutAddr[6] = TLKMMI_RDT_T005_BTADDR_DUT;
	uint08 aut1Addr[6] = TLKMMI_RDT_T005_BTADDR_AUT;
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	uint08 aut2Addr[6] = TLKMMI_RDT_T005_BTADDR_AUT2;
	#endif

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutStart");

	sTlkMmiRdtT005Dut.isFail = false;
	sTlkMmiRdtT005Dut.isStart = true;
	sTlkMmiRdtT005Dut.dbgTimer = 0;
	sTlkMmiRdtT005Dut.aut1.handle = 0;
	sTlkMmiRdtT005Dut.aut2.handle = 0;
	sTlkMmiRdtT005Dut.aut1.flags  = TLKMMI_RDT_T005_FLAG_NONE;
	sTlkMmiRdtT005Dut.aut1.busys  = TLKMMI_RDT_T005_BUSY_NONE;
	sTlkMmiRdtT005Dut.aut2.flags  = TLKMMI_RDT_T005_FLAG_NONE;
	sTlkMmiRdtT005Dut.aut2.busys  = TLKMMI_RDT_T005_BUSY_NONE;
	
	
	bth_hci_sendWriteLocalNameCmd((uint08*)TLKMMI_RDT_T005_BTNAME_DUT);
	bth_hci_sendSetBtAddrCmd(dutAddr);
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_RDT_T005_BTDEV_CLASS_DUT);
	bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_t005DutAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_t005DutAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_t005DutAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_t005DutAclDisconnEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CHANNEL,  tlkmmi_rdt_t005DutPrfChannelEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  tlkmmi_rdt_t005DutPrfConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  tlkmmi_rdt_t005DutPrfDisconnEvt);
	
	tlkmmi_rdt_t005TableInit();

	bth_acl_connect(aut1Addr, TLKMMI_RDT_T005_BTDEV_CLASS_AUT, BTH_ROLE_SLAVE, 40000);
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	bth_acl_connect(aut2Addr, TLKMMI_RDT_T005_BTDEV_CLASS_AUT, BTH_ROLE_MASTER, 40000);
	#endif
}
void tlkmmi_rdt_t005DutClose(void)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutClose");
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  nullptr);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  nullptr);

	sTlkMmiRdtT005Dut.isStart = false;
	if(sTlkMmiRdtT005Dut.aut1.handle != 0){
		bth_acl_disconn(sTlkMmiRdtT005Dut.aut1.handle, 0x00);
		tmemset(&sTlkMmiRdtT005Dut.aut1, 0, sizeof(tlkmmi_rdt_t005DutUnit_t));
	}else{
		uint08 autAddr[6] = TLKMMI_RDT_T005_BTADDR_AUT;
		bth_acl_connectCancel(autAddr);
	}
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	if(sTlkMmiRdtT005Dut.aut2.handle != 0){
		bth_acl_disconn(sTlkMmiRdtT005Dut.aut2.handle, 0x00);
		tmemset(&sTlkMmiRdtT005Dut.aut2, 0, sizeof(tlkmmi_rdt_t005DutUnit_t));
	}else{
		uint08 autAddr[6] = TLKMMI_RDT_T005_BTADDR_AUT2;
		bth_acl_connectCancel(autAddr);
	}
	#endif
}
void tlkmmi_rdt_t005DutTimer(void)
{
	if(!sTlkMmiRdtT005Dut.isStart) return;
	if(sTlkMmiRdtT005Dut.aut1.handle != 0 && sTlkMmiRdtT005Dut.aut1.connTimer != 0 
		&& clock_time_exceed(sTlkMmiRdtT005Dut.aut1.connTimer, 10000000)){
		bth_acl_disconn(sTlkMmiRdtT005Dut.aut1.handle, 0x00);
		sTlkMmiRdtT005Dut.aut1.connTimer = 0;
		sTlkMmiRdtT005Dut.aut1.waitTimer = 0;
	}
	if(sTlkMmiRdtT005Dut.aut2.handle != 0 && sTlkMmiRdtT005Dut.aut2.connTimer != 0 
		&& clock_time_exceed(sTlkMmiRdtT005Dut.aut2.connTimer, 10000000)){
		bth_acl_disconn(sTlkMmiRdtT005Dut.aut2.handle, 0x00);
		sTlkMmiRdtT005Dut.aut2.connTimer = 0;
		sTlkMmiRdtT005Dut.aut2.waitTimer = 0;
	}
	if(sTlkMmiRdtT005Dut.aut1.handle != 0 && sTlkMmiRdtT005Dut.aut1.waitTimer != 0 
		&& clock_time_exceed(sTlkMmiRdtT005Dut.aut1.waitTimer, 3000000)){
		bth_acl_disconn(sTlkMmiRdtT005Dut.aut1.handle, 0x00);
		sTlkMmiRdtT005Dut.aut1.connTimer = 0;
		sTlkMmiRdtT005Dut.aut1.waitTimer = 0;
	}
	if(sTlkMmiRdtT005Dut.aut2.handle != 0 && sTlkMmiRdtT005Dut.aut2.waitTimer != 0 
		&& clock_time_exceed(sTlkMmiRdtT005Dut.aut2.waitTimer, 3000000)){
		bth_acl_disconn(sTlkMmiRdtT005Dut.aut2.handle, 0x00);
		sTlkMmiRdtT005Dut.aut2.connTimer = 0;
		sTlkMmiRdtT005Dut.aut2.waitTimer = 0;
	}
	if(sTlkMmiRdtT005Dut.dbgTimer != 0 && !clock_time_exceed(sTlkMmiRdtT005Dut.dbgTimer, 3000000)){
		return;
	}
	sTlkMmiRdtT005Dut.dbgTimer = clock_time()|1;
	if(sTlkMmiRdtT005Dut.isFail){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t005DutTimer: failure ptype[%d]-usrID[%d]",
			sTlkMmiRdtT005Dut.failPtype, sTlkMmiRdtT005Dut.failUsrID);
	}
	
}
void tlkmmi_rdt_t005DutInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	
}



static int tlkmmi_rdt_t005DutAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T005_BTADDR_AUT;
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	uint08 aut2Addt[6] = TLKMMI_RDT_T005_BTADDR_AUT2;
	#endif
	bth_aclConnRequestEvt_t *pEvt;

	if(!sTlkMmiRdtT005Dut.isStart || sTlkMmiRdtT005Dut.isFail) return -TLK_EFAIL;

	pEvt = (bth_aclConnRequestEvt_t*)pData;
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
		bth_acl_setInitRole(pEvt->peerMac, BTH_ROLE_SLAVE);
	}
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	else if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
		bth_acl_setInitRole(pEvt->peerMac, BTH_ROLE_NOT_SET);
	}
	#endif
	else{
		return -TLK_EFAIL;
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_t005DutAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T005_BTADDR_AUT;
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	uint08 aut2Addt[6] = TLKMMI_RDT_T005_BTADDR_AUT2;
	#endif
	bth_aclConnCompleteEvt_t *pEvt;
	tlkmmi_rdt_t005DutUnit_t *pUnit;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(!sTlkMmiRdtT005Dut.isStart){
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	if(pEvt->status != 0){
		uint08 aut1Addr[6] = TLKMMI_RDT_T005_BTADDR_AUT;
		#if (TLKMMI_RDT_T005_AUT2_ENABLE)
		uint08 aut2Addr[6] = TLKMMI_RDT_T005_BTADDR_AUT2;
		#endif
		if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
			bth_acl_connect(aut1Addr, TLKMMI_RDT_T005_BTDEV_CLASS_AUT, BTH_ROLE_SLAVE, 40000);
		}
		#if (TLKMMI_RDT_T005_AUT2_ENABLE)
		if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
			bth_acl_connect(aut2Addr, TLKMMI_RDT_T005_BTDEV_CLASS_AUT, BTH_ROLE_MASTER, 40000);
		}
		#endif
		tlkmmi_rdt_t005SendAclConnectEvt(pEvt->handle, pEvt->status, pEvt->peerMac);
		return TLK_ENONE;
	}

	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
		pUnit = &sTlkMmiRdtT005Dut.aut1;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN,
			"tlkmmi_rdt_t005DutAclConnectEvt: AUT1 addr[%02x:%02x:%02x:%02x:%02x:%02x]", 
			pEvt->peerMac[0], pEvt->peerMac[1], pEvt->peerMac[2],
			pEvt->peerMac[3], pEvt->peerMac[4], pEvt->peerMac[5]);
	}
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	else if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
		pUnit = &sTlkMmiRdtT005Dut.aut2;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN,
			"tlkmmi_rdt_t005DutAclConnectEvt: AUT2 addr[%02x:%02x:%02x:%02x:%02x:%02x]", 
			pEvt->peerMac[0], pEvt->peerMac[1], pEvt->peerMac[2],
			pEvt->peerMac[3], pEvt->peerMac[4], pEvt->peerMac[5]);
	}
	#endif
	else{
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutAclConnectEvt: Unexpected");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	pUnit->handle = pEvt->handle;
	pUnit->connTimer = clock_time() | 1;
	pUnit->waitTimer = clock_time() | 1;
	pUnit->flags = TLKMMI_RDT_T005_FLAG_NONE;
	pUnit->busys = TLKMMI_RDT_T005_BUSY_NONE;
	tmemcpy(pUnit->peerMac, pEvt->peerMac, 6);
	tlkmmi_rdt_t005SendAclConnectEvt(pUnit->handle, pEvt->status, pUnit->peerMac);

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutAclConnectEvt");
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t005DutAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;
	tlkmmi_rdt_t005DutUnit_t *pUnit;

	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(pEvt->handle == sTlkMmiRdtT005Dut.aut1.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut1;
	}else if(pEvt->handle == sTlkMmiRdtT005Dut.aut2.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut2;
	}else{
		return -TLK_EFAIL;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutAclEncryptEvt");
	
	btp_sdpclt_connect(pEvt->handle);
	pUnit->waitTimer = clock_time() | 1;
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t005DutAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bool isAut1 = false;
	bool isAut2 = false;
	uint08 aut1Addt[6] = TLKMMI_RDT_T005_BTADDR_AUT;
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	uint08 aut2Addt[6] = TLKMMI_RDT_T005_BTADDR_AUT2;
	#endif
	bth_aclDiscCompleteEvt_t *pEvt;
	tlkmmi_rdt_t005DutUnit_t *pUnit;

	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	if(pEvt->handle == sTlkMmiRdtT005Dut.aut1.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut1;
	}
	#if (TLKMMI_RDT_T005_AUT2_ENABLE)
	else if(pEvt->handle == sTlkMmiRdtT005Dut.aut2.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut2;
	}
	#endif
	else{
		pUnit = nullptr;
	}
	
	if(pUnit != nullptr){
		isAut1 = true;
		pUnit->flags = TLKMMI_RDT_T005_FLAG_NONE;
		pUnit->busys = TLKMMI_RDT_T005_BUSY_NONE;
		pUnit->connTimer = 0;
		pUnit->waitTimer = 0;
		tmemset(pUnit, 0, sizeof(tlkmmi_rdt_t005DutUnit_t));
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutAclDisconnEvt: AUT1");
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutAclDisconnEvt");
		
	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);
	tlkmmi_rdt_t005SendAclDisconnEvt(pEvt->handle, pEvt->reason, pEvt->peerMac);
	if(!sTlkMmiRdtT005Dut.isStart || sTlkMmiRdtT005Dut.isFail) return -TLK_EFAIL;
	
	if(isAut1){
		uint08 aut1Addr[6] = TLKMMI_RDT_T005_BTADDR_AUT;
		if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
			bth_acl_connect(aut1Addr, TLKMMI_RDT_T005_BTDEV_CLASS_AUT, BTH_ROLE_SLAVE, 40000);
		}
	}else if(isAut2){
		#if (TLKMMI_RDT_T005_AUT2_ENABLE)
		uint08 aut2Addr[6] = TLKMMI_RDT_T005_BTADDR_AUT2;
		if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
			bth_acl_connect(aut2Addr, TLKMMI_RDT_T005_BTDEV_CLASS_AUT, BTH_ROLE_MASTER, 40000);
		}
		#endif
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t005DutPrfChannelEvt(uint08 *pData, uint16 dataLen)
{
	btp_channelEvt_t *pEvt;
	tlkmmi_rdt_t005DutUnit_t *pUnit;

	pEvt = (btp_channelEvt_t*)pData;
	if(pEvt->handle == sTlkMmiRdtT005Dut.aut1.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut1;
	}else if(pEvt->handle == sTlkMmiRdtT005Dut.aut2.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut2;
	}else{
		return -TLK_EFAIL;
	}
	
	if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE){
		pUnit->hfChannel = pEvt->channel;
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE_AGW){
		pUnit->agChannel = pEvt->channel;
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_SERIAL_PORT){
		pUnit->sppChannel = pEvt->channel;
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_PBAP_PSE){
		pUnit->pbapChannel = pEvt->channel;
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t005DutPrfConnectEvt(uint08 *pData, uint16 dataLen)
{
	btp_connectEvt_t *pEvt;
	tlkmmi_rdt_t005DutUnit_t *pUnit;

	pEvt = (btp_connectEvt_t*)pData;
	if(pEvt->handle == sTlkMmiRdtT005Dut.aut1.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut1;
	}else if(pEvt->handle == sTlkMmiRdtT005Dut.aut2.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut2;
	}else{
		return -TLK_EFAIL;
	}

	if(pEvt->ptype == BTP_PTYPE_RFC){
		pUnit->flags |= TLKMMI_RDT_T005_FLAG_RFC;
	}
	if((pUnit->flags & TLKMMI_RDT_T005_FLAG_SDP) != 0 && (pUnit->flags & TLKMMI_RDT_T005_FLAG_RFC) != 0){
		if(pUnit->sppChannel != 0){
			btp_spp_connect(pEvt->handle, pUnit->sppChannel);
		}
		if(pUnit->hfChannel != 0){
			btp_hfp_connect(pEvt->handle, BTP_USRID_CLIENT, pUnit->hfChannel);
		}
		if(pUnit->pbapChannel != 0){
			btp_pbap_connect(pEvt->handle, BTP_USRID_CLIENT, pUnit->pbapChannel, false);
		}
		btp_hidd_connect(pEvt->handle);
		btp_avrcp_connect(pEvt->handle, BTP_USRID_IGNORE);
		btp_a2dp_connect(pEvt->handle, BTP_USRID_CLIENT);
	}
	if(pEvt->status != 0 && tlkmmi_rdt_t005TableIsHave(pEvt->ptype) && sTlkMmiRdtT005Dut.aut1.connTimer != 0){
		sTlkMmiRdtT005Dut.isFail = true;
		sTlkMmiRdtT005Dut.failPtype = pEvt->ptype;
		sTlkMmiRdtT005Dut.failUsrID = pEvt->usrID;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005DutPrfConnectEvt: failure ");
	}
	pUnit->waitTimer = clock_time() | 1;
	if(pEvt->ptype != BTP_PTYPE_RFC && pEvt->ptype != BTP_PTYPE_SDP){
		tlkmmi_rdt_t005SendProfConnectEvt(pEvt->handle, pEvt->status, pEvt->ptype, pEvt->usrID, pUnit->peerMac);
	}
			
	return TLK_ENONE;
}
static int tlkmmi_rdt_t005DutPrfDisconnEvt(uint08 *pData, uint16 dataLen)
{
	btp_disconnEvt_t *pEvt;
	tlkmmi_rdt_t005DutUnit_t *pUnit;

	pEvt = (btp_disconnEvt_t*)pData;
	if(pEvt->handle == sTlkMmiRdtT005Dut.aut1.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut1;
	}else if(pEvt->handle == sTlkMmiRdtT005Dut.aut2.handle){
		pUnit = &sTlkMmiRdtT005Dut.aut2;
	}else{
		return -TLK_EFAIL;
	}
	if(pEvt->ptype == BTP_PTYPE_SDP && pEvt->usrID == BTP_USRID_CLIENT){
		btp_rfcomm_connect(pEvt->handle);
		pUnit->flags |= TLKMMI_RDT_T005_FLAG_SDP;
	}
	if(pEvt->ptype != BTP_PTYPE_RFC && pEvt->ptype != BTP_PTYPE_SDP){
		tlkmmi_rdt_t005SendProfDisconnEvt(pEvt->handle, 0, pEvt->ptype, pEvt->usrID, pUnit->peerMac);
	}
	
	return TLK_ENONE;
}



void tlkmmi_rdt_t005TableInit(void)
{
	sTlkMmiRdtT005WaitCount = 0;

	tlkmmi_rdt_t005TableAppend(BTP_PTYPE_HFP, BTP_USRID_CLIENT);
	tlkmmi_rdt_t005TableAppend(BTP_PTYPE_SPP, BTP_USRID_IGNORE);
	tlkmmi_rdt_t005TableAppend(BTP_PTYPE_HID, BTP_USRID_SERVER);
	tlkmmi_rdt_t005TableAppend(BTP_PTYPE_A2DP, BTP_USRID_CLIENT);
	tlkmmi_rdt_t005TableAppend(BTP_PTYPE_PBAP, BTP_USRID_CLIENT);
	tlkmmi_rdt_t005TableAppend(BTP_PTYPE_AVRCP, BTP_USRID_IGNORE);
}
bool tlkmmi_rdt_t005TableIsHave(uint08 ptype)
{
	uint08 index;
	for(index=0; index<sTlkMmiRdtT005WaitCount; index++){
		if(sTlkMmiRdtT005WaitPtype[index] == ptype) break;
	}
	if(index == sTlkMmiRdtT005WaitCount) return false;
	return true;
}
void tlkmmi_rdt_t005TableAppend(uint08 ptype, uint08 usrID)
{
	uint08 count;

	count = sTlkMmiRdtT005WaitCount;
	if(count >= TLKMMI_RDT_T005_WAIT_MAX_NUMB) return;
	if(tlkmmi_rdt_t005TableIsHave(ptype)) return;
	
	sTlkMmiRdtT005WaitPtype[count] = ptype;
	sTlkMmiRdtT005WaitUsrID[count] = usrID;
	
	count++;
	sTlkMmiRdtT005WaitCount = count;
}
void tlkmmi_rdt_t005TableRemove(uint08 ptype)
{
	uint08 index;
	uint08 count;
	
	count = sTlkMmiRdtT005WaitCount;
	if(count == 0) return;
	
	for(index=0; index<count; index++){
		if(sTlkMmiRdtT005WaitPtype[index] == ptype) break;
	}
	if(index == count) return;
	
	if(index+1 != count){
		tmemcpy(&sTlkMmiRdtT005WaitPtype[index], &sTlkMmiRdtT005WaitPtype[index+1], count-index-1);
		tmemcpy(&sTlkMmiRdtT005WaitUsrID[index], &sTlkMmiRdtT005WaitUsrID[index+1], count-index-1);
	}
	count --;
	sTlkMmiRdtT005WaitCount = count;
}


static int tlkmmi_rdt_r005DutSendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_BT;
	head[headLen++] = evtID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND, head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
static void tlkmmi_rdt_t005SendAclConnectEvt(uint16 handle, uint08 status, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_BT_CHN_ACL; //Object
	buffer[buffLen++] = status; //Status
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_rdt_r005DutSendCommEvt(TLKPRT_COMM_EVTID_BT_CONNECT, buffer, buffLen);
}
static void tlkmmi_rdt_t005SendAclDisconnEvt(uint16 handle, uint08 reason, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_BT_CHN_ACL; //Object
	buffer[buffLen++] = reason; //reason
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_rdt_r005DutSendCommEvt(TLKPRT_COMM_EVTID_BT_DISCONN, buffer, buffLen);
}

uint08 tlkmmi_rdt_r005DutPtypeToCtype(uint08 ptype, uint08 usrID)
{
	uint08 ctype = 0xFF;
	switch(ptype){
		case BTP_PTYPE_HFP:
			if(usrID == BTP_USRID_SERVER) ctype = TLKPRT_COMM_BT_CHN_HFP_AG;
			else ctype = TLKPRT_COMM_BT_CHN_HFP_HF;
			break;
		case BTP_PTYPE_IAP:
			ctype = TLKPRT_COMM_BT_CHN_IAP;
			break;
		case BTP_PTYPE_SPP:
			ctype = TLKPRT_COMM_BT_CHN_SPP;
			break;
		case BTP_PTYPE_HID:
			ctype = TLKPRT_COMM_BT_CHN_HID;
			break;
		case BTP_PTYPE_ATT:
			ctype = TLKPRT_COMM_BT_CHN_ATT;
			break;
		case BTP_PTYPE_A2DP:
			if(usrID == BTP_USRID_CLIENT) ctype = TLKPRT_COMM_BT_CHN_A2DP_SNK;
			else ctype = TLKPRT_COMM_BT_CHN_A2DP_SRC;
			break;
		case BTP_PTYPE_AVRCP:
			ctype = TLKPRT_COMM_BT_CHN_AVRCP;
			break;
		case BTP_PTYPE_PBAP:
			ctype = TLKPRT_COMM_BT_CHN_PBAP;
			break;
		case BTP_PTYPE_AVRCP_BROWSING:
			ctype = TLKPRT_COMM_BT_CHN_AVRCP_BROWSING;
			break;
	}
	return ctype;
}
static void tlkmmi_rdt_t005SendProfConnectEvt(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	buffLen = 0;
	buffer[buffLen++] = tlkmmi_rdt_r005DutPtypeToCtype(ptype, usrID); //Object
	buffer[buffLen++] = status; //Status
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_rdt_r005DutSendCommEvt(TLKPRT_COMM_EVTID_BT_PROF_CONN, buffer, buffLen);
}
static void tlkmmi_rdt_t005SendProfDisconnEvt(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	buffLen = 0;
	buffer[buffLen++] = tlkmmi_rdt_r005DutPtypeToCtype(ptype, usrID); //Object
	buffer[buffLen++] = reason; //reason
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6);  //MAC
	buffLen += 6;
	tlkmmi_rdt_r005DutSendCommEvt(TLKPRT_COMM_EVTID_BT_PROF_DISC, buffer, buffLen);
}




#endif //#if (TLKMMI_RDT_CASE_T005_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


/********************************************************************************************************
 * @file	tlkmmi_rdt_t002Dut.c
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
#include "tlkmmi_rdt_t002Dut.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/hci/hci_cmd.h"
#include "drivers.h"

extern void btc_sco_regDataCB(void *prx, void *ptx);
static int tlkmmi_rdt_t002DutAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002DutAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002DutAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002DutAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002DutScoConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_t002DutScoDisconnEvt(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_t002DutAddSpkFrame(uint08 id, uint08 *pData, int dataLen);
static void tlkmmi_rdt_t002DutGetMicFrame(uint08 id, uint08 *pBuff, int buffLen);

static tlkmmi_rdt_t002Dut_t sTlkMmiRdtT002Dut = {0};


void tlkmmi_rdt_t002DutStart(void)
{
	uint08 dutAddr[6] = TLKMMI_RDT_T002_BTADDR_DUT;
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutStart");
	
	sTlkMmiRdtT002Dut.isStart = true;
	sTlkMmiRdtT002Dut.dbgTimer = clock_time() | 1;
	sTlkMmiRdtT002Dut.aut1Handle = 0;
	sTlkMmiRdtT002Dut.aut2Handle = 0;
	sTlkMmiRdtT002Dut.aut1Flags  = TLKMMI_RDT_T002_FLAG_NONE;
	sTlkMmiRdtT002Dut.aut1Busys  = TLKMMI_RDT_T002_BUSY_NONE;
	sTlkMmiRdtT002Dut.aut2Flags  = TLKMMI_RDT_T002_FLAG_NONE;
	sTlkMmiRdtT002Dut.aut2Busys  = TLKMMI_RDT_T002_BUSY_NONE;
	
	bth_hci_sendWriteLocalNameCmd(TLKMMI_RDT_T002_BTNAME_DUT);
	bth_hci_sendSetBtAddrCmd(dutAddr);
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_RDT_T002_BTDEV_CLASS_DUT);
	bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_t002DutAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_t002DutAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_t002DutAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_t002DutAclDisconnEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmmi_rdt_t002DutScoConnectEvt);
	bth_event_regCB(BTH_EVTID_SCODISC_COMPLETE, tlkmmi_rdt_t002DutScoDisconnEvt);
	
	btc_sco_regDataCB(tlkmmi_rdt_t002DutAddSpkFrame, tlkmmi_rdt_t002DutGetMicFrame);
	
	bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
}
void tlkmmi_rdt_t002DutClose(void)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutClose");
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  nullptr);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, nullptr);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT,  nullptr);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN,  nullptr);

	sTlkMmiRdtT002Dut.isStart = false;
	if(sTlkMmiRdtT002Dut.aut1Handle != 0){
		bth_acl_disconn(sTlkMmiRdtT002Dut.aut1Handle, 0x00);
		sTlkMmiRdtT002Dut.aut1Handle = 0;
		sTlkMmiRdtT002Dut.aut1Busys = TLKMMI_RDT_T002_BUSY_NONE;
		sTlkMmiRdtT002Dut.aut1Flags = TLKMMI_RDT_T002_FLAG_NONE;
	}
	if(sTlkMmiRdtT002Dut.aut2Handle != 0){
		bth_acl_disconn(sTlkMmiRdtT002Dut.aut2Handle, 0x00);
		sTlkMmiRdtT002Dut.aut2Handle = 0;
		sTlkMmiRdtT002Dut.aut2Busys = TLKMMI_RDT_T002_BUSY_NONE;
		sTlkMmiRdtT002Dut.aut2Flags = TLKMMI_RDT_T002_FLAG_NONE;
	}
	bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_DISABLE);
}
void tlkmmi_rdt_t002DutTimer(void)
{
	if(!sTlkMmiRdtT002Dut.isStart) return;
	if(sTlkMmiRdtT002Dut.dbgTimer != 0 && clock_time_exceed(sTlkMmiRdtT002Dut.dbgTimer, 3000000)){
		sTlkMmiRdtT002Dut.dbgTimer = clock_time()|1;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t002DutTimer: Handle1[0x%x],Handle2[%x],Send[0x%04x],Recv[0x%04x]",
			sTlkMmiRdtT002Dut.aut1Handle, sTlkMmiRdtT002Dut.aut2Handle,
			sTlkMmiRdtT002Dut.sndCount, sTlkMmiRdtT002Dut.rcvCount);
	}
	if(sTlkMmiRdtT002Dut.scoHandle == 0) return;
	if(sTlkMmiRdtT002Dut.sndTimer != 0 && clock_time_exceed(sTlkMmiRdtT002Dut.sndTimer, 500000)){
		sTlkMmiRdtT002Dut.sndTimer = clock_time()|1;
		tlkapi_fatal(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t002DutTimer: SCO Send Stall !!! - Send[0x%04x],Recv[0x%04x]",
			sTlkMmiRdtT002Dut.sndCount, sTlkMmiRdtT002Dut.rcvCount);
	}
	if(sTlkMmiRdtT002Dut.rcvTimer != 0 && clock_time_exceed(sTlkMmiRdtT002Dut.rcvTimer, 500000)){
		sTlkMmiRdtT002Dut.rcvTimer = clock_time()|1;
		tlkapi_fatal(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, 
			"tlkmmi_rdt_t002DutTimer: SCO Recv Stall !!! - Send[0x%04x],Recv[0x%04x]",
			sTlkMmiRdtT002Dut.sndCount, sTlkMmiRdtT002Dut.rcvCount);
	}
}
void tlkmmi_rdt_t002DutInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	
}


static int tlkmmi_rdt_t002DutAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T002_BTADDR_AUT;
	uint08 aut2Addt[6] = TLKMMI_RDT_T002_BTADDR_AUT2;
	bth_aclConnRequestEvt_t *pEvt;

	pEvt = (bth_aclConnRequestEvt_t*)pData;
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) != 0 && tmemcmp(pEvt->peerMac, aut2Addt, 6) != 0){
		return -TLK_EFAIL;
	}
	
	bth_acl_setInitRole(pEvt->peerMac, BTH_ROLE_NOT_SET);
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002DutAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T002_BTADDR_AUT;
	uint08 aut2Addt[6] = TLKMMI_RDT_T002_BTADDR_AUT2;
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
		return TLK_ENONE;
	}
	if(!sTlkMmiRdtT002Dut.isStart){
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) != 0 && tmemcmp(pEvt->peerMac, aut2Addt, 6) != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutAclConnectEvt: Unexpected");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
		sTlkMmiRdtT002Dut.aut1Handle = pEvt->handle;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutAclConnectEvt: AUT1");
	}
	if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
		sTlkMmiRdtT002Dut.aut2Handle = pEvt->handle;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutAclConnectEvt: AUT2");
	}
	if(sTlkMmiRdtT002Dut.aut1Handle == 0 || sTlkMmiRdtT002Dut.aut2Handle == 0){
		bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002DutAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;
	
	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(sTlkMmiRdtT002Dut.aut1Handle == pEvt->handle){
		sTlkMmiRdtT002Dut.aut1Flags |= TLKMMI_RDT_T002_FLAG_CRYPT;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutAclEncryptEvt: AUT1");
	}
	if(sTlkMmiRdtT002Dut.aut2Handle == pEvt->handle){
		sTlkMmiRdtT002Dut.aut2Flags |= TLKMMI_RDT_T002_FLAG_CRYPT;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutAclEncryptEvt: AUT2");
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002DutAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	uint08 aut1Addt[6] = TLKMMI_RDT_T002_BTADDR_AUT;
	uint08 aut2Addt[6] = TLKMMI_RDT_T002_BTADDR_AUT2;
	bth_aclDiscCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	if(tmemcmp(pEvt->peerMac, aut1Addt, 6) == 0){
		sTlkMmiRdtT002Dut.aut1Handle = 0;
		sTlkMmiRdtT002Dut.aut1Busys = TLKMMI_RDT_T002_BUSY_NONE;
		sTlkMmiRdtT002Dut.aut1Flags = TLKMMI_RDT_T002_FLAG_NONE;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutAclDisconnEvt: AUT1");
	}
	if(tmemcmp(pEvt->peerMac, aut2Addt, 6) == 0){
		sTlkMmiRdtT002Dut.aut2Handle = 0;
		sTlkMmiRdtT002Dut.aut2Busys = TLKMMI_RDT_T002_BUSY_NONE;
		sTlkMmiRdtT002Dut.aut2Flags = TLKMMI_RDT_T002_FLAG_NONE;
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutAclDisconnEvt: AUT2");
	}
		
	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);
	if(sTlkMmiRdtT002Dut.aut1Handle == 0 || sTlkMmiRdtT002Dut.aut2Handle == 0){
		if(sTlkMmiRdtT002Dut.isStart){
			bth_hci_sendWriteScanEnableCmd(BOTH_SCAN_ENABLE);
		}
	}

	if(sTlkMmiRdtT002Dut.aut1Handle == 0){
		sTlkMmiRdtT002Dut.rcvTimer = 0;
		sTlkMmiRdtT002Dut.sndTimer = 0;
		sTlkMmiRdtT002Dut.scoHandle = 0;
	}
		
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002DutScoConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_scoConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutScoConnectEvt: ConnFailure");
		return -TLK_EFAIL;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutScoConnectEvt");
	sTlkMmiRdtT002Dut.scoHandle = pEvt->scoHandle;
	sTlkMmiRdtT002Dut.sndTimer = clock_time() | 1;
	sTlkMmiRdtT002Dut.rcvTimer = clock_time() | 1;
	sTlkMmiRdtT002Dut.sndCount = 0;
	sTlkMmiRdtT002Dut.rcvCount = 0;
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002DutScoDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoDiscCompleteEvt_t *pEvt;

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002DutScoDisconnEvt");
	pEvt = (bth_scoDiscCompleteEvt_t*)pData;
	if(sTlkMmiRdtT002Dut.scoHandle == pEvt->scoHandle){
		sTlkMmiRdtT002Dut.scoHandle = 0;
		sTlkMmiRdtT002Dut.sndTimer = 0;
		sTlkMmiRdtT002Dut.rcvTimer = 0;
	}

	return TLK_ENONE;
}

_attribute_bt_ram_code_
static void tlkmmi_rdt_t002DutAddSpkFrame(uint08 id, uint08 *pData, int dataLen)
{
	sTlkMmiRdtT002Dut.rcvCount ++;
	sTlkMmiRdtT002Dut.rcvTimer = clock_time() | 1;
}
_attribute_bt_ram_code_
static void tlkmmi_rdt_t002DutGetMicFrame(uint08 id, uint08 *pBuff, int buffLen)
{
	sTlkMmiRdtT002Dut.sndCount ++;
	sTlkMmiRdtT002Dut.sndTimer = clock_time() | 1;
}


#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


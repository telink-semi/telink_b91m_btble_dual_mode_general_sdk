/********************************************************************************************************
 * @file	tlkmmi_rdt_btCore.c
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
#include "tlkmmi_rdt_bt.h"
#include "tlkmmi_rdt_btCore.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"


extern void bth_func_setAclHandle(uint16 aclHandle);
extern void bth_func_setScoHandle(uint16 scoHandle);
extern void btp_func_setAclHandle(uint16 aclHandle);
extern void btp_func_setSppRfcChn(uint08 rfcChannel);
extern void btp_func_setSppHfpHfChn(uint08 rfcChannel);
extern void btp_func_setSppHfpAgChn(uint08 rfcChannel);

static int tlkmmi_rdt_btAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btScoRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btScoConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btScoDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btProfileServiceEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btProfileChannelEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btProfileRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btProfileConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_btProfileDisconnEvt(uint08 *pData, uint16 dataLen);


static uint16 sTlkMmiRdtBtAclHandle = 0;
static uint16 sTlkMmiRdtBtScoHandle = 0;
static RdtBtAclConnectCB sTlkMmiRdtBtAclConnectCB = nullptr;
static RdtBtAclEncryptCB sTlkMmiRdtBtAclEncryptCB = nullptr;
static RdtBtAclDisconnCB sTlkMmiRdtBtAclDisconnCB = nullptr;
static RdtBtScoConnectCB sTlkMmiRdtBtScoConnectCB = nullptr;
static RdtBtScoDisconnCB sTlkMmiRdtBtScoDisconnCB = nullptr;
static RdtBtPrfConnectCB sTlkMmiRdtBtPrfConnectCB = nullptr;
static RdtBtPrfDisconnCB sTlkMmiRdtBtPrfDisconnCB = nullptr;
static RdtBtPrfSPPsendCB sTlkMmiRdtBtSPPsendCB = nullptr;


int tlkmmi_rdt_btCoreInit(void)
{
	uint08 index;
	uint08 btAddr[6];
	uint08 btName[TLK_CFG_FLASH_BT_NAME_LENS+1];
	uint08 bdaddr[6] = TLKMMI_BTRDT_BDADDR_DEF;
	unsigned int btNameAddr, btAddrAddr;
	
	btNameAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_NAME_ADDR);
	btAddrAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_ADDR_ADDR);
	if(btNameAddr == 0 || btAddrAddr){
		return -TLK_EFAIL;
	}		

	tlkapi_flash_read(btAddrAddr, btAddr, 6);
	if(btAddr[0] == 0xFF && btAddr[1] == 0xFF && btAddr[2] == 0xFF){
		tmemcpy(btAddr, bdaddr, 6);
	}
	bth_hci_sendSetBtAddrCmd(btAddr);
	
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_BTRDT_DEVICE_CLASS);
	bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode
	
	tlkapi_flash_read(btNameAddr, btName, TLK_CFG_FLASH_BT_NAME_LENS-1);
	for(index=0; index<TLK_CFG_FLASH_BT_NAME_LENS-1; index++){
		if(btName[index] == 0xFF || btName[index] == 0x00) break;
	}
	if(index == 0){
		index = strlen(TLKMMI_BTRDT_BTNAME_DEF);
		if(index > TLK_CFG_FLASH_BT_NAME_LENS) index = TLK_CFG_FLASH_BT_NAME_LENS;
		tmemcpy(btName, TLKMMI_BTRDT_BTNAME_DEF, index);
	}
	btName[index] = 0;
	bth_hci_sendWriteLocalNameCmd(btName);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_rdt_btAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_rdt_btAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_rdt_btAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_rdt_btAclDisconnEvt);

	bth_event_regCB(BTH_EVTID_SCOCONN_REQUEST,  tlkmmi_rdt_btScoRequestEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmmi_rdt_btScoConnectEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmmi_rdt_btScoDisconnEvt);

	btp_event_regCB(BTP_EVTID_PROFILE_SERVICE, tlkmmi_rdt_btProfileServiceEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CHANNEL, tlkmmi_rdt_btProfileChannelEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_REQUEST, tlkmmi_rdt_btProfileRequestEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT, tlkmmi_rdt_btProfileConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN, tlkmmi_rdt_btProfileDisconnEvt);

	return TLK_ENONE;
}

void tlkmmi_rdt_btSetName(uint08 *pName, uint08 nameLen)
{
	uint08 btName[TLK_CFG_FLASH_BT_NAME_LENS+1];

	if(pName == nullptr || nameLen == 0) return;
	if(nameLen > TLK_CFG_FLASH_BT_NAME_LENS) nameLen = TLK_CFG_FLASH_BT_NAME_LENS;
	tmemcpy(btName, pName, nameLen);
	pName[nameLen] = 0;
	bth_hci_sendWriteLocalNameCmd(btName);
}
void tlkmmi_rdt_btSetAddr(uint08 *pAddr)
{
	if(pAddr == nullptr) return;
	bth_hci_sendSetBtAddrCmd(pAddr);
}

void tlkmmi_rdt_btRegAclConnectCB(RdtBtAclConnectCB connectCB)
{
	sTlkMmiRdtBtAclConnectCB = connectCB;
}
void tlkmmi_rdt_btRegAclEncryptCB(RdtBtAclEncryptCB encryptCB)
{
	sTlkMmiRdtBtAclEncryptCB = encryptCB;
}
void tlkmmi_rdt_btRegAclDisconnCB(RdtBtAclDisconnCB disconnCB)
{
	sTlkMmiRdtBtAclDisconnCB = disconnCB;
}
void tlkmmi_rdt_btRegScoConnectCB(RdtBtScoConnectCB connectCB)
{
	sTlkMmiRdtBtScoConnectCB = connectCB;
}
void tlkmmi_rdt_btRegScoDisconnCB(RdtBtScoDisconnCB disconnCB)
{
	sTlkMmiRdtBtScoDisconnCB = disconnCB;
}
void tlkmmi_rdt_btRegPrfConnectCB(RdtBtPrfConnectCB connectCB)
{
	sTlkMmiRdtBtPrfConnectCB = connectCB;
}
void tlkmmi_rdt_btRegPrfDisconnCB(RdtBtPrfDisconnCB disconnCB)
{
	sTlkMmiRdtBtPrfDisconnCB = disconnCB;
}
void tlkmmi_rdt_btRegsppsendCB(RdtBtPrfSPPsendCB sppsendCB)
{
	sTlkMmiRdtBtSPPsendCB = sppsendCB;
}

int tlkmmi_rdt_btStartScan(bool enInqScan, bool enPageScan)
{
	uint08 scan = 0;
	if(enInqScan) scan |= 0x01;
	if(enPageScan) scan |= 0x02;
	return bth_hci_sendWriteScanEnableCmd(scan);
}
int tlkmmi_rdt_btCloseScan(void)
{
	return bth_hci_sendWriteScanEnableCmd(0x00);
}

int tlkmmi_rdt_btConnect(uint08 btaddr[6], uint32 devClass, uint08 initRole)
{
	uint08 data[10];
	tmemcpy(data, btaddr, 6);
	tmemcpy(data+6, &devClass, 3);
	data[9] = initRole;
	return bth_acl_connect(btaddr, devClass, initRole, 40000);
}
int tlkmmi_rdt_btDisconn(uint16 handle)
{
	if(handle == 0) handle = sTlkMmiRdtBtAclHandle;
	return bth_acl_disconn(handle, 0x00);
}

void tlkmmi_rdt_btClsPeerInfo(void)
{
	bth_device_clsItem();
}

static int tlkmmi_rdt_btAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	uint08 btRole;
	bth_aclConnRequestEvt_t *pEvt;

	pEvt = (bth_aclConnRequestEvt_t*)pData;
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btAclRequestEvt: {devClass - 0x%x}", pEvt->devClass);

	uint08 devType = bth_devClassToDevType(pEvt->devClass);
	if(devType == BTH_REMOTE_DTYPE_HEADSET){
		btRole = BTH_ROLE_MASTER;
	}else if((devType == BTH_REMOTE_DTYPE_COMPUTER)||( devType == BTH_REMOTE_DTYPE_PHONE)){
		btRole = BTH_ROLE_SLAVE;
	}else{
		btRole = BTH_ROLE_NOT_SET;
	}
	btRole = BTH_ROLE_NOT_SET;
	bth_acl_setInitRole(pEvt->peerMac, btRole);
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_btAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btAclConnectEvt: failure -- %d", pEvt->status);
		return TLK_ENONE;
	}
	
	tlkapi_array(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btAclConnectEvt", pData, dataLen);
	
	sTlkMmiRdtBtAclHandle = pEvt->handle;
	bth_func_setAclHandle(sTlkMmiRdtBtAclHandle);
	btp_func_setAclHandle(sTlkMmiRdtBtAclHandle);
	if(sTlkMmiRdtBtAclConnectCB != nullptr){
		sTlkMmiRdtBtAclConnectCB(pEvt->handle, pEvt->status);
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_btAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	bth_encryptCompleteEvt_t *pEvt;
	
	pEvt = (bth_encryptCompleteEvt_t*)pData;
	if(sTlkMmiRdtBtAclEncryptCB != nullptr){
		sTlkMmiRdtBtAclEncryptCB(pEvt->handle, pEvt->status);
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_btAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclDiscCompleteEvt_t *pEvt;

	pEvt = (bth_aclDiscCompleteEvt_t*)pData;

	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btAclDisconnEvt: success - 0x%x", pEvt->handle);

	sTlkMmiRdtBtScoHandle = 0;
	bth_func_setAclHandle(sTlkMmiRdtBtAclHandle);
	btp_func_setAclHandle(sTlkMmiRdtBtAclHandle);
	if(sTlkMmiRdtBtAclDisconnCB != nullptr){
		sTlkMmiRdtBtAclDisconnCB(pEvt->handle, pEvt->reason);
	}

	return TLK_ENONE;
}


static int tlkmmi_rdt_btScoRequestEvt(uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static int tlkmmi_rdt_btScoConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_scoConnCompleteEvt_t*)pData;
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btScoConnectEvt: failure -- %d", pEvt->status);
		return TLK_ENONE;
	}
	
	tlkapi_array(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btScoConnectEvt", pData, dataLen);
	
	sTlkMmiRdtBtScoHandle = pEvt->scoHandle;
	bth_func_setScoHandle(sTlkMmiRdtBtScoHandle);
	if(sTlkMmiRdtBtScoConnectCB != nullptr){
		sTlkMmiRdtBtScoConnectCB(pEvt->scoHandle, pEvt->status);
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_btScoDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoDiscCompleteEvt_t *pEvt;

	pEvt = (bth_scoDiscCompleteEvt_t*)pData;

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btScoDisconnEvt: success - 0x%x", pEvt->scoHandle);

	sTlkMmiRdtBtScoHandle = 0;
	bth_func_setScoHandle(sTlkMmiRdtBtScoHandle);
	if(sTlkMmiRdtBtScoDisconnCB != nullptr){
		sTlkMmiRdtBtScoDisconnCB(pEvt->scoHandle, pEvt->reason);
	}
	
	return TLK_ENONE;
}



static int tlkmmi_rdt_btProfileServiceEvt(uint08 *pData, uint16 dataLen)
{
	//Not Support.
	return TLK_ENONE;
}
static int tlkmmi_rdt_btProfileChannelEvt(uint08 *pData, uint16 dataLen)
{
	btp_channelEvt_t *pEvt;

	pEvt = (btp_channelEvt_t*)pData;
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_btProfileChannelEvt:{handle-0x%x,service-%d,channel-%d}",
		pEvt->handle, pEvt->service, pEvt->channel);
	
	if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE){
		btp_func_setSppHfpHfChn(pEvt->channel);
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE_AGW){
		btp_func_setSppHfpAgChn(pEvt->channel);
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_SERIAL_PORT){
		tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "BTP_SDP_SRVCLASS_ID_SERIAL_PORT:{handle-0x%x,service-%d,channel-%d}",
			pEvt->handle, pEvt->service, pEvt->channel);
		btp_func_setSppRfcChn(pEvt->channel);
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_PBAP_PSE){
		
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_IAP2_TEMP){
		
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_btProfileRequestEvt(uint08 *pData, uint16 dataLen)
{
//	btp_requestEvt_t *pEvt;
//	pEvt = (btp_requestEvt_t*)pData;
	return TLK_ENONE;
}
static int tlkmmi_rdt_btProfileConnectEvt(uint08 *pData, uint16 dataLen)
{
	btp_connectEvt_t *pEvt;
	pEvt = (btp_connectEvt_t*)pData;
	if(sTlkMmiRdtBtPrfConnectCB != nullptr){
		sTlkMmiRdtBtPrfConnectCB(sTlkMmiRdtBtAclHandle, pEvt->ptype, pEvt->status);
	}
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_btProfileDisconnEvt(uint08 *pData, uint16 dataLen)
{
	btp_disconnEvt_t *pEvt;
	pEvt = (btp_disconnEvt_t*)pData;
	if(sTlkMmiRdtBtPrfDisconnCB != nullptr){
		sTlkMmiRdtBtPrfDisconnCB(pEvt->handle, pEvt->ptype, TLK_ENONE);
	}
	return TLK_ENONE;
}






#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


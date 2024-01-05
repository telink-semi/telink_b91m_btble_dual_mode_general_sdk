/********************************************************************************************************
 * @file	tlkmmi_ptsBt.c
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
#include "../tlkmmi_testStdio.h"
#if (TLK_TEST_PTS_ENABLE)
#include "tlkmmi_pts.h"
#include "tlkmmi_ptsBt.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlkstk/bt/bth/bth_sco.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"


extern void bth_acl_setWaitTimeout(uint16 timeout);
extern void bth_func_setAclHandle(uint16 aclHandle);
extern void bth_func_setScoHandle(uint16 scoHandle);
extern void btp_func_setAclHandle(uint16 aclHandle);
extern void btp_func_setSppRfcChn(uint08 rfcChannel);
extern void btp_func_setSppHfpHfChn(uint08 rfcChannel);
extern void btp_func_setSppHfpAgChn(uint08 rfcChannel);

static int tlkmmi_pts_btAclRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btAclConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btAclEncryptEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btAclDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btScoRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btScoConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btScoDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileServiceEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileChannelEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_btProfileDisconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmmi_pts_sinkStatusChangedEvt(uint08 *pData, uint16 dataLen);


uint08 gTlkMmiTestPtsHfpChannel = 0;
uint08 gTlkMmiTestPtsSppChannel = 0;
uint16 gTlkMmiTestPtsBtAclHandle = 0;
uint16 gTlkMmiTestPtsBtScoHandle = 0;
uint08 gTlkMmiTestPtsDongleAddr[6] = {0};


int tlkmmi_pts_btInit(void)
{
	uint08 index;
	uint08 btAddr[6];
	uint08 btName[TLK_CFG_FLASH_BT_NAME_LENS+1];
	uint08 bdaddr[6] = TLKMMI_BTPTS_BDADDR_DEF;
	unsigned int btNameAddr, btAddrAddr;
	
	btNameAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_NAME_ADDR);
	btAddrAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_ADDR_ADDR);
	if(btNameAddr == 0 || btAddrAddr == 0){
		return -TLK_EFAIL;
	}		

	bth_acl_setWaitTimeout(15000);
	tlkapi_flash_read(btAddrAddr, btAddr, 6);
	if(btAddr[0] == 0xFF && btAddr[1] == 0xFF && btAddr[2] == 0xFF){
		tmemcpy(btAddr, bdaddr, 6);
	}
	bth_hci_sendSetBtAddrCmd(btAddr);
	
	bth_hci_sendWriteClassOfDeviceCmd(TLKMMI_BTPTS_DEVICE_CLASS);
	bth_hci_sendWriteSimplePairingModeCmd(1);// enable simple pairing mode
	
	tlkapi_flash_read(btNameAddr, btName, TLK_CFG_FLASH_BT_NAME_LENS-1);
	for(index=0; index<TLK_CFG_FLASH_BT_NAME_LENS-1; index++){
		if(btName[index] == 0xFF || btName[index] == 0x00) break;
	}
	if(index == 0){
		index = strlen(TLKMMI_BTPTS_BTNAME_DEF);
		if(index > TLK_CFG_FLASH_BT_NAME_LENS) index = TLK_CFG_FLASH_BT_NAME_LENS;
		tmemcpy(btName, TLKMMI_BTPTS_BTNAME_DEF, index);
	}
	btName[index] = 0;
	bth_hci_sendWriteLocalNameCmd(btName);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();
	
	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmmi_pts_btAclRequestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmmi_pts_btAclConnectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmmi_pts_btAclEncryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmmi_pts_btAclDisconnEvt);

	bth_event_regCB(BTH_EVTID_SCOCONN_REQUEST,  tlkmmi_pts_btScoRequestEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmmi_pts_btScoConnectEvt);
	bth_event_regCB(BTH_EVTID_SCODISC_COMPLETE, tlkmmi_pts_btScoDisconnEvt);

	btp_event_regCB(BTP_EVTID_PROFILE_SERVICE, tlkmmi_pts_btProfileServiceEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CHANNEL, tlkmmi_pts_btProfileChannelEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_REQUEST, tlkmmi_pts_btProfileRequestEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT, tlkmmi_pts_btProfileConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN, tlkmmi_pts_btProfileDisconnEvt);

	btp_event_regCB(BTP_EVTID_A2DPSNK_STATUS_CHANGED, tlkmmi_pts_sinkStatusChangedEvt);

	return TLK_ENONE;
}

void tlkmmi_pts_btConnDevice(uint08 *pAddr)
{
	bth_acl_connect(gTlkMmiTestPtsDongleAddr, 0x240404, 1, 5000);
}
void tlkmmi_pts_btDiscDevice(void)
{
	bth_acl_disconn(gTlkMmiTestPtsBtAclHandle, 0x00);
}

void tlkmmi_pts_btConnProfile(uint08 ptype, uint08 usrID)
{

}
void tlkmmi_pts_btDiscProfile(uint08 ptype, uint08 usrID)
{

}

static int tlkmmi_pts_btAclRequestEvt(uint08 *pData, uint16 dataLen)
{
	uint08 btRole;
	bth_aclConnRequestEvt_t *pEvt;

	pEvt = (bth_aclConnRequestEvt_t*)pData;
	
	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclRequestEvt: {devClass - 0x%x}", pEvt->devClass);

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
static int tlkmmi_pts_btAclConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclConnectEvt: failure -- %d", pEvt->status);
		return TLK_ENONE;
	}
	
	tlkapi_array(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclConnectEvt", pData, dataLen);
	
	gTlkMmiTestPtsBtAclHandle = pEvt->handle;
	bth_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);
	btp_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);
	
	return TLK_ENONE;
}
static int tlkmmi_pts_btAclEncryptEvt(uint08 *pData, uint16 dataLen)
{
	return TLK_ENONE;
}
static int tlkmmi_pts_btAclDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_aclDiscCompleteEvt_t *pEvt;

	pEvt = (bth_aclDiscCompleteEvt_t*)pData;

	btp_destroy(pEvt->handle);
	bth_destroy(pEvt->handle);

	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btAclDisconnEvt: success - 0x%x", pEvt->handle);

	gTlkMmiTestPtsBtScoHandle = 0;
	bth_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);
	btp_func_setAclHandle(gTlkMmiTestPtsBtAclHandle);
	bth_device_clsItem();

	return TLK_ENONE;
}


static int tlkmmi_pts_btScoRequestEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnRequestEvt_t *pEvt;
	bth_acl_handle_t *pAclItem;

	uint08 airMode;
	uint08 codec = 0;

	pEvt = (bth_scoConnRequestEvt_t*)pData;
	tlkapi_array(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN,  "tlkmdi_btsco_connReqEvt:", pEvt->peerMac, 6);
	pAclItem = bth_handle_searchConnAcl(pEvt->peerMac);
	if(pAclItem == nullptr)
	{
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "no connected ACL device");
		return -TLK_ENODEV;
	}

	btp_hfp_getCodec(pAclItem->aclHandle, &codec);

	if(codec == BTP_HFP_CODEC_TYPE_MSBC) airMode = BTH_SCO_AIRMODE_TRANS;
	else airMode = BTH_SCO_AIRMODE_CVSD;
	
	return bth_sco_setAirModeByAclHandle(pAclItem->aclHandle, airMode);
}
static int tlkmmi_pts_btScoConnectEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_scoConnCompleteEvt_t*)pData;
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btScoConnectEvt: failure -- %d", pEvt->status);
		return TLK_ENONE;
	}
	
	tlkapi_array(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btScoConnectEvt", pData, dataLen);
	
	gTlkMmiTestPtsBtScoHandle = pEvt->scoHandle;
	bth_func_setScoHandle(gTlkMmiTestPtsBtScoHandle);
	
	return TLK_ENONE;
}
static int tlkmmi_pts_btScoDisconnEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoDiscCompleteEvt_t *pEvt;

	pEvt = (bth_scoDiscCompleteEvt_t*)pData;

	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btScoDisconnEvt: success - 0x%x", pEvt->scoHandle);

	gTlkMmiTestPtsBtScoHandle = 0;
	bth_func_setScoHandle(gTlkMmiTestPtsBtScoHandle);
	
	return TLK_ENONE;
}



static int tlkmmi_pts_btProfileServiceEvt(uint08 *pData, uint16 dataLen)
{
	//Not Support.
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileChannelEvt(uint08 *pData, uint16 dataLen)
{
	btp_channelEvt_t *pEvt;

	pEvt = (btp_channelEvt_t*)pData;
	
	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_btProfileChannelEvt:{handle-0x%x,service-%d,channel-%d}",
		pEvt->handle, pEvt->service, pEvt->channel);
	
	if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE){
		btp_func_setSppHfpHfChn(pEvt->channel);
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE_AGW){
		btp_func_setSppHfpAgChn(pEvt->channel);
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_SERIAL_PORT){
		btp_func_setSppRfcChn(pEvt->channel);
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_PBAP_PSE){
		
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_IAP2_TEMP){
		
	}
	
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileRequestEvt(uint08 *pData, uint16 dataLen)
{
//	btp_requestEvt_t *pEvt;
//	pEvt = (btp_requestEvt_t*)pData;
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileConnectEvt(uint08 *pData, uint16 dataLen)
{
//	btp_connectEvt_t *pEvt;
//	pEvt = (btp_connectEvt_t*)pData;
	return TLK_ENONE;
}
static int tlkmmi_pts_btProfileDisconnEvt(uint08 *pData, uint16 dataLen)
{
//	btp_disconnEvt_t *pEvt;
//	pEvt = (btp_disconnEvt_t*)pData;

	return TLK_ENONE;
}

static int tlkmmi_pts_sinkStatusChangedEvt(uint08 *pData, uint16 dataLen)
{
	btp_a2dpStatusChangeEvt_t *pEvt;
	pEvt = (btp_a2dpStatusChangeEvt_t*)pData;
	if(pEvt->status == BTP_A2DP_STATUS_STREAM){
		btp_avrcp_setPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PLAYING);
	}else{
		btp_avrcp_setPlayState(pEvt->handle, BTP_AVRCP_PLAY_STATE_PAUSED);
	}
	return TLK_ENONE;
}


#endif //#if (TLK_TEST_PTS_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

/********************************************************************************************************
 * @file     tlkmdi_btsco.c
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
#include "tlkmdi/bt/tlkmdi_btsco.h"
#include "tlksys/tlksys_stdio.h"
#if (TLK_STK_BT_ENABLE)
#include "tlkstk/bt/bth/bth_event.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth_sco.h"
#include "tlkstk/bt/bth/bth.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"

#define TLKMDI_BTSCO_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_SCO << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTSCO_DBG_SIGN       "[MSCO]"


extern int btp_hfp_getCodec(uint16 aclHandle, uint08 *pCodec);
extern uint16 btp_hfp_getAgHandle(void);
extern uint16 btp_hfp_getHfHandle(void);

static int tlkmdi_btsco_connReqEvt(uint08 *pData,uint16 dataLen);
static int tlkmdi_btsco_connectEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btsco_disconnEvt(uint08 *pData, uint16 dataLen);
tlkmdi_btsco_handle_t *tlkmdi_btsco_getIdleItem(void);
static void tlkmdi_btsco_resetItemByAclHandle(uint16 aclHandle);
static tlkmdi_btsco_handle_t *tlkmdi_btsco_getOtherUsedItem(uint16 aclHandle);

static TlkMdiBtScoConnCB sTlkMdiBtScoConnCB = nullptr;
static tlkmdi_btsco_ctrl_t sTlkMdiBtScoCtrl;

int tlkmdi_btsco_init(void)
{
	tmemset(&sTlkMdiBtScoCtrl, 0, sizeof(tlkmdi_btsco_ctrl_t));

	bth_event_regCB(BTH_EVTID_SCOCONN_REQUEST,  tlkmdi_btsco_connReqEvt);
	bth_event_regCB(BTH_EVTID_SCOCONN_COMPLETE, tlkmdi_btsco_connectEvt);
	bth_event_regCB(BTH_EVTID_SCODISC_COMPLETE, tlkmdi_btsco_disconnEvt);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_audsco_regCB
 * Descript: Register SCO connection status switch callback. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_btsco_regCB(TlkMdiBtScoConnCB connCB)
{
	sTlkMdiBtScoConnCB = connCB;
}


void tlkmdi_btsco_setCurRole(uint08 role)
{
	sTlkMdiBtScoCtrl.curRole = role;
}
uint08 tlkmdi_btsco_getCurRole()
{
	return sTlkMdiBtScoCtrl.curRole;
}
static int tlkmdi_btsco_connReqEvt(uint08 * pData, uint16 dataLen)
{
	bth_scoConnRequestEvt_t *pEvt;
	bth_acl_handle_t *pAclItem;
	
	uint08 airMode;
	uint08 codec = 0;

	pEvt = (bth_scoConnRequestEvt_t*)pData;
	tlkapi_array(TLKMDI_BTSCO_DBG_FLAG, TLKMDI_BTSCO_DBG_SIGN, "tlkmdi_btsco_connReqEvt:", pEvt->peerMac, 6);
	pAclItem = bth_handle_searchConnAcl(pEvt->peerMac);
	if(pAclItem == nullptr)
	{
		tlkapi_error(TLKMDI_BTSCO_DBG_FLAG,TLKMDI_BTSCO_DBG_SIGN,"no connected ACL device");
		return -TLK_ENODEV;
	}

	btp_hfp_getCodec(pAclItem->aclHandle, &codec);

	if(codec == BTP_HFP_CODEC_TYPE_MSBC) airMode = BTH_SCO_AIRMODE_TRANS;
	else airMode = BTH_SCO_AIRMODE_CVSD;

	return bth_sco_setAirModeByAclHandle(pAclItem->aclHandle, airMode);
}

static int tlkmdi_btsco_connectEvt(uint08 *pData, uint16 dataLen)
{
	bth_scoConnCompleteEvt_t *pEvt;
	tlkmdi_btsco_handle_t *pItem;
	uint08 pBuffer[10];
	uint16 buffLen;

	pEvt = (bth_scoConnCompleteEvt_t*)pData;
	if(pEvt->status != 0){
		tlkapi_error(TLKMDI_BTSCO_DBG_FLAG, TLKMDI_BTSCO_DBG_SIGN, "tlkmdi_btsco_connectEvt: failure - %d", pEvt->status);
		return TLK_ENONE;
	}
	tlkapi_trace(TLKMDI_BTSCO_DBG_FLAG, TLKMDI_BTSCO_DBG_SIGN, 
		"tlkmdi_btsco_connectEvt: {status-%d,handle-0x%x,scoHandle-0x%x,linkType-%d}", 
		pEvt->status, pEvt->aclHandle, pEvt->scoHandle, pEvt->linkType);
	sTlkMdiBtScoCtrl.scoCnt++;
	pItem = tlkmdi_btsco_getIdleItem();
	if(pItem == nullptr) {
		tlkapi_error(TLKMDI_BTSCO_DBG_FLAG, TLKMDI_BTSCO_DBG_SIGN, "tlkmdi_btsco_connectEvt - no enough item:%d", pEvt->aclHandle);
		return -TLK_ENOITEM;
	}
	pItem->aclHandle = pEvt->aclHandle;
	pItem->scoHandle = pEvt->scoHandle;
	buffLen = 0;

	if(bth_handle_getConnScoCount() < TLKMDI_BTSCO_MAX_NUM){
		pBuffer[buffLen++] = (pItem->aclHandle & 0xFF);
		pBuffer[buffLen++] = (pItem->aclHandle & 0xFF00) >> 8;
		pBuffer[buffLen++] = (pItem->scoHandle & 0xFF);
		pBuffer[buffLen++] = (pItem->scoHandle & 0xFF00) >> 8;
		if(!btp_hfp_getCodec(pEvt->aclHandle,&pItem->codec)) {
			pBuffer[buffLen++] = pItem->codec;
		}else {
			pBuffer[buffLen++] = TLKMDI_BTSCO_CODEC_ID_CVSD;
		}
		tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_ENABLE_SCO, pBuffer, buffLen);
	}else if(bth_handle_getConnScoCount() == TLKMDI_BTSCO_MAX_NUM && btp_hfp_getHfHandle() && btp_hfp_getAgHandle()) {  //enable esco - only hf+ag
		#if (TLK_MDI_AUDDSCO_ENABLE)
		uint08 index;
		for(index = 0;index < TLKMDI_BTSCO_MAX_NUM;index++) {
			if(sTlkMdiBtScoCtrl.pItem[index].aclHandle == 0 || sTlkMdiBtScoCtrl.pItem[index].scoHandle == 0) break;
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].aclHandle & 0xFF);
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].aclHandle & 0xFF00) >> 8;
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].scoHandle & 0xFF);
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].scoHandle & 0xFF00) >> 8;
			if(!btp_hfp_getCodec(pEvt->aclHandle,&sTlkMdiBtScoCtrl.pItem[index].codec)) {
				pBuffer[buffLen++] = sTlkMdiBtScoCtrl.pItem[index].codec;
			}else {
				pBuffer[buffLen++] = TLKMDI_BTSCO_CODEC_ID_CVSD;
			}
		}	
		if(index == TLKMDI_BTSCO_MAX_NUM) {
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_ENABLE_DSCO, pBuffer, buffLen);
		}
		
		#endif
	}

	if(sTlkMdiBtScoConnCB != nullptr){
		sTlkMdiBtScoConnCB(pEvt->aclHandle, pEvt->scoHandle, true);
	}

	#if (TLK_MDI_MULTIPNT_ENABLE)
		buffLen = 0;
		pBuffer[buffLen++] = pEvt->aclHandle & 0xFF;
		pBuffer[buffLen++] = ((pEvt->aclHandle & 0xFF00) >> 8);
		pBuffer[buffLen++] = 1;//sco conn;
		tlksys_sendInnerMsg(TLKSYS_TASKID_PHONE, TLKPTI_PHONE_MSGID_CALL_SCO_UPDATE_EVT, pBuffer, buffLen);
	#endif
	
	return TLK_ENONE;
}

static int tlkmdi_btsco_disconnEvt(uint08 * pData, uint16 dataLen)
{
	bth_scoDiscCompleteEvt_t *pEvt;
	uint08 pBuffer[10];
	uint16 buffLen;

	pEvt = (bth_scoDiscCompleteEvt_t*)pData;
	tlkapi_trace(TLKMDI_BTSCO_DBG_FLAG, TLKMDI_BTSCO_DBG_SIGN, "tlkmdi_btsco_disconnEvt: {status-%d,handle-%d,scoHandle-%d,linkType-%d}", 
		pEvt->reason, pEvt->aclHandle, pEvt->scoHandle, pEvt->linkType);
	sTlkMdiBtScoCtrl.scoCnt--;
	tlkmdi_btsco_resetItemByAclHandle(pEvt->aclHandle);

	if(sTlkMdiBtScoConnCB != nullptr){
		sTlkMdiBtScoConnCB(pEvt->aclHandle, pEvt->scoHandle, false);
	}
	
	buffLen = 0;
	if(sTlkMdiBtScoCtrl.scoCnt < TLKMDI_BTSCO_MAX_NUM) {
		tlkmdi_btsco_handle_t *pItem = tlkmdi_btsco_getOtherUsedItem(pEvt->aclHandle);
		if(pItem != nullptr) {
			pBuffer[buffLen++] = (pItem->aclHandle & 0xFF);
			pBuffer[buffLen++] = (pItem->aclHandle & 0xFF00) >> 8;
			pBuffer[buffLen++] = (pItem->scoHandle & 0xFF);
			pBuffer[buffLen++] = (pItem->scoHandle & 0xFF00) >> 8;
			if(!btp_hfp_getCodec(pItem->aclHandle,&pItem->codec)) {
				pBuffer[buffLen++] = pItem->codec;
			}else {
				pBuffer[buffLen++] = TLKMDI_BTSCO_CODEC_ID_CVSD;
			}
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_ENABLE_SCO, pBuffer, buffLen);
		}else {
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_DISABLE_SCO, nullptr, 0);
		}
		
	}

	#if (TLK_MDI_MULTIPNT_ENABLE)
		buffLen = 0;
		pBuffer[buffLen++] = pEvt->aclHandle & 0xFF;
		pBuffer[buffLen++] = ((pEvt->aclHandle & 0xFF00) >> 8);
		pBuffer[buffLen++] = 0;//sco disc;
		tlksys_sendInnerMsg(TLKSYS_TASKID_PHONE, TLKPTI_PHONE_MSGID_CALL_SCO_UPDATE_EVT, pBuffer, buffLen);
	#endif
	#if (0)
	else if(sTlkMdiBtScoCtrl.scoCnt == TLKMDI_BTSCO_MAX_NUM) {
		#if (TLK_MDI_AUDDSCO_ENABLE)
		uint08 index;
		for(index = 0;index < TLKMDI_BTSCO_MAX_NUM;index++) {
			if(sTlkMdiBtScoCtrl.pItem[index].aclHandle == 0 || sTlkMdiBtScoCtrl.pItem[index].scoHandle == 0) break;
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].aclHandle & 0xFF);
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].aclHandle & 0xFF00) >> 8;
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].scoHandle & 0xFF);
			pBuffer[buffLen++] = (sTlkMdiBtScoCtrl.pItem[index].scoHandle & 0xFF00) >> 8;
			if(!btp_hfp_getCodec(pEvt->aclHandle,&sTlkMdiBtScoCtrl.pItem[index].codec)) {
				pBuffer[buffLen++] = sTlkMdiBtScoCtrl.pItem[index].codec;
			}else {
				pBuffer[buffLen++] = TLKMDI_BTSCO_CODEC_ID_CVSD;
			}
		}	
		if(index == TLKMDI_BTSCO_MAX_NUM) {
			tlksys_sendInnerMsg(TLKSYS_TASKID_BTMGR, TLKPTI_BT_MSGID_ENABLE_DSCO, pBuffer, buffLen);
		}
		#endif

	}
	#endif
	
	return TLK_ENONE;
}

tlkmdi_btsco_handle_t *tlkmdi_btsco_getIdleItem(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTSCO_MAX_NUM; index++){
		if(sTlkMdiBtScoCtrl.pItem[index].aclHandle == 0 && sTlkMdiBtScoCtrl.pItem[index].scoHandle == 0) break;
	}
	if(index == TLKMDI_BTSCO_MAX_NUM) return nullptr;
	return &sTlkMdiBtScoCtrl.pItem[index];
}

tlkmdi_btsco_handle_t *tlkmdi_btsco_getUsedItemByAclHandle(uint16 aclHandle)
{
	uint08 index;
	for(index=0;index<TLKMDI_BTSCO_MAX_NUM;index++) {
		if(sTlkMdiBtScoCtrl.pItem[index].aclHandle != 0 && sTlkMdiBtScoCtrl.pItem[index].aclHandle == aclHandle) break;
	}
	if(index == TLKMDI_BTSCO_MAX_NUM) return nullptr;
	return &sTlkMdiBtScoCtrl.pItem[index];
}

static void tlkmdi_btsco_resetItemByAclHandle(uint16 aclHandle)
{
	tlkmdi_btsco_handle_t *pItem = tlkmdi_btsco_getUsedItemByAclHandle(aclHandle);
	if(pItem != nullptr) {
		memset(pItem,0,sizeof(tlkmdi_btsco_handle_t));
	}
}
static tlkmdi_btsco_handle_t *tlkmdi_btsco_getOtherUsedItem(uint16 aclHandle)
{
	uint08 index;
	for(index=0;index<TLKMDI_BTSCO_MAX_NUM;index++) {
		if(sTlkMdiBtScoCtrl.pItem[index].aclHandle != 0 && sTlkMdiBtScoCtrl.pItem[index].aclHandle != aclHandle) break;
	}
	if(index == TLKMDI_BTSCO_MAX_NUM) return nullptr;
	return &sTlkMdiBtScoCtrl.pItem[index];
}
#endif //#if (TLK_STK_BT_ENABLE)


/********************************************************************************************************
 * @file	tlkmmi_btmgrMsgOuter.c
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
#if (TLKMMI_BTMGR_ENABLE)
#include "tlksys/prt/tlkpto_stdio.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkmdi/bt/tlkmdi_btacl.h"
#include "tlkmdi/bt/tlkmdi_btrec.h"
#include "tlkmmi_btmgr.h"
#include "tlkmmi_btmgrMsgInner.h"
#include "tlkmmi_btmgrMsgOuter.h"
#include "tlkmmi_btmgrCtrl.h"
#include "tlkmmi_btmgrAcl.h"
#include "tlkmmi_btmgrInq.h"


static void tlkmmi_btmgr_recvGetNameCmdDeal(void);
static void tlkmmi_btmgr_recvGetAddrCmdDeal(void);
static void tlkmmi_btmgr_recvSetNameCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvSetAddrCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvGetLinkKeyCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvGetPeerNameCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvVolIncCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvVolDecCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvGetPDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvDelPDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvClsPDLCmdDeal(void);
static void tlkmmi_btmgr_recvGetCDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvInquiryCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvInqCancelCmdDeal(void);
static void tlkmmi_btmgr_recvConnectCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvDisconnCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvConnProfCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvDiscProfCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvStartPairCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_btmgr_recvClosePairCmdDeal(uint08 *pData, uint08 dataLen);


extern volatile uint08 AAAA_linkKey[];
extern uint08 *bth_device_getLinkKey(uint08 *pDevAddr);

/******************************************************************************
 * Function: tlkmmi_btmgr_sendAclConnectEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendAclConnectEvt(uint16 handle, uint08 status, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendAclConnectEvt[handle-0x%x,status-%d,btaddr-0x%04x]", handle, status, *(uint32*)pBtAddr);
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_BT_CHN_ACL; //Object
	buffer[buffLen++] = status; //Status
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_btmgr_sendCommEvt(TLKPRT_COMM_EVTID_BT_CONNECT, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_sendAclDisconnEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendAclDisconnEvt(uint16 handle, uint08 reason, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendAclDisconnEvt[handle-0x%x,status-%d,btaddr-0x%04x]", handle, reason, *(uint32*)pBtAddr);
	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_BT_CHN_ACL; //Object
	buffer[buffLen++] = reason; //reason
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_btmgr_sendCommEvt(TLKPRT_COMM_EVTID_BT_DISCONN, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_sendProfConnectEvt
 * Descript: Send profile connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id(Client/Server).
 *        @pBtAddr[IN]--The Bt address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendProfConnectEvt(uint16 handle, uint08 status, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendProfConnectEvt[handle-0x%x,ptype-%d,status-%d,usrID-%d]", handle, ptype, status, usrID);
	buffLen = 0;
	buffer[buffLen++] = tlkmmi_btmgr_ptypeToCtype(ptype, usrID); //Object
	buffer[buffLen++] = status; //Status
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_btmgr_sendCommEvt(TLKPRT_COMM_EVTID_BT_PROF_CONN, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_sendProfDisconnEvt
 * Descript: Send profle disconnect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id(Client/Server).
 *        @pBtAddr[IN]--The Bt address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_btmgr_sendProfDisconnEvt(uint16 handle, uint08 reason, uint08 ptype, uint08 usrID, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "SendProfDisconnEvt[handle-0x%x,ptype-%d,reason-%d,usrID-%d]", handle, ptype, reason, usrID);
	buffLen = 0;
	buffer[buffLen++] = tlkmmi_btmgr_ptypeToCtype(ptype, usrID); //Object
	buffer[buffLen++] = reason; //reason
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6);  //MAC
	buffLen += 6;
	tlkmmi_btmgr_sendCommEvt(TLKPRT_COMM_EVTID_BT_PROF_DISC, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_ptypeToCtype
 * Descript: convert the ptype to ctype. 
 * Params:
 *        @ptype[IN]--The profile type
 *        @usrID[IN]--The user id(Client or Server).
 * Return: The communication type(AG/HFP/A2DPSNK/A2DPSRC..).
 * Others: None.
*******************************************************************************/
uint08 tlkmmi_btmgr_ptypeToCtype(uint08 ptype, uint08 usrID)
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

int tlkmmi_btmgr_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_cmdHandler: %d", msgID);
	if(msgID == TLKPRT_COMM_CMDID_BT_CONNECT){
		tlkmmi_btmgr_recvConnectCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_DISCONN){
		tlkmmi_btmgr_recvDisconnCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_CONN_PROF){
		tlkmmi_btmgr_recvConnProfCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_DISC_PROF){
		tlkmmi_btmgr_recvDiscProfCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_INQUIRY){
		tlkmmi_btmgr_recvInquiryCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_INQ_CANCEL){
		tlkmmi_btmgr_recvInqCancelCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_NAME){
		tlkmmi_btmgr_recvGetNameCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_ADDR){
		tlkmmi_btmgr_recvGetAddrCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_SET_NAME){
		tlkmmi_btmgr_recvSetNameCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_SET_ADDR){
		tlkmmi_btmgr_recvSetAddrCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_LINKEY){
		tlkmmi_btmgr_recvGetLinkKeyCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_PEER_NAME){
		tlkmmi_btmgr_recvGetPeerNameCmdDeal(pData,dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_VOL_INC){
		tlkmmi_btmgr_recvVolIncCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_VOL_DEC){
		tlkmmi_btmgr_recvVolDecCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_PDL){
		tlkmmi_btmgr_recvGetPDLCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_DEL_PDL){
		tlkmmi_btmgr_recvDelPDLCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_CLS_PDL){
		tlkmmi_btmgr_recvClsPDLCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_BT_GET_CDL){
		tlkmmi_btmgr_recvGetCDLCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_START_PAIR){
		tlkmmi_btmgr_recvStartPairCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_BT_CLOSE_PAIR){
		tlkmmi_btmgr_recvClosePairCmdDeal(pData, dataLen);
	}else{
		tlkmmi_btmgr_sendCommRsp(msgID, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
	return TLK_ENONE;
}


static void tlkmmi_btmgr_recvGetNameCmdDeal(void)
{
	uint08 buffLen;
	uint08 nameLen;
	uint08 *pBtName;
	uint08 buffer[64+4];

	pBtName = tlkmmi_btmgr_getName();
	if(pBtName == nullptr) nameLen = 0;
	else nameLen = strlen((char*)pBtName);
	if(nameLen == 0 || nameLen > 60){
		nameLen = 60;
	}
		
	buffLen = 0;
	buffer[buffLen++] = nameLen;
	tmemcpy(buffer+buffLen, pBtName, nameLen);
	buffLen += nameLen;
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_NAME, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_recvGetAddrCmdDeal(void)
{
	uint08 buffLen;
	uint08 buffer[6];
	uint08 *pBtAddr;
	
	pBtAddr = tlkmmi_btmgr_getAddr();
	
	buffLen = 0;
	tmemcpy(buffer+buffLen, pBtAddr, 6);
	buffLen += 6;
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_ADDR, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_recvSetNameCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 2 || 1+pData[0] > dataLen){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_SET_NAME, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	tlkmmi_btmgr_setName(pData+1, pData[0]);
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_SET_NAME, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_btmgr_recvSetAddrCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 6){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_SET_ADDR, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	tlkmmi_btmgr_setAddr(pData);
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_SET_ADDR, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_btmgr_recvGetLinkKeyCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 *pLinkKey;
	if(dataLen == 0){
		pLinkKey = (uint08*)AAAA_linkKey;
	}else if(dataLen < 6){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_LINKEY, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}else{
		pLinkKey = bth_device_getLinkKey(pData);
	}
	if(pLinkKey == nullptr){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_LINKEY, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFAIL, nullptr, 0);
	}else{
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_LINKEY, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, pLinkKey, 16);
	}
}
static void tlkmmi_btmgr_recvGetPeerNameCmdDeal(uint08 *pData, uint08 dataLen)
{
#if (BTH_DEVICE_NAME_ENABLE)
	uint08 buffer[BTH_DEVICE_NAME_LENGTH-1];
	uint16 buffLen;
	if(dataLen < 6){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_PEER_NAME, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	if(bth_device_getName(pData, buffer, BTH_DEVICE_NAME_LENGTH-1, &buffLen) == TLK_ENONE){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_PEER_NAME, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, buffer, buffLen);
	}else{
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_PEER_NAME, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
	}
#endif
}
static void tlkmmi_btmgr_recvVolIncCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret = tlkmmi_btmgr_ctrlVolInc();
	if(ret == TLK_ENONE){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_VOL_INC, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		if(ret < 0) ret = -ret;
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_VOL_INC, TLKPRT_COMM_RSP_STATUE_FAILURE, (uint08)ret, nullptr, 0);
	}
}
static void tlkmmi_btmgr_recvVolDecCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret = tlkmmi_btmgr_ctrlVolDec();
	if(ret == TLK_ENONE){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_VOL_DEC, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		if(ret < 0) ret = -ret;
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_VOL_DEC, TLKPRT_COMM_RSP_STATUE_FAILURE, (uint08)ret, nullptr, 0);
	}
}

static void tlkmmi_btmgr_recvGetPDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 index;
	uint08 start;
	uint08 count;
	uint08 devNum;
	uint08 buffLen;
	uint08 buffer[86];
	bth_device_item_t *pItem;

	start = pData[0];
	count = pData[1];
	if(dataLen < 2){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	devNum = bth_device_count();
	if(start >= devNum) count = 0;
	else if(start+count > devNum) count = devNum-start;
	if(count > 10) count = 10;
	
	buffLen = 0;
	buffer[buffLen++] = devNum;
	buffer[buffLen++] = start;
	buffer[buffLen++] = count;
	for(index=0; index<count; index++){
		pItem = bth_device_getInfo(start+index);
		if(pItem == nullptr) break;
		buffer[buffLen++] = bth_devClassToDevType(pItem->devClass);
		buffer[buffLen++] = 0;
		tmemcpy(buffer+buffLen, pItem->devAddr, 6);
		buffLen += 6;
	}
	buffer[2] = index;
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_recvDelPDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 index;
	uint08 count;
	uint08 devNum;
	uint08 bdaddr[6];
	uint08 buffLen;
	uint08 buffer[20];
	bth_device_item_t *pPeerItem;
	tlkmdi_btacl_item_t  *pConnItem;

	count = pData[0];
	if(dataLen < 2){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DEL_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}

	devNum = bth_device_count();
	if(count > devNum) count = devNum;
	if(count > 10) count = 10;

	pData   += 1;
	dataLen -= 1;

	buffLen = 0;
	buffer[buffLen++] = count;
	for(index=0; index<count; index++){
		if(dataLen < 6){
			buffer[buffLen++] = TLK_EPARAM;
			continue;
		}
		tmemcpy(bdaddr, pData, 6);
		pPeerItem = bth_device_getItem(bdaddr, nullptr);
		#if (TLK_MDI_BTACL_ENABLE)
		pConnItem = tlkmdi_btacl_searchUsedItem(bdaddr);
		#else
		pConnItem = nullptr;
		#endif
		if(pPeerItem == nullptr){
			buffer[buffLen++] = TLK_EPARAM;
		}else if(pConnItem != nullptr){
			buffer[buffLen++] = TLK_EBUSY;
		}else{
			buffer[buffLen++] = TLK_ENONE;
			bth_device_delItem(bdaddr);
		}
		pData   += 6;
		dataLen -= 6;
	}
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DEL_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_btmgr_recvClsPDLCmdDeal(void)
{
	#if (TLK_MDI_BTACL_ENABLE)
	if(tlkmdi_btacl_getUsedCount() != 0){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CLS_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EBUSY, nullptr, 0);
	}else
	#endif //#if (TLK_MDI_BTACL_ENABLE)
	if(bth_device_count() == 0){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CLS_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, nullptr, 0);
	}else{
		bth_device_clsItem();
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CLS_PDL, TLKPRT_COMM_RSP_STATUE_SUCCESS,
			TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_btmgr_recvGetCDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	#if !(TLK_MDI_BTACL_ENABLE)
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_CDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_ENOSUPPORT, nullptr, 0);
	#else
	uint08 index;
	uint08 start;
	uint08 count;
	uint08 devNum;
	uint08 buffLen;
	uint08 buffer[86];
	tlkmdi_btacl_item_t *pItem;

	start = pData[0];
	count = pData[1];
	if(dataLen < 2){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_CDL, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	devNum = tlkmdi_btacl_getConnCount();
	if(start >= devNum) count = 0;
	else if(start+count > devNum) count = devNum-start;
	if(count > 10) count = 10;
	
	buffLen = 0;
	buffer[buffLen++] = devNum;
	buffer[buffLen++] = start;
	buffer[buffLen++] = count;
	for(index=0; index<count; index++){
		pItem = tlkmdi_btacl_getConnItemByIndex(start+index);
		if(pItem == nullptr) break;
		buffer[buffLen++] = pItem->handle & 0xFF;
		buffer[buffLen++] = (pItem->handle & 0xFF00) >> 8;
		tmemcpy(buffer+buffLen, pItem->btaddr, 6);
		buffLen += 6;
	}
	buffer[2] = index;
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_GET_CDL, TLKPRT_COMM_RSP_STATUE_SUCCESS, 
		TLK_ENONE, buffer, buffLen);
	#endif
}
static void tlkmmi_btmgr_recvInquiryCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 type;
	uint08 numb;
	uint08 rssi;
	uint08 reason;
	uint08 inqWind;
	uint08 inqNumb;
	
	type = pData[0];
	numb = pData[1];
	rssi = pData[2];
	inqWind = pData[3];
	inqNumb = pData[4];
	if(dataLen < 5 || (type != 1 && type != 2 && type != 4) || inqWind > 100 || inqWind < 3){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_INQUIRY, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	ret = tlkmmi_btmgr_startInquiry(type, rssi, numb, inqWind, inqNumb, true);
	if(ret == TLK_ENONE) reason = TLK_ENONE;
	else if(ret == -TLK_EBUSY) reason = TLK_EBUSY;
	else reason = TLK_EPARAM;

	if(reason == TLK_ENONE){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_INQUIRY, TLKPRT_COMM_RSP_STATUE_SUCCESS, reason, nullptr, 0);
	}else{
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_INQUIRY, TLKPRT_COMM_RSP_STATUE_FAILURE, reason, nullptr, 0);
	}
}
static void tlkmmi_btmgr_recvInqCancelCmdDeal(void)
{
	int ret;
	uint08 reason;
	
	ret = tlkmmi_btmgr_closeInquiry();
	if(ret == TLK_ENONE) reason = TLK_ENONE;
	else if(ret == -TLK_EBUSY) reason = TLK_EBUSY;
	else reason = TLK_EPARAM;
	if(reason == TLK_ENONE){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_INQ_CANCEL, TLKPRT_COMM_RSP_STATUE_SUCCESS, reason, nullptr, 0);
	}else{
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_INQ_CANCEL, TLKPRT_COMM_RSP_STATUE_FAILURE, reason, nullptr, 0);
	}
}
static void tlkmmi_btmgr_recvConnectCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 object;
	uint16 handle;
	uint08 btAddr[6];
	uint32 timeout;
	
	if(dataLen < 6 || (pData[0] != 0 && pData[0] != 1)){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnectCmdDeal: Error Param");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	if(pData[0] == 0 && dataLen < 10){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnectCmdDeal: Error Length");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	object = pData[1];
	timeout = (((uint16)pData[3] << 8) | pData[2])*100;
	if(pData[0] == 0) tmemcpy(btAddr, pData+4, 6);
	else handle = ((uint16)pData[5] << 8) | pData[4];
	
	(void)handle;
	if(object == TLKPRT_COMM_BT_CHN_ACL){
		if(pData[0] != 0){
			tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnectCmdDeal: Only Support Method - MAC");
			tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
			return;
		}
		ret = tlkmmi_btmgr_connect(btAddr, timeout);
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnectCmdDeal: Start Connect - none");
			tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else if(ret == -TLK_EBUSY){
			tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnectCmdDeal: Start Connect - busy");
			tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_COMPLETE, TLK_ENONE, nullptr, 0);
		}else{
			tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnectCmdDeal: Start Connect - fail");
			tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnectCmdDeal: Only Support Object-ACL");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_btmgr_recvDisconnCmdDeal(uint08 *pData, uint08 dataLen)
{
	int ret;
	uint08 object;
	uint16 handle = 0;
	uint08 btAddr[6] = {0};

	if(dataLen < 4 || (pData[0] != 0 && pData[0] != 1)){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	if(pData[0] == 0 && dataLen < 8){
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	object = pData[1];
	if(pData[0] == 0) tmemcpy(btAddr, pData+2, 6);
	else handle = ((uint16)pData[3] << 8) | pData[2];
	if(object == TLKPRT_COMM_BT_CHN_ACL){
		tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvDisconnCmdDeal:object-%d,handle-0x%x", object, handle);
		if(pData[0] == 0) ret = tlkmmi_btmgr_disconnByAddr(btAddr);
		else ret = tlkmmi_btmgr_disconn(handle);
		if(ret == TLK_ENONE){
			tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else if(ret == -TLK_EBUSY){
			tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_COMPLETE, TLK_ENONE, nullptr, 0);
		}else{
			tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvDisconnCmdDeal: Only Support Object-ACL");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_btmgr_recvConnProfCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvConnProfCmdDeal: Not Support");
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CONN_PROF, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
}
static void tlkmmi_btmgr_recvDiscProfCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvDisconnCmdDeal: Not Support");
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_DISC_PROF, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
}
static void tlkmmi_btmgr_recvStartPairCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 mode;
	uint08 isForce;
	uint08 enInqScan;
	uint08 enPageScan;

	if(dataLen < 3){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvStartPairCmdDeal: error length");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_START_PAIR, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	isForce = pData[0];
	enInqScan = pData[1];
	enPageScan = pData[2];
	if(!enInqScan && !enPageScan){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvStartPairCmdDeal: error param");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_START_PAIR, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	
	#if (TLKMMI_BTMGR_BTREC_ENABLE)
	if(!isForce && tlkmmi_btmgr_recIsBusy()){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvStartPairCmdDeal: busy");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_START_PAIR, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EBUSY, nullptr, 0);
		return;
	}
	#endif
	
	if(tlkmdi_btacl_getIdleCount() == 0){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recvStartPairCmdDeal: no quota");
		tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_START_PAIR, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EQUOTA, nullptr, 0);
		return;
	}

	if(enInqScan && !enPageScan) mode = TLKMDI_BTREC_KEEP_MODE_INQUIRY_SCAN;
	else if(!enInqScan && enPageScan) mode = TLKMDI_BTREC_KEEP_MODE_PAGE_SCAN;
	else mode = TLKMDI_BTREC_KEEP_MODE_BOTH_SCAN;
	tlkmdi_btrec_setKeepMode(mode, 60);
	
	tlkmmi_btmgr_recStart(nullptr, 0, false, false);
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_START_PAIR, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_btmgr_recvClosePairCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkmmi_btmgr_recClose();
	tlkmmi_btmgr_sendCommRsp(TLKPRT_COMM_CMDID_BT_CLOSE_PAIR, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}



#endif //#if (TLKMMI_BTMGR_ENABLE)


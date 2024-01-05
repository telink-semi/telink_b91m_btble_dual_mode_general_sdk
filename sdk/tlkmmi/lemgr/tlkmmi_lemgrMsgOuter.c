/********************************************************************************************************
 * @file	tlkmmi_lemgrMsgOuter.c
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
#if (TLKMMI_LEMGR_ENABLE)
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmmi_lemgr.h"
#include "tlkmmi_lemgrMsgOuter.h"
#include "tlkmmi_lemgrMsgInner.h"
#include "tlkmmi_lemgrCtrl.h"
#include "tlkmmi_lemgrAcl.h"


static void tlkmmi_lemgr_recvConnectCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvDisconnCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvStartAdvCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvCloseAdvCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvStartScanCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvCloseScanCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvGetNameCmdDeal(void);
static void tlkmmi_lemgr_recvGetAddrCmdDeal(void);
static void tlkmmi_lemgr_recvSetNameCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvSetAddrCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvVolIncCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvVolDecCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvGetPDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvDelPDLCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_lemgr_recvClsPDLCmdDeal(void);
static void tlkmmi_lemgr_recvGetCDLCmdDeal(uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: tlkmmi_lemgr_sendAclConnectEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @status[IN]--The current status.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_lemgr_sendAclConnectEvt(uint16 handle, uint08 status,
	uint08 peerRole, uint08 peerType, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "SendAclConnectEvt[handle-0x%x,status-%d,btaddr-0x%x]", handle, status, *(uint32*)pBtAddr);
	buffLen = 0;
	buffer[buffLen++] = 0x00; //Object: ACL
	buffer[buffLen++] = status; //Status
	buffer[buffLen++] = peerRole; //
	buffer[buffLen++] = peerType; //
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_lemgr_sendCommEvt(TLKPRT_COMM_EVTID_LE_CONNECT, buffer, buffLen);
}

/******************************************************************************
 * Function: tlkmmi_lemgr_sendAclDisconnEvt
 * Descript: Send ACL connect Evt to app. 
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 *        @pBtAddr[IN]--The BT Address.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_lemgr_sendAclDisconnEvt(uint16 handle, uint08 reason, uint08 *pBtAddr)
{
	uint08 buffLen;
	uint08 buffer[32];
	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "SendAclDisconnEvt[handle-0x%x,status-%d,btaddr-0x%x]", handle, reason, *(uint32*)pBtAddr);
	buffLen = 0;
	buffer[buffLen++] = 0x00; //Object: ACL
	buffer[buffLen++] = reason; //reason
	buffer[buffLen++] = (handle & 0x00FF); //Handle
	buffer[buffLen++] = (handle & 0xFF00) >> 8;
	tmemcpy(buffer+buffLen, pBtAddr, 6); //MAC
	buffLen += 6;
	tlkmmi_lemgr_sendCommEvt(TLKPRT_COMM_EVTID_LE_DISCONN, buffer, buffLen);
}
void tlkmmi_lemgr_sendAclAdvReportEvt(uint08 pData_byte,uint08 *pData,s8 rssi)
{
	uint08 buffLen;
	uint08 buffer[32];
	buffLen = 0;
	buffer[0] = 0x00; //Object: ACL
	buffer[1] = pData_byte; //reason
	tmemcpy(buffer+1, pData, 6); //MAC
	buffLen += 8;
	buffer[buffLen++] = rssi; //reason
	tlkmmi_lemgr_sendCommEvt(TLKPRT_COMM_EVTID_LE_ADV_REPORT, buffer, buffLen);
}

void tlkmmi_lemgr_sendAdvStartEvt(void)
{
	tlkmmi_lemgr_sendCommEvt(TLKPRT_COMM_EVTID_LE_ADV_START, nullptr, 0);
}
void tlkmmi_lemgr_sendAdvCompleteEvt(void)
{
	tlkmmi_lemgr_sendCommEvt(TLKPRT_COMM_EVTID_LE_ADV_COMPLETE, nullptr, 0);
}

void tlkmmi_lemgr_sendScanStartEvt(void)
{
	tlkmmi_lemgr_sendCommEvt(TLKPRT_COMM_EVTID_LE_SCAN_START, nullptr, 0);
}
void tlkmmi_lemgr_sendScanCompleteEvt(void)
{
	tlkmmi_lemgr_sendCommEvt(TLKPRT_COMM_EVTID_LE_SCAN_COMPLETE, nullptr, 0);
}



int tlkmmi_lemgr_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "tlkmmi_lemgr_cmdHandler: msgID-%d", msgID);
	if(msgID == TLKPRT_COMM_CMDID_LE_CONNECT){
		tlkmmi_lemgr_recvConnectCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_DISCONN){
		tlkmmi_lemgr_recvDisconnCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_START_ADV){
		tlkmmi_lemgr_recvStartAdvCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_CLOSE_ADV){
		tlkmmi_lemgr_recvCloseAdvCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_START_SCAN){
		tlkmmi_lemgr_recvStartScanCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_CLOSE_SCAN){
		tlkmmi_lemgr_recvCloseScanCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_GET_NAME){
		tlkmmi_lemgr_recvGetNameCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_LE_GET_ADDR){
		tlkmmi_lemgr_recvGetAddrCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_LE_SET_NAME){
		tlkmmi_lemgr_recvSetNameCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_SET_ADDR){
		tlkmmi_lemgr_recvSetAddrCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_VOL_INC){
		tlkmmi_lemgr_recvVolIncCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_VOL_DEC){
		tlkmmi_lemgr_recvVolDecCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_GET_PDL){
		tlkmmi_lemgr_recvGetPDLCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_DEL_PDL){
		tlkmmi_lemgr_recvDelPDLCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_LE_CLS_PDL){
		tlkmmi_lemgr_recvClsPDLCmdDeal();
	}else if(msgID == TLKPRT_COMM_CMDID_LE_GET_CDL){
		tlkmmi_lemgr_recvGetCDLCmdDeal(pData, dataLen);
	}else{
		tlkmmi_lemgr_sendCommRsp(msgID, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
	return TLK_ENONE;
}

static void tlkmmi_lemgr_recvConnectCmdDeal(uint08 *pData, uint08 dataLen)
{

	if(tlkmmi_lemgr_createconnect(pData,dataLen)== TLK_ENONE){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_CONNECT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_CONNECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
}
static void tlkmmi_lemgr_recvDisconnCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(tlkmmi_lemgr_disconn(0, nullptr) == TLK_ENONE){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_DISCONN, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_DISCONN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
}
static void tlkmmi_lemgr_recvStartAdvCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint32 timeout;

	if(dataLen < 3){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_START_ADV, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	timeout = (((uint16)pData[1] << 8) | pData[0])*100;
	if(tlkmmi_lemgr_startAdv(timeout, pData[2]) != TLK_ENONE){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_START_ADV, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}else{
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_START_ADV, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_lemgr_recvCloseAdvCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkmmi_lemgr_closeAdv();
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_CLOSE_ADV, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_lemgr_recvStartScanCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(tlkmmi_lemgr_startScan() != TLK_ENONE){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_START_SCAN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}else{
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_START_SCAN, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}
}
static void tlkmmi_lemgr_recvCloseScanCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkmmi_lemgr_closeScan();
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_CLOSE_SCAN, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_lemgr_recvGetNameCmdDeal(void)
{
	uint08 buffLen;
	uint08 nameLen;
	uint08 *pBtName;
	uint08 buffer[64+4];

	pBtName = tlkmmi_lemgr_getName();
	if(pBtName == nullptr) nameLen = 0;
	else nameLen = strlen((char*)pBtName);
	if(nameLen == 0 || nameLen > 60){
		nameLen = 60;
	}
	
	buffLen = 0;
	buffer[buffLen++] = nameLen;
	tmemcpy(buffer+buffLen, pBtName, nameLen);
	buffLen += nameLen;
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_GET_NAME, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_lemgr_recvGetAddrCmdDeal(void)
{
	uint08 buffLen;
	uint08 buffer[6];
	uint08 *pBtAddr;
	
	pBtAddr = tlkmmi_lemgr_getAddr();
	
	buffLen = 0;
	tmemcpy(buffer+buffLen, pBtAddr, 6);
	buffLen += 6;
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_GET_ADDR, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
}
static void tlkmmi_lemgr_recvSetNameCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 2 || 1+pData[0] > dataLen){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_SET_NAME, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	tlkapi_trace(TLKMMI_LEMGR_DBG_FLAG, TLKMMI_LEMGR_DBG_SIGN, "tlkmmi_lemgr_recvSetNameCmdDeal: nameLen-%d", pData[0]);
	tlkmmi_lemgr_setName(pData+1, pData[0]);
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_SET_NAME, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_lemgr_recvSetAddrCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(dataLen < 6){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_SET_ADDR, TLKPRT_COMM_RSP_STATUE_FAILURE,
			TLK_EFORMAT, nullptr, 0);
		return;
	}
	tlkmmi_lemgr_setAddr(pData);
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_SET_ADDR, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}
static void tlkmmi_lemgr_recvVolIncCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(tlkmmi_lemgr_volumeInc()){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_VOL_INC, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_VOL_INC, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOREADY, nullptr, 0);
	}
}
static void tlkmmi_lemgr_recvVolDecCmdDeal(uint08 *pData, uint08 dataLen)
{
	if(tlkmmi_lemgr_volumeDec()){
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_VOL_DEC, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_VOL_DEC, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOREADY, nullptr, 0);
	}
}
static void tlkmmi_lemgr_recvGetPDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_GET_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
}
static void tlkmmi_lemgr_recvDelPDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_DEL_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
}
static void tlkmmi_lemgr_recvClsPDLCmdDeal(void)
{
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_CLS_PDL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
}
static void tlkmmi_lemgr_recvGetCDLCmdDeal(uint08 *pData, uint08 dataLen)
{
	tlkmmi_lemgr_sendCommRsp(TLKPRT_COMM_CMDID_LE_GET_CDL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
}


#endif //TLKMMI_LEMGR_ENABLE


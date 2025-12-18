/********************************************************************************************************
 * @file	tlkmmi_rdtMsg.c
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
#if (TLK_TEST_RDT_ENABLE)
#include "tlkmmi_rdt.h"
#include "tlkmmi_rdtStd.h"
#include "tlkmmi_rdtMsg.h"


#if (TLK_STK_BTH_ENABLE)
extern int bth_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
#endif
#if (TLK_STK_BTP_ENABLE)
extern int btp_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
#endif
#if (TLK_STK_LE_ENABLE)
extern int leh_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
#endif
#if (TLK_STK_LE_ENABLE)
extern int lep_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
#endif

static int tlkmmi_rdt_recvRebootMsgDeal(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_recvStartMsgDeal(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_recvPauseMsgDeal(uint08 *pData, uint16 dataLen);
static int tlkmmi_rdt_recvCloseMsgDeal(uint08 *pData, uint16 dataLen);

static void tlkmmi_rdt_recvCallFuncMsgDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_recvCallBthFuncMsgDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_recvCallBtpFuncMsgDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_recvCallLehFuncMsgDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_rdt_recvCallLepFuncMsgDeal(uint08 *pData, uint16 dataLen);


int tlkmmi_rdt_recvMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvMsgHandler->%d", msgID);
	if(msgID == TLKMMI_RDT_MSGID_REBOOT){
		tlkmmi_rdt_recvRebootMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_START){
		tlkmmi_rdt_recvStartMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_PAUSE){
		tlkmmi_rdt_recvPauseMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_CLOSE){
		tlkmmi_rdt_recvCloseMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_CALL_FUNC){
		tlkmmi_rdt_recvCallFuncMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_CALL_BTH_FUNC){
		tlkmmi_rdt_recvCallBthFuncMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_CALL_BTP_FUNC){
		tlkmmi_rdt_recvCallBtpFuncMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_CALL_LEH_FUNC){
		tlkmmi_rdt_recvCallLehFuncMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_RDT_MSGID_CALL_LEP_FUNC){
		tlkmmi_rdt_recvCallLepFuncMsgDeal(pData, dataLen);
	}
	
	if(msgID >= TLKMMI_RDT_MSGID_USER_START && msgID <= TLKMMI_RDT_MSGID_USER_END){
		tlkmmi_rdt_testInput(msgID, pData, dataLen);
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}


static int tlkmmi_rdt_recvRebootMsgDeal(uint08 *pData, uint16 dataLen)
{
	tlkmmi_rdt_reboot();
	return TLK_ENONE;
}
static int tlkmmi_rdt_recvStartMsgDeal(uint08 *pData, uint16 dataLen)
{
	uint08 role;
	uint16 caseID;
	
	if(dataLen < 3){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvStartMsgDeal: error length(%d)", dataLen);
		return -TLK_EFORMAT;
	}

	caseID = ((uint16)pData[1]<<8) | pData[0];
	role   = pData[2];
	if(caseID == 0x0000 || caseID == 0xFFFF){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvStartMsgDeal: error caseID(%d)", caseID);
		return -TLK_EFORMAT;
	}
		
	return tlkmmi_rdt_startTest(caseID, role);
}
static int tlkmmi_rdt_recvPauseMsgDeal(uint08 *pData, uint16 dataLen)
{
	return tlkmmi_rdt_pauseTest();
}
static int tlkmmi_rdt_recvCloseMsgDeal(uint08 *pData, uint16 dataLen)
{
	return tlkmmi_rdt_closeTest();
}


static void tlkmmi_rdt_recvCallFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	uint08 type;

	if(pData == nullptr || dataLen < 3){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallFuncMsgDeal: error param");
		return;
	}

	type = ((uint16)pData[1]<<8) | pData[0];
	if(type == TLKMMI_RDT_FUNC_TYPE_BTH){
		tlkmmi_rdt_recvCallBthFuncMsgDeal(pData+2, dataLen-2);
	}else if(type == TLKMMI_RDT_FUNC_TYPE_BTP){
		tlkmmi_rdt_recvCallBthFuncMsgDeal(pData+2, dataLen-2);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallFuncMsgDeal: unknown type [%d]", type);
	}
}
static void tlkmmi_rdt_recvCallBthFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	int ret;
	uint16 funcID;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallBthFuncMsgDeal: error param");
		return;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallBthFuncMsgDeal:");
	funcID = ((uint16)pData[1] << 8) | pData[0];
#if (TLK_STK_LE_ENABLE)
	ret = bth_func_call(funcID, pData+2, dataLen-2);
#else
	ret = -TLK_ENOSUPPORT;
#endif
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallBthFuncMsgDeal: failure !");
	}
}
static void tlkmmi_rdt_recvCallBtpFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	int ret;
	uint16 funcID;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallBtpFuncMsgDeal: error param");
		return;
	}
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallBtpFuncMsgDeal:");
	funcID = ((uint16)pData[1] << 8) | pData[0];
#if (TLK_STK_LE_ENABLE)
	ret = btp_func_call(funcID, pData+2, dataLen-2);
#else
	ret = -TLK_ENOSUPPORT;
#endif
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallBtpFuncMsgDeal: failure !");
	}
}
static void tlkmmi_rdt_recvCallLehFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	int ret;
	uint16 funcID;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallLehFuncMsgDeal: error param");
		return;
	}

	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallLehFuncMsgDeal:");
	funcID = ((uint16)pData[1] << 8) | pData[0];
#if (TLK_STK_LE_ENABLE)
	ret = leh_func_call(funcID, pData+2, dataLen-2);
#else
	ret = -TLK_ENOSUPPORT;
#endif
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallLehFuncMsgDeal: failure !");
	}
}
static void tlkmmi_rdt_recvCallLepFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	int ret;
	uint16 funcID;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallLepFuncMsgDeal: error param");
		return;
	}
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallLepFuncMsgDeal:");
	funcID = ((uint16)pData[1] << 8) | pData[0];
#if (TLK_STK_LE_ENABLE)
	ret = lep_func_call(funcID, pData+2, dataLen-2);
#else
	ret = -TLK_ENOSUPPORT;
#endif
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_recvCallLepFuncMsgDeal: failure !");
	}
}



#endif //#if (TLK_TEST_RDT_ENABLE)


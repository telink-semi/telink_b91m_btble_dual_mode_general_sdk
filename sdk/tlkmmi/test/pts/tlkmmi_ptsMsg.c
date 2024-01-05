/********************************************************************************************************
 * @file	tlkmmi_ptsMsg.c
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
#if (TLK_TEST_PTS_ENABLE)
#include "tlkmmi_pts.h"
#include "tlkmmi_ptsMsg.h"
#include "tlksys/prt/tlkpto_comm.h"

#define TLKMMI_DongleAddr_INFO_SIZE		6 


extern void tlkmmi_test_reboot(uint16 timeout);
extern int bth_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
extern int btp_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);

static void tlkmmi_pts_recvRebootMsgDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_pts_recvCallFuncMsgDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_pts_recvCallBthFuncMsgDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_pts_recvCallBtpFuncMsgDeal(uint08 *pData, uint16 dataLen);


int tlkmmi_pts_recvMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvMsgHandler->%d", msgID);
	if(msgID == TLKMMI_PTS_MSGID_REBOOT){
		tlkmmi_pts_recvRebootMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_PTS_MSGID_CALL_FUNC){
		tlkmmi_pts_recvCallFuncMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_PTS_MSGID_CALL_BTH_FUNC){
		tlkmmi_pts_recvCallBthFuncMsgDeal(pData, dataLen);
	}else if(msgID == TLKMMI_PTS_MSGID_CALL_BTP_FUNC){
		tlkmmi_pts_recvCallBtpFuncMsgDeal(pData, dataLen);
	}

	return TLK_ENONE;
}


static void tlkmmi_pts_recvRebootMsgDeal(uint08 *pData, uint16 dataLen)
{
	tlkmmi_test_reboot(500);
}

static void tlkmmi_pts_recvCallFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	uint08 type;

	if(pData == nullptr || dataLen < 3){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallFuncMsgDeal: error param");
		return;
	}

	type = ((uint16)pData[1]<<8) | pData[0];
	if(type == TLKMMI_PTS_FUNC_TYPE_BTH){
		tlkmmi_pts_recvCallBthFuncMsgDeal(pData+2, dataLen-2);
	}else if(type == TLKMMI_PTS_FUNC_TYPE_BTP){
		tlkmmi_pts_recvCallBthFuncMsgDeal(pData+2, dataLen-2);
	}else{
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallFuncMsgDeal: unknown type [%d]", type);
	}
}
static void tlkmmi_pts_recvCallBthFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	uint16 funcID;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallBthFuncMsgDeal: error param");
		return;
	}

	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallBthFuncMsgDeal:");
	funcID = ((uint16)pData[1] << 8) | pData[0];
	if(bth_func_call(funcID, pData+2, dataLen-2) != TLK_ENONE){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallBthFuncMsgDeal: failure !");
	}
}
static void tlkmmi_pts_recvCallBtpFuncMsgDeal(uint08 *pData, uint16 dataLen)
{
	uint16 funcID;

	if(pData == nullptr || dataLen < 2){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallBtpFuncMsgDeal: error param");
		return;
	}
	
	tlkapi_trace(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallBtpFuncMsgDeal:");
	funcID = ((uint16)pData[1] << 8) | pData[0];
	if(btp_func_call(funcID, pData+2, dataLen-2) != TLK_ENONE){
		tlkapi_error(TLKMMI_PTS_DBG_FLAG, TLKMMI_PTS_DBG_SIGN, "tlkmmi_pts_recvCallBtpFuncMsgDeal: failure !");
	}
}




#endif //#if (TLK_TEST_PTS_ENABLE)


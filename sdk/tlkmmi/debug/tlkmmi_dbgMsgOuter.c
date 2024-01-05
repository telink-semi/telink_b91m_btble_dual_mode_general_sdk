/********************************************************************************************************
 * @file	tlkmmi_dbgMsgOuter.c
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
#if (TLKMMI_DEBUG_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_dbg.h"
#include "tlkmmi_dbgSync.h"
#include "tlkmmi_dbgMsgInner.h"
#include "tlkmmi_dbgMsgOuter.h"

#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"
#include "tlklib/dbg/tlkdbg_config.h"
#include "tlklib/dbg/tlkdbg_hpudwn.h"


#if (TLKDBG_CFG_HPU_VCD_ENABLE)
extern void tlkdbg_hpuvcd_enable(bool enable);
#endif

static void tlkmmi_debug_recvStartToneDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_debug_recvSimulateKeyDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_debug_recvFirmwareUpdateDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_debug_recvStartVCDDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_debug_recvSyncSwitchCmdDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_debug_recvUpdateSwitchCmdDeal(uint08 *pData, uint16 dataLen);


int tlkmmi_debug_outerMsgHandler(uint16 msgID, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_outerMsgHandler: msgID-%d", msgID);
	
	if(msgID == TLKPRT_COMM_CMDID_DBG_START_TONE){
		tlkmmi_debug_recvStartToneDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_SIMULATE_KEY){
		tlkmmi_debug_recvSimulateKeyDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE){
		tlkmmi_debug_recvFirmwareUpdateDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_START_VCD){
		tlkmmi_debug_recvStartVCDDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_SYNC_SWITCH){
		tlkmmi_debug_recvSyncSwitchCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DBG_UPDATE_ITEM){
		tlkmmi_debug_recvUpdateSwitchCmdDeal(pData, dataLen);
	}
	return TLK_ENONE;
}



static void tlkmmi_debug_recvStartToneDeal(uint08 *pData, uint16 dataLen)
{
	if(dataLen < 3){
		tlkapi_error(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_recvStartToneDeal: length error - %d", dataLen);
		return;
	}
	
	tlkapi_array(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_recvStartToneDeal: start", pData, dataLen);
	tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_START_TONE_CMD, pData, 3);
}
static void tlkmmi_debug_recvSimulateKeyDeal(uint08 *pData, uint16 dataLen)
{
	
}
static void tlkmmi_debug_recvFirmwareUpdateDeal(uint08 *pData, uint16 dataLen)
{
	if(dataLen == 0) return;
	if(pData[0] == 0x01){ //Start Transfer
		core_reboot();
	}
}
static void tlkmmi_debug_recvStartVCDDeal(uint08 *pData, uint16 dataLen)
{
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	if(dataLen < 1){
		tlkapi_error(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_recvStartVCDDeal: length error - %d", dataLen);
		return;
	}
	tlkdbg_hpuvcd_enable(pData[0]);
#else
	tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_START_VCD, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#endif
}
static void tlkmmi_debug_recvSyncSwitchCmdDeal(uint08 *pData, uint16 dataLen)
{
#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)
	bool isVCD;
	bool syncItem;
	
	if(dataLen < 2){
		tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_SYNC_SWITCH, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}

	if(pData[0] != 0x00) isVCD = true;
	else isVCD = false;
	if(pData[1] != 0x00) syncItem = true;
	else syncItem = false;
	if(tlkmmi_debug_syncList(isVCD, syncItem) == TLK_ENONE){
		tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_SYNC_SWITCH, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_SYNC_SWITCH, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
	}
#else
	tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_SYNC_SWITCH, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#endif
}
static void tlkmmi_debug_recvUpdateSwitchCmdDeal(uint08 *pData, uint16 dataLen)
{
#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)
	bool isVCD;
	uint16 offset;
	uint08 number;
	uint16 listID;
	uint32 listMask;
	
	if(dataLen < 2){
		tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_UPDATE_ITEM, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}

	if(pData[0] != 0x00) isVCD = true;
	else isVCD = false;
	number = pData[1];
	if(pData[0] != 0x00 && pData[0] != 0x01){
		tlkapi_error(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_recvUpdateSwitchCmdDeal: length type - %d", pData[0]);
		return;
	}

	tlkapi_trace(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_debug_recvUpdateSwitchCmdDeal: isVCD[%d], number[%d]", isVCD, number);

	offset = 2;
	while(offset+6 < dataLen && number != 0){
		number --;
		ARRAY_TO_UINT16L(pData, offset, listID);
		ARRAY_TO_UINT32L(pData, offset+2, listMask);
		offset += 6;
		#if (TLK_DBG_VCD_ENABLE)
		if(isVCD){
			tlk_debug_setVcdMask(listID, listMask);
		}
		#endif
		#if (TLK_DBG_LOG_ENABLE)
		if(!isVCD){
			tlk_debug_setLogMask(listID, listMask);
		}
		#endif
	}
	tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_UPDATE_ITEM, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
#else
	tlkmmi_debug_sendCommRsp(TLKPRT_COMM_CMDID_DBG_UPDATE_ITEM, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
#endif
}



#endif //#if (TLKMMI_DEBUG_ENABLE)



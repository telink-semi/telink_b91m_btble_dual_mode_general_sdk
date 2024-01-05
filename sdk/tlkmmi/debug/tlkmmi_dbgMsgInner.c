/********************************************************************************************************
 * @file	tlkmmi_dbgMsgInner.c
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
#include "tlkmmi_dbgMsgInner.h"
#if ((TLK_DBG_LOG_ENABLE) || (TLK_DBG_VCD_ENABLE))
#include <stdio.h>
#include <stdarg.h>
#include "tlklib/dbg/tlkdbg_config.h"
#if (TLKDBG_CFG_HPU_LOG_ENABLE)
#include "tlklib/dbg/tlkdbg_hpulog.h"
#endif
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
#include "tlklib/dbg/tlkdbg_hpuvcd.h"
#endif
#endif

static int tlkmmi_debug_recvRegHpuLogCbDeal(uint08 *pData, uint16 dataLen);
static int tlkmmi_debug_recvRegHpuVcdCbDeal(uint08 *pData, uint16 dataLen);


int tlkmmi_debug_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_DBG;
	head[headLen++] = cmdID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_debug_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_DBG;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_debug_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_DBG;
	head[headLen++] = evtID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}



int tlkmmi_debug_innerMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKPTI_DBG_MSGID_REG_HPULOG_CB){
		return tlkmmi_debug_recvRegHpuLogCbDeal(pData, dataLen);
	}else if(msgID == TLKPTI_DBG_MSGID_REG_HPUVCD_CB){
		return tlkmmi_debug_recvRegHpuVcdCbDeal(pData, dataLen);
	}
	
	return -TLK_ENOSUPPORT;
}


static int tlkmmi_debug_recvRegHpuLogCbDeal(uint08 *pData, uint16 dataLen)
{
#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	uint32 tempVar0;
	uint32 tempVar1;
	
	if(dataLen < 8) return -TLK_EPARAM;
	ARRAY_TO_UINT32L(pData, 0, tempVar0);
	ARRAY_TO_UINT32L(pData, 4, tempVar1);
	tlkdbg_hpulog_regSendCB((TlkDbgHpuLogSendCB)tempVar1, tempVar0);
#endif
	return TLK_ENONE;
}
static int tlkmmi_debug_recvRegHpuVcdCbDeal(uint08 *pData, uint16 dataLen)
{
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	uint32 tempVar0;
	uint32 tempVar1;
	
	if(dataLen < 8) return -TLK_EPARAM;
	ARRAY_TO_UINT32L(pData, 0, tempVar0);
	ARRAY_TO_UINT32L(pData, 4, tempVar1);
	tlkdbg_hpuvcd_regSendCB((TlkDbgHpuLogSendCB)tempVar1, tempVar0);
#endif
	return TLK_ENONE;
}



#endif // #if (TLKMMI_DEBUG_ENABLE)

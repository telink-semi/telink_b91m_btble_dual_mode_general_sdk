/********************************************************************************************************
 * @file	tlkmmi_debug.c
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
#include "tlkmdi/misc/tlkmdi_debug.h"
#include "tlkmmi_dbg.h"
#include "tlkmmi_dbgAdapt.h"
#include "tlkmmi_dbgShell.h"
#include "tlkmmi_dbgMsgInner.h"
#include "tlkmmi_dbgMsgOuter.h"
#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)
#include "tlkmmi_dbgSync.h"
#endif


TLKSYS_MMI_TASK_DEFINE(debug, Debug);


static int tlkmmi_debug_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];

	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_DBG;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);
	
	tlkmmi_debug_adaptInit(procID);

	#if (TLK_MDI_DEBUG_ENABLE)
	tlkmdi_debug_init();
	#endif

	tlkmmi_debug_shellInit();
	#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)
	tlkmmi_debug_syncInit();
	#endif

	return TLK_ENONE;
}
static int tlkmmi_debug_start(void)
{

	return TLK_ENONE;
}
static int tlkmmi_debug_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_debug_close(void)
{

	return TLK_ENONE;
}
static int tlkmmi_debug_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_debug_innerMsgHandler(msgID, pData, dataLen);
	}else if(mtype == TLKPRT_COMM_MTYPE_DBG){
		return tlkmmi_debug_outerMsgHandler(msgID, pData, dataLen);
	}
	return -TLK_ENOSUPPORT;
}
static void tlkmmi_debug_handler(void)
{
	#if (TLK_MDI_DEBUG_ENABLE)
	tlkmdi_debug_handler();
	#endif
}




#endif //#if (TLKMMI_DEBUG_ENABLE)


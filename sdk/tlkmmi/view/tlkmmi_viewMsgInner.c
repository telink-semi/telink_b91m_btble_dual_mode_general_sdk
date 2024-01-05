/********************************************************************************************************
 * @file	tlkmmi_viewMsg.c
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
#if (TLKMMI_VIEW_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi/view/tlkmmi_view.h"
#include "tlkmmi/view/tlkmmi_viewMsg.h"


/******************************************************************************
 * Function: tlkmmi_view_sendCommXxx.
 * Descript: send the view command or Response or Event.
 * Params:
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_view_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_VIEW;
	head[headLen++] = cmdID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_view_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_VIEW;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_view_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_VIEW;
	head[headLen++] = evtID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}





#endif //#if (TLKMMI_VIEW_ENABLE)


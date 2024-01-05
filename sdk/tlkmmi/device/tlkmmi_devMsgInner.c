/********************************************************************************************************
 * @file	tlkmmi_devMsgInner.c
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
#if (TLKMMI_DEVICE_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_dev.h"
#include "tlkmmi_devMsgInner.h"


int tlkmmi_device_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_DEVICE;
	head[headLen++] = cmdID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_DEVICE, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_device_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_DEVICE;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_DEVICE, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_device_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_DEVICE;
	head[headLen++] = evtID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_DEVICE, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}



int tlkmmi_device_innerMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKPTI_DEV_MSGID_KEY_EVENT_REPORT){
		tlkapi_trace(TLKMMI_DEV_DBG_FLAG, TLKMMI_DEV_DBG_SIGN, 
			"tlkmmi_device_keyEventCB: keyID[%d], evtID[%d], isPress[%d]",
			pData[0], pData[1], pData[2]);
		return TLK_ENONE;
	}else if(msgID == TLKPTI_DEV_MSGID_KEYBOARD_EVENT_REPORT){
		tlkapi_trace(TLKMMI_DEV_DBG_FLAG, TLKMMI_DEV_DBG_SIGN, 
			"tlkmmi_device_keyEventCB: evtID[%d], isPress[%d], keyNumb[%d], keyID[%d]",
			pData[0], pData[1], pData[2], pData[3]);
		return TLK_ENONE;
	}
	return -TLK_ENOSUPPORT;
}



#endif //(TLKMMI_DEVICE_ENABLE)

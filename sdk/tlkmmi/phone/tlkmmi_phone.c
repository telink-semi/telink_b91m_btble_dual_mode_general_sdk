/********************************************************************************************************
 * @file	tlkmmi_phone.c
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
#if (TLKMMI_PHONE_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_phone.h"
#include "tlkmmi_phoneAdapt.h"
#include "tlkmmi_phoneCtrl.h"
#include "tlkmmi_phoneBook.h"
#include "tlkmmi_phoneMsgInner.h"
#include "tlkmmi_phoneMsgOuter.h"


TLKSYS_MMI_TASK_DEFINE(phone, Phone);


static int tlkmmi_phone_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];

	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_CALL;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);

	tlkmmi_phone_adaptInit(procID);
	tlkmmi_phone_ctrlInit();
	tlkmmi_phone_bookInit();
	
	return TLK_ENONE;
}
static int tlkmmi_phone_start(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_phone_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_phone_close(void)
{
	
	return TLK_ENONE;
}
static int tlkmmi_phone_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_phone_innerMsgHandler(msgID, pData, dataLen);
	}else{
		return tlkmmi_phone_outerMsgHandler(msgID, pData, dataLen);
	}
}
static void tlkmmi_phone_handler(void)
{
	
}



#endif //#if (TLKMMI_PHONE_ENABLE)


/********************************************************************************************************
 * @file	tlkats.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../tlkmmi_testStdio.h"
#include "../tlkmmi_testAdapt.h"
#include "../tlkmmi_testModinf.h"
#if (TLK_TEST_ATS_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_atsDefine.h"
#include "tlkmmi_ats.h"
#include "tlkmmi_atsAuth.h"
#include "tlkmmi_atsCtrl.h"

extern int bth_hci_sendResetCmd(void);

static int  tlkmmi_ats_init(uint08 procID, uint16 taskID);
static int  tlkmmi_ats_start(void);
static int  tlkmmi_ats_pause(void);
static int  tlkmmi_ats_close(void);
static int  tlkmmi_ats_input(uint32 msgID, uint08 *pData, uint16 dataLen);
const tlkmmi_testModinf_t gTlkMmiAtsModinf = 
{
	tlkmmi_ats_init,  //.Init
	tlkmmi_ats_start, //.Start
	tlkmmi_ats_pause, //.Pause
	tlkmmi_ats_close, //.Close
	tlkmmi_ats_input, //.Input
};



static int tlkmmi_ats_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];

	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_AUTHEN;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);
	
	tlkmmi_ats_ctrlInit();	
	tlkmmi_ats_authInit();
	
	return TLK_ENONE;
}
static int tlkmmi_ats_start(void)
{

	return TLK_ENONE;
}
static int tlkmmi_ats_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_ats_close(void)
{
	bth_hci_sendResetCmd();
	return TLK_ENONE;
}
static int tlkmmi_ats_input(uint32 msgID, uint08 *pData, uint16 dataLen)
{
	uint08 mtype = (msgID & 0x00FF0000) >> 16;
	uint16 cmdID = msgID & 0x0FFF;

	if(mtype == TLKPRT_COMM_MTYPE_AUTHEN){
		tlkmmi_ats_recvHandler(cmdID, pData, dataLen);
	}
	
	return TLK_ENONE;
}




#endif //#if (TLK_TEST_ATS_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


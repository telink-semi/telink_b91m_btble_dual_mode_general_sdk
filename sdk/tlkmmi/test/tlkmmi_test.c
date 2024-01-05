/********************************************************************************************************
 * @file	tlkmmi_test.c
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
#include "tlksys/tlksys_stdio.h"
#include "tlkstk/tlkstk_stdio.h"
#include "tlkmmi_test.h"
#include "tlkmmi_testAdapt.h"
#include "tlkmmi_testModinf.h"
#include "tlkmmi_testMsg.h"
#include "drivers.h"


static bool tlkmmi_test_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmmi_test_rebootDeal(void);

extern uint tlkcfg_getWorkMode(void);
#if (TLK_CFG_SYS_ENABLE)
static bool tlkmmi_test_pmIsBusy(void);
#endif
tlkmmi_test_ctrl_t sTlkMmiTestCtrl;


TLKSYS_MMI_TASK_DEFINE(test, Test);


static int tlkmmi_test_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];
	uint08 workMode;

	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_TEST;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);
	
	workMode = tlkcfg_getWorkMode();
	tmemset(&sTlkMmiTestCtrl, 0, sizeof(tlkmmi_test_ctrl_t));
	if(workMode == TLK_WORK_MODE_TEST_PTS) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_PTS;
	else if(workMode == TLK_WORK_MODE_TEST_RDT) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_RDT;
	else if(workMode == TLK_WORK_MODE_TEST_FAT) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_FAT;
	else if(workMode == TLK_WORK_MODE_TEST_EMI) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_EMI;
	else if(workMode == TLK_WORK_MODE_TEST_USR) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_USR;
	else if(workMode == TLK_WORK_MODE_TEST_BQB) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_BQB;
	else if(workMode == TLK_WORK_MODE_TEST_ATS) sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_ATS;
	else sTlkMmiTestCtrl.wmode = TLKMMI_TEST_MODTYPE_NONE;
	if(sTlkMmiTestCtrl.wmode == TLKMMI_TEST_MODTYPE_NONE) return TLK_ENONE;

	tlkmmi_test_adaptInit(procID);
	tlkmmi_test_adaptInitTimer(&sTlkMmiTestCtrl.timer, tlkmmi_test_timer, NULL, TLKMMI_TEST_TIMEOUT);
	tlkmmi_test_modInit(sTlkMmiTestCtrl.wmode, procID, taskID);

	#if (TLK_CFG_SYS_ENABLE)
	tlksys_pm_appendBusyCheckCB(tlkmmi_test_pmIsBusy, "test");
	#endif

	return TLK_ENONE;
}
static int tlkmmi_test_start(void)
{
	tlkmmi_test_modStart(sTlkMmiTestCtrl.wmode);
	return TLK_ENONE;
}
static int tlkmmi_test_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_test_close(void)
{
	return TLK_ENONE;
}
static int tlkmmi_test_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	tlkapi_trace(TLKMMI_TEST_DBG_FLAG, TLKMMI_TEST_DBG_SIGN, "mtype->%d msgID->%d", mtype, msgID);
	tlkapi_array(TLKMMI_TEST_DBG_FLAG, TLKMMI_TEST_DBG_SIGN, "Payload", pData, dataLen);
	if(mtype == TLKPRT_COMM_MTYPE_TEST && msgID < TLKPRT_COMM_CMDID_TEST_CONTEXT_START){
		return tlkmmi_test_recvMsgHandler(msgID, pData, dataLen);	
	}
	return tlkmmi_test_modInput(sTlkMmiTestCtrl.wmode, ((uint32)mtype<<16) | msgID, pData, dataLen);
}
static void tlkmmi_test_handler(void)
{
	tlkmmi_test_modHandler(sTlkMmiTestCtrl.wmode);
}
#if (TLK_CFG_SYS_ENABLE)
static bool tlkmmi_test_pmIsBusy(void)
{
	return true;
}
#endif

void tlkmmi_test_reboot(uint16 timeout)
{
	tlkmmi_test_close();
	sTlkMmiTestCtrl.rebootTime = timeout/TLKMMI_TEST_TIMEOUT;
	if(timeout != 0 && sTlkMmiTestCtrl.rebootTime == 0) sTlkMmiTestCtrl.rebootTime = 1;
	if(sTlkMmiTestCtrl.rebootTime == 0){
		core_reboot();
	}else{
		sTlkMmiTestCtrl.busys |= TLKMMI_TEST_BUSY_REBOOT;
		tlkmmi_test_adaptInsertTimer(&sTlkMmiTestCtrl.timer);
	}
}

static bool tlkmmi_test_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if((sTlkMmiTestCtrl.busys & TLKMMI_TEST_BUSY_REBOOT) != 0)
	{
		sTlkMmiTestCtrl.busys &= ~TLKMMI_TEST_BUSY_REBOOT;
		tlkmmi_test_rebootDeal();
	}
	return true;
}
static void tlkmmi_test_rebootDeal(void)
{
	if(sTlkMmiTestCtrl.rebootTime != 0) sTlkMmiTestCtrl.rebootTime--;
	if(sTlkMmiTestCtrl.rebootTime != 0){
		sTlkMmiTestCtrl.busys |= TLKMMI_TEST_BUSY_REBOOT;
	}else{
		core_reboot();
	}
}



#endif //#if (TLKMMI_TEST_ENABLE)


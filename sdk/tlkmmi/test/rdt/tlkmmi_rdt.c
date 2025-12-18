/********************************************************************************************************
 * @file	tlkmmi_rdt.c
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
#if (TLK_TEST_RDT_ENABLE)
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmmi_rdt.h"
#include "./bt/tlkmmi_rdt_bt.h"
#include "./le/tlkmmi_rdt_le.h"
#include "tlkmmi_rdtStd.h"
#include "tlkmmi_rdtMsg.h"
#include "tlkmmi_rdtModinf.h"


extern void tlkmmi_test_reboot(uint16 timeout);
static int tlkmmi_rdt_init(uint08 procID, uint16 taskID);
static int tlkmmi_rdt_start(void);
static int tlkmmi_rdt_pause(void);
static int tlkmmi_rdt_close(void);
static int tlkmmi_rdt_input(uint32 msgID, uint08 *pData, uint16 dataLen);


static uint16 sTlkmmiRdtTestCase = 0;
const tlkmmi_testModinf_t gTlkMmiRatModinf = 
{
	tlkmmi_rdt_init,  //.Init
	tlkmmi_rdt_start, //.Start
	tlkmmi_rdt_pause, //.Pause
	tlkmmi_rdt_close, //.Close
	tlkmmi_rdt_input, //.Input
};


void tlkmmi_rdt_reboot(void)
{
	if(sTlkmmiRdtTestCase != 0){
		tlkmmi_rdt_closeTest();
	}
	tlkmmi_test_reboot(500);
}

//Role - Refer TLKMMI_RDT_ROLE_ENUM.
int tlkmmi_rdt_startTest(uint16 caseID, uint08 role)
{
	int ret;
	if(caseID == 0){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_startTest: error caseID");
		return -TLK_EPARAM;
	}
	if(sTlkmmiRdtTestCase != 0){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_startTest: test has be started, please closing it firstly");
		return -TLK_EFAIL;
	}
	
	ret = tlkmmi_rdt_modStart(caseID, role);
	if(ret == TLK_ENONE){
		sTlkmmiRdtTestCase = caseID;
	}else{
		tlkapi_warn(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_startTest: start failure");
	}
	return ret;
}
int tlkmmi_rdt_pauseTest(void)
{
	if(sTlkmmiRdtTestCase == 0){
		tlkapi_warn(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_pauseTest: invalid caseID");
	}else{
		tlkapi_warn(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_pauseTest: not supported");
	}
	return TLK_ENONE;
}
int tlkmmi_rdt_closeTest(void)
{
	if(sTlkmmiRdtTestCase == 0){
		tlkapi_warn(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_startTest: invalid caseID");
	}else{
		tlkmmi_rdt_modClose(sTlkmmiRdtTestCase);
		sTlkmmiRdtTestCase = 0;
	}
	return TLK_ENONE;
}
int tlkmmi_rdt_testInput(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(sTlkmmiRdtTestCase == 0){
		tlkapi_warn(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_testInput: invalid caseID");
	}else{
		return tlkmmi_rdt_modInput(sTlkmmiRdtTestCase, msgID, pData, dataLen);
	}
	return TLK_ENONE;
}


static int tlkmmi_rdt_init(uint08 procID, uint16 taskID)
{
	tlkmmi_rdt_btInit();
	tlkmmi_rdt_leInit();
	
	return TLK_ENONE;
}
static int tlkmmi_rdt_start(void)
{
	return TLK_ENONE;
}
static int tlkmmi_rdt_pause(void)
{
	if(sTlkmmiRdtTestCase != 0){
		tlkapi_warn(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_pause: not supported");
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_close(void)
{
	if(sTlkmmiRdtTestCase != 0){
		tlkmmi_rdt_modClose(sTlkmmiRdtTestCase);
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_input(uint32 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID != TLKPRT_COMM_CMDID_TEST_RDT || dataLen == 0) return -TLK_EPARAM;
	return tlkmmi_rdt_recvMsgHandler(pData[0], pData+1, dataLen-1);
}




#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


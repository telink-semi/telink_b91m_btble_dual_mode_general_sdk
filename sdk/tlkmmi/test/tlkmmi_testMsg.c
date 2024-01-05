/********************************************************************************************************
 * @file	tlkmmi_testMsg.c
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
#include "tlkmmi_test.h"
#include "tlkmmi_testMsg.h"
#include "tlkmmi_testModinf.h"

static void tlkmmi_test_recvStartDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_test_recvPauseDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_test_recvCloseDeal(uint08 *pData, uint16 dataLen);
static void tlkmmi_test_recvRebootDeal(void);


extern tlkmmi_test_ctrl_t sTlkMmiTestCtrl;


int tlkmmi_test_recvMsgHandler(uint16 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKPRT_COMM_CMDID_TEST_START){
		tlkmmi_test_recvStartDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_TEST_CLOSE){
		tlkmmi_test_recvPauseDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_TEST_PAUSE){
		tlkmmi_test_recvCloseDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_TEST_REBOOT){
		tlkmmi_test_recvRebootDeal();
	}else{
		tlkmmi_test_sendCommRsp(msgID, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_test_sendCommXxx.
 * Descript: send the test command or Response or Event.
 * Params:
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_test_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_TEST;
	head[headLen++] = cmdID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_test_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_RSP; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_TEST;
	head[headLen++] = cmdID;
	head[headLen++] = status;
	head[headLen++] = reason;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}
int tlkmmi_test_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen = 0;
	uint08 head[TLKPTI_SYS_SERIAL_HEADLEN] = {0};
	head[headLen++] = TLKPRT_COMM_PTYPE_EVT; //Cmd
	head[headLen++] = TLKPRT_COMM_MTYPE_TEST;
	head[headLen++] = evtID;
	return tlksys_sendInnerExtMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_SERIAL_SEND,
		head, TLKPTI_SYS_SERIAL_HEADLEN, pData, dataLen);
}

static void tlkmmi_test_recvStartDeal(uint08 *pData, uint16 dataLen)
{
	int ret = tlkmmi_test_modStart(sTlkMmiTestCtrl.wmode);
	if(ret < 0) ret = -ret;
	if(ret == TLK_ENONE){
		tlkmmi_test_sendCommRsp(TLKPRT_COMM_CMDID_TEST_START, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_test_sendCommRsp(TLKPRT_COMM_CMDID_TEST_START, TLKPRT_COMM_RSP_STATUE_FAILURE, ret, nullptr, 0);
	}
}
static void tlkmmi_test_recvPauseDeal(uint08 *pData, uint16 dataLen)
{
	int ret = tlkmmi_test_modPause(sTlkMmiTestCtrl.wmode);
	if(ret < 0) ret = -ret;
	if(ret == TLK_ENONE){
		tlkmmi_test_sendCommRsp(TLKPRT_COMM_CMDID_TEST_PAUSE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_test_sendCommRsp(TLKPRT_COMM_CMDID_TEST_PAUSE, TLKPRT_COMM_RSP_STATUE_FAILURE, ret, nullptr, 0);
	}
}
static void tlkmmi_test_recvCloseDeal(uint08 *pData, uint16 dataLen)
{
	int ret = tlkmmi_test_modClose(sTlkMmiTestCtrl.wmode);
	if(ret < 0) ret = -ret;
	if(ret == TLK_ENONE){
		tlkmmi_test_sendCommRsp(TLKPRT_COMM_CMDID_TEST_CLOSE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_test_sendCommRsp(TLKPRT_COMM_CMDID_TEST_CLOSE, TLKPRT_COMM_RSP_STATUE_FAILURE, ret, nullptr, 0);
	}
}
static void tlkmmi_test_recvRebootDeal(void)
{
	tlkmmi_test_modClose(sTlkMmiTestCtrl.wmode);
	tlkmmi_test_reboot(500);
	tlkmmi_test_sendCommRsp(TLKPRT_COMM_CMDID_TEST_REBOOT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
}



#endif //#if (TLKMMI_TEST_ENABLE)


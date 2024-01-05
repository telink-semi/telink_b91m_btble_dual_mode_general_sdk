/********************************************************************************************************
 * @file	tlkmmi_pts.c
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
#if (TLK_TEST_PTS_ENABLE)
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmmi_pts.h"
#include "tlkmmi_ptsBt.h"
#include "tlkmmi_ptsMsg.h"


extern int bth_hci_sendResetCmd(void);

static int  tlkmmi_pts_init(uint08 procID, uint16 taskID);
static int  tlkmmi_pts_start(void);
static int  tlkmmi_pts_pause(void);
static int  tlkmmi_pts_close(void);
static int  tlkmmi_pts_input(uint32 msgID, uint08 *pData, uint16 dataLen);
static bool tlkmmi_pts_timer(tlkapi_timer_t *pTimer, uint32 userArg);
const tlkmmi_testModinf_t gTlkMmiPtsModinf = 
{
	tlkmmi_pts_init,  //.Init
	tlkmmi_pts_start, //.Start
	tlkmmi_pts_pause, //.Pause
	tlkmmi_pts_close, //.Close
	tlkmmi_pts_input, //.Input
};
tlkmmi_pts_ctrl_t gTlkMmiPtsCtrl;


static int tlkmmi_pts_init(uint08 procID, uint16 taskID)
{
	tlkmmi_test_adaptInitTimer(&gTlkMmiPtsCtrl.timer, tlkmmi_pts_timer, NULL, TLKMMI_PTS_TIMEOUT);
	tlkmmi_pts_btInit();
	return TLK_ENONE;
}
static int tlkmmi_pts_start(void)
{
	return TLK_ENONE;
}
static int tlkmmi_pts_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_pts_close(void)
{
	bth_hci_sendResetCmd();
	return TLK_ENONE;
}
static int tlkmmi_pts_input(uint32 msgID, uint08 *pData, uint16 dataLen)
{
	msgID = msgID & 0xFFFF;
	if(msgID != TLKPRT_COMM_CMDID_TEST_PTS || dataLen == 0) return -TLK_EPARAM;
	return tlkmmi_pts_recvMsgHandler(pData[0], pData+1, dataLen-1);
}
static bool tlkmmi_pts_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	
	return true;
}




#endif //#if (TLK_TEST_PTS_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


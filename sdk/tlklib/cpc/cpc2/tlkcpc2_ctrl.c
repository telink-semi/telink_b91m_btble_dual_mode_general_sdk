/********************************************************************************************************
 * @file	tlkmdi_atsComm.c
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
#if (TLK_CFG_COMM_ENABLE)
#include "tlksys/prt/tlkpto_comm.h"
#include "../tlkcpc.h"
#include "tlkcpc2.h"
#include "tlkcpc2_ctrl.h"
#include "tlkcpc2_pool.h"
#include "tlkcpc2_recv.h"
#include "tlkcpc2_send.h"
#include "drivers.h"



static void tlkcpc2_ctrl_resetHandler(void);
static void tlkcpc2_ctrl_readyHandler(void);


extern tlkcpc2_ctrl_t *spTlkCpc2Ctrl;



int tlkcpc2_ctrl_init(void)
{
	tlkcpc2_pool_init();
	tlkcpc2_send_init();
	tlkcpc2_recv_init();
	
	return TLK_ENONE;
}

void tlkcpc2_ctrl_reset(void)
{
	tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_ctrl_reset");

	spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_NONE;
	if(spTlkCpc2Ctrl->authState != TLKCPC2_AUTH_STATUS_NONE){
		spTlkCpc2Ctrl->authState = TLKCPC2_AUTH_STATUS_RUNNING;
	}
	
    spTlkCpc2Ctrl->pollTimer = 0;
    spTlkCpc2Ctrl->authTimer = 0;
	spTlkCpc2Ctrl->syncTimer = 0;
	spTlkCpc2Ctrl->resetTimer = clock_time() | 1;
	
	spTlkCpc2Ctrl->sendNumb = 0;
	spTlkCpc2Ctrl->recvNumb = 0;
	spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_WAIT_REQ;
	
	tlkcpc2_pool_reset(false);
	tlkcpc2_recv_reset();
}


void tlkcpc2_enable_authen(bool enable)
{
	if(enable && spTlkCpc2Ctrl->authState == TLKCPC2_AUTH_STATUS_NONE){
		spTlkCpc2Ctrl->authState = TLKCPC2_AUTH_STATUS_RUNNING;
	}else if(!enable){
		spTlkCpc2Ctrl->authState = TLKCPC2_AUTH_STATUS_NONE;
	}
}
void tlkcpc2_set_authenStatus(uint08 status)
{
	if(status == TLKCPC2_AUTH_STATUS_NONE){
		return;
	}
	if(spTlkCpc2Ctrl->authState == TLKCPC2_AUTH_STATUS_NONE){
		return;
	}
	spTlkCpc2Ctrl->authState = status;
	if(status == TLKCPC2_AUTH_STATUS_SUCCESS){
		spTlkCpc2Ctrl->authTimer = 0;
		spTlkCpc2Ctrl->authState = TLKCPC2_AUTH_STATUS_SUCCESS;
		tlkcpc2_pool_activeCopyItem();
	}else if(status == TLKCPC2_AUTH_STATUS_FAILURE){
		spTlkCpc2Ctrl->authTimer = 0;
		spTlkCpc2Ctrl->authState = TLKCPC2_AUTH_STATUS_FAILURE;
		spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_NONE;
		spTlkCpc2Ctrl->resetTimer = 0;
	}
}



void tlkcpc2_abort_handler(uint08 abortCode)
{
	tlkapi_fatal(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "tlkcpc2_abort_handler: %d!", abortCode);

	spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_NONE;
    spTlkCpc2Ctrl->pollTimer = 0;
    spTlkCpc2Ctrl->authTimer = 0;
	spTlkCpc2Ctrl->syncTimer = 0;
	spTlkCpc2Ctrl->resetTimer = 0;
	spTlkCpc2Ctrl->sendNumb = 0;
	spTlkCpc2Ctrl->recvNumb = 0;
}


void tlkcpc2_ctrl_handler(void)
{
	if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_DONE){
		tlkcpc2_ctrl_readyHandler();
	}else if(spTlkCpc2Ctrl->resetStat != TLKCPC2_RESET_STATUS_NONE){
		tlkcpc2_ctrl_resetHandler();
	}
		
//	if(AAAA_log_test001 == 0 || clock_time_exceed(AAAA_log_test001, 3000000))
//	{
//		AAAA_log_test001 = clock_time() | 1;
//		tlkapi_trace(TLKCPC2_DBG_FLAG, TLKCPC2_DBG_SIGN, "dut log test %d", AAAA_log_test002++);
//	}
}

static void tlkcpc2_ctrl_resetHandler(void)
{
	if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_NONE){
		return;
	}
	if(spTlkCpc2Ctrl->resetStat == TLKCPC2_RESET_STATUS_WAIT_REQ){
		if(spTlkCpc2Ctrl->pollTimer == 0 || clock_time_exceed(spTlkCpc2Ctrl->pollTimer, TLKCPC2_SOCKET_RESET_SEND_TIMEOUT)){
			spTlkCpc2Ctrl->pollTimer = clock_time() | 1;
			tlkcpc2_sendResetReqPacket();
		}
	}
	if(spTlkCpc2Ctrl->resetTimer != 0 && clock_time_exceed(spTlkCpc2Ctrl->resetTimer, TLKCPC2_SOCKET_RESET_TIMEOUT)){
		spTlkCpc2Ctrl->resetTimer = 0;
		spTlkCpc2Ctrl->resetStat = TLKCPC2_RESET_STATUS_NONE;
		if(spTlkCpc2Ctrl->authState != TLKCPC2_AUTH_STATUS_NONE){
			spTlkCpc2Ctrl->authState  = TLKCPC2_AUTH_STATUS_RUNNING;
		}
	}
}
static void tlkcpc2_ctrl_readyHandler(void)
{
	if(spTlkCpc2Ctrl->stallTimer != 0 && clock_time_exceed(spTlkCpc2Ctrl->stallTimer, TLKCPC2_SOCKET_STALL_TIMEOUT)){
		spTlkCpc2Ctrl->stallTimer = 0;
		tlkcpc2_abort_handler(TLKCPC2_ABORT_EVTID_TIMEOUT);
		return;
	}
	if(spTlkCpc2Ctrl->syncTimer != 0 && clock_time_exceed(spTlkCpc2Ctrl->syncTimer, TLKCPC2_SOCKET_SYNC_TIMEOUT)){
		spTlkCpc2Ctrl->syncTimer = 0;
		if(tlkcpc2_pool_getCacheNumb() != 0){
        	tlkcpc2_abort_handler(TLKCPC2_ABORT_EVTID_TIMEOUT);
			return;
		}
	}
	if(spTlkCpc2Ctrl->stallTimer != 0 && clock_time_exceed(spTlkCpc2Ctrl->stallTimer, TLKCPC2_SOCKET_STALL_TIMEOUT)){
		spTlkCpc2Ctrl->stallTimer = 0;
		tlkcpc2_abort_handler(TLKCPC2_ABORT_EVTID_TIMEOUT);
		return;
	}
	if(spTlkCpc2Ctrl->authTimer != 0 && clock_time_exceed(spTlkCpc2Ctrl->authTimer, TLKCPC2_SOCKET_AUTH_TIMEOUT)){
		tlkcpc2_abort_handler(TLKCPC2_ABORT_EVTID_AUTHEN);
		return;
	}
	if(spTlkCpc2Ctrl->pollTimer != 0 && clock_time_exceed(spTlkCpc2Ctrl->pollTimer, TLKCPC2_SOCKET_POLL_TIMEOUT)){
		uint08 bitP = 0;
		uint08 bitR = 0;
		if(spTlkCpc2Ctrl->stallTimer != 0) bitR = 1;
		if(spTlkCpc2Ctrl->syncTimer != 0) bitP = 1;
		tlkcpc2_sendFcsPacket(bitP, 1, bitR, spTlkCpc2Ctrl->recvNumb, spTlkCpc2Ctrl->sendNumb);
		if(spTlkCpc2Ctrl->stallTimer != 0 || spTlkCpc2Ctrl->syncTimer != 0){
			spTlkCpc2Ctrl->pollTimer = clock_time() | 1;
		}else{
			spTlkCpc2Ctrl->pollTimer = 0;
		}
	}
}



#endif //#if (TLK_TEST_ATS_ENABLE)


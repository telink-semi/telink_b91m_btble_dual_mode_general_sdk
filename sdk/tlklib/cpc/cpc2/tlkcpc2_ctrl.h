/********************************************************************************************************
 * @file	tlkmdi_atsComm.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKCPC2_COMM_H
#define TLKCPC2_COMM_H

#if (TLK_CFG_COMM_ENABLE)



#define TLKCPC2_SOCKET_TIMER_INTV      200000
#define TLKCPC2_SOCKET_POLL_TIMEOUT    30000
#define TLKCPC2_SOCKET_AUTH_TIMEOUT    5000000 //5s
#define TLKCPC2_SOCKET_SYNC_TIMEOUT    3000000
#define TLKCPC2_SOCKET_STALL_TIMEOUT         3000000
#define TLKCPC2_SOCKET_RESET_TIMEOUT         3000000
#define TLKCPC2_SOCKET_RESET_SEND_TIMEOUT    200000
#define TLKCPC2_SOCKET_RESET_MAX_NUMB        12


typedef enum{
	TLKCPC2_ABORT_EVTID_RESET   = 0x01,
	TLKCPC2_ABORT_EVTID_AUTHEN  = 0x02,
	TLKCPC2_ABORT_EVTID_TIMEOUT = 0x03,
	TLKCPC2_ABORT_EVTID_SYNC    = 0x04,
	TLKCPC2_ABORT_EVTID_WAIT    = 0x05,
	TLKCPC2_ABORT_EVTID_RESTORE = 0x06,
}TLKCPC2_ABORT_EVTID_ENUM;

typedef enum{
	TLKCPC2_RESET_STATUS_NONE      = 0x00,
	TLKCPC2_RESET_STATUS_DONE      = 0x01,
	TLKCPC2_RESET_STATUS_WAIT_REQ  = 0x02,
	TLKCPC2_RESET_STATUS_WAIT_ACK  = 0x03,
	TLKCPC2_RESET_STATUS_WAIT_CFM  = 0x04,
	TLKCPC2_RESET_STATUS_WAIT_DONE = 0x05,
}TLKCPC2_RESET_STATUS_ENUM;



int tlkcpc2_ctrl_init(void);

void tlkcpc2_ctrl_reset(void);

void tlkcpc2_enable_authen(bool enable);
void tlkcpc2_set_authenStatus(uint08 status);

void tlkcpc2_abort_handler(uint08 abortCode);

void tlkcpc2_ctrl_handler(void);


#endif //#if (TLK_TEST_ATS_ENABLE)

#endif //TLKCPC2_COMM_H


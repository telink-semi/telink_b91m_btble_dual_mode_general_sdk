/********************************************************************************************************
 * @file	tlkmdi_ats.h
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
#ifndef TLKCPC2_H
#define TLKCPC2_H

#if (TLK_CFG_COMM_ENABLE)



#define TLKCPC2_TIMEOUT                 200000 //50ms

#define TLKCPC2_DBG_FLAG                ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_COMM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKCPC2_DBG_SIGN                "[COMM]"


#define TLKCPC2_FRAME_HEAD_SIGN         0xAC
#define TLKCPC2_FRAME_TAIL_SIGN         0xCA
#define TLKCPC2_ESCAPE_CHARS            0xDC //Must not TLKCPC1_HEAD_SIGN or TLKPRT_COMM_HEAD_TAIL
#define TLKCPC2_ESCAPE_CHARS_RAW        0x01 //eg: DC -> DC 01
#define TLKCPC2_ESCAPE_CHARS_HEAD       0x02 //eg: AC -> DC 02
#define TLKCPC2_ESCAPE_CHARS_TAIL       0x03 //eg: AC -> DC 02

#define TLKCPC2_SEND_BUFF_SIZE          128
#define TLKCPC2_RECV_CACHE_SIZE         280

#define TLKCPC2_POOL_ITEM_SIZE          TLKCPC2_SEND_BUFF_SIZE
#define TLKCPC2_POOL_ITEM_NUMB          16 //32
#define TLKCPC2_POOL_ITEM_90PER_THRD    15 //(TLKCPC2_POOL_ITEM_NUMB*90%)
#define TLKCPC2_POOL_ITEM_80PER_THRD    13 //(TLKCPC2_POOL_ITEM_NUMB*80%) 
#define TLKCPC2_POOL_ITEM_60PER_THRD    10 //(TLKCPC2_POOL_ITEM_NUMB*60%)
#define TLKCPC2_POOL_ITEM_30PER_THRD    5 //(TLKCPC2_POOL_ITEM_NUMB*30%)
#define TLKCPC2_POOL_ITEM_10PER_THRD    2 //(TLKCPC2_POOL_ITEM_NUMB*10%)
#define TLKCPC2_POOL_ITEM_05PER_THRD    1 //(TLKCPC2_POOL_ITEM_NUMB*5%)


typedef enum{
    TLKCPC2_AUTH_STATUS_NONE    = 0,
    TLKCPC2_AUTH_STATUS_SUCCESS = 1,
    TLKCPC2_AUTH_STATUS_FAILURE = 2,
    TLKCPC2_AUTH_STATUS_RUNNING = 3,
}TLKCPC2_AUTH_STATUS_ENUM;
typedef enum{
	TLKCPC2_FCS_FLAG_P = 0x0001, //Poll
	TLKCPC2_FCS_FLAG_F = 0x0002, //Final
	TLKCPC2_FCS_FLAG_R = 0x0004, //retransmission
}TLKCPC2_FCS_FLAG_ENUM;
typedef enum {
	TLKCPC2_RST_CODE_REQ = 1,
	TLKCPC2_RST_CODE_ACK = 2,
	TLKCPC2_RST_CODE_CFM = 3,
	TLKCPC2_RST_CODE_CFM_ACK = 4,
}TLKCPC2_RST_CODE_ENUM;


typedef struct{
	uint08 itemNumb; //It will be set by system, and should note be changed by user.
	uint08 sendNumb; //Set by user
	uint16 ctrlTran: 1;
	uint16 dataSize: 15; //Set by user
	uint08 itemData[TLKCPC2_POOL_ITEM_SIZE];
}tlkcpc2_pool_item_t;
typedef struct{
	uint08 idleNumb;
	uint08 copyNumb;
	uint08 sendNumb;
	uint08 cacheNumb;
	uint08 idleList[TLKCPC2_POOL_ITEM_NUMB];
	uint08 copyList[TLKCPC2_POOL_ITEM_NUMB];
	uint08 sendList[TLKCPC2_POOL_ITEM_NUMB];
	uint08 cacheList[TLKCPC2_POOL_ITEM_NUMB];
	tlkcpc2_pool_item_t itemList[TLKCPC2_POOL_ITEM_NUMB];
}tlkcpc2_pool_list_t;


typedef struct{
	uint08 mstate;
	uint08 reserved;
	uint16 recvLens;
	uint08 recvBuff[TLKCPC2_RECV_CACHE_SIZE];
	uint16 buffLen;
	uint08 *pBuffer;
	TlkCpcRecvCB recvCB;
	TlkCpcResetCB resetRequestCB;
	TlkCpcResetCB resetCompleteCB;
}tlkcpc2_recv_ctrl_t;
typedef struct{
	uint32 buffLen;
	uint08 *pBuffer;
	TlkCpcSendCB sendCB;
}tlkcpc2_send_ctrl_t;

typedef struct{
	uint32 pollTimer; //After sending the poll packet, wait for the detection timer.
    uint32 authTimer; //After the first connection, start the authentication detection timer.quint32 m_waitTimer;
	uint32 syncTimer;
	uint32 stallTimer; //After packet loss synchronization times out, the system resets
	uint32 resetTimer;

	uint08 sendNumb;
	uint08 recvNumb;
	uint08 resetStat;
	uint08 authState;

	tlkcpc2_send_ctrl_t sendCtrl;
	tlkcpc2_recv_ctrl_t recvCtrl;
	tlkcpc2_pool_list_t poolCtrl;
}tlkcpc2_ctrl_t;



#endif //#if (TLK_TEST_ATS_ENABLE)

#endif //TLKCPC2_H


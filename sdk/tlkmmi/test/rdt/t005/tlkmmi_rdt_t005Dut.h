/********************************************************************************************************
 * @file	tlkmmi_rdt_t005Dut.h
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
#ifndef TLKMMI_RDT_T005_DUT_H
#define TLKMMI_RDT_T005_DUT_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T005_ENABLE)



#define TLKMMI_RDT_T005_AUT2_ENABLE        0
#define TLKMMI_RDT_T005_WAIT_MAX_NUMB      10


typedef struct{
	uint08 enable;
	uint08 method;
	uint16 number; //Send Packet Number
	uint16 minLength; //Send Packet Length
	uint16 maxLength;
	uint16 minIntval; //Send Packet Interval. Unit-ms
	uint16 maxIntval;
}tlkmmi_rdt_t005DutSend_t;
typedef struct{
	uint08 flags;
	uint08 busys;
	uint16 handle;
	uint32 timer;
	uint32 connTimer;
	uint32 waitTimer;
	uint08 hfChannel;
	uint08 agChannel;
	uint08 sppChannel;
	uint08 pbapChannel;
	uint08 peerMac[6];
	uint08 waitCount;
	uint08 connCount;
	uint08 waitTable[TLKMMI_RDT_T005_WAIT_MAX_NUMB];
	uint08 connTable[TLKMMI_RDT_T005_WAIT_MAX_NUMB];
}tlkmmi_rdt_t005DutUnit_t;
typedef struct{
	uint08 isStart;
	uint08 isFail;
	uint08 failPtype;
	uint08 failUsrID;
	uint32 dbgTimer;
	tlkapi_queue_t queue;
	tlkmmi_rdt_t005DutUnit_t aut1;
	tlkmmi_rdt_t005DutUnit_t aut2;
}tlkmmi_rdt_t005Dut_t;


void tlkmmi_rdt_t005DutStart(void);
void tlkmmi_rdt_t005DutClose(void);
void tlkmmi_rdt_t005DutTimer(void);
void tlkmmi_rdt_t005DutInput(uint08 msgID, uint08 *pData, uint16 dataLen);


void tlkmmi_rdt_t005TableInit(void);
bool tlkmmi_rdt_t005TableIsHave(uint08 ptype);
void tlkmmi_rdt_t005TableAppend(uint08 ptype, uint08 usrID);
void tlkmmi_rdt_t005TableRemove(uint08 ptype);



#endif //#if (TLKMMI_RDT_CASE_T005_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T005_DUT_H


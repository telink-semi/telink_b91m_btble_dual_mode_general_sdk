/********************************************************************************************************
 * @file	tlkmmi_rdt_t003Aut2.h
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
#ifndef TLKMMI_RDT_T003_AUT2_H
#define TLKMMI_RDT_T003_AUT2_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T003_ENABLE)


typedef struct{
	uint08 enable;
	uint08 method;
	uint16 number; //Send Packet Number
	uint16 minLength; //Send Packet Length
	uint16 maxLength;
	uint16 minIntval; //Send Packet Interval. Unit-ms
	uint16 maxIntval;
}tlkmmi_rdt_t003Aut2Send_t;
typedef struct{
	uint08 flags;
	uint08 busys;
	uint16 handle;
	uint16 length;
	uint32 intval;
	uint32 timer;
	uint32 sendFail;
	uint32 sendNumb;
	uint32 recvNumb;
}tlkmmi_rdt_t003Aut2Unit_t;


typedef struct{
	uint08 isStart;
	uint08 reserve1;
	uint16 reserve2;
	uint32 dbgTimer;
	uint32 connIntvl;
	uint32 connTimer;
	tlkapi_queue_t queue;
	tlkmmi_rdt_t003Aut2Unit_t dut;
}tlkmmi_rdt_t003Aut2_t;



void tlkmmi_rdt_t003Aut2Start(void);
void tlkmmi_rdt_t003Aut2Close(void);
void tlkmmi_rdt_t003Aut2Timer(void);
void tlkmmi_rdt_t003Aut2Input(uint08 msgID, uint08 *pData, uint16 dataLen);



#endif //#if (TLKMMI_RDT_CASE_T003_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T003_AUT2_H


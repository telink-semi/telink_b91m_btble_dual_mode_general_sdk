/********************************************************************************************************
 * @file	tlkmmi_rdt_t003.c
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
#include "../../tlkmmi_testStdio.h"
#if (TLK_TEST_RDT_ENABLE)
#include "../tlkmmi_rdtStd.h"
#if (TLKMMI_RDT_CASE_T003_ENABLE)
#include "tlkmmi_rdt_t003.h"
#include "tlkmmi_rdt_t003Dut.h"
#include "tlkmmi_rdt_t003Aut.h"
#include "tlkmmi_rdt_t003Aut2.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"


TLKMMI_RDT_MODINF_DEFINE(003);


static bool tlkmmi_rdt_t003Timer(tlkapi_timer_t *pTimer, uint32 userArg);


static tlkmmi_rdt_t003_t sTlkMmiRdtT003 = {0};
static uint08 sTlkMmiRdtT003SendBuffer[4+TLKMMI_RDT_T003_SEND_BUFF_LEN];


int tlkmmi_rdt_t003SendSppData(uint16 aclHandle, uint16 dataLen, uint32 serial)
{
	if(dataLen > TLKMMI_RDT_T003_SEND_BUFF_LEN) dataLen = TLKMMI_RDT_T003_SEND_BUFF_LEN;
	UINT32L_TO_ARRAY(serial, sTlkMmiRdtT003SendBuffer, 0);
	return btp_spp_sendData(aclHandle, nullptr, 0, sTlkMmiRdtT003SendBuffer, 4+dataLen);
}


static int tlkmmi_rdt_t003Start(uint08 role)
{
	uint16 index;
	
	if(role != TLKMMI_RDT_ROLE_DUT && role != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Start: error role[%d]",
			role);
		return -TLK_EROLE;
	}
	if(sTlkMmiRdtT003.state != TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Start: error status[%d]",
			sTlkMmiRdtT003.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Start:role[%d]", role);

	tlkmmi_test_adaptInitTimer(&sTlkMmiRdtT003.timer, tlkmmi_rdt_t003Timer,
		nullptr, TLKMMI_RDT_T003_TIMEOUT);
	tlkmmi_test_adaptInsertTimer(&sTlkMmiRdtT003.timer);
	
	sTlkMmiRdtT003.drole = role;
	sTlkMmiRdtT003.state = TLK_STATE_OPENED;
	if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t003DutStart();
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t003AutStart();
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT2){
		tlkmmi_rdt_t003Aut2Start();
	}else{
		sTlkMmiRdtT003.state = TLK_STATE_CLOSED;
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Start:role error");
	}
	for(index=0; index<TLKMMI_RDT_T003_SEND_BUFF_LEN; index++){
		sTlkMmiRdtT003SendBuffer[4+index] = index;
	}
	sTlkMmiRdtT003SendBuffer[0] = 0;
	sTlkMmiRdtT003SendBuffer[1] = 0;
	sTlkMmiRdtT003SendBuffer[2] = 0;
	sTlkMmiRdtT003SendBuffer[3] = 0;

	return TLK_ENONE;
}
static int tlkmmi_rdt_t003Close(void)
{
	if(sTlkMmiRdtT003.state == TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Close: error status[%d]",
			sTlkMmiRdtT003.state);
		return -TLK_ESTATUS;
	}
	tlkmmi_test_adaptRemoveTimer(&sTlkMmiRdtT003.timer);
	sTlkMmiRdtT003.state = TLK_STATE_CLOSED;
	if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t003DutClose();
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t003AutClose();
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT2){
		tlkmmi_rdt_t003Aut2Close();
	}
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Close");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t003Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(sTlkMmiRdtT003.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Input: error status[%d]",
			sTlkMmiRdtT003.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Input: msgID[0x%x]", msgID);
	if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t003DutInput(msgID, pData, dataLen);
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t003AutInput(msgID, pData, dataLen);
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT2){
		tlkmmi_rdt_t003Aut2Input(msgID, pData, dataLen);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Input: unknown role [%d]",
			sTlkMmiRdtT003.drole);
	}
	return TLK_ENONE;
}


static bool tlkmmi_rdt_t003Timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(sTlkMmiRdtT003.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t003Timer: error status[%d]",
			sTlkMmiRdtT003.state);
		return false;
	}
	if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t003DutTimer();
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t003AutTimer();
	}else if(sTlkMmiRdtT003.drole == TLKMMI_RDT_ROLE_AUT2){
		tlkmmi_rdt_t003Aut2Timer();
	}
	return true;
}



#endif //#if (TLKMMI_RDT_CASE_T003_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


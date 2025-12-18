/********************************************************************************************************
 * @file	tlkmmi_rdt_t002.c
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
#if (TLKMMI_RDT_CASE_T002_ENABLE)
#include "tlkmmi_rdt_t002.h"
#include "tlkmmi_rdt_t002Dut.h"
#include "tlkmmi_rdt_t002Aut.h"

static bool tlkmmi_rdt_t002Timer(tlkapi_timer_t *pTimer, uint32 userArg);


TLKMMI_RDT_MODINF_DEFINE(002);

static tlkmmi_rdt_t002_t sTlkMmiRdtT002 = {0};



static int tlkmmi_rdt_t002Start(uint08 role)
{
	if(role != TLKMMI_RDT_ROLE_DUT && role != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Start: error role[%d]",
			role);
		return -TLK_EROLE;
	}
	if(sTlkMmiRdtT002.state != TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Start: error status[%d]",
			sTlkMmiRdtT002.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Start:role[%d]",role);

	sTlkMmiRdtT002.drole = role;
	sTlkMmiRdtT002.state = TLK_STATE_OPENED;
	if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t002DutStart();
	}else if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t002AutStart();
	}
	
	tlkmmi_test_adaptInitTimer(&sTlkMmiRdtT002.timer, tlkmmi_rdt_t002Timer,
		nullptr, TLKMMI_RDT_T002_TIMEOUT);
	tlkmmi_test_adaptInsertTimer(&sTlkMmiRdtT002.timer);

	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Close(void)
{
	if(sTlkMmiRdtT002.state == TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Close: error status[%d]",
			sTlkMmiRdtT002.state);
		return -TLK_ESTATUS;
	}
	tlkmmi_test_adaptRemoveTimer(&sTlkMmiRdtT002.timer);
	sTlkMmiRdtT002.state = TLK_STATE_CLOSED;
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Close");
	if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t002DutClose();
	}else if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t002AutClose();
	}
	return TLK_ENONE;
}
static int tlkmmi_rdt_t002Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(sTlkMmiRdtT002.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Input: error status[%d]",
			sTlkMmiRdtT002.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Input: msgID[0x%x]", msgID);
	if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t002DutInput(msgID, pData, dataLen);
	}else if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t002AutInput(msgID, pData, dataLen);
	}
	return TLK_ENONE;
}


static bool tlkmmi_rdt_t002Timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(sTlkMmiRdtT002.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t002Timer: error status[%d]",
			sTlkMmiRdtT002.state);
		return false;
	}
	if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t002DutTimer();
	}else if(sTlkMmiRdtT002.drole == TLKMMI_RDT_ROLE_AUT){
		tlkmmi_rdt_t002AutTimer();
	}
	return true;
}



#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


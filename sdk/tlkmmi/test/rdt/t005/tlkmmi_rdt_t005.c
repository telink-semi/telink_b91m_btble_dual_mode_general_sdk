/********************************************************************************************************
 * @file	tlkmmi_rdt_t005.c
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
#if (TLKMMI_RDT_CASE_T005_ENABLE)
#include "tlkmmi_rdt_t005.h"
#include "tlkmmi_rdt_t005Dut.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"


TLKMMI_RDT_MODINF_DEFINE(005);


static bool tlkmmi_rdt_t005Timer(tlkapi_timer_t *pTimer, uint32 userArg);


static tlkmmi_rdt_t005_t sTlkMmiRdtT005 = {0};


static int tlkmmi_rdt_t005Start(uint08 role)
{
	if(role != TLKMMI_RDT_ROLE_DUT && role != TLKMMI_RDT_ROLE_AUT){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Start: error role[%d]",
			role);
		return -TLK_EROLE;
	}
	if(sTlkMmiRdtT005.state != TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Start: error status[%d]",
			sTlkMmiRdtT005.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Start:role[%d]", role);

	tlkmmi_test_adaptInitTimer(&sTlkMmiRdtT005.timer, tlkmmi_rdt_t005Timer,
		nullptr, TLKMMI_RDT_T005_TIMEOUT);
	tlkmmi_test_adaptInsertTimer(&sTlkMmiRdtT005.timer);
	
	sTlkMmiRdtT005.drole = role;
	sTlkMmiRdtT005.state = TLK_STATE_OPENED;
	if(sTlkMmiRdtT005.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t005DutStart();
	}else{
		sTlkMmiRdtT005.state = TLK_STATE_CLOSED;
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Start:role error");
	}

	return TLK_ENONE;
}
static int tlkmmi_rdt_t005Close(void)
{
	if(sTlkMmiRdtT005.state == TLK_STATE_CLOSED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Close: error status[%d]",
			sTlkMmiRdtT005.state);
		return -TLK_ESTATUS;
	}
	tlkmmi_test_adaptRemoveTimer(&sTlkMmiRdtT005.timer);
	sTlkMmiRdtT005.state = TLK_STATE_CLOSED;
	if(sTlkMmiRdtT005.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t005DutClose();
	}
	
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Close");
	return TLK_ENONE;
}
static int tlkmmi_rdt_t005Input(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(sTlkMmiRdtT005.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Input: error status[%d]",
			sTlkMmiRdtT005.state);
		return -TLK_ESTATUS;
	}
	tlkapi_trace(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Input: msgID[0x%x]", msgID);
	if(sTlkMmiRdtT005.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t005DutInput(msgID, pData, dataLen);
	}else{
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Input: unknown role [%d]",
			sTlkMmiRdtT005.drole);
	}
	return TLK_ENONE;
}


static bool tlkmmi_rdt_t005Timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(sTlkMmiRdtT005.state != TLK_STATE_OPENED){
		tlkapi_error(TLKMMI_RDT_DBG_FLAG, TLKMMI_RDT_DBG_SIGN, "tlkmmi_rdt_t005Timer: error status[%d]",
			sTlkMmiRdtT005.state);
		return false;
	}
	if(sTlkMmiRdtT005.drole == TLKMMI_RDT_ROLE_DUT){
		tlkmmi_rdt_t005DutTimer();
	}
	return true;
}



#endif //#if (TLKMMI_RDT_CASE_T005_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)


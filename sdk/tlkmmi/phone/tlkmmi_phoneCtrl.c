/********************************************************************************************************
 * @file	tlkmmi_phoneCtrl.c
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
#if (TLKMMI_PHONE_ENABLE)
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneCtrl.h"
#include "tlkmmi/phone/tlkmmi_phoneBook.h"
#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_sco.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkmmi/phone/tlkmmi_phoneAdapt.h"
#include "tlkmdi/bt/tlkmdi_btsco.h"
#include "drivers.h"
#endif

#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
static void tlkmmi_phone_scoConnCB(uint16 aclHandle, uint16 scoHandle, bool isConn);
static bool tlkmmi_phone_timer(tlkapi_timer_t *pTimer, uint32 userArg);
#endif


#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
static tlkmmi_phone_ctrl_t sTlkMmiPhoneCtrl;
#endif


/******************************************************************************
 * Function: tlkmmi_phone_ctrlInit
 * Descript: 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_ctrlInit(void)
{
	#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
	tmemset(&sTlkMmiPhoneCtrl, 0, sizeof(tlkmmi_phone_ctrl_t));
	tlkmmi_phone_adaptInitTimer(&sTlkMmiPhoneCtrl.timer, tlkmmi_phone_timer, (uint32)&sTlkMmiPhoneCtrl, TLKMMI_PHONE_TIMEOUT);
	tlkmdi_btsco_regCB(tlkmmi_phone_scoConnCB);
	#endif
		
	return TLK_ENONE;
}

#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
void tlkmmi_phone_resetHfCtrl(void)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_resetHfCtrl");
	
	sTlkMmiPhoneCtrl.aclHandle = 0;
	sTlkMmiPhoneCtrl.scoHandle = 0;
	
	sTlkMmiPhoneCtrl.optTimer = 0;
	sTlkMmiPhoneCtrl.hfOpcode = 0;
	sTlkMmiPhoneCtrl.hfStatus = 0;
	tlkmmi_phone_adaptRemoveTimer(&sTlkMmiPhoneCtrl.timer);
}
#endif
void tlkmmi_phone_setHfCallStatus(uint16 aclHandle, uint08 hfStatus)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus");
	
	#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
	//Only a single call is managed, and the call automatically fails when multiple calls are made.
	if(sTlkMmiPhoneCtrl.aclHandle != 0 && sTlkMmiPhoneCtrl.aclHandle != aclHandle){
		tlkmmi_phone_resetHfCtrl();
		return;
	}
	if(sTlkMmiPhoneCtrl.hfStatus == hfStatus){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: Repeat Status");
		return;
	}
	
	if(hfStatus == TLKMMI_PHONE_CALL_STATUS_INCOMING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_INCOMING");
		sTlkMmiPhoneCtrl.aclHandle = aclHandle;
		sTlkMmiPhoneCtrl.hfStatus  = hfStatus;
		sTlkMmiPhoneCtrl.hfOpcode  = TLKMMI_PHONE_MANUAL_CODE_NONE;
		sTlkMmiPhoneCtrl.optTimer  = 0;
	}else if(hfStatus == TLKMMI_PHONE_CALL_STATUS_OUTGOING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_OUTGOING");
		sTlkMmiPhoneCtrl.aclHandle = aclHandle;
		sTlkMmiPhoneCtrl.hfStatus  = hfStatus;
		if(sTlkMmiPhoneCtrl.scoHandle == 0){
			sTlkMmiPhoneCtrl.scoHandle = bth_handle_getConnScoHandle(aclHandle);
		}
		if(sTlkMmiPhoneCtrl.hfOpcode == TLKMMI_PHONE_MANUAL_CODE_DIAL){
			if(sTlkMmiPhoneCtrl.scoHandle == 0){
				sTlkMmiPhoneCtrl.optTimer = clock_time() | 1;
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_CALL_STATUS_OUTGOING: update timer  001");
			}
		}else{
			if(sTlkMmiPhoneCtrl.scoHandle != 0){
				bth_sco_disconn(sTlkMmiPhoneCtrl.scoHandle, 0x13);
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_CALL_STATUS_OUTGOING: disconn  001");
			}
		}
	}else if(hfStatus == TLKMMI_PHONE_CALL_STATUS_ACTIVE){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_ACTIVE");
		if(sTlkMmiPhoneCtrl.aclHandle != 0){
			if(sTlkMmiPhoneCtrl.scoHandle != 0 && sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_ANSWER){
				bth_sco_disconn(sTlkMmiPhoneCtrl.scoHandle, 0x13);
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_CALL_STATUS_OUTGOING: disconn  002");
			}else if(sTlkMmiPhoneCtrl.scoHandle == 0 && sTlkMmiPhoneCtrl.hfOpcode == TLKMMI_PHONE_MANUAL_CODE_ANSWER){
				sTlkMmiPhoneCtrl.optTimer = clock_time() | 1;
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_CALL_STATUS_OUTGOING: update timer  002");
			}
		}
	}else{
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: TLKMMI_PHONE_CALL_STATUS_IDLE");
		tlkmmi_phone_resetHfCtrl();
	}
	if(sTlkMmiPhoneCtrl.optTimer != 0){
		tlkmmi_phone_adaptInsertTimer(&sTlkMmiPhoneCtrl.timer);
	}
	#endif
}
void tlkmmi_phone_setHfManualCode(uint16 aclHandle, uint08 hfOpcode)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfManualCode");
	#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
	if(sTlkMmiPhoneCtrl.aclHandle != 0 && sTlkMmiPhoneCtrl.aclHandle != aclHandle){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfManualCode: Conflicting Manual Code");
		return;
	}
	if(sTlkMmiPhoneCtrl.hfOpcode == hfOpcode){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfManualCode: Repeat Opcode");
		return;
	}
		
	if(hfOpcode == TLKMMI_PHONE_MANUAL_CODE_DIAL){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_MANUAL_CODE_DIAL");
		if(sTlkMmiPhoneCtrl.hfStatus != TLKMMI_PHONE_CALL_STATUS_IDLE){
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 0: Invalid Operate");
		}else{
			sTlkMmiPhoneCtrl.aclHandle = aclHandle;
			sTlkMmiPhoneCtrl.hfOpcode  = hfOpcode;
		}
	}else if(hfOpcode == TLKMMI_PHONE_MANUAL_CODE_ANSWER){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_MANUAL_CODE_ANSWER");
		if(sTlkMmiPhoneCtrl.aclHandle != aclHandle || sTlkMmiPhoneCtrl.hfStatus != TLKMMI_PHONE_CALL_STATUS_INCOMING){
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 1: Invalid Operate");
		}else{
			sTlkMmiPhoneCtrl.aclHandle = aclHandle;
			sTlkMmiPhoneCtrl.hfOpcode = hfOpcode;
		}
	}else if(hfOpcode == TLKMMI_PHONE_MANUAL_CODE_HUNGUP){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "TLKMMI_PHONE_MANUAL_CODE_HUNGUP");
		if(sTlkMmiPhoneCtrl.aclHandle != aclHandle || sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_IDLE){
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus 2: Invalid Operate");
		}else{
			tlkmmi_phone_resetHfCtrl();
		}
	}else{
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_setHfCallStatus: Error Opcode");
		return;
	}
	#endif
}


#if (TLKMMI_PHONE_SCOMGR_METHOD == TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL)
static void tlkmmi_phone_scoConnCB(uint16 aclHandle, uint16 scoHandle, bool isConn)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB");

	if(sTlkMmiPhoneCtrl.aclHandle == 0){
		return;
	}
	if((sTlkMmiPhoneCtrl.aclHandle != 0 && sTlkMmiPhoneCtrl.aclHandle != aclHandle)
		|| (sTlkMmiPhoneCtrl.scoHandle != 0 && sTlkMmiPhoneCtrl.scoHandle != scoHandle)){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: Conflicting SCO connections");
		tlkmmi_phone_resetHfCtrl();
		return;
	}
	if(!isConn){
		tlkmmi_phone_resetHfCtrl();
		return;
	}
	
	if(sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_INCOMING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_MANUAL_CODE_ANSWER");
		if(sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_NONE){
			sTlkMmiPhoneCtrl.hfOpcode = TLKMMI_PHONE_MANUAL_CODE_NONE;
			sTlkMmiPhoneCtrl.optTimer = 0;
		}
		sTlkMmiPhoneCtrl.aclHandle = aclHandle;
		sTlkMmiPhoneCtrl.scoHandle = scoHandle;
	}else if(sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_OUTGOING){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_CALL_STATUS_OUTGOING");
		if(sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_DIAL){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB 0: manual disconn SCO");
			bth_sco_disconn(scoHandle, 0x13);
		}
		tlkmmi_phone_resetHfCtrl();
	}else if(sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_ACTIVE){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_CALL_STATUS_ACTIVE");
		if(sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_ANSWER){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB 1: manual disconn SCO");
			bth_sco_disconn(scoHandle, 0x13);
		}
		tlkmmi_phone_resetHfCtrl();
	}else{
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_scoConnCB: TLKMMI_PHONE_CALL_STATUS_IDLE");
		sTlkMmiPhoneCtrl.aclHandle = aclHandle;
		sTlkMmiPhoneCtrl.scoHandle = scoHandle;
	}
}
static bool tlkmmi_phone_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(sTlkMmiPhoneCtrl.optTimer == 0 || sTlkMmiPhoneCtrl.aclHandle == 0){
		return false;
	}
	
	if(!clock_time_exceed(sTlkMmiPhoneCtrl.optTimer, 600000)){
		return true;
	}
	if(sTlkMmiPhoneCtrl.scoHandle == 0 && sTlkMmiPhoneCtrl.hfOpcode == TLKMMI_PHONE_MANUAL_CODE_ANSWER
		&& sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_ACTIVE){
		uint08 codec = 0;
		uint08 airMode = 0;
		btp_hfp_getCodec(sTlkMmiPhoneCtrl.aclHandle, &codec);
		if(codec == TLKMDI_SCO_CODEC_ID_MSBC) airMode = BTH_SCO_AIRMODE_TRANS;
		else airMode = BTH_SCO_AIRMODE_CVSD;
		bth_sco_connect(sTlkMmiPhoneCtrl.aclHandle, BTH_SCO_LINK_TYPE_ESCO, airMode);
	}else if(sTlkMmiPhoneCtrl.scoHandle != 0 && sTlkMmiPhoneCtrl.hfOpcode != TLKMMI_PHONE_MANUAL_CODE_ANSWER
		&& sTlkMmiPhoneCtrl.hfStatus == TLKMMI_PHONE_CALL_STATUS_ACTIVE){
		bth_sco_disconn(sTlkMmiPhoneCtrl.scoHandle, 0x13);
	}
	tlkmmi_phone_resetHfCtrl();
	
	return false;
}
#endif



#endif //#if (TLKMMI_PHONE_ENABLE)


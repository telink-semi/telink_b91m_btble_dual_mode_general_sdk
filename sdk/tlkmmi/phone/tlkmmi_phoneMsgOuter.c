/********************************************************************************************************
 * @file	tlkmmi_phoneMsgOuter.c
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
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmdi/bt/tlkmdi_bthfphf.h"
#include "tlkmdi/bt/tlkmdi_bthfpag.h"
#include "tlkmmi_phone.h"
#include "tlkmmi_phoneCtrl.h"
#include "tlkmmi_phoneBook.h"
#include "tlkmmi_phoneMsgOuter.h"
#include "tlkmmi_phoneMsgInner.h"

#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkmmi/phone/tlkmmi_phoneAdapt.h"


static tlkapi_timer_t sTlkMmiPhoneTimer;
static uint08 sTlkMmiPhoneCallState   = TLKMMI_THREE_WAY_NONE;
static uint16 sTlkMmiPhoneWaitTimeout = 50;

#if (TLK_MDI_BTHFP_ENABLE)
static bool tlkmmi_phone_timer(tlkapi_timer_t *pTimer, uint32 userArg);
#endif
static void tlkmmi_phone_recvDialCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvHoldCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvRedialCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvActiveCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvRejectCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvHungUpCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvSetVgmCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvSetVgsCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvMuteCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvCreateScoCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvDisconnScoCmdDeal(uint08 *pData, uint08 dataLen);
static void tlkmmi_phone_recvGetPhoneBookDeal(uint08 *pData, uint08 dataLen);


int tlkmmi_phone_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_outerMsgHandler: msgID-%d", msgID);
	if(msgID == TLKPRT_COMM_CMDID_CALL_DIAL){
		tlkmmi_phone_recvDialCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_HOLD){
		tlkmmi_phone_recvHoldCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_REDIAL){
		tlkmmi_phone_recvRedialCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_ACCEPT){
		tlkmmi_phone_recvActiveCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_REJECT){
		tlkmmi_phone_recvRejectCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_HUNGUP){
		tlkmmi_phone_recvHungUpCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_SET_VGS){
		tlkmmi_phone_recvSetVgsCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_SET_VGM){
		tlkmmi_phone_recvSetVgmCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_MUTE){
		tlkmmi_phone_recvMuteCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_CREATE_SCO){
		tlkmmi_phone_recvCreateScoCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_DESTROY_SCO){
		tlkmmi_phone_recvDisconnScoCmdDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_CALL_GET_PHONE_BOOK){
		tlkmmi_phone_recvGetPhoneBookDeal(pData, dataLen);
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}


static void tlkmmi_phone_recvDialCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;
	uint08 numbLen;
	bth_acl_handle_t *pAclHandle;
	bth_sco_handle_t *pScoHandle;
	if(dataLen < 2){
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	numbLen = pData[1];
	if(numbLen == 0 || numbLen+2 > dataLen){
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_dial(aclHandle, (char*)(pData+2), numbLen) == TLK_ENONE){
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_DIAL);
		}else{
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		#if (TLK_MDI_BTHFP_ENABLE)
		uint16 aclHandle = btp_hfp_getAgHandle();
		if(tlkmdi_bthfpag_insertCall(pData+2, numbLen, false) == TLK_ENONE){
			pAclHandle = bth_handle_getConnAcl(aclHandle);
			if (pAclHandle == nullptr) {
				tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvDialCmdDeal: device not exist");
			}
			pScoHandle = bth_handle_searchConnSco(aclHandle);
			if (pScoHandle != nullptr) {
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvDialCmdDeal: three-way processing");
				tlkmmi_phone_adaptInitTimer(&sTlkMmiPhoneTimer, tlkmmi_phone_timer, (uint32)pData+1, 100000);
				tlkmmi_phone_adaptInsertTimer(&sTlkMmiPhoneTimer);
				tlkmmi_phone_timer(&sTlkMmiPhoneTimer, (uint32)pData+1);
			} else {
				if (tlkmdi_bthfpag_createSco(pAclHandle->btaddr) == TLK_ENONE){
					tlkmdi_bthfpag_activeCall(pData+2, numbLen);
				} else {
					tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvDialCmdDeal: failure");
				}
			}
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else{
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
		#endif
	}
}
static void tlkmmi_phone_recvHoldCmdDeal(uint08 *pData, uint08 dataLen)
{
	#if (TLK_MDI_BTHFP_ENABLE)
	int ret;
	uint08 role;
	uint16 aclHandle;
	if(dataLen < 2){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvHoldCmdDeal: failure - param");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HOLD, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}

	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		aclHandle = btp_hfp_getHfHandle();
		if(pData[1] == 0x00){
			ret = tlkmdi_bthfphf_rejectWaitAndKeepActive(aclHandle);
		}else if(pData[1] == 0x01){
			ret = tlkmdi_bthfphf_acceptWaitAndHoldActive(aclHandle);
		}else if(pData[1] == 0x02){
			ret = tlkmdi_bthfphf_hungupActiveAndResumeHold(aclHandle);
		}else{
			ret = -TLK_EPARAM;
		}
		if(ret < 0) ret = -ret;
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvHoldCmdDeal: success");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HOLD, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else{
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvHoldCmdDeal: failure");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HOLD, TLKPRT_COMM_RSP_STATUE_FAILURE, ret, nullptr, 0);
		}
	}else{
		aclHandle = btp_hfp_getAgHandle();
		if(pData[1] == 0x00){
			sTlkMmiPhoneCallState = TLKMMI_THREE_WAY_REJ_WAIT_KEEP_ACTIVE;
			ret = tlkmdi_bthfpag_rejectWaitAndKeepActive(aclHandle);
		}else if(pData[1] == 0x01){
			sTlkMmiPhoneCallState = TLKMMI_THREE_WAY_ACC_WAIT_HOLD_ACTIVE;
			ret = tlkmdi_bthfpag_acceptWaitAndHoldActive(aclHandle);
		}else if(pData[1] == 0x02){
			sTlkMmiPhoneCallState = TLKMMI_THREE_WAY_HUNGUP_ACTIVE_RESUME_HOLD;
			ret = tlkmdi_bthfpag_hungUpActiveAndResumeHold(aclHandle);
		}else{
			ret = -TLK_EPARAM;
		}
		if(ret < 0) ret = -ret;
		if(ret == TLK_ENONE){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvHoldCmdDeal: success");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HOLD, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else{
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvHoldCmdDeal: failure");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HOLD, TLKPRT_COMM_RSP_STATUE_FAILURE, ret, nullptr, 0);
		}
	}
	#endif
}
static void tlkmmi_phone_recvRedialCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;

	if(dataLen < 1){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: failure - param");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_redial(aclHandle) == TLK_ENONE){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: success");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_DIAL);
		}else{
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: failure - reject");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvRedialCmdDeal: failure - role - %d", role);
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REDIAL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
}
static void tlkmmi_phone_recvActiveCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;
	uint08 numbLen;

	if(dataLen < 1){
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	numbLen = pData[1];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_answer(aclHandle) == TLK_ENONE){
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_ANSWER);
		}else{
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		#if (TLK_MDI_BTHFP_ENABLE)
		tlkmdi_bthfpag_activeCall(pData+2, numbLen);
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_ACCEPT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
		#endif
	}
}
static void tlkmmi_phone_recvRejectCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;

	if(dataLen < 1){
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_hungUp(aclHandle) == TLK_ENONE){
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_HUNGUP);
		}else{
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		#if (TLK_MDI_BTHFP_ENABLE)
		if (tlkmdi_bthfpag_hungupCall() == 0) {
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		} else {
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_REJECT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
		}
		#endif

	}
}
static void tlkmmi_phone_recvHungUpCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;

	if(dataLen < 1){
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		uint16 aclHandle = btp_hfphf_getCurHandle();
		if(btp_hfphf_hungUp(aclHandle) == TLK_ENONE){
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
			tlkmmi_phone_setHfManualCode(aclHandle, TLKMMI_PHONE_MANUAL_CODE_HUNGUP);
		}else{
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
	}else{
		#if (TLK_MDI_BTHFP_ENABLE)
		if (tlkmdi_bthfpag_hungupCall() == 0) {
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		} else {
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_HUNGUP, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
		}
		#endif
	}
}
static void tlkmmi_phone_recvSetVgmCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;
	uint08 volume;
	uint08 *pAddr = nullptr;
	uint16 aclHandle;
	uint08 buffer[6];

	if(dataLen < 2){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgmCmdDeal: failure - param");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGM, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	volume = pData[1];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgmCmdDeal: failure - role - %d", role);
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGM, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}else{
		#if (TLK_MDI_BTHFP_ENABLE)
		aclHandle = btp_hfp_getAgHandle();
		pAddr = bth_handle_getBtAddr(aclHandle);
		tmemcpy(buffer, pAddr, 6);
		if(tlkmdi_bthfpag_setVgm(buffer, volume) == TLK_ENONE){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgmCmdDeal_AG: success");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGM, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else{
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgmCmdDeal_AG: failure - reject");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGM, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
		#endif
	}
}
static void tlkmmi_phone_recvSetVgsCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;
	uint08 volume;
	uint08 *pAddr = nullptr;
	uint16 aclHandle;
	uint08 buffer[6];

	if(dataLen < 2){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgsCmdDeal: failure - param");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGS, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role = pData[0];
	volume = pData[1];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgsCmdDeal: failure - role - %d", role);
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGS, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}else{
		#if (TLK_MDI_BTHFP_ENABLE)
		aclHandle = btp_hfp_getAgHandle();
		pAddr = bth_handle_getBtAddr(aclHandle);
		tmemcpy(buffer, pAddr, 6);
		if(tlkmdi_bthfpag_setVgs(buffer, volume) == TLK_ENONE){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgsCmdDeal_AG: success");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGS, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else{
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvSetVgsCmdDeal_AG: failure - reject");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_SET_VGS, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
		#endif
	}
}
static void tlkmmi_phone_recvMuteCmdDeal(uint08 *pData, uint08 dataLen)
{
	uint08 role;
	uint08 micSpk;
	uint08 isEnable;
	uint08 *pAddr = nullptr;
	uint16 aclHandle;
	uint08 buffer[6];

	if(dataLen < 3){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvMuteCmdDeal: failure - param");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_MUTE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFORMAT, nullptr, 0);
		return;
	}
	
	role     = pData[0];
	micSpk   = pData[1];
	isEnable = pData[2];
	if(role == TLKPRT_COMM_CALL_ROLE_CLIENT){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvMuteCmdDeal: failure - role - %d", role);
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_MUTE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}else{
		#if (TLK_MDI_BTHFP_ENABLE)
		aclHandle = btp_hfp_getAgHandle();
		pAddr = bth_handle_getBtAddr(aclHandle);
		tmemcpy(buffer, pAddr, 6);
		if(tlkmdi_bthfpag_sendMute(buffer, micSpk, isEnable) == TLK_ENONE){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvMuteCmdDeal_AG: success");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_MUTE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
		}else{
			tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvMuteCmdDeal_AG: failure - reject");
			tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_MUTE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
		}
		#endif
	}
}
static void tlkmmi_phone_recvCreateScoCmdDeal(uint08 *pData, uint08 dataLen)
{
	#if (TLK_MDI_BTHFP_ENABLE)
	uint08 btAddr[6];
		
	if (dataLen < 6) {
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvCreateScoCmdDeal: Error Param");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_CREATE_SCO, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}
	tlkapi_array(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvCreateScoCmdDeal: pData ", pData, dataLen);
	tmemcpy(btAddr, pData, 6);
	if(tlkmdi_bthfpag_createSco(btAddr) == TLK_ENONE){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvCreateScoCmdDeal_AG: success");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_CREATE_SCO, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvCreateScoCmdDeal_AG: failure - reject");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_CREATE_SCO, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
	#endif
}
static void tlkmmi_phone_recvDisconnScoCmdDeal(uint08 *pData, uint08 dataLen)
{
	#if (TLK_MDI_BTHFP_ENABLE)
	uint08 btAddr[6];
		
	if (dataLen < 6) {
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvDisconnScoCmdDeal: Error Param");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DESTROY_SCO, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EPARAM, nullptr, 0);
		return;
	}

	tmemcpy(btAddr, pData, 6);
	tlkapi_array(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvDisconnScoCmdDeal: pData ", pData, 6);
	if(tlkmdi_bthfag_disconnSco(btAddr) == TLK_ENONE){
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvDisconnScoCmdDeal: success");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DESTROY_SCO, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_recvDisconnScoCmdDeal: failure - reject");
		tlkmmi_phone_sendCommRsp(TLKPRT_COMM_CMDID_CALL_DESTROY_SCO, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
	#endif

}

static void tlkmmi_phone_recvGetPhoneBookDeal(uint08 *pData, uint08 dataLen)
{
#if (TLKBTP_CFG_PBAPCLT_ENABLE)
	uint08 posi;
	uint08 type;
	uint08 sort;
	uint16 offset;
	uint16 number;
	uint16 aclHandle;
	bth_acl_handle_t *pHandle;
	
	if(dataLen < 7){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_debug_recvGetPhoneBookDeal: length error - %d", dataLen);
		return;
	}
		
	aclHandle = btp_pbapclt_getAnyConnHandle(0);
	if(aclHandle == 0){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_debug_recvGetPhoneBookDeal: failure - no device");
		return;
	}
	
	pHandle = bth_handle_getConnAcl(aclHandle);
	if(pHandle == nullptr){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_debug_recvGetPhoneBookDeal: fault - ACL handle not exist");
		return;
	}
	
	tlkapi_array(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_debug_recvGetPhoneBookDeal: start", pData, 7);
	
	posi = pData[0];
	type = pData[1];
	sort = pData[2];
	offset = ((uint16)pData[4]<<8)|pData[3];
	number = ((uint16)pData[6]<<8)|pData[5];
	#if (TLKMMI_PHONE_ENABLE)
	tlkmmi_phone_bookSetParam(posi, type, sort, offset, number);
	tlkmmi_phone_startSyncBook(pHandle->aclHandle, pHandle->btaddr, true);
	#endif
#endif
}


#if (TLK_MDI_BTHFP_ENABLE)
static bool tlkmmi_phone_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	uint16 aclHandle;
	bth_acl_handle_t *pAclHandle = nullptr;

	aclHandle = btp_hfp_getAgHandle();
	pAclHandle = bth_handle_getConnAcl(aclHandle);
	if (pAclHandle == nullptr) {
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_timer: device not exist");
		return false;
	}
	if (sTlkMmiPhoneWaitTimeout != 0) {
		sTlkMmiPhoneWaitTimeout --;
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_timer: sTlkMmiPhoneWaitTimeout %d", sTlkMmiPhoneWaitTimeout);
		if (sTlkMmiPhoneCallState != TLKMMI_THREE_WAY_NONE) {
			if (sTlkMmiPhoneCallState == TLKMMI_THREE_WAY_REJ_WAIT_KEEP_ACTIVE) {
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_timer: TLKMMI_THREE_WAY_REJ_WAIT_KEEP_ACTIVE Success");
			} else if (sTlkMmiPhoneCallState == TLKMMI_THREE_WAY_ACC_WAIT_HOLD_ACTIVE) {
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_timer: TLKMMI_THREE_WAY_ACC_WAIT_HOLD_ACTIVE Success");
			} else {
				tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_timer: TLKMMI_THREE_WAY_HUNGUP_ACTIVE_RESUME_HOLD Success");
			}
			sTlkMmiPhoneWaitTimeout = 50;
			sTlkMmiPhoneCallState = TLKMMI_THREE_WAY_NONE;
			return false;
		} else return true;
	} else return true;
	return false;
}
#endif	// #if (TLK_MDI_BTHFP_ENABLE)

#endif //#if (TLKMMI_PHONE_ENABLE)


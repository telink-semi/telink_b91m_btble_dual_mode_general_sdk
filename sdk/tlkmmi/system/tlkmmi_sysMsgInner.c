/********************************************************************************************************
 * @file	tlkmmi_sysMsgInner.c
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
#if (TLKMMI_SYSTEM_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/misc/tlkmdi_comm.h"
#include "tlkmmi/system/tlkmmi_sys.h"
#include "tlkmmi/system/tlkmmi_sysMsgInner.h"


static int tlkmmi_sys_batteryReportMsgDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_sys_serialSendMsgDeal(uint08 *pData, uint08 dataLen);
#if (TLK_CFG_COMM_ENABLE)
static int tlkmmi_sys_regCommTaskDeal(uint08 *pData, uint08 dataLen);
#endif


/******************************************************************************
 * Function: tlkmmi_sys_sendCommXxx.
 * Descript: send the sys command or Response or Event.
 * Params:
 *     @cmdID[IN]--which command will be to send.
 *     @pData[IN]--The data.
 *     @dataLen[IN]--The data length.
 *     @status[IN]--The status.
 *     @reason[IN]--The reason.
 * Return: Return TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_sys_sendCommCmd(uint08 cmdID, uint08 *pData, uint08 dataLen)
{
#if TLK_CFG_COMM_ENABLE
	return tlkmdi_comm_sendCmd(TLKPRT_COMM_MTYPE_SYS, cmdID, pData, dataLen);
#else
	return -TLK_ENOSUPPORT;
#endif
}
int tlkmmi_sys_sendCommRsp(uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
#if TLK_CFG_COMM_ENABLE
	return tlkmdi_comm_sendRsp(TLKPRT_COMM_MTYPE_SYS, cmdID, status, reason, pData, dataLen);
#else
	return -TLK_ENOSUPPORT;
#endif
}
int tlkmmi_sys_sendCommEvt(uint08 evtID, uint08 *pData, uint08 dataLen)
{
#if TLK_CFG_COMM_ENABLE
	return tlkmdi_comm_sendEvt(TLKPRT_COMM_MTYPE_SYS, evtID, pData, dataLen);
#else
	return -TLK_ENOSUPPORT;
#endif
}



int tlkmmi_sys_innerMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen)
{
	if(msgID == TLKPTI_SYS_MSGID_BATTERY_REPORT){
		return tlkmmi_sys_batteryReportMsgDeal(pData, dataLen);
	}else if(msgID == TLKPTI_SYS_MSGID_SERIAL_SEND){
		int ret;
		ret = tlkmmi_sys_serialSendMsgDeal(pData, dataLen);
		return ret;
	}else if(msgID == TLKPTI_SYS_MSGID_REG_COMM_TASK){
		#if (TLK_CFG_COMM_ENABLE)
		return tlkmmi_sys_regCommTaskDeal(pData, dataLen);
		#endif
	}
		
	return -TLK_ENOSUPPORT;
}

static int tlkmmi_sys_batteryReportMsgDeal(uint08 *pData, uint08 dataLen)
{
	return tlkmmi_sys_sendCommEvt(TLKPRT_COMM_EVTID_SYS_BATTERY, pData, dataLen);
}
static int tlkmmi_sys_serialSendMsgDeal(uint08 *pData, uint08 dataLen)
{
#if (TLK_CFG_COMM_ENABLE)
	if(pData == nullptr || dataLen < TLKPTI_SYS_SERIAL_HEADLEN){
		tlkapi_error(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN,
			"tlkmmi_sys_serialSendMsgDeal: error format dataLen[%d]",
			dataLen);
		return -TLK_EFORMAT;
	}
//	tlkapi_trace(TLKMMI_SYS_DBG_FLAG, TLKMMI_SYS_DBG_SIGN, "tlkmmi_sys_serialSendMsgDeal: ptype[%d]", pData[0]);
	if(pData[0] == TLKPRT_COMM_PTYPE_CMD){
		return tlkmdi_comm_sendCmd(pData[1], pData[2], pData+TLKPTI_SYS_SERIAL_HEADLEN, dataLen-TLKPTI_SYS_SERIAL_HEADLEN);
	}else if(pData[0] == TLKPRT_COMM_PTYPE_RSP){
		return tlkmdi_comm_sendRsp(pData[1], pData[2], pData[3], pData[4], pData+TLKPTI_SYS_SERIAL_HEADLEN, dataLen-TLKPTI_SYS_SERIAL_HEADLEN);
	}else if(pData[0] == TLKPRT_COMM_PTYPE_EVT){
		return tlkmdi_comm_sendEvt(pData[1], pData[2], pData+TLKPTI_SYS_SERIAL_HEADLEN, dataLen-TLKPTI_SYS_SERIAL_HEADLEN);
	}else if(pData[0] == TLKPRT_COMM_PTYPE_DAT){
		return tlkmdi_comm_sendDat(pData[1], ((uint16)pData[3]<<8)|pData[2], pData+TLKPTI_SYS_SERIAL_HEADLEN, dataLen-TLKPTI_SYS_SERIAL_HEADLEN);
	}else{
		return -TLK_ENOSUPPORT;
	}
#else
	return -TLK_ENOSUPPORT;
#endif
}


#if (TLK_CFG_COMM_ENABLE)
static int tlkmmi_sys_regCommTaskDeal(uint08 *pData, uint08 dataLen)
{
	uint08 mtype;
	uint16 taskID;
	mtype = pData[0];
	taskID = ((uint16)pData[2]<<8) | pData[1];
	tlkmdi_comm_regCmdCB(mtype, taskID);	
	return TLK_ENONE;
}
#endif


#endif //#if (TLKMMI_SYSTEM_ENABLE)


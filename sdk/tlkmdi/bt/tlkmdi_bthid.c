/********************************************************************************************************
 * @file	tlkmdi_bthid.c
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
#if (TLK_MDI_BTHID_ENABLE)
#include "tlkmdi/bt/tlkmdi_bthid.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"
#include "tlkstk/bt/btp/hid/btp_hid.h"


#define TLKMDI_BTHID_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_HID << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTHID_DBG_SIGN       "[MDI]"


static uint tlkmdi_bthid_setProtocolCB(uint16 aclHandle, uint08 protoMode);
static uint tlkmdi_bthid_getProtocolCB(uint16 aclHandle, uint08 *pProtoMode);

static uint tlkmdi_bthid_setReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pData, uint16 dataLen);
static uint tlkmdi_bthid_getReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pBuff, uint16 *pBuffLen);

static int tlkmdi_bthid_irqRecvDataCB(uint16 handle, uint08 *pData, uint16 dataLen);


static uint08 sTlkMdiBtHidProtocolMode;


int tlkmdi_bthid_init(void)
{
	sTlkMdiBtHidProtocolMode = BTP_HID_PROTO_BOOT;
    btp_hidd_regCB(tlkmdi_bthid_setReportCB, tlkmdi_bthid_getReportCB,
    	tlkmdi_bthid_setProtocolCB, tlkmdi_bthid_getProtocolCB,
		nullptr, tlkmdi_bthid_irqRecvDataCB);
	
	return TLK_ENONE;
}

int tlkmdi_bthid_connect(uint16 aclHandle)
{
	return btp_hidd_connect(aclHandle);
}

int tlkmdi_bthid_disconn(uint16 aclHandle)
{
	return btp_hidd_disconn(aclHandle);
}


int tlkmdi_bthid_sendData(uint16 aclHandle, uint08 reportID, uint08 reportType, uint08 *pData, uint16 dataLen)
{
    int ret;
	
    ret = btp_hidd_sendData(aclHandle, reportID, reportType, pData, dataLen);
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendData error");
		return ret;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendData");
	return TLK_ENONE;
}
int tlkmdi_bthid_sendDataWithoutReportID(uint16 aclHandle, uint08 reportType, uint08 *pData, uint16 dataLen)
{
    int ret;
	
    ret = btp_hidd_sendDataWithoutReportID(aclHandle, reportType, pData, dataLen);
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendDataWithoutReportID error");
		return ret;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_sendDataWithoutReportID");
	return TLK_ENONE;
}

static uint tlkmdi_bthid_setProtocolCB(uint16 aclHandle, uint08 protoMode)
{
	if(protoMode > 2){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setProtocolCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setProtocolCB");
	sTlkMdiBtHidProtocolMode = protoMode;
	return BTP_HID_HSHK_SUCCESS;
}
static uint tlkmdi_bthid_getProtocolCB(uint16 aclHandle, uint08 *pProtoMode)
{
	if(pProtoMode == nullptr){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getProtocolCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getProtocolCB");
	*pProtoMode = sTlkMdiBtHidProtocolMode;
	return BTP_HID_HSHK_SUCCESS;
}

static uint tlkmdi_bthid_setReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pData, uint16 dataLen)
{
	if(pData == NULL || dataLen == 0){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}
	//Bond with gcBtpSdpHidReportMap.
	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_setReportCB");
	return BTP_HID_HSHK_SUCCESS;
}
static uint tlkmdi_bthid_getReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pBuff, uint16 *pBuffLen)
{
	uint16 buffLen;
	
	if(pBuff == NULL || pBuffLen == nullptr || (*pBuffLen) == 0){
		tlkapi_error(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getReportCB: invalid param");
		return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
	}

	tlkapi_trace(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_getReportCB");
	buffLen = *pBuffLen;
	//Bond with gcBtpSdpHidReportMap.
	if(reportID == 0x01){ //keyboard
		if(buffLen > 20) buffLen = 20;
		tmemset(pBuff, 0, buffLen);
	}else if(reportID == 0x02){
		if(buffLen > 2) buffLen = 2;
		tmemset(pBuff, 0, buffLen);
	}
	
	return BTP_HID_HSHK_SUCCESS;
}
static int tlkmdi_bthid_irqRecvDataCB(uint16 handle, uint08 *pData, uint16 dataLen)
{
	tlkapi_array(TLKMDI_BTHID_DBG_FLAG, TLKMDI_BTHID_DBG_SIGN, "tlkmdi_bthid_irqRecvDataCB", pData, dataLen);
	return TLK_ENONE;
}




#endif //#if (TLK_MDI_BTHID_ENABLE)


/********************************************************************************************************
 * @file	btp_ptsHid.c
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
#if (TLK_STK_BTP_ENABLE)
#include "../../bth/bth_stdio.h"
#include "../btp_stdio.h"
#if (TLKBTP_CFG_PTSHID_ENABLE)
#include "../../bth/bth_l2cap.h"
#include "../../bth/bth_signal.h"
#include "../btp_adapt.h"
#include "../hid/btp_hid.h"
#include "btp_ptsHid.h"


#define BTP_PTSHID_REPORT_ID_KEYBOARD_INPUT 1 //!< Keyboard input report ID
#define BTP_PTSHID_REPORT_ID_CONSUMER_INPUT 2 //!< Consumer Control input report ID


static uint btp_ptshid_setProtocolCB(uint16 aclHandle, uint08 protoMode);
static uint btp_ptshid_getProtocolCB(uint16 aclHandle, uint08 *pProtoMode);
static uint btp_ptshid_setReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pData, uint16 dataLen);
static uint btp_ptshid_getReportCB(uint16 aclHandle,
                                   uint08 reportType,
                                   uint08 reportID,
                                   uint08 *pBuff,
                                   uint16 *pBuffLen);
static int btp_ptshid_irqRecvDataCB(uint16 handle, uint08 *pData, uint16 dataLen);


#define BTP_PTSHID_MAJOR_SERVICE_CLASS (BTH_COD_SERVICE_CLASS_AUDIO | BTH_COD_SERVICE_CLASS_RENDER)
#define BTP_PTSHID_MAJOR_DEVICE_CLASS  (BTH_COD_MAJOR_CLASS_PERIPHERAL)
#define BTP_PTSHID_MINOR_DEVICE_CLASS  (BTH_COD_PERIPHERAL_COMBO | BTH_COD_PERIPHERAL_JOYSTICK)
#define BTP_PTSHID_DEVICE_CLASS                                       \
    (BTP_PTSHID_MAJOR_SERVICE_CLASS | BTP_PTSHID_MAJOR_DEVICE_CLASS | \
     BTP_PTSHID_MINOR_DEVICE_CLASS) // 0x240404//0x5a020c


static uint08 sBtpPtsHidIsSuppProto;
static uint08 sBtpPtsHidProtocolMode;
static uint08 sBtpPtsHidInputLength     = 4;
static uint08 sBtpPtsHidOutputLength    = 20;
static uint08 sBtpPtsHidFeatureLength   = 20;
static uint08 sBtpPtsHidInputData[4]    = { 0xA1, 0xA2, 0xA3, 0xA4 };
static uint08 sBtpPtsHidOutputData[20]  = { 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28 };
static uint08 sBtpPtsHidFeatureData[20] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };


int btp_ptshid_init(void)
{
    sBtpPtsHidIsSuppProto  = false;
    sBtpPtsHidProtocolMode = BTP_HID_PROTO_REPORT;
    //	btp_hid_setRtnMode(BTH_L2CAP_SIG_RTNMODE_ENHANCE);
    btp_hidd_regCB(btp_ptshid_setReportCB,
                   btp_ptshid_getReportCB,
                   btp_ptshid_setProtocolCB,
                   btp_ptshid_getProtocolCB,
                   nullptr,
                   btp_ptshid_irqRecvDataCB);

    return TLK_ENONE;
}

int btp_ptshid_connect(uint16 aclHandle)
{
    return btp_hidd_connect(aclHandle);
}

int btp_ptshid_disconn(uint16 aclHandle)
{
    return btp_hidd_disconn(aclHandle);
}

void btp_ptshid_destroy(uint16 handle)
{
    btp_hid_destroy(handle);
}

void btp_ptshid_setClassOfDevice(void)
{
    bth_hci_sendWriteClassOfDeviceCmd(BTP_PTSHID_DEVICE_CLASS);
}

int btp_ptshid_sendData(uint16 aclHandle, uint08 reportID, uint08 *pData, uint16 dataLen)
{
    int ret;
    uint08 rtype;

    if (reportID == BTP_PTSHID_INPUT_REPORT_ID) {
        rtype = BTP_HID_DATA_RTYPE_INPUT;
    } else if (reportID == BTP_PTSHID_OUTPUT_REPORT_ID) {
        rtype = BTP_HID_DATA_RTYPE_OUTPUT;
    } else if (reportID == BTP_PTSHID_FEATURE_REPORT_ID) {
        rtype = BTP_HID_DATA_RTYPE_FEATURE;
    } else {
        return -TLK_EPARAM;
    }

    ret = btp_hidd_sendData(aclHandle, reportID, rtype, pData, dataLen);
    if (ret != TLK_ENONE) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_sendData error");
        return ret;
    }
    tlkapi_trace(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_sendData");
    return TLK_ENONE;
}
int btp_ptshid_sendDataWithoutReportID(uint16 aclHandle, uint08 reportType, uint08 *pData, uint16 dataLen)
{
    int ret;

    ret = btp_hidd_sendDataWithoutReportID(aclHandle, reportType, pData, dataLen);
    if (ret != TLK_ENONE) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_sendData error");
        return ret;
    }
    tlkapi_trace(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_sendDataWithoutReportID");
    return TLK_ENONE;
}


static bool btp_ptshid_isValid(uint08 repotType, uint08 reportID)
{
    if ((repotType == BTP_HID_DATA_RTYPE_INPUT && reportID == BTP_PTSHID_INPUT_REPORT_ID) ||
        (repotType == BTP_HID_DATA_RTYPE_OUTPUT && reportID == BTP_PTSHID_OUTPUT_REPORT_ID) ||
        (repotType == BTP_HID_DATA_RTYPE_FEATURE && reportID == BTP_PTSHID_FEATURE_REPORT_ID)) {
        return true;
    }
    return false;
}
static uint btp_ptshid_setProtocolCB(uint16 aclHandle, uint08 protoMode)
{
    if (protoMode > 2) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_setProtocolCB: invalid param");
        return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
    }
    if (!sBtpPtsHidIsSuppProto) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_setProtocolCB: unsupported");
        return BTP_HID_HSHK_ERR_UNSUPPORTED_REQUEST;
    }
    tlkapi_trace(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_setProtocolCB");
    sBtpPtsHidProtocolMode = protoMode;
    return BTP_HID_HSHK_SUCCESS;
}
static uint btp_ptshid_getProtocolCB(uint16 aclHandle, uint08 *pProtoMode)
{
    if (pProtoMode == nullptr) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_getProtocolCB: invalid param");
        return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
    }
    if (!sBtpPtsHidIsSuppProto) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_getProtocolCB: unsupported");
        return BTP_HID_HSHK_ERR_UNSUPPORTED_REQUEST;
    }
    tlkapi_trace(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_getProtocolCB");
    *pProtoMode = sBtpPtsHidProtocolMode;
    return BTP_HID_HSHK_SUCCESS;
}

static uint btp_ptshid_setReportCB(uint16 aclHandle, uint08 reportType, uint08 reportID, uint08 *pData, uint16 dataLen)
{
    if (!btp_ptshid_isValid(reportType, reportID)) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_setReportCB: invalid report id");
        return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
    }

    tlkapi_trace(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_setReportCB");
    if (reportID == BTP_PTSHID_INPUT_REPORT_ID) {
        return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
        //		if(dataLen > 4) return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
        //		tmemcpy(sBtpPtsHidInputData, pData, dataLen);
    } else if (reportID == BTP_PTSHID_OUTPUT_REPORT_ID) {
        if (dataLen > 20) {
            return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
        }
        tmemcpy(sBtpPtsHidOutputData, pData, dataLen);
    } else if (reportID == BTP_PTSHID_FEATURE_REPORT_ID) {
        return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
        //		if(dataLen > 20) return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
        //		tmemcpy(sBtpPtsHidFeatureData, pData, dataLen);
    } else {
        return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
    }

    return BTP_HID_HSHK_SUCCESS;
}
static uint btp_ptshid_getReportCB(uint16 aclHandle,
                                   uint08 reportType,
                                   uint08 reportID,
                                   uint08 *pBuff,
                                   uint16 *pBuffLen)
{
    uint16 buffLen;
    uint08 dataLen;
    uint08 *pData;

    if (pBuff == nullptr || pBuffLen == nullptr || (*pBuffLen) == 0) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_getReportCB: invalid param");
        return BTP_HID_HSHK_ERR_INVALID_PARAMETER;
    }

    if (!btp_ptshid_isValid(reportType, reportID)) {
        tlkapi_error(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_getReportCB: invalid report id");
        return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
    }

    if (reportID == BTP_PTSHID_INPUT_REPORT_ID) {
        pData   = sBtpPtsHidInputData;
        dataLen = sBtpPtsHidInputLength;
    } else if (reportID == BTP_PTSHID_OUTPUT_REPORT_ID) {
        pData   = sBtpPtsHidOutputData;
        dataLen = sBtpPtsHidOutputLength;
    } else if (reportID == BTP_PTSHID_FEATURE_REPORT_ID) {
        pData   = sBtpPtsHidFeatureData;
        dataLen = sBtpPtsHidFeatureLength;
    } else {
        return BTP_HID_HSHK_ERR_INVALID_REPORT_ID;
    }

    buffLen = *pBuffLen;
    tlkapi_trace(BTP_PTSHID_DBG_FLAG,
                 BTP_PTSHID_DBG_SIGN,
                 "btp_ptshid_getReportCB: reportType[%d], reportID[%d]",
                 reportType,
                 reportID);

    if (buffLen > dataLen) {
        buffLen = dataLen;
    }
    tmemcpy(pBuff, pData, buffLen);
    *pBuffLen = buffLen;

    return BTP_HID_HSHK_SUCCESS;
}
static int btp_ptshid_irqRecvDataCB(uint16 handle, uint08 *pData, uint16 dataLen)
{
    tlkapi_array(BTP_PTSHID_DBG_FLAG, BTP_PTSHID_DBG_SIGN, "btp_ptshid_irqRecvDataCB", pData, dataLen);
    return TLK_ENONE;
}


#endif // #if (TLKBTP_CFG_PTSHID_ENABLE)
#endif // #if (TLK_STK_BTP_ENABLE)

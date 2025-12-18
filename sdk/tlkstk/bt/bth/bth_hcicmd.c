/********************************************************************************************************
 * @file	bth_hcicmd.c
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
#include "tlkstk/bt/btc/btc_hci.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/bth/bth_hcicmd.h"


#define BTH_HCICMD_DBG_FLAG ((TLK_MAJOR_DBGID_BTH << 24) | (TLK_MINOR_DBGID_BTH_CMD << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTH_HCICMD_DBG_SIGN nullptr

extern int tlkbt_hci_sendH2cCmd(uint16 opcode, uint08 *pData, uint08 dataLen);


/******************************************************************************
 * Function: bth_hci_sendResetCmd
 * Descript: Reset controller.
 * Params:

 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendResetCmd(void)
{
    return tlkbt_hci_sendH2cCmd(HCI_RESET_CMD_OPCODE, 0, 0);
}

/******************************************************************************
 * Function: bth_hci_sendInquiryCmd
 * Descript: Start to Inquiry.
 * Params:
 *        @period[IN]--The inquiry time.
 *        @numRsp[IN]--The num of response.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
// period = n*1.28(n=0x01~0x30)
int bth_hci_sendInquiryCmd(uint08 period, uint08 numRsp)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendInquiryCmd");

    buffLen           = 0;
    buffer[buffLen++] = 0x33; // LAP
    buffer[buffLen++] = 0x8b;
    buffer[buffLen++] = 0x9e;
    buffer[buffLen++] = period; // Inquiry_Length: Maximum amount of time specified before the Inquiry is halted.
    buffer[buffLen++] = numRsp; // Num_Responses
    return tlkbt_hci_sendH2cCmd(HCI_INQ_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendInquiryCancelCmd
 * Descript: Cancel Inquiry.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendInquiryCancelCmd(void)
{
    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendInquiryCancelCmd");
    return tlkbt_hci_sendH2cCmd(HCI_INQ_CANCEL_CMD_OPCODE, nullptr, 0);
}

/******************************************************************************
 * Function: bth_hci_sendInquiryCancelCmd
 * Descript: Cancel Inquiry.
 * Params:
 *        @period[IN]--The inquiry time.
 *        @numRsp[IN]--The num of response.
 *        @maxPeriod[IN]--The maximum time.
 *        @minPeriod[IN]--The minimum time.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendPeriodicInquiryCmd(uint08 period, uint08 numRsp, uint16 maxPeriod, uint16 minPeriod)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendPeriodicInquiryCmd");

    buffLen           = 0;
    buffer[buffLen++] = (maxPeriod & 0x00FF); // Packet_Type
    buffer[buffLen++] = (maxPeriod & 0xFF00) >> 8;
    buffer[buffLen++] = (minPeriod & 0x00FF); // Packet_Type
    buffer[buffLen++] = (minPeriod & 0xFF00) >> 8;
    buffer[buffLen++] = 0x33;                 // LAP
    buffer[buffLen++] = 0x8b;
    buffer[buffLen++] = 0x9e;
    buffer[buffLen++] = period; // Inquiry_Length: Maximum amount of time specified before the Inquiry is halted.
    buffer[buffLen++] = numRsp; // Num_Responses
    return tlkbt_hci_sendH2cCmd(HCI_PER_INQ_MODE_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendPeriodicInquiryCancelCmd
 * Descript: Cancel periodic Inquiry.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendPeriodicInquiryCancelCmd(void)
{
    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendPeriodicInquiryCancelCmd");
    return tlkbt_hci_sendH2cCmd(HCI_EXIT_PER_INQ_MODE_CMD_OPCODE, nullptr, 0);
}

/******************************************************************************
 * Function: bth_hci_sendCreateConnectCmd
 * Descript: Create a connection.
 * Params:
 *        @mac[IN]--The bt address.
 *        @pktType[IN]--The packet type.
 *        @mode[IN]--The mode.
 *        @clkOffs[IN]--The clock offset.
 *        @allowRoleSwitch[IN]--allow role switch.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendCreateConnectCmd(uint08 mac[6], uint16 pktType, uint08 mode, uint16 clkOffs, uint08 allowRoleSwitch)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendCreateConnectCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6);      // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = (pktType & 0x00FF); // Packet_Type
    buffer[buffLen++] = (pktType & 0xFF00) >> 8;
    buffer[buffLen++] = mode;               // Page_Scan_Repetition_Mode
    buffer[buffLen++] = 0x00;               // Reserved
    buffer[buffLen++] = (clkOffs & 0x00FF); // Clock_Offset
    buffer[buffLen++] = (clkOffs & 0xFF00) >> 8;
    buffer[buffLen++] = allowRoleSwitch;    // Allow_Role_Switch
    return tlkbt_hci_sendH2cCmd(HCI_CREATE_CON_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendCancelConnectCmd
 * Descript: Cancel the connection.
 * Params:
 *        @mac[IN]--The bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendCancelConnectCmd(uint08 mac[6])
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendCancelConnectCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    return tlkbt_hci_sendH2cCmd(HCI_CREATE_CON_CANCEL_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendDisconnCmd
 * Descript: Disconnect a connection.
 * Params:
 *        @handle[IN]--The connected acl handle.
 *        @reason[IN]--Disconnect reason.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendDisconnCmd(uint16 handle, uint08 reason)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendDisconnCmd");

    buffLen           = 0;
    buffer[buffLen++] = (handle & 0x00FF); // Connection_Handle
    buffer[buffLen++] = (handle & 0xFF00) >> 8;
    buffer[buffLen++] = reason;            // Reason
    return tlkbt_hci_sendH2cCmd(HCI_DISCONNECT_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendRejectConnReqCmd
 * Descript: Reject a connection.
 * Params:
 *        @handle[IN]--The connected acl handle.
 *        @reason[IN]--Disconnect reason.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendRejectConnReqCmd(uint08 mac[6], uint08 reason)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendRejectConnReqCmd: reason-%d", reason);

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = reason;
    return tlkbt_hci_sendH2cCmd(HCI_REJECT_CON_REQ_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendAcceptConnReqCmd
 * Descript: Accept a connection.
 * Params:
 *        @handle[IN]--The connected acl handle.
 *        @role[IN]--The role of aclHandle.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendAcceptConnReqCmd(uint08 mac[6], uint08 role)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendAcceptConnReqCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = role; // 0=Become the Master for this connection; 1-Remain the Slave for this connection.
    return tlkbt_hci_sendH2cCmd(HCI_ACCEPT_CON_REQ_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendLinkkeyReqReplyCmd
 * Descript: Reply the link key request.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @pLinkkey[IN]--The link key.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendLinkkeyReqReplyCmd(uint08 mac[6], uint08 *pLinkkey)
{
    uint08 buffLen;
    uint08 buffer[32];

    tlkapi_array(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendLinkkeyReqReplyCmd - linkkey", pLinkkey, 16);

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    tmemcpy(&buffer[buffLen], pLinkkey, 16);
    buffLen += 16;
    return tlkbt_hci_sendH2cCmd(HCI_LK_REQ_REPLY_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendLinkkeyReqNegReplyCmd
 * Descript: Reply negative to the link key request.
 * Params:
 *        @mac[IN]--The Bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendLinkkeyReqNegReplyCmd(uint08 mac[6])
{
    uint08 buffLen;
    uint08 buffer[32];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendLinkkeyReqNegReplyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    return tlkbt_hci_sendH2cCmd(HCI_LK_REQ_NEG_REPLY_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendPinCodeReqReplyCmd
 * Descript: Reply  pin code to the pin code request.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @pPinCode[IN]--The pin code.
 *        @pinLen[In]--The pin code length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendPinCodeReqReplyCmd(uint08 mac[6], uint08 *pPinCode, uint08 pinlen)
{
    uint08 buffLen;
    uint08 buffer[32] = { 0 };

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendPinCodeReqReplyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = pinlen;
    if (pinlen > 0x10) {
        pinlen = 0x10;
    }
    if (pinlen != 0) {
        tmemcpy(&buffer[buffLen], pPinCode, pinlen);
    }
    buffLen += 16;
    return tlkbt_hci_sendH2cCmd(HCI_PIN_CODE_REQ_REPLY_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendPinCodeReqNegReplyCmd
 * Descript: Reply negative to the pin code request.
 * Params:
 *        @mac[IN]--The Bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendPinCodeReqNegReplyCmd(uint08 mac[6])
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendPinCodeReqNegReplyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    return tlkbt_hci_sendH2cCmd(HCI_PIN_CODE_REQ_NEG_REPLY_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendAuthenticationReqCmd
 * Descript: start authenticate.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendAuthenticationReqCmd(uint16 aclHandle)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendAuthenticationReqCmd");

    buffLen           = 0;
    buffer[buffLen++] = (aclHandle & 0x00FF); // Connection_Handle
    buffer[buffLen++] = (aclHandle & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_AUTH_REQ_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendSetConnectEncryptReqCmd
 * Descript: start encrypt.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 *        @encryptEnable[IN]--is encrypt enable.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendSetConnectEncryptReqCmd(uint16 aclHandle, uint08 encrytEnable)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendSetConnectEncryptReqCmd");

    buffLen           = 0;
    buffer[buffLen++] = (aclHandle & 0x00FF); // Connection_Handle
    buffer[buffLen++] = (aclHandle & 0xFF00) >> 8;
    buffer[buffLen++] = encrytEnable;         // Reason
    return tlkbt_hci_sendH2cCmd(HCI_SET_CON_ENC_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendRemoteNameReqCmd
 * Descript: Request peer device name.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @mode[IN]--The mode.
 *        @clockoffset[IN]--The clock offset.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendRemoteNameReqCmd(uint08 mac[6], uint08 mode, uint16 clockOffset)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendRemoteNameReqCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6);          // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = mode;                   // Page_Scan_Repetition_Mode
    buffer[buffLen++] = 0x00;                   // Reserved
    buffer[buffLen++] = (clockOffset & 0x00FF); // Clock_Offset
    buffer[buffLen++] = (clockOffset & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_REM_NAME_REQ_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendRemoteNameReqCancelCmd
 * Descript: Cancel request peer device name.
 * Params:
 *        @mac[IN]--The Bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendRemoteNameReqCancelCmd(uint08 mac[6])
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendRemoteNameReqCancelCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    return tlkbt_hci_sendH2cCmd(HCI_REM_NAME_REQ_CANCEL_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendCreateSyncConnReqCmd
 * Descript: Create a sync connection.
 * Params:
 *        @aclHandle[IN]--The acl link handle.
 *        @txBandwidth[IN]--The transfer bandwidth.
 *        @rxBandwidth[IN]--The receive bandwidth.
 *        @maxLatency[IN]--The maximum latency.
 *        @voiceSetting[IN]--The voice setting.
 *        @rtnEffort[IN]--Theretransmission effeort.
 *        @pktType[IN]--The packet type.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendCreateSyncConnReqCmd(uint16 aclHandle,
                                     uint32 txBandwidth,
                                     uint32 rxBandwidth,
                                     uint16 maxLatency,
                                     uint16 voiceSetting,
                                     uint08 rtnEffort,
                                     uint16 pktType)
{
    uint08 buffLen;
    uint08 buffer[32];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendSetupSyncConnReqCmd");

    buffLen           = 0;
    buffer[buffLen++] = (aclHandle & 0x00FF);       // Connection_Handle
    buffer[buffLen++] = (aclHandle & 0xFF00) >> 8;
    buffer[buffLen++] = (txBandwidth & 0x000000FF); // Transmit_Bandwidth
    buffer[buffLen++] = (txBandwidth & 0x0000FF00) >> 8;
    buffer[buffLen++] = (txBandwidth & 0x00FF0000) >> 16;
    buffer[buffLen++] = (txBandwidth & 0xFF000000) >> 24;
    buffer[buffLen++] = (rxBandwidth & 0x000000FF); // Receive_Bandwidth
    buffer[buffLen++] = (rxBandwidth & 0x0000FF00) >> 8;
    buffer[buffLen++] = (rxBandwidth & 0x00FF0000) >> 16;
    buffer[buffLen++] = (rxBandwidth & 0xFF000000) >> 24;
    buffer[buffLen++] = (maxLatency & 0x00FF);   // Max_Latency
    buffer[buffLen++] = (maxLatency & 0xFF00) >> 8;
    buffer[buffLen++] = (voiceSetting & 0x00FF); // Voice_Settings
    buffer[buffLen++] = (voiceSetting & 0xFF00) >> 8;
    buffer[buffLen++] = rtnEffort;               // Retransmission_Effort
    buffer[buffLen++] = (pktType & 0x00FF);      // Packet_Type
    buffer[buffLen++] = (pktType & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_SETUP_SYNC_CON_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendAcceptSyncConnReqCmd
 * Descript: Accept a sync connection.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @txBandwidth[IN]--The transfer bandwidth.
 *        @rxBandwidth[IN]--The receive bandwidth.
 *        @maxLatency[IN]--The maximum latency.
 *        @voiceSetting[IN]--The voice setting.
 *        @rtnEffort[IN]--Theretransmission effeort.
 *        @pktType[IN]--The packet type.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendAcceptSyncConnReqCmd(uint08 mac[6],
                                     uint32 txBandwidth,
                                     uint32 rxBandwidth,
                                     uint16 maxLatency,
                                     uint16 voiceSetting,
                                     uint08 rtnEffort,
                                     uint16 pktType)
{
    uint08 buffLen;
    uint08 buffer[32];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendAcceptSyncConnReqCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6);              // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = (txBandwidth & 0x000000FF); // Transmit_Bandwidth
    buffer[buffLen++] = (txBandwidth & 0x0000FF00) >> 8;
    buffer[buffLen++] = (txBandwidth & 0x00FF0000) >> 16;
    buffer[buffLen++] = (txBandwidth & 0xFF000000) >> 24;
    buffer[buffLen++] = (rxBandwidth & 0x000000FF); // Receive_Bandwidth
    buffer[buffLen++] = (rxBandwidth & 0x0000FF00) >> 8;
    buffer[buffLen++] = (rxBandwidth & 0x00FF0000) >> 16;
    buffer[buffLen++] = (rxBandwidth & 0xFF000000) >> 24;
    buffer[buffLen++] = (maxLatency & 0x00FF);   // Max_Latency
    buffer[buffLen++] = (maxLatency & 0xFF00) >> 8;
    buffer[buffLen++] = (voiceSetting & 0x00FF); // Voice_Settings
    buffer[buffLen++] = (voiceSetting & 0xFF00) >> 8;
    buffer[buffLen++] = rtnEffort;               // Retransmission_Effort
    buffer[buffLen++] = (pktType & 0x00FF);      // Packet_Type
    buffer[buffLen++] = (pktType & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_ACCEPT_SYNC_CON_REQ_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendRejectSyncConnReqCmd
 * Descript: Reject a sync connection.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @reason[IN]--The reject reason.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendRejectSyncConnReqCmd(uint08 mac[6], uint08 reason)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendRejectSyncConnReq");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = reason;        // Retransmission_Effort
    return tlkbt_hci_sendH2cCmd(HCI_REJECT_SYNC_CON_REQ_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendIOCapReqReplyCmd
 * Descript: Reply a IO capability request.
 * Params:
 *        @mac[IN]--The Bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendIOCapReqReplyCmd(uint08 mac[6])
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendIOCapReqReplyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = 3;             // IO_Capability
    buffer[buffLen++] = 0;             // OOB_Data_Present
    buffer[buffLen++] = 4;             // Authentication_Requirements
    return tlkbt_hci_sendH2cCmd(HCI_IO_CAP_REQ_REPLY_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendQosSetupCmd
 * Descript: The HCI_QoS_Setup command is used to specify Quality of Service
			 parameters for a Connection_Handle.
 * Params:
 *        @aclHandle[IN]--The acl link handle. Refer to SPEC v5.2 P1085
 *        @unused[IN]-- The value shall be used by the Host.
 *        @serviceType[IN]--The level of service required.
 *        @tokenRate[IN]--The average data rate with which the application transmits data.
 *        @peakBandwidth[IN]--The value limits how fast packets from applications may be sent back-to-back.
 *        @latency[IN]--The maximum acceptable delay of an L2CAP packet to the air-interface.
 *        @delayVariation[IN]--The value of this field is the difference, in microseconds, between the
							   maximum and minimum possible delay of an L2CAP SDU between two L2CAP peers.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendQosSetupCmd(uint16 aclHandle,
							uint08 unused,
							uint08 serviceType,
							uint32 tokenRate,
							uint32 peakBandwidth,
							uint32 latency,
							uint32 delayVariation)
{
    uint08 buffLen;
    uint08 buffer[32];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendQosSetupCmd");

    buffLen = 0;
    buffer[buffLen++] = (aclHandle & 0x00FF);       		// Connection_Handle
    buffer[buffLen++] = (aclHandle & 0xFF00) >> 8;
    buffer[buffLen++] = unused;		    					// Unused
    buffer[buffLen++] = serviceType;						// Service_Type
    buffer[buffLen++] = (tokenRate & 0x000000FF); 			// Token_Rate
    buffer[buffLen++] = (tokenRate & 0x0000FF00) >> 8;
    buffer[buffLen++] = (tokenRate & 0x00FF0000) >> 16;
    buffer[buffLen++] = (tokenRate & 0xFF000000) >> 24;
    buffer[buffLen++] = (peakBandwidth & 0x000000FF); 		// Peak_Bandwidth
    buffer[buffLen++] = (peakBandwidth & 0x0000FF00) >> 8;
    buffer[buffLen++] = (peakBandwidth & 0x00FF0000) >> 16;
    buffer[buffLen++] = (peakBandwidth & 0xFF000000) >> 24;
    buffer[buffLen++] = (latency & 0x000000FF); 			// Latency
    buffer[buffLen++] = (latency & 0x0000FF00) >> 8;
    buffer[buffLen++] = (latency & 0x00FF0000) >> 16;
    buffer[buffLen++] = (latency & 0xFF000000) >> 24;
    buffer[buffLen++] = (delayVariation & 0x000000FF); 		// Delay_Variation
    buffer[buffLen++] = (delayVariation & 0x0000FF00) >> 8;
    buffer[buffLen++] = (delayVariation & 0x00FF0000) >> 16;
    buffer[buffLen++] = (delayVariation & 0xFF000000) >> 24;

    return tlkbt_hci_sendH2cCmd(HCI_QOS_SETUP_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendUsrConfirmReqReplyCmd
 * Descript: Send a reply user confirm request.
 * Params:
 *        @mac[IN]--The Bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendUsrConfirmReqReplyCmd(uint08 mac[6])
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendUsrConfirmReqReplyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    return tlkbt_hci_sendH2cCmd(HCI_USER_CFM_REQ_REPLY_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendUsrConfirmReqNegReplyCmd
 * Descript: Send a negative reply user confirm request.
 * Params:
 *        @mac[IN]--The Bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendUsrConfirmReqNegReplyCmd(uint08 mac[6])
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendUsrConfirmReqNegReplyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    return tlkbt_hci_sendH2cCmd(HCI_USER_CFM_REQ_NEG_REPLY_CMD_OPCODE, buffer, buffLen);
}


/******************************************************************************
 * Function: bth_hci_sendSniffModeCmd
 * Descript: start enter into sniff mode request.
 * Params:
 *        @connHandle[IN]--The connection handle.
 *        @maxInterval[IN]--The maximum interval.
 *        @minInterval[IN]--The minimum interval.
 *        @attempt[IN]--The sniff attempt times.
 *        @timeout[IN]--The sniff time.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendSniffModeCmd(uint16 connHandle, uint16 maxInterval, uint16 minInterval, uint16 attempt, uint16 timeout)
{
    uint08 buffLen;
    uint08 buffer[16];

    //	tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendSniffModeCmd");

    buffLen           = 0;
    buffer[buffLen++] = (connHandle & 0x00FF);  // Connection_Handle
    buffer[buffLen++] = (connHandle & 0xFF00) >> 8;
    buffer[buffLen++] = (maxInterval & 0x00FF); // Sniff_Max_Interval
    buffer[buffLen++] = (maxInterval & 0xFF00) >> 8;
    buffer[buffLen++] = (minInterval & 0x00FF); // Sniff_Min_Interval
    buffer[buffLen++] = (minInterval & 0xFF00) >> 8;
    buffer[buffLen++] = (attempt & 0x00FF);     // Sniff_Attempt
    buffer[buffLen++] = (attempt & 0xFF00) >> 8;
    buffer[buffLen++] = (timeout & 0x00FF);     // Sniff_Timeout
    buffer[buffLen++] = (timeout & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_SNIFF_MODE_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendExitSniffModeCmd
 * Descript: start exit sniff mode request.
 * Params:
 *        @connHandle[IN]--The connection handle.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendExitSniffModeCmd(uint16 connHandle)
{
    uint08 buffLen;
    uint08 buffer[16];

    //	tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendExitSniffModeCmd");

    buffLen           = 0;
    buffer[buffLen++] = (connHandle & 0x00FF); // Connection_Handle
    buffer[buffLen++] = (connHandle & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_EXIT_SNIFF_MODE_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendRoleDiscoveryCmd
 * Descript: Start role discovery.
 * Params:
 *        @connHandle[IN]--The connection handle.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendRoleDiscoveryCmd(uint16 connHandle)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendRoleDiscoveryCmd");

    buffLen           = 0;
    buffer[buffLen++] = (connHandle & 0x00FF); // Connection_Handle
    buffer[buffLen++] = (connHandle & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_ROLE_DISCOVERY_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendSwitchRoleReqCmd
 * Descript: Start role switch.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @role[IN]--The link role.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendSwitchRoleReqCmd(uint08 mac[6], uint08 role)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendSwitchRoleReqCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = role;          // Role
    return tlkbt_hci_sendH2cCmd(HCI_SWITCH_ROLE_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteLinkPolicy
 * Descript: Set the link governor policy.
 * Params:
 *        @connHandle[IN]--The acl handle.
 *        @policySettings[IN]--The link policy.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteLinkPolicy(uint16 connHandle, uint16 policySettings)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteLinkPolicy");

    buffLen           = 0;
    buffer[buffLen++] = (connHandle & 0x00FF);     // Connection_Handle
    buffer[buffLen++] = (connHandle & 0xFF00) >> 8;
    buffer[buffLen++] = (policySettings & 0x00FF); // Link_Policy_Settings
    buffer[buffLen++] = (policySettings & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_WR_LINK_POL_STG_CMD_OPCODE, buffer, buffLen);
}


/******************************************************************************
 * Function: bth_hci_sendReadStoredLinkkeyCmd
 * Descript: Read the link key.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @readAll[IN]--is read all.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendReadStoredLinkkeyCmd(uint08 mac[6], uint08 readAll)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendReadStoredLinkkeyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR
    buffLen += 6;
    buffer[buffLen++] = readAll;       // Read_All
    return tlkbt_hci_sendH2cCmd(HCI_RD_STORED_LK_CMD_OPCODE, buffer, buffLen);
}

int bth_hci_sendWriteStoredLinkkeyCmd(uint08 mac[6], uint08 *pLinkkey)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteStoredLinkkeyCmd");

    buffLen           = 0;
    buffer[buffLen++] = 1;                   // Num_Keys_To_Write
    tmemcpy(&buffer[buffLen], mac, 6);       // BD_ADDR[i]
    buffLen += 6;
    tmemcpy(&buffer[buffLen], pLinkkey, 16); // Link_Key[i]
    return tlkbt_hci_sendH2cCmd(HCI_WR_STORED_LK_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendDeleteStoredLinkkeyCmd
 * Descript: delete the link key.
 * Params:
 *        @mac[IN]--The Bt address.
 *        @deleteAll[IN]--is delete all link key.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendDeleteStoredLinkkeyCmd(uint08 mac[6], uint08 deleteAll)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendDeleteStoredLinkkeyCmd");

    buffLen = 0;
    tmemcpy(&buffer[buffLen], mac, 6); // BD_ADDR[i]
    buffLen += 6;
    buffer[buffLen++] = deleteAll;     // Delete_All
    return tlkbt_hci_sendH2cCmd(HCI_DEL_STORED_LK_CMD_OPCODE, buffer, buffLen);
}
int bth_hci_sendWriteLocalNameCmd(uint08 *pLocalName)
{
    uint08 nameLen;

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteLocalNameCmd");

    if (pLocalName == nullptr) {
        return -TLK_EPARAM;
    }
    nameLen = strlen((char *)pLocalName);
    return tlkbt_hci_sendH2cCmd(HCI_WR_LOCAL_NAME_CMD_OPCODE, pLocalName, nameLen);
}


/******************************************************************************
 * Function: bth_hci_sendWritePageTimeoutCmd
 * Descript: Set a page time to listen page timeout.
 * Params:
 *        @timeout[IN]--The time value.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
/* Page_Timeout, Page Timeout measured in number of Baseband slots.
 * Interval Length = N * 0.625 ms (1 Baseband slot),Range: 0x0001 to 0xFFFF, Time Range: 0.625 ms to 40.9 s
 * Notice that these are just part of but not all Page_Timeout value */
int bth_hci_sendWritePageTimeoutCmd(uint16 timeout)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWritePageTimeoutCmd");

    buffLen           = 0;
    buffer[buffLen++] = (timeout & 0x00FF); // Page_Timeout
    buffer[buffLen++] = (timeout & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_WR_PAGE_TO_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteScanEnableCmd
 * Descript: Start a page Scan.
 * Params:
 *        @enable[IN]--is page scan enable.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteScanEnableCmd(uint08 enable)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteScanEnable: enable-%d", enable);

    buffLen           = 0;
    buffer[buffLen++] = enable; // Scan_Enable
    return tlkbt_hci_sendH2cCmd(HCI_WR_SCAN_EN_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWritePageScanActivityCmd
 * Descript: Set a page Scan parameter.
 * Params:
 *        @interval[IN]--is page scan interval.
 *        @window[IN]--The page scan window.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
// n*0x625  ms
int bth_hci_sendWritePageScanActivityCmd(uint16 interval, uint16 window)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWritePageScanActivityCmd");

    buffLen           = 0;
    buffer[buffLen++] = (interval & 0x00FF); //
    buffer[buffLen++] = (interval & 0xFF00) >> 8;
    buffer[buffLen++] = (window & 0x00FF);   //
    buffer[buffLen++] = (window & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_WR_PAGE_SCAN_ACT_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteInquiryScanActivityCmd
 * Descript: Set a inquiry Scan parameter.
 * Params:
 *        @interval[IN]--is inquiry scan interval.
 *        @window[IN]--The inquiry scan window.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteInquiryScanActivityCmd(uint16 interval, uint16 window)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteInquiryScanActivityCmd");

    buffLen           = 0;
    buffer[buffLen++] = (interval & 0x00FF); //
    buffer[buffLen++] = (interval & 0xFF00) >> 8;
    buffer[buffLen++] = (window & 0x00FF);   //
    buffer[buffLen++] = (window & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_WR_INQ_SCAN_ACT_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteAuthenEnableCmd
 * Descript: Start a anthentication.
 * Params:
 *        @enable[IN]--is enable authen.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteAuthenEnableCmd(uint08 enable)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteAuthenEnableCmd");

    buffLen           = 0;
    buffer[buffLen++] = enable; // Authentication_Enable
    return tlkbt_hci_sendH2cCmd(HCI_WR_AUTH_EN_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteClassOfDeviceCmd
 * Descript: Set the COD.
 * Params:
 *        @devClass[IN]--the device class.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteClassOfDeviceCmd(uint32 devClass)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteClassOfDeviceCmd");

    buffLen           = 0;
    buffer[buffLen++] = (devClass & 0x000000FF); // Class_Of_Device:
    buffer[buffLen++] = (devClass & 0x0000FF00) >> 8;
    buffer[buffLen++] = (devClass & 0x00FF0000) >> 16;
    return tlkbt_hci_sendH2cCmd(HCI_WR_CLASS_OF_DEV_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteLinkSupervisionTimeoutCmd
 * Descript: Set the link supervision time.
 * Params:
 *        @aclHandle[IN]--the acl handle.
 *        @timeout[IN]--The timeout.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteLinkSupervisionTimeoutCmd(uint16 aclHandle, uint16 timeout)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteLinkSupervisionTimeoutCmd");

    buffLen           = 0;
    buffer[buffLen++] = (aclHandle & 0x00FF); // Handle
    buffer[buffLen++] = (aclHandle & 0xFF00) >> 8;
    buffer[buffLen++] = (timeout & 0x00FF);   // Link_Supervision_Timeou
    buffer[buffLen++] = (timeout & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_WR_LINK_SUPV_TO_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteInquiryModeCmd
 * Descript: Set the inquiry mode
 * Params:
 *        @mode[IN]--the inquiry mode.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteInquiryModeCmd(uint08 mode)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteInquiryModeCmd");

    buffLen           = 0;
    buffer[buffLen++] = mode; // Inquiry_Mode
    return tlkbt_hci_sendH2cCmd(HCI_WR_INQ_MODE_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteExtendedInquiryRspCmd
 * Descript: Set the EIR
 * Params:
 *        @fecReq[IN]--0x00 FEC is not required, 0x01 FEC is required.
 *        @pData[IN]--The eir data.
 *        @datalen[IN]--Eir datalen
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteExtendedInquiryRspCmd(uint08 fecReq, uint08 *pData, uint08 dataLen)
{
	// Spec request 'Extended_Inquiry_Response' param 240 octets, but TLKBT_HCI_H2C_CMD_FIFO_SIZE < 240 octets,
	// so here buffer size temp set TLKBT_HCI_H2C_CMD_FIFO_SIZE(100). 
	uint08 buffer[100];	

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteExtendedInquiryRspCmd");

	buffer[0] = fecReq;
	tmemset(&buffer[1], 0, 100);
	tmemcpy(&buffer[1], pData, dataLen);

	return tlkbt_hci_sendH2cCmd(HCI_WR_EXT_INQ_RSP_CMD_OPCODE, buffer, sizeof(buffer));
}


/******************************************************************************
 * Function: bth_hci_sendWriteSimplePairingModeCmd
 * Descript: Set SSP mode.
 * Params:
 *        @spMode[IN]--the mode.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteSimplePairingModeCmd(uint08 spMode)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteSimplePairingModeCmd");

    buffLen           = 0;
    buffer[buffLen++] = spMode; // Simple_Pairing_Mode
    return tlkbt_hci_sendH2cCmd(HCI_WR_SP_MODE_CMD_OPCODE, buffer, buffLen);
}


/******************************************************************************
 * Function: bth_hci_sendWriteLookbackModeCmd
 * Descript: Set loopback mode.
 * Params:
 *        @mode[IN]--the mode.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteLookbackModeCmd(uint08 mode)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteLookbackModeCmd");

    buffLen           = 0;
    buffer[buffLen++] = mode; // Loopback_Mode
    return tlkbt_hci_sendH2cCmd(HCI_WR_LOOPBACK_MODE_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendWriteSimpleDebugModeCmd
 * Descript: Set debug mode.
 * Params:
 *        @debugMode[IN]--the mode.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteSimpleDebugModeCmd(uint08 debugMode)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteSimpleDebugModeCmd");

    buffLen           = 0;
    buffer[buffLen++] = debugMode; // Simple_Pairing_Debug_Mode
    return tlkbt_hci_sendH2cCmd(HCI_WR_SP_DBG_MODE_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendReadSecureConnHostSupportCmd
 * Descript: Run the HCI command to enable the low-level security connection function.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendReadSecureConnHostSupportCmd(void)
{
    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendReadSecureConnHostSupportCmd");
    return tlkbt_hci_sendH2cCmd(HCI_RD_SECURE_CONN_HOST_SUPP_CMD_OPCODE, nullptr, 0);
}

/******************************************************************************
 * Function: bth_hci_sendWriteSecureConnHostSupportCmd
 * Descript: Run the HCI command to control whether the underlying security
 *           connection function is enabled.
 * Params:
 *     @isSupport: True if you want to enable the sercurity connection.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendWriteSecureConnHostSupportCmd(uint08 isSupport)
{
    uint08 buffLen;
    uint08 buffer[8];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendWriteSecureConnHostSupportCmd");

    buffLen = 0;
    if (isSupport) {
        buffer[buffLen++] = 1;
    } else {
        buffer[buffLen++] = 0;
    }
    return tlkbt_hci_sendH2cCmd(HCI_WR_SECURE_CONN_HOST_SUPP_CMD_OPCODE, buffer, buffLen);
}

/******************************************************************************
 * Function: bth_hci_sendReadRssiCmd
 * Descript: This command reads the Received Signal Strength Indication (RSSI)
 *           value from a Controller.
 * Params:
 *     @connHandle: The Handle for the connection for which the RSSI is to be read.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendReadRssiCmd(uint16 connHandle)
{
    uint08 buffLen;
    uint08 buffer[8];
	
    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendReadRssiCmd");
	
    buffLen = 0;
    buffer[buffLen++] = (connHandle & 0xFF);
    buffer[buffLen++] = (connHandle & 0xFF00) >> 8;
    return tlkbt_hci_sendH2cCmd(HCI_RD_RSSI_CMD_OPCODE, buffer, buffLen);
}


/******************************************************************************
 * Function: bth_hci_sendSetESCOMuteCmd
 * Descript: Set sco mute mode.
 * Params:
 *        @escoHandle[IN]--the esco handle.
 *        @micSpk[IN]--The mic speaker
 *        @muteEn[IN]--ebable mute.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendSetESCOMuteCmd(uint16 escoHandle, uint08 micSpk, uint08 muteEn)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendSetESCOMuteCmd");

    buffLen           = 0;
    buffer[buffLen++] = (escoHandle & 0x00FF);
    buffer[buffLen++] = (escoHandle & 0xFF00) >> 8;
    buffer[buffLen++] = micSpk;
    buffer[buffLen++] = muteEn;
    return tlkbt_hci_sendH2cCmd(HCI_SET_ESCO_MUTE_CMD_OPCODE, buffer, buffLen);
}
/******************************************************************************
 * Function: bth_hci_sendSetBtAddrCmd
 * Descript: Set sco mute mode.
 * Params:
 *        @pBtAddr[IN]--the bt address.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_hci_sendSetBtAddrCmd(uint08 pBtAddr[6])
{
    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendSetBtAddrCmd");
    if (pBtAddr == nullptr) {
        return -TLK_EPARAM;
    }

    return tlkbt_hci_sendH2cCmd(HCI_TDB_SET_BT_BD_ADDR_CMD_OPCODE, pBtAddr, 6);
}

extern void tlkbt_hci_exeCmdNow(void);
extern void tlkbt_hci_exeEvtNow(void);
/******************************************************************************
 * Function: bth_hci_exeCmdNow
 * Descript: Execute the Host layer HCI instruction cached in the fifo
 *           immediately to ensure that there is enough space for the
 *           instructions added later. This interface is suitable for use
 *           during the initialization phase.
 * Params: None.
 * Note: "bth_hci_exeCmdNow" and "bth_hci_exeEvtNow" is only used in Init stage.
 *******************************************************************************/
void bth_hci_exeCmdNow(void)
{
    tlkbt_hci_exeCmdNow();
}
void bth_hci_exeEvtNow(void)
{
    tlkbt_hci_exeEvtNow();
}
/******************************************************************************
 * Function: bth_hci_sendSetMaxSlotCmd
 * Descript: set max slot command.
 * Params:
 *        @pBtAddr[IN]--the bt address.

 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendSetMaxSlotCmd(uint08 pBtAddr[6])
{
    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "bth_hci_sendSetMaxSlotCmd");
    return tlkbt_hci_sendH2cCmd(HCI_MAX_SLOT_CHG_EVT_CODE, pBtAddr, 6);
}

/******************************************************************************
 * Function: bth_hci_sendEnhancedPowerCntlCmd
 * Descript: set enhanced power control command.
 * Params:
 *        @conn_handle[IN]--acl connection handle
 *        @option[IN]--0: decrement power one step, 1: increment power one step, 2: increase to maximum power

 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int bth_hci_sendEnhancedPowerCntlCmd(uint16 conn_handle, uint08 option)
{
    uint08 buffLen;
    uint08 buffer[16];

    tlkapi_trace(BTH_HCICMD_DBG_FLAG, BTH_HCICMD_DBG_SIGN, "HCI_ENHANCED_POWER_CNTL_OPCODE");

    buffLen           = 0;
    buffer[buffLen++] = (conn_handle & 0x00FF);
    buffer[buffLen++] = (conn_handle & 0xFF00) >> 8;
    buffer[buffLen++] = option;

    return tlkbt_hci_sendH2cCmd(HCI_RD_ENH_TX_PWR_LVL_CMD_OPCODE, buffer, buffLen);
}


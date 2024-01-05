/********************************************************************************************************
 * @file	bth_signal.h
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
#ifndef BTH_SIGNAL_H
#define BTH_SIGNAL_H


/* command code */
typedef enum {
    BTH_SIGNAL_OPCODE_REJECT   = 0x01,
    BTH_SIGNAL_OPCODE_CONN_REQ = 0x02,
    BTH_SIGNAL_OPCODE_CONN_RSP = 0x03,
    BTH_SIGNAL_OPCODE_CFG_REQ  = 0x04,
    BTH_SIGNAL_OPCODE_CFG_RSP  = 0x05,
    BTH_SIGNAL_OPCODE_DISC_REQ = 0x06,
    BTH_SIGNAL_OPCODE_DISC_RSP = 0x07,
    BTH_SIGNAL_OPCODE_ECHO_REQ = 0x08,
    BTH_SIGNAL_OPCODE_ECHO_RSP = 0x09,
    BTH_SIGNAL_OPCODE_INFO_REQ = 0x0A,
    BTH_SIGNAL_OPCODE_INFO_RSP = 0x0B,
} BTH_SIGNAL_OPCODE_ENUM;


typedef enum {
    BTH_SIGNAL_FLAG_NONE           = 0x00,
    BTH_SIGNAL_FLAG_WAIT_DISC_REQ  = 0x01,
    BTH_SIGNAL_FLAG_WAIT_DISC_RSP  = 0x02,
    BTH_SIGNAL_FLAG_WAIT_CONN_REQ  = 0x04,
    BTH_SIGNAL_FLAG_WAIT_CONN_RSP  = 0x08,
    BTH_SIGNAL_FLAG_WAIT_CFG_REQ   = 0x10,
    BTH_SIGNAL_FLAG_WAIT_CFG_RSP   = 0x20,
    BTH_SIGNAL_FLAG_RE_NEGOTIATION = 0x40,
} BTH_SIGNAL_FLAGS_ENUM;
typedef enum {
    BTH_SIGNAL_BUSY_NONE          = 0x00,
    BTH_SIGNAL_BUSY_SEND_REJECT   = 0x80,
    BTH_SIGNAL_BUSY_SEND_DISC_REQ = 0x01,
    BTH_SIGNAL_BUSY_SEND_DISC_RSP = 0x02,
    BTH_SIGNAL_BUSY_SEND_CONN_REQ = 0x04,
    BTH_SIGNAL_BUSY_SEND_CONN_RSP = 0x08,
    BTH_SIGNAL_BUSY_SEND_CFG_REQ  = 0x10,
    BTH_SIGNAL_BUSY_SEND_CFG_RSP  = 0x20,
} BTH_SIGNAL_BUSYS_ENUM;
typedef enum {
    BTH_SIGNAL_ATTR_NONE    = 0x00,
    BTH_SIGNAL_ATTR_CONNECT = 0x40,
    BTH_SIGNAL_ATTR_ACTIVE  = 0x80,
} BTH_SIGNAL_ATTRS_ENUM;


typedef enum {
    BTH_SIGNAL_INFO_RESULT_SUCCESS = 0x0000, // Success
    BTH_SIGNAL_INFO_RESULT_NOTSUPP = 0x0001, // Success
} BTH_SIGNAL_INFO_RESULT_ENUM;

typedef enum {
    BTH_SIGNAL_CONN_RESULT_SUCCESS                   = 0x0000, // Connection successful
    BTH_SIGNAL_CONN_RESULT_PENDING                   = 0x0001, // Connection pending
    BTH_SIGNAL_CONN_RESULT_REFUSED_PSM_NOT_SUPPORTED = 0x0002, // Connection refused: PSM not supported
    BTH_SIGNAL_CONN_RESULT_REFUSED_SECURITY_BLOCK    = 0x0003, // Connection refused: security block
    BTH_SIGNAL_CONN_RESULT_REFUSED_NO_RESOURCES      = 0x0004, // Connection refused: no resources available
    BTH_SIGNAL_CONN_RESULT_INVALID_SCID              = 0x0006, // Connection refused: invalid Source CID
    BTH_SIGNAL_CONN_RESULT_SCID_ALREADY_ALLOCATED    = 0x0007, // Connection refused: Source CID already allocated
} BTH_SIGNAL_CONN_RESULT_ENUM;

typedef enum {
    BTH_SIGNAL_CONN_STATUS_NONE           = 0x0000, // No further information available
    BTH_SIGNAL_CONN_STATUS_AUTHEN_PENDING = 0x0001, // Authentication pending
    BTH_SIGNAL_CONN_STATUS_AUTHOR_PENDING = 0x0002, // Authorization pending
} BTH_SIGNAL_CONN_STATUS_ENUM;

typedef enum {
    BTH_SIGNAL_CFG_RESULT_SUCCESS            = 0x0000, //
    BTH_SIGNAL_CFG_RESULT_UNACCEPT_PARAM     = 0x0001,
    BTH_SIGNAL_CFG_RESULT_REJECT             = 0x0002,
    BTH_SIGNAL_CFG_RESULT_UNKNOWN_OPTIONS    = 0x0003,
    BTH_SIGNAL_CFG_RESULT_PENDING            = 0x0004,
    BTH_SIGNAL_CFG_RESULT_FLOW_SPEC_REJECTED = 0x0005,
} BTH_SIGNAL_CFG_RESULT_ENUM;

typedef enum {
    BTH_SIGNAL_REASON_NOT_UNDERSTOOD = 0x00,
    BTH_SIGNAL_REASON_MTU_EXCEED     = 0x01,
    BTH_SIGNAL_REASON_INVALID_CID    = 0x02,
} BTH_SIGNAL_REASON_ENUM;


/******************************************************************************
 * FCS: It is non-negotiable. The FCS option shall only be used when the mode
 * is being, or is already configured to Enhanced Retransmission mode or
 * Streaming mode.
 *******************************************************************************/
int bth_signal_enableRtnOption(uint16 aclHandle, uint16 scid, bool enable);
int bth_signal_enableFcsOption(uint16 aclHandle, uint16 scid, bool enable);
int bth_signal_enableEfsOption(uint16 aclHandle, uint16 scid, bool enable);
int bth_signal_enableQosOption(uint16 aclHandle, uint16 scid, bool enable);
int bth_signal_setFcsType(uint16 aclHandle, uint16 scid, uint16 fcsType);
int bth_signal_setRtnMode(uint16 aclHandle, uint16 scid, uint08 rtnMode);
int bth_signal_setRtnMps(uint16 aclHandle, uint16 scid, uint16 rtnMps);
int bth_signal_getFcsType(uint16 aclHandle, uint16 scid, uint08 *pFcsType);
int bth_signal_getRtnMode(uint16 aclHandle, uint16 scid, uint08 *pRtnMode);
int bth_signal_setRfsParam(uint16 aclHandle,
                           uint16 scid,
                           uint08 ientifier,
                           uint08 serviceType,
                           uint16 maxSduSize,
                           uint32 sduArrivalTime,
                           uint32 accessLatency,
                           uint32 flushTimeout);
int bth_signal_getRtnMonTime(uint16 aclHandle, uint16 scid, uint16 *pMonTime);
int bth_signal_getRtnRtnTime(uint16 aclHandle, uint16 scid, uint16 *pRtnTime);
int bth_signal_getRtnCount(uint16 aclHandle, uint16 scid, uint08 *pCount);
int bth_signal_setQosServiceType(uint16 aclHandle, uint16 scid, uint08 serviceType);


/******************************************************************************
 * Function: bth_signal_createConnect
 * Descript: Create a profile connection.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 *        @psmID[IN]--The psm id.
 *        @usrID[IN]--The user id.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_createConnect(uint16 aclHandle, uint16 psmID, uint08 usrID, uint16 *pScid);

/******************************************************************************
 * Function: bth_signal_destroyConnect
 * Descript: Disconnect a profile connection.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 *        @psmID[IN]--The psm id.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_destroyConnect(uint16 aclHandle, uint16 psmID);

/******************************************************************************
 * Function: bth_signal_disconnChannel
 * Descript: User trigger Disconnect a profile connection.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 *        @chnID[IN]--The channel id.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_disconnChannel(uint16 aclHandle, uint16 chnID);

/******************************************************************************
 * Function: bth_signal_disconnChannelByUsrID
 * Descript: Disconnect channel by user id.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 *        @psmID[IN]--The psm id.
 *        @usrID[IN]--The user id.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_disconnChannelByUsrID(uint16 aclHandle, uint16 psmID, uint08 usrID);

/******************************************************************************
 * Function: bth_signal_destroyConnectByUsrID
 * Descript: Destroy channel by user id.
 * Params:
 *        @aclHandle[IN]--The acl handle.
 *        @psmID[IN]--The psm id.
 *        @usrID[IN]--The user id.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_destroyConnectByUsrID(uint16 aclHandle, uint16 psmID, uint08 usrID);


/******************************************************************************
 * Function: bth_signal_sendClosedEvt
 * Descript: handle close event.
 * Params:
 *        @pChannel[IN]--The l2cap channel.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendClosedEvt(bth_l2cap_channel_t *pChannel);

/******************************************************************************
 * Function: bth_signal_sendRequestEvt
 * Descript: handle request event.
 * Params:
 *        @psmID[IN]--The psm id.
 *        @handle[IN]--The acl handle.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendRequestEvt(uint16 psmID, uint16 handle, uint16 scid);

/******************************************************************************
 * Function: bth_signal_sendRequestEvt
 * Descript: handle connect event.
 * Params:
 *        @pChannel[IN]--The l2cap channel.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendConnectEvt(bth_l2cap_channel_t *pChannel);

/******************************************************************************
 * Function: bth_signal_sendDisconnEvt
 * Descript: handle disconnect event.
 * Params:
 *        @pChannel[IN]--The l2cap channel.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendDisconnEvt(bth_l2cap_channel_t *pChannel);

/******************************************************************************
 * Function: bth_signal_chnStart
 * Descript: Start Setup a l2cap channel.
 * Params:
 *        @pChannel[IN]--The l2cap channel.
 *        @aclHandle[IN]--The acl handle.
 *        @psmID[IN]--The psm id.
 *        @usrID[IN]--The user id.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_chnStart(bth_l2cap_channel_t *pChannel, uint16 aclHandle, uint16 psmID, uint08 usrID);

/******************************************************************************
 * Function: bth_signal_chnClose
 * Descript: disconnect a l2cap channel.
 * Params:
 *        @pChannel[IN]--The l2cap channel.
 *        @isForce[IN]--is Force.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_chnClose(bth_l2cap_channel_t *pChannel, uint08 isForce);

/******************************************************************************
 * Function: bth_signal_chnReset
 * Descript: reset a l2cap channel.
 * Params:
 *        @pChannel[IN]--The l2cap channel.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
void bth_signal_chnReset(bth_l2cap_channel_t *pChannel);

/******************************************************************************
 * Function: bth_signal_chnTimer
 * Descript: signal channel timer.
 * Params:
 *        @pTimer[IN]--The l2cap channel timer.
 *        @userArg[IN]--The data need to handle.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
bool bth_signal_chnTimer(tlkapi_timer_t *pTimer, uint32 userArg);

/******************************************************************************
 * Function: bth_signal_recvData
 * Descript: handle the l2cap data.
 * Params:
 *        @handle[IN]--The l2cap handle.
 *        @pData[IN]--The data need to handle.
 *        @dataLen[IN]--The data length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
void bth_signal_recvData(uint16 handle, uint08 *pData, uint16 dataLen);
void bth_signal_recvHandler(uint16 handle, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: bth_signal_busyProc
 * Descript:L2cap signal busy event handler.
 * Params:
 *        @pChannel[IN]--The l2cap channel.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
void bth_signal_busyProc(bth_l2cap_channel_t *pChannel);

/******************************************************************************
 * Function: bth_signal_sendInfoReq
 * Descript: Send l2cap info request.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @infoType[IN]--The info type.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendInfoReq(uint16 handle, uint08 identify, uint16 infoType);

/******************************************************************************
 * Function: bth_signal_sendInfoRsp
 * Descript: Send l2cap info response.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @infoType[IN]--The info type.
 *        @result[IN]--The result.
 *        @pData[IN]--The payload.
 *        @dataLen[IN]--The payload length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendInfoRsp(uint16 handle,
                           uint08 identify,
                           uint16 infoType,
                           uint16 result,
                           uint08 *pData,
                           uint16 dataLen);

/******************************************************************************
 * Function: bth_signal_sendEchoReq
 * Descript: Send the l2cap echo request.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @pData[IN]--The payload.
 *        @dataLen[IN]--The payload length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendEchoReq(uint16 handle, uint08 identify, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_signal_sendEchoRsp
 * Descript: Send the l2cap echo response.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @pData[IN]--The payload.
 *        @dataLen[IN]--The payload length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendEchoRsp(uint16 handle, uint08 identify, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_signal_sendConnectReq
 * Descript: Send the l2cap connect request.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @psm[IN]--The psm id.
 *        @scid[IN]--The scid.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendConnectReq(uint16 handle, uint08 identify, uint16 psm, uint16 scid);

/******************************************************************************
 * Function: bth_signal_sendConnectRsp
 * Descript: Send the l2cap connect response.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @scid[IN]--The scid.
 *        @dcid[IN]--The dcid.
 *        @result[IN]--The result.
 *        @status[IN]--The status.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendConnectRsp(uint16 handle, uint08 identify, uint16 scid, uint16 dcid, uint16 result, uint16 status);

/******************************************************************************
 * Function: bth_signal_sendDisconnReq
 * Descript: Send the l2cap disconnect request.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @scid[IN]--The scid.
 *        @dcid[IN]--The dcid.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendDisconnReq(uint16 handle, uint08 identify, uint16 dcid, uint16 scid);

/******************************************************************************
 * Function: bth_signal_sendDisconnRsp
 * Descript: Send the l2cap disconnect response.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @scid[IN]--The scid.
 *        @dcid[IN]--The dcid.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendDisconnRsp(uint16 handle, uint08 identify, uint16 dcid, uint16 scid);

/******************************************************************************
 * Function: bth_signal_sendConfigReq
 * Descript: Send the l2cap config request.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @dcid[IN]--The dcid.
 *        @flags[IN]--The flags.
 *        @pData[IN]--The payload.
 *        @dataLen[IN]--The data length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendConfigReq(uint16 handle, uint08 identify, uint16 dcid, uint16 flags, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: bth_signal_sendConfigRsp
 * Descript: Send the l2cap config response.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @dcid[IN]--The dcid.
 *        @flags[IN]--The flags.
 *        @result[IN]--The result.
 *        @pData[IN]--The payload.
 *        @dataLen[IN]--The data length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendConfigRsp(uint16 handle,
                             uint08 identify,
                             uint16 dcid,
                             uint16 flags,
                             uint16 result,
                             uint08 *pData,
                             uint16 dataLen);

/******************************************************************************
 * Function: bth_signal_sendRejectRsp
 * Descript: Send reject response.
 * Params:
 *        @handle[IN]--The acl link handle.
 *        @identify[IN]--The identify.
 *        @reason[IN]--The reason for reject.
 *        @pData[IN]--The payload.
 *        @dataLen[IN]--The data length.
 * Return: TLK_ENONE is success, other value is failure.
 *******************************************************************************/
int bth_signal_sendRejectRsp(uint16 handle, uint08 identify, uint16 reason, uint08 *pData, uint16 dataLen);

int bth_signal_enableRtnOption(uint16 aclHandle, uint16 psmID, bool enable);

#endif // BTH_SIGNAL_H

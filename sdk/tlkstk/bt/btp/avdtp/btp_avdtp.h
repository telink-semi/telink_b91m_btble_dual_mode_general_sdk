/********************************************************************************************************
 * @file	btp_avdtp.h
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
#ifndef BTP_AVDTP_H
#define BTP_AVDTP_H


#define BTP_AVDTP_MSGTYPE_MASK 0x03
#define BTP_AVDTP_MSGTYPE_OFFS 0

#define BTP_AVDTP_PKTTYPE_MASK 0x03
#define BTP_AVDTP_PKTTYPE_OFFS 2

#define BTP_AVDTP_TRANLAB_MASK 0x0F
#define BTP_AVDTP_TRANLAB_OFFS 4

#define BTP_AVDTP_SIGID_MASK   0x3F


typedef enum {
    BTP_AVDTP_MSG_TYPE_COMMAND = 0x00, // Command
    BTP_AVDTP_MSG_TYPE_GENERAL = 0x01, // General Reject
    BTP_AVDTP_MSG_TYPE_ACCEPT  = 0x02, // Response Accept
    BTP_AVDTP_MSG_TYPE_REJECT  = 0x03, // Response Reject
} BTP_AVDTP_MSGTYPE_ENUM;

typedef enum {
    BTP_AVDTP_PKT_TYPE_SINGLE   = 0x00,
    BTP_AVDTP_PKT_TYPE_START    = 0x01,
    BTP_AVDTP_PKT_TYPE_CONTINUE = 0x02,
    BTP_AVDTP_PKT_TYPE_END      = 0x03,
} BTP_AVDTP_PKTTYPE_ENUM;

typedef enum {
    BTP_AVDTP_SIGID_DISCOVERY    = 0x01,
    BTP_AVDTP_SIGID_GET_CAP      = 0x02,
    BTP_AVDTP_SIGID_SET_CONFIG   = 0x03,
    BTP_AVDTP_SIGID_GET_CONFIG   = 0x04,
    BTP_AVDTP_SIGID_RECONFIG     = 0x05,
    BTP_AVDTP_SIGID_OPEN         = 0x06,
    BTP_AVDTP_SIGID_START        = 0x07,
    BTP_AVDTP_SIGID_CLOSE        = 0x08,
    BTP_AVDTP_SIGID_SUSPEND      = 0x09,
    BTP_AVDTP_SIGID_ABORT        = 0x0A,
    BTP_AVDTP_SIGID_SEC_CONTROL  = 0x0B,
    BTP_AVDTP_SIGID_GET_ALL_CAP  = 0x0C,
    BTP_AVDTP_SIGID_DELAY_REPORT = 0x0D,
} BTP_AVDTP_SIGID_ENUM;


typedef enum {
    BTP_AVDTP_ERRID_BAD_HEADER_FORMAT          = 0x01,
    BTP_AVDTP_ERRID_BAD_LENGTH                 = 0x11,
    BTP_AVDTP_ERRID_BAD_ACP_SEID               = 0x12,
    BTP_AVDTP_ERRID_SEP_IN_USE                 = 0x13,
    BTP_AVDTP_ERRID_SEP_NOT_IN_USE             = 0x14,
    BTP_AVDTP_ERRID_BAD_SERV_CATEGORY          = 0x17,
    BTP_AVDTP_ERRID_BAD_PAYLOAD_FORMAT         = 0x18,
    BTP_AVDTP_ERRID_NOT_SUPPORTED_COMMAND      = 0x19,
    BTP_AVDTP_ERRID_INVALID_CAPABILITIES       = 0x1A,
    BTP_AVDTP_ERRID_BAD_RECOVERY_TYPE          = 0x22,
    BTP_AVDTP_ERRID_BAD_MEDIA_TRANSPORT_FORMAT = 0x23,
    BTP_AVDTP_ERRID_BAD_RECOVERY_FORMAT        = 0x25,
    BTP_AVDTP_ERRID_BAD_ROHC_FORMAT            = 0x26,
    BTP_AVDTP_ERRID_BAD_CP_FORMAT              = 0x27,
    BTP_AVDTP_ERRID_BAD_MULTIPLEXING_FORMAT    = 0x28,
    BTP_AVDTP_ERRID_UNSUPPORTED_CONFIGURATION  = 0x29,
    BTP_AVDTP_ERRID_BAD_STATE                  = 0x31,
} BTP_AVDTP_ERRID_ENUM;

/******************************************************************************
 * Function: AVDTP Send media data interface
 * Descript: Defines trigger the AVDTP data transfer to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @seqNumber--The data sequence id.
 *        @timestamp--The data endode's time.
 *        @srcDataID--The source data id.
 *        @pPayload--The payload.
 *        @payloadSize--payload data size.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendMedia(uint16 aclHandle,
                        uint16 scid,
                        uint16 seqNumber,
                        uint32 timestamp,
                        uint32 srcDataID,
                        uint08 *pPayload,
                        uint16 payloadSize);

/******************************************************************************
 * Function: AVDTP Send packet data interface
 * Descript: Defines trigger the AVDTP data transfer to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @msgType--The data message type.
 *        @label--The transaction id.
 *        @sigId--the signal id
 *        @pData--The payload.
 *        @datalen--payload data size.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendPacket(uint16 aclHandle,
                         uint16 scid,
                         uint08 msgType,
                         uint08 label,
                         uint08 sigID,
                         uint08 *pData,
                         uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send start data interface
 * Descript: Defines trigger the AVDTP start packet of data to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @msgType--The data message type.
 *        @label--The transaction id.
 *        @nosp--the num.
 *        @sigId--the signal id
 *        @pData--The payload.
 *        @datalen--payload data size.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendStartPacket(uint16 aclHandle,
                              uint16 scid,
                              uint08 msgType,
                              uint08 label,
                              uint08 nosp,
                              uint08 sigID,
                              uint08 *pData,
                              uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send continue packet of whole data interface
 * Descript: Defines trigger the AVDTP data continue send to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @msgType--The message type.
 *        @label--The transaction id.
 *        @pData--The end of payload.
 *        @dataLen--the end of payload data size.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendContinuePacket(uint16 aclHandle,
                                 uint16 scid,
                                 uint08 msgType,
                                 uint08 label,
                                 uint08 *pData,
                                 uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send the end data interface
 * Descript: Defines the barar fail send the data in one time,
 *           and this command send the end of packet of a whole data.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @msgType--The message type.
 *        @label--The transaction id.
 *        @pData--The end of payload.
 *        @dataLen--the end of payload data size.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendEndPacket(uint16 aclHandle, uint16 scid, uint08 msgType, uint08 label, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send Discovery command interface
 * Descript: Defines trigger the AVDTP Discovery command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSepDiscoveryCmd(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send Discovery response interface
 * Descript: Defines trigger the AVDTP send the codec to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pData--The data of config's content.
 *        @dataLen--The data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSepDiscoveryRsp(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send Discovery rejectinterface
 * Descript: Defines trigger the AVDTP reject the Discovery command from
 *           peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSepDiscoveryRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send get capability command interface
 * Descript: Defines trigger the AVDTP get the codec's capability of the peer endpoint.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--accept endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetCapabilityCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid);

/******************************************************************************
 * Function: AVDTP Send get capability response interface
 * Descript: Defines trigger the AVDTP the codec's capability to peer device.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pData--The data of config's content.
 *        @dataLen--The data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetCapabilityRsp(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Reject get capability interface
 * Descript: Defines trigger the AVDTP send reject to peer AVDTP entity,
 *           when codec's parameter abnormal.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetCapabilityRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send get all capability command interface
 * Descript: Defines trigger the AVDTP send get capability command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetAllCapabilityCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid);

/******************************************************************************
 * Function: AVDTP Send Get all capanility response interface
 * Descript: Defines trigger the AVDTP send response to src device.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pData--The data of config's content.
 *        @dataLen--The data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetAllCapabilityRsp(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send reject get all capability interface
 * Descript: Defines trigger the AVDTP send reject when src process get all capability
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetAllCapabilityRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send set config interface
 * Descript: Defines trigger the AVDTP set config command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--peer endpoint id.
 *        @intSeid--local endpoint id.
 *        @pData--The data of config's content.
 *        @dataLen--The data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSetConfigCmd(uint16 aclHandle,
                               uint16 scid,
                               uint08 label,
                               uint08 acpSeid,
                               uint08 intSeid,
                               uint08 *pData,
                               uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send set config response interface
 * Descript: Defines trigger the AVDTP send response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSetConfigRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send set config reject interface
 * Descript: Defines trigger the AVDTP set configuration reject to peer
 *           device when parameter abnormal.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @errID--The error id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSetConfigRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pErrID);

/******************************************************************************
 * Function: AVDTP Send get config command interface
 * Descript: Defines trigger the AVDTP send get config command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetConfigCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid);

/******************************************************************************
 * Function: AVDTP Sendget config response interface
 * Descript: Defines trigger the AVDTP data transfer to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pData--The codec configuration data.
 *        @dataLen--the data size.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetConfigRsp(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: AVDTP reject get config interface
 * Descript: Defines trigger the AVDTP get codec's configuration of peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @errID--The error id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGetConfigRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send reconfig command interface
 * Descript: Defines trigger the AVDTP send reconfig command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--peer endpoint id.
 *        @pData--The data of reconfig's content.
 *        @dataLen--The data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendReconfigCmd(uint16 aclHandle,
                              uint16 scid,
                              uint08 label,
                              uint08 acpSeid,
                              uint08 *pData,
                              uint16 dataLen);

/******************************************************************************
 * Function: AVDTP reconfig response interface
 * Descript: Defines trigger the AVDTP send reconfig response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendReconfigRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send reconfig reject interface
 * Descript: Defines trigger the AVDTP reconfig reject to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pData--The data of reconfig's content.
 *        @dataLen--The data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendReconfigRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send open command interface
 * Descript: Defines trigger the AVDTP send open command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendOpenCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid);

/******************************************************************************
 * Function: AVDTP Send open response interface
 * Descript: Defines trigger the AVDTP send open response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendOpenRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send Open reject  interface
 * Descript: Defines trigger the AVDTP send open reject response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @errID--The error id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendOpenRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send start command interface
 * Descript: Defines trigger the AVDTP send start command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pAcpSeid--The peer endpoint id.
 *        @numb--the number.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendStartCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pAcpSeid, uint08 numb);

/******************************************************************************
 * Function: AVDTP Send start response interface
 * Descript: Defines trigger the AVDTP send start response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendStartRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send start reject interface
 * Descript: Defines trigger the AVDTP send start reject response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @errID--The error id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendStartRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pErrID);

/******************************************************************************
 * Function: AVDTP Send close command interface
 * Descript: Defines trigger the AVDTP close command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendCloseCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid);

/******************************************************************************
 * Function: AVDTP Send close response interface
 * Descript: Defines trigger the AVDTP send close response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendCloseRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send close reject interface
 * Descript: Defines trigger the AVDTP send close reject response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @errID--The error id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendCloseRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send suspend command interface
 * Descript: Defines trigger the AVDTP command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pAcpSeid--The peer endpoint id.
 *        @numb--The number.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSuspendCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pAcpSeid, uint08 numb);

/******************************************************************************
 * Function: AVDTP Send suspend response interface
 * Descript: Defines trigger the AVDTP send suspend response to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSuspendRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send suspend reject interface
 * Descript: Defines trigger the AVDTP send suspend reject to peer AVDTP entity
 *           when the status or parameter error.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 *        @errID--The error data id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSuspendRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send abort command interface
 * Descript: Defines trigger the AVDTP abort command to peer AVDTP entity when
 *           the status abnormal.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The peer endpoint id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendAbortCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid);

/******************************************************************************
 * Function: AVDTP Send abort response interface
 * Descript: Defines trigger the AVDTP abort response to peer AVDTP entity
 *           when the status abnormal.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendAbortRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send seccontrol command interface
 * Descript: Defines trigger the AVDTP security control command to peer AVDTP entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The endpoint Id of peer device.
 *        @pData--The sec control command data.
 *        @dataLen--The payload length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSecControlCmd(uint16 aclHandle,
                                uint16 scid,
                                uint08 label,
                                uint08 acpSeid,
                                uint08 *pData,
                                uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send security control response interface
 * Descript: Defines the AVDTP security control response send to peer AVDTP
 *           entity when support this feature.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @pData--The data of security's content.
 *        @dataLen--The data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSecControlRsp(uint16 aclHandle, uint16 scid, uint08 label, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: AVDTP Send security control reject interface
 * Descript: Defines the AVDTP security reject send to peer AVDTP entity when
 *           not support this feature.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @errID--The error Id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendSecControlRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);

/******************************************************************************
 * Function: AVDTP Send General reject interface
 * Descript: Defines trigger the AVDTP transfer the general reject to peer
 *           AVDTP entity when the command or data is abnormal.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @sigID--The AVDTP signal Id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendGeneralReject(uint16 aclHandle, uint16 scid, uint08 label, uint08 sigID);

/******************************************************************************
 * Function: AVDTP Send delay report command interface
 * Descript: Defines trigger the AVDTP send delay report command data to peer
 *           src AVDTP entity when sink check each data's timing.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @acpSeid--The accept stream endpoint id.
 *        @delay--The value of delay.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendDelayReportCmd(uint16 aclHandle, uint16 scid, uint08 label, uint08 acpSeid, uint16 delay);

/******************************************************************************
 * Function: AVDTP Send delay report response interface
 * Descript: Defines trigger the AVDTP send delay report response data to peer
 *           sink AVDTP entity when src support this feature.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendDelayReportRsp(uint16 aclHandle, uint16 scid, uint08 label);

/******************************************************************************
 * Function: AVDTP Send delay report reject interface
 * Descript: Defines trigger the AVDTP send delay report reject data to peer
 *           sink AVDTP entity when src not support this feature.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @label--The transaction id.
 *        @errID--The error id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avdtp_sendDelayReportRej(uint16 aclHandle, uint16 scid, uint08 label, uint08 errID);


#endif // BTP_AVDTP_H

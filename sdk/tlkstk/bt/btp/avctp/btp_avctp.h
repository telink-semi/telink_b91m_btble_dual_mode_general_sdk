/********************************************************************************************************
 * @file	btp_avctp.h
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
#ifndef BTP_AVCTP_H
#define BTP_AVCTP_H


typedef enum {
    BTP_AVCTP_PKT_TYPE_SINGLE   = 0,
    BTP_AVCTP_PKT_TYPE_START    = 1,
    BTP_AVCTP_PKT_TYPE_CONTINUE = 2,
    BTP_AVCTP_PKT_TYPE_END      = 3,
} BTP_AVCTP_PKT_TYPE_ENUM;


/******************************************************************************
 * Function: AVRCP Send Command interface
 * Descript: Defines trigger the avrcp cmd transfer to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @trid--The transaction id.
 *        @pid--product id.
 *        @pHead--The data header which to transfer.
 *        @headLen--The header length.
 *        @pData--payload data.
 *        @dataLen--the data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avrcp_sendCmd(uint16 aclHandle,
                      uint16 scid,
                      uint08 trid,
                      uint16 pid,
                      uint08 *pHead,
                      uint08 headLen,
                      uint08 *pData,
                      uint08 dataLen);

/******************************************************************************
 * Function: AVRCP Send response interface
 * Descript: Defines trigger the avrcp rsp transfer to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @trid--The transaction id.
 *        @pid--product id.
 *        @pHead--The data header which to transfer.
 *        @headLen--The header length.
 *        @pData--payload data.
 *        @dataLen--the data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avrcp_sendRsp(uint16 aclHandle,
                      uint16 scid,
                      uint08 trid,
                      uint16 pid,
                      uint08 *pHead,
                      uint08 headLen,
                      uint08 *pData,
                      uint08 dataLen);

/******************************************************************************
 * Function: AVRCP Send error interface
 * Descript: Defines trigger the avrcp error rsp transfer to peer avrcp entity.
 * Params:
 *        @aclHandle--The Acl Handle identifier.
 *        @scid--The service channel id.
 *        @trid--The transaction id.
 *        @pid--product id.
 *        @pHead--The data header which to transfer.
 *        @headLen--The header length.
 *        @pData--payload data.
 *        @dataLen--the data length.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_avrcp_sendErr(uint16 aclHandle,
                      uint16 scid,
                      uint08 trid,
                      uint16 pid,
                      uint08 *pHead,
                      uint08 headLen,
                      uint08 *pData,
                      uint08 dataLen);


#endif // BTP_AVCTP_H

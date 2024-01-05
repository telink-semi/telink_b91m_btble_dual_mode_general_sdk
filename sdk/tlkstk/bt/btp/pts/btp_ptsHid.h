/********************************************************************************************************
 * @file	btp_ptsHid.h
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
#ifndef BTP_PTS_HID_H
#define BTP_PTS_HID_H

#if (TLKBTP_CFG_PTSHID_ENABLE)


#define BTP_PTSHID_DBG_FLAG          ((TLK_MAJOR_DBGID_BTP << 24) | (TLK_MINOR_DBGID_BTP_PTS << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_PTSHID_DBG_SIGN          nullptr


#define BTP_PTSHID_INPUT_REPORT_ID   0x01
#define BTP_PTSHID_OUTPUT_REPORT_ID  0x02
#define BTP_PTSHID_FEATURE_REPORT_ID 0x03


int btp_ptshid_init(void);

int btp_ptshid_connect(uint16 aclHandle);
int btp_ptshid_disconn(uint16 aclHandle);

void btp_ptshid_destroy(uint16 handle);

int btp_ptshid_sendData(uint16 aclHandle, uint08 reportID, uint08 *pData, uint16 dataLen);
int btp_ptshid_sendDataWithoutReportID(uint16 aclHandle, uint08 reportType, uint08 *pData, uint16 dataLen);

void btp_ptshid_setClassOfDevice(void);


#endif // #if (TLKBTP_CFG_PTSHID_ENABLE)

#endif /* BTP_PTS_HID_H */

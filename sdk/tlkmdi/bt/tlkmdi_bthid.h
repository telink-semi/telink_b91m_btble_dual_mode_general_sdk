/********************************************************************************************************
 * @file	tlkmdi_bthid.h
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
#ifndef TLKMDI_HID_H
#define TLKMDI_HID_H

#if (TLK_MDI_BTHID_ENABLE)


#define TLKMDI_BTHID_REPORT_ID_KEYBOARD_INPUT         1   //!< Keyboard input report ID, Bond with gcBtpSdpHidReportMap.
#define TLKMDI_BTHID_REPORT_ID_CONSUMER_INPUT         2   //!< Consumer Control input report ID, Bond with gcBtpSdpHidReportMap.


typedef struct{
	uint08 enChg; //Enable SetReport
	uint08 bsize; //Buffer Size
	uint08 rtype; //Report Type
	uint08 rptID; //Report ID
	uint08 dlens; //Data Length
	uint08 *pData;
}tlkmdi_bthid_report_t;


int tlkmdi_bthid_init(void);
int tlkmdi_bthid_connect(uint16 aclHandle);
int tlkmdi_bthid_disconn(uint16 aclHandle);

int tlkmdi_bthid_sendData(uint16 aclHandle, uint08 reportID, uint08 reportType, uint08 *pData, uint16 dataLen);
int tlkmdi_bthid_sendDataWithoutReportID(uint16 aclHandle, uint08 reportType, uint08 *pData, uint16 dataLen);



#endif //#if (TLK_MDI_BTHID_ENABLE)

#endif //TLKMDI_HID_H


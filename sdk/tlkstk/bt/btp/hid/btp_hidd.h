/********************************************************************************************************
 * @file	btp_hidd.h
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
#ifndef BTP_HIDD_H
#define BTP_HIDD_H

#if (TLKBTP_CFG_HIDD_ENABLE)


#define BTP_HIDD_GET_BUFFER_SIZE 256

typedef enum {
    BTP_HID_BUSY_NONE                 = 0x0000,
    BTP_HID_BUSY_SEND_HANDSHAKE       = 0x0001,
    BTP_HID_BUSY_SEND_GETREPORT_RSP   = 0x0002,
    BTP_HID_BUSY_SEND_GETPROTOCOL_RSP = 0x0008,
    BTP_HID_BUSY_SEND_CONTROL_RSP     = 0x0020,
} BTP_HIDP_BUSYS_ENUM;


int btp_hidd_init(void);


/******************************************************************************
 * Function: HID device initial interface.
 * Descript: Defines the interface for initial hid connected control block exist.
 * Params: [IN]callback--callback function
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
void btp_hidd_regCB(BtpHidSetReportCallback setReportCB,
                    BtpHidGetReportCallback getReportCB,
                    BtpHidSetProtocolCallback setProtocolCB,
                    BtpHidGetProtocolCallback getProtocolCB,
                    BtpHiddRecvDataCallback ctrDataCB,
                    BtpHiddRecvDataCallback irqDataCB);

/******************************************************************************
 * Function: HID connect interface.
 * Descript: Defines hid connect interface.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
int btp_hidd_connect(uint16 handle);

/******************************************************************************
 * Function: HID disconnect interface.
 * Descript: Defines hid disconnect interface.
 * Params: [IN]handle--ACL handle
 * Return: Returning TLK_ENONE is success.
 *******************************************************************************/
int btp_hidd_disconn(uint16 handle);


/******************************************************************************
 * Function: HID send device report data interface.
 * Descript: Defines send the device data.
 * Params: [IN]pData--the user data
 *         [IN]datalen--the data length
 * Return: Returning The total number of hid control block.
 *******************************************************************************/
int btp_hidd_sendData(uint16 aclHandle, uint08 reportID, uint08 reportType, uint08 *pData, uint16 dataLen);
int btp_hidd_sendDataWithoutReportID(uint16 aclHandle, uint08 reportType, uint08 *pData, uint16 dataLen);

uint16 btp_hidd_getAnyConnHandle(void);


void btp_hidd_connectEvt(btp_hid_item_t *pItem);
void btp_hidd_disconnEvt(btp_hid_item_t *pItem);

void btp_hidd_ctrRdataCB(btp_hid_item_t *pHid, uint08 *pData, uint16 dataLen);
void btp_hidd_irqRdataCB(btp_hid_item_t *pHid, uint08 *pData, uint16 dataLen);


#endif // #if (TLKBTP_CFG_HIDD_ENABLE)

#endif // BTP_HIDD_H

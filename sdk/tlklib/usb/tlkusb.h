/********************************************************************************************************
 * @file	tlkusb.h
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
#ifndef TLKUSB_H
#define TLKUSB_H

#if (TLK_CFG_USB_ENABLE)


//Module type
typedef enum{
	TLKUSB_MODTYPE_UDB = 0,
	TLKUSB_MODTYPE_UAC,
	TLKUSB_MODTYPE_MSC,
	TLKUSB_MODTYPE_CDC,
	TLKUSB_MODTYPE_HID,
	TLKUSB_MODTYPE_USR,
	TLKUSB_MODTYPE_MAX,
}TLKUSB_MODTYPE_ENUM;


int tlkusb_init(uint16 usbID);

int tlkusb_open(TLKUSB_MODTYPE_ENUM modType);
int tlkusb_close(void);

void tlkusb_handler(void);


bool tlkusb_isBusy(void);
void tlkusb_enterSleep(uint mcuMode);
void tlkusb_leaveSleep(uint wakeSrc);


#endif //#if (TLK_CFG_USB_ENABLE)

#endif //TLKUSB_H


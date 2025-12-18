/********************************************************************************************************
 * @file	tlkusb_cdcCtrl.h
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
#ifndef TLKUSB_CDC_CTRL_H
#define TLKUSB_CDC_CTRL_H

#if (TLK_USB_CDC_ENABLE)




typedef struct{
	struct{
		uint08 ControlInterfaceNumber;
		uint08 DataINEndpointNumber;
		uint16 DataINEndpointSize;
		uint08 DataINEndpointDoubleBank;
		uint08 DataOUTEndpointNumber;
		uint16 DataOUTEndpointSize;
		uint08 DataOUTEndpointDoubleBank;
		uint08 NotificationEndpointNumber;
		uint16 NotificationEndpointSize;
		uint08 NotificationEndpointDoubleBank;
	}config;
	struct{
		uint16 HostToDevice;
		uint16 DeviceToHost;
		uint32 BaudRateBPS;
		uint08 CharFormat;
		uint08 ParityType;
		uint08 DataBits;
	}param;
} tlkusb_cdc_param_t;




#endif //#if (TLK_USB_CDC_ENABLE)

#endif //TLKUSB_CDC_CTRL_H


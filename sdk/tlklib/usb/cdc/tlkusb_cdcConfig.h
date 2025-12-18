/********************************************************************************************************
 * @file	tlkusb_cdcConfig.h
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
#ifndef TLKUSB_CDC_CONFIG_H
#define TLKUSB_CDC_CONFIG_H

#if (TLK_USB_CDC_ENABLE)



#define TLKUSB_CDC_SECOND_ENABLE         1


/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define TLKUSB_CDC_NOTIFY_EDPSIZE        8

/** Size in bytes of the CDC data IN and OUT endpoints. */
#define TLKUSB_CDC_TXRX_EDPSIZE          64



#endif //#if (TLK_USB_CDC_ENABLE)

#endif //TLKUSB_CDC_CONFIG_H


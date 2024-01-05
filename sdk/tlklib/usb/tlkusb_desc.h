/********************************************************************************************************
 * @file	tlkusb_desc.h
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
#ifndef TLKUSB_DESC_H
#define TLKUSB_DESC_H


#if (TLK_CFG_USB_ENABLE)



uint16 tlkusb_getLanguageLens(void);
uint16 tlkusb_getProductLens(void);
uint16 tlkusb_getVendorLens(void);
uint16 tlkusb_getSerialLens(void);

uint16 tlkusb_getDeviceLens(void);
uint16 tlkusb_getConfigLens(void);


uint08 *tlkusb_getLanguageDesc(void);
uint08 *tlkusb_getVendorDesc(void);
uint08 *tlkusb_getProductDesc(void);
uint08 *tlkusb_getSerialDesc(void);

uint08 *tlkusb_getDeviceDesc(void);
uint08 *tlkusb_getConfigDesc(void);



#endif //#if (TLK_CFG_USB_ENABLE)

#endif //TLKUSB_DESC_H


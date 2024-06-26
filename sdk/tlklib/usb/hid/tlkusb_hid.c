/********************************************************************************************************
 * @file	tlkusb_hid.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLK_USB_HID_ENABLE)
#include "drivers.h"
#include "tlklib/usb/hid/tlkusb_hidConfig.h"
#include "tlklib/usb/hid/tlkusb_hidDefine.h"
#include "tlklib/usb/hid/tlkusb_hid.h"
#include "tlklib/usb/hid/tlkusb_hidDesc.h"
#include "tlklib/usb/hid/tlkusb_hidCtrl.h"



extern const tlkusb_modCtrl_t sTlkUsbHidModCtrl;
extern const tlkusb_modDesc_t sTlkUsbHidModDesc;
const tlkusb_module_t gTlkUsbHidModule = {
	TLKUSB_MODTYPE_HID,
	&sTlkUsbHidModDesc,
	&sTlkUsbHidModCtrl,
};


int tlkusb_hid_init(void)
{
	return TLK_ENONE;
}




#endif //#if (TLK_USB_HID_ENABLE)



/********************************************************************************************************
 * @file	tlkusb_hidConfig.h
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
#ifndef TLKUSB_HID_CONFIG_H
#define TLKUSB_HID_CONFIG_H

#if (TLK_USB_HID_ENABLE)



#define TLKUSB_HID_MOUSE_ENABLE             1
#define TLKUSB_HID_KEYBOARD_ENABLE          1


#define TLKUSB_HID_MOUSE_POLL_INTERVAL      4  // in ms
#define TLKUSB_HID_KEYBOARD_POLL_INTERVAL   10 // in ms

#define TLKUSB_HID_REPORT_ID_AUDIO          1 
#define TLKUSB_HID_REPORT_ID_MOUSE          2 // mouse
#define TLKUSB_HID_REPORT_ID_KB_MEDIA       3 // media
#define TLKUSB_HID_REPORT_ID_KB_SYS         4 // system : power,sleep,wakeup
#define TLKUSB_HID_REPORT_ID_SOMATIC        5 // somatic sensor,  may have many report ids



#endif //#if (TLK_USB_HID_ENABLE)

#endif //TLKUSB_HID_CONFIG_H


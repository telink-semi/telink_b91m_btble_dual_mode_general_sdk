/********************************************************************************************************
 * @file	tlkdbg_usbvcd.h
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
#ifndef TLKDBG_USB_VCD_H
#define TLKDBG_USB_VCD_H

#if (TLK_DBG_LOG_ENABLE)
#if (TLKDBG_CFG_USB_VCD_ENABLE)




void tlkdbg_usbvcd_init(void);
void tlkdbg_usbvcd_reset(void);
bool tlkdbg_usbvcd_isBusy(void);
void tlkdbg_usbvcd_handler(void);

void tlkdbg_usbvcd_ref(void);
void tlkdbg_usbvcd_sync(void);
void tlkdbg_usbvcd_tick(uint08 id);
void tlkdbg_usbvcd_level(uint08 id, uint08 level);
void tlkdbg_usbvcd_event(uint08 id);
void tlkdbg_usbvcd_byte(uint08 id, uint08 value);
void tlkdbg_usbvcd_word(uint08 id, uint16 value);



#endif //#if (TLKDBG_CFG_USB_VCD_ENABLE)
#endif //#if (TLK_DBG_LOG_ENABLE)

#endif //TLKDBG_USB_VCD_H


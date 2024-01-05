/********************************************************************************************************
 * @file	tlkusb_uac.h
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
#ifndef TLKUSB_AUDIO_H
#define TLKUSB_AUDIO_H

#if (TLK_USB_UAC_ENABLE)



typedef void(*TlkUsbUacStatusChangeCB)(bool enable, uint08 channel, uint08 bitDepth, uint32 sampleRate);


int tlkusb_uac_init(void);


extern void tlkusb_uacmic_autoZero(bool enable);
extern void tlkusb_uacspk_autoFlush(bool enable);

extern bool tlkusb_uacmic_sendData(uint08 *pData, uint16 dataLen, bool isCover);
extern uint tlkusb_uacspk_readData(uint08 *pBuff, uint16 buffLen, bool isParty);

extern void tlkusb_uacmic_regStatusChangeCB(TlkUsbUacStatusChangeCB cb);
extern void tlkusb_uacspk_regStatusChangeCB(TlkUsbUacStatusChangeCB cb);



#endif //#if (TLK_USB_UAC_ENABLE)

#endif //TLKUSB_AUDIO_H


/********************************************************************************************************
 * @file	tlkusb_udb.h
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
#ifndef TLKUSB_UDB_H
#define TLKUSB_UDB_H


#if (TLK_USB_UDB_ENABLE)


typedef void(*TlkUsbUsrDebugCB)(uint08 *pData, uint16 dataLen);


int tlkusb_udb_init(void);


void tlkusb_udb_regEqCB(TlkUsbUsrDebugCB cb);
void tlkusb_udb_regDbgCB(TlkUsbUsrDebugCB cb);

void tlkusb_udb_recvHandler(void);

void tlkusb_udb_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen);


int tlkusb_udb_sendData(uint08 *pData, uint08 dataLen);


#endif //#if (TLK_USB_UDB_ENABLE)

#endif //TLKUSB_UDB_H


/********************************************************************************************************
 * @file	tlkmmi_rdt_le.h
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
#ifndef TLKMMI_RDT_LE_H
#define TLKMMI_RDT_LE_H

#if (TLK_TEST_RDT_ENABLE)


typedef void(*RdtLeConnectCB)(uint16 handle, uint08 status);
typedef void(*RdtLeEncryptCB)(uint16 handle, uint08 status);
typedef void(*RdtLeDisconnCB)(uint16 handle, uint08 reason);


extern int tlkmmi_rdt_leInit(void);

extern void tlkmmi_rdt_leStartAdv(void);
extern void tlkmmi_rdt_leCloseAdv(void);

extern void tlkmmi_rdt_leDisconn(uint16 handle, uint08 reason);
extern void tlkmmi_rdt_leClsPeerInfo(void);

extern void tlkmmi_rdt_leRegConnectCB(RdtLeConnectCB connectCB);
extern void tlkmmi_rdt_leRegEncryptCB(RdtLeEncryptCB encryptCB);
extern void tlkmmi_rdt_leRegDisconnCB(RdtLeDisconnCB disconnCB);

extern void tlkmmi_rdt_leUpdateConnParam(uint16 handle, uint16 timeMs, uint08 latency);



#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_LE_H


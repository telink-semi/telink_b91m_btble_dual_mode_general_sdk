/********************************************************************************************************
 * @file	tlkmmi_rdt_bt.h
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
#ifndef TLKMMI_RDT_BT_H
#define TLKMMI_RDT_BT_H

#if (TLK_TEST_RDT_ENABLE)



#define TLKMMI_BTRDT_DEVICE_CLASS     0x240404//0x5a020c

#define TLKMMI_BTRDT_BDADDR_DEF       {0x66, 0x66, 0x55, 0x55, 0x11, 0x11}
#define TLKMMI_BTRDT_BTNAME_DEF       "Telink-Rdt-Host"

typedef void(*RdtBtAclConnectCB)(uint16 handle, uint08 status);
typedef void(*RdtBtAclEncryptCB)(uint16 handle, uint08 status);
typedef void(*RdtBtAclDisconnCB)(uint16 handle, uint08 reason);
typedef void(*RdtBtScoConnectCB)(uint16 handle, uint08 status);
typedef void(*RdtBtScoDisconnCB)(uint16 handle, uint08 reason);
typedef void(*RdtBtPrfConnectCB)(uint16 handle, uint08 ptype, uint08 status);
typedef void(*RdtBtPrfDisconnCB)(uint16 handle, uint08 ptype, uint08 reason);
typedef void(*RdtBtPrfSPPsendCB)(uint16 aclHandle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);

int tlkmmi_rdt_btInit(void);


extern void tlkmmi_rdt_btRegAclConnectCB(RdtBtAclConnectCB connectCB);
extern void tlkmmi_rdt_btRegAclEncryptCB(RdtBtAclEncryptCB encryptCB);
extern void tlkmmi_rdt_btRegAclDisconnCB(RdtBtAclDisconnCB disconnCB);
extern void tlkmmi_rdt_btRegScoConnectCB(RdtBtScoConnectCB connectCB);
extern void tlkmmi_rdt_btRegScoDisconnCB(RdtBtScoDisconnCB disconnCB);
extern void tlkmmi_rdt_btRegPrfConnectCB(RdtBtPrfConnectCB connectCB);
extern void tlkmmi_rdt_btRegPrfDisconnCB(RdtBtPrfDisconnCB disconnCB);

extern int tlkmmi_rdt_btStartScan(bool enInqScan, bool enPageScan);
extern int tlkmmi_rdt_btCloseScan(void);

extern int tlkmmi_rdt_btConnect(uint08 btaddr[6], uint32 devClass, uint08 initRole);
extern int tlkmmi_rdt_btDisconn(uint16 handle);

extern void tlkmmi_rdt_btClsPeerInfo(void);

extern void tlkmmi_rdt_btSetName(uint08 *pName, uint08 nameLen);
extern void tlkmmi_rdt_btSetAddr(uint08 *pAddr);

#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_BT_H


/********************************************************************************************************
 * @file	tlkmmi_rdt_btCore.h
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
#ifndef TLKMMI_RDT_BTCORE_H
#define TLKMMI_RDT_BTCORE_H

#if (TLK_TEST_RDT_ENABLE)




int tlkmmi_rdt_btCoreInit(void);


void tlkmmi_rdt_btRegAclConnectCB(RdtBtAclConnectCB connectCB);
void tlkmmi_rdt_btRegAclEncryptCB(RdtBtAclEncryptCB encryptCB);
void tlkmmi_rdt_btRegAclDisconnCB(RdtBtAclDisconnCB disconnCB);
void tlkmmi_rdt_btRegScoConnectCB(RdtBtScoConnectCB connectCB);
void tlkmmi_rdt_btRegScoDisconnCB(RdtBtScoDisconnCB disconnCB);
void tlkmmi_rdt_btRegPrfConnectCB(RdtBtPrfConnectCB connectCB);
void tlkmmi_rdt_btRegPrfDisconnCB(RdtBtPrfDisconnCB disconnCB);

int tlkmmi_rdt_btStartScan(bool enInqScan, bool enPageScan);
int tlkmmi_rdt_btCloseScan(void);

int tlkmmi_rdt_btConnect(uint08 btaddr[6], uint32 devClass, uint08 initRole);
int tlkmmi_rdt_btDisconn(uint16 handle);

void tlkmmi_rdt_btClsPeerInfo(void);


void tlkmmi_rdt_btSetName(uint08 *pName, uint08 nameLen);
void tlkmmi_rdt_btSetAddr(uint08 *pAddr);



#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_BTCORE_H


/********************************************************************************************************
 * @file	tlkmmi_rdt_leCore.h
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
#ifndef TLKMMI_RDT_LECORE_H
#define TLKMMI_RDT_LECORE_H

#if (TLK_TEST_RDT_ENABLE)


#define TLKMMI_RDT_LE_PUBLIC_ADDR     {0xC7, 0xCC, 0xC7, 0xCC, 0x01, 0x01}
#define TLKMMI_RDT_LE_RANDOM_ADDR     {0xC7, 0xCC, 0xC7, 0xCC, 0x01, 0x01}


#define TLKMMI_RDT_LE_M_ROLE_NUMB         0
#define TLKMMI_RDT_LE_S_ROLE_NUMB         1

#define TLKMMI_RDT_LE_MAX_RX_SIZE         64//27
#define TLKMMI_RDT_LE_M_MAX_RX_SIZE       (TLKMMI_RDT_LE_MAX_RX_SIZE)
#define TLKMMI_RDT_LE_S_MAX_RX_SIZE       (TLKMMI_RDT_LE_MAX_RX_SIZE)

#define TLKMMI_RDT_LE_RX_FIFO_SIZE        CAL_LL_ACL_RX_FIFO_SIZE(TLKMMI_RDT_LE_MAX_RX_SIZE) // TLKMMI_RDT_LE_MAX_RX_SIZE + 21, then 16 Byte align
#define TLKMMI_RDT_LE_RX_FIFO_NUMB        8 // must be: 2^n
#define TLKMMI_RDT_LE_S_TX_FIFO_SIZE      CAL_LL_ACL_TX_FIFO_SIZE(TLKMMI_RDT_LE_S_MAX_RX_SIZE) // TLKMMI_RDT_LE_M_MAX_RX_SIZE + 10, then 16 Byte align
#define TLKMMI_RDT_LE_S_TX_FIFO_NUMB      9 // must be: (2^n) + 1

#define TLKMMI_RDT_LE_MTU_SIZE            256//23
#define	TLKMMI_RDT_LE_S_MTU_SIZE          CAL_MTU_BUFF_SIZE(TLKMMI_RDT_LE_MTU_SIZE)


int tlkmmi_rdt_leCoreInit(void);

void tlkmmi_rdt_leStartAdv(void);
void tlkmmi_rdt_leCloseAdv(void);

void tlkmmi_rdt_leDisconn(uint16 handle, uint08 reason);
void tlkmmi_rdt_leClsPeerInfo(void);

void tlkmmi_rdt_leRegConnectCB(RdtLeConnectCB connectCB);
void tlkmmi_rdt_leRegEncryptCB(RdtLeEncryptCB encryptCB);
void tlkmmi_rdt_leRegDisconnCB(RdtLeDisconnCB disconnCB);

void tlkmmi_rdt_leUpdateConnParam(uint16 handle, uint16 timeMs, uint08 latency);



#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_LECORE_H


/********************************************************************************************************
 * @file	tlkmdi_le_command.h
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
#ifndef TLKMDI_LE_COMMAND_H
#define TLKMDI_LE_COMMAND_H

#if (1)
#include "tlkstk/ble/ble.h"

#define TLKMDI_LE_COMMAND_FLAG       ((TLK_MAJOR_DBGID_MDI_LE << 24) | (TLK_MINOR_DBGID_MDI_LE_COMMAND << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_LE_COMMAND_SIGN       "[MDI]"



int tlkmdi_le_startAdv(void);


int tlkmdi_le_closeAdv(void);


int tlkmdi_le_disconn(uint16 handle,u8 reason);


int tlkmdi_le_setAclName(uint08 *pName, uint08 nameLen);


int tlkmdi_le_setAclAddr(uint08 *pAddr, uint08 addrLen);


bool tlkmdi_le_aclVolumeInc(uint16 handle);


bool tlkmdi_le_aclVolumeDec(uint16 handle);


int tlkmdi_le_setAddr1(uint08 *pPubAddr, uint08 *pRndAddr);


int tlkmdi_le_createConnection(uint08 pAddr_type,uint08 *pAddr);


int tlkmdi_le_startScan(void);


int tlkmdi_le_closeScan(void);
#endif //TLK_MDI_LEACL_ENABLE

#endif //TLKMDI_LE_COMMAND_H


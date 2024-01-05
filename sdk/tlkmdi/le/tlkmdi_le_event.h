/********************************************************************************************************
 * @file	tlkmmi_lemgrAcl.h
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
#ifndef TLKMDI_LEMGR_EVENT_H
#define TLKMDI_LEMGR_EVENT_H

#if (1)
#include "tlkstk/ble/ble.h"

#define TLKMDI_LE_EVENT_FLAG       ((TLK_MAJOR_DBGID_MDI_LE << 24) | (TLK_MINOR_DBGID_MDI_LE_EVENT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_LE_EVENT_SIGN       "[MDI]"

/**
 * @brief  TlkMdiLeAclCallback
*/
typedef int (*TlkMdiLeAclCallback) (uint08 *para, uint16 n);

/******************************************************************************
 * Function: tlkmdi_le_aclReg****
 * Descript: Register callbacks
 * Params:  TlkMdiLeAclCallback
 * Return:	None
 * Others:  None.
*******************************************************************************/
void tlkmdi_le_aclRegDiscCB(TlkMdiLeAclCallback callback);

void tlkmdi_le_aclRegConCB(TlkMdiLeAclCallback callback);

void tlkmdi_le_aclRegConUpdateCB(TlkMdiLeAclCallback callback);

void tlkmdi_le_aclRegPhyUpdateCB(TlkMdiLeAclCallback callback);

void tlkmdi_le_aclRegAdvReportCB(TlkMdiLeAclCallback callback);


#endif //TLK_MDI_LEACL_ENABLE

#endif //TLKMDI_LEMGR_EVENT_H


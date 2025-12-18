/********************************************************************************************************
 * @file	tlkmdi_lemgrAcl.h
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
#ifndef TLKMDI_LEMGR_ACL_H
#define TLKMDI_LEMGR_ACL_H

#if (TLK_MDI_LEACL_ENABLE)
#include "tlkstk/ble/ble.h"

#define TLKMDI_LE_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_LE << 24) | (TLK_MINOR_DBGID_MDI_LE_ACL << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_LE_DBG_SIGN       "[MDI]"


#define TLKMDI_LE_CENTRAL_NUM_ENABLE             (ACL_CENTRAL_MAX_NUM)
#define TLKMDI_LE_CENTRAL_SMP_ENABLE             (1 && TLK_LE_CENTRAL_SMP_ENABLE && (TLKMDI_LE_CENTRAL_NUM_ENABLE > 0))
#define TLKMDI_LE_CENTRAL_SDP_ENABLE             (0 && (TLKMDI_LE_CENTRAL_NUM_ENABLE > 0))
#define TLKMDI_LEMGR_OTA_SERVER_ENABLE            0

#if ((TLKMDI_LE_CENTRAL_NUM_ENABLE > 0)&&(TLK_CFG_PM_ENABLE))
#warning " master does not support pm"
#endif


int tlkmdi_le_aclInit(void);


void tlkmdi_le_aclDeepInit(void);


void tlkmdi_le_sdpTask(void);

#endif //TLK_MDI_LEACL_ENABLE

#endif //TLKMDI_LEMGR_ACL_H


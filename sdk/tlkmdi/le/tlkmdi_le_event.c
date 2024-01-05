/********************************************************************************************************
 * @file	tlkmdi_le_event.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_MDI_LEACL_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmdi/le/tlkmdi_le_common.h"
#include "tlkmdi/le/tlkmdi_le_device_manage.h"
#include "tlkmdi/le/tlkmdi_le_simple_sdp.h"
#include "tlkmdi/le/tlkmdi_le_custom_pair.h"
#include "tlkmdi/le/tlkmdi_lemgrAtt.h"
#include "tlkmdi/le/tlkmdi_leAcl.h"
#include "tlkmdi/le/tlkmdi_le_command.h"
#include "tlkmdi/le/tlkmdi_le_event.h"



 TlkMdiLeAclCallback sTlkMdiLeAclDiscCB;
 TlkMdiLeAclCallback sTlkMdiLeAclConCB;
 TlkMdiLeAclCallback sTlkMdiLeAclConUpdateCB;
 TlkMdiLeAclCallback sTlkMdiLeAclPhyUpdateCB;
 TlkMdiLeAclCallback sTlkMdiLeAclAdvReportCB;


void tlkmdi_le_aclRegDiscCB(TlkMdiLeAclCallback callback)
{
	sTlkMdiLeAclDiscCB = callback;
}

void tlkmdi_le_aclRegConCB(TlkMdiLeAclCallback callback)
{
	sTlkMdiLeAclConCB = callback;
}

void tlkmdi_le_aclRegConUpdateCB(TlkMdiLeAclCallback callback)
{
	sTlkMdiLeAclConUpdateCB = callback;
}

void tlkmdi_le_aclRegPhyUpdateCB(TlkMdiLeAclCallback callback)
{
	sTlkMdiLeAclPhyUpdateCB = callback;
}

void tlkmdi_le_aclRegAdvReportCB(TlkMdiLeAclCallback callback)
{
	sTlkMdiLeAclAdvReportCB = callback;
}

#endif


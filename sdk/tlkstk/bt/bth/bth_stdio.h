/********************************************************************************************************
 * @file	bth_stdio.h
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
#ifndef BTH_STDIO_H
#define BTH_STDIO_H


#include "tlkstk/bt/bth/bth_config.h"
#include "tlkstk/bt/bth/bth_define.h"
#include "tlkstk/bt/bth/bth_device.h"
#include "tlkstk/bt/bth/bth_struct.h"
#include "tlkstk/bt/bth/bth_handle.h"
#include "tlkstk/bt/bth/bth_hcicod.h"
#include "tlkstk/bt/bth/bth_hcicmd.h"
#include "tlkstk/bt/bth/bth_event.h"
#include "tlkstk/bt/bth/bth.h"
#include "tlkstk/tlkstk_mem.h"

extern void bth_acl_enterSniff(uint16 aclHandle, bool isForce);
extern void bth_acl_leaveSniff(uint16 aclHandle, uint16 busyTime);


#endif // BTH_STDIO_H

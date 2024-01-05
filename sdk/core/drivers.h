/********************************************************************************************************
 * @file	drivers.h
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
#pragma once


#if (MCU_CORE_TYPE == MCU_CORE_B91)
#include "tlkdrv/B91/driver_b91.h"
#include "tlkdrv/B91/compatibility_pack/cmpt.h"
#include "tlkdrv/B91/ext_driver/driver_ext.h"
#endif

#if (MCU_CORE_TYPE == MCU_CORE_B92)
#include "tlkdrv/B92/compatibility_pack/cmpt.h"
#include "tlkdrv/B92/driver.h"
#include "tlkdrv/B92/ext_driver/driver_ext.h"
#endif

#include "types.h"
#include "bit.h"
#include "string.h"
#include "bit.h"




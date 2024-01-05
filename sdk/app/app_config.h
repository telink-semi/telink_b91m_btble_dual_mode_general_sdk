/********************************************************************************************************
 * @file	app_config.h
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
#ifndef APP_CONFIG_H
#define APP_CONFIG_H



#ifdef TLKAPP_GENERAL_ENABLE
	#include "app/tlkapp_general/tlkapp_config.h"
#endif
#ifdef TLKAPP_CONTROLLER_ENABLE
	#include "app/tlkapp_controller/tlkapp_config.h"
#endif
#ifdef TLKAPP_BOOTLOADER_ENABLE
	#include "app/tlkapp_bootloader/tlkapp_config.h"
#endif
#ifdef TLKAPP_DUALMODE_DONGLE_ENABLE
	#include "app/tlkapp_general/tlkapp_config.h"		//Reuse general app code, diff on MMI lager.
#endif
#ifdef TLKAPP_RFTEST_ENABLE
	#include "app/tlkapp_rftest/tlkapp_config.h"
#endif


#endif /* APP_CONFIG_H */


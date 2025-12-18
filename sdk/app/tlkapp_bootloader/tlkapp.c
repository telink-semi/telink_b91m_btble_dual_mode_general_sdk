/********************************************************************************************************
 * @file	tlkapp.c
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
#include "tlkapp_config.h"
#include "tlkapp_irq.h"
#include "tlkapp.h"
#include "tlkapp_dfu.h"
#include "drivers.h"






/******************************************************************************
 * Function: tlkapp_init
 * Descript: user initialization when MCU power on or wake_up from deepSleep mode.
 * Params: None.
 * Return: TLK_ENONE is success.
 * Others: None.
*******************************************************************************/  
int tlkapp_init(void)
{
	
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkapp_handler
 * Descript: BTBLE SDK main loop.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
volatile uint32 AAAA_trace01 = 0;
volatile uint32 AAAA_trace02 = 0;
volatile uint32 AAAA_trace03 = 0;
volatile uint32 AAAA_trace04 = 0;

void tlkapp_handler(void)
{
	AAAA_trace01 ++;
	AAAA_trace03 ++;

	if(AAAA_trace02 == 1){
		tlkapp_dfu_load();
		AAAA_trace02 = 0;
//		TLKAPP_JUMP_TO_APP();
	}

	if(AAAA_trace04 == 1){
		AAAA_trace04 = 0;
		TLKAPP_JUMP_TO_APP();
	}
	
}



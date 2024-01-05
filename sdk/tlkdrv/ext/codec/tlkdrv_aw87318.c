/********************************************************************************************************
 * @file	tlkdrv_aw87318.c
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
#include "tlkdrv_codec.h"
#include "tlkdrv_aw87318.h"
#include "drivers.h"


static uint08 gTlkDrvAw87318IsOpen = false;


bool tlkdrv_aw87318_isOpen(void)
{
	return gTlkDrvAw87318IsOpen;
}

void tlkdrv_aw87318_init(void)
{
	gpio_function_en(TLKDRV_AW87318_ENABLE_PIN);
	gpio_output_en(TLKDRV_AW87318_ENABLE_PIN);
	gpio_set_low_level(TLKDRV_AW87318_ENABLE_PIN);
}

void tlkdrv_aw87318_open(void)
{
	if(gTlkDrvAw87318IsOpen) return;
	
	gpio_function_en(TLKDRV_AW87318_ENABLE_PIN);
	gpio_output_en(TLKDRV_AW87318_ENABLE_PIN);
	gpio_set_high_level(TLKDRV_AW87318_ENABLE_PIN);
	
	gTlkDrvAw87318IsOpen = true;
}
void tlkdrv_aw87318_close(void)
{
	if(!gTlkDrvAw87318IsOpen) return;
	
	gTlkDrvAw87318IsOpen = false;
	gpio_set_low_level(TLKDRV_AW87318_ENABLE_PIN);
}


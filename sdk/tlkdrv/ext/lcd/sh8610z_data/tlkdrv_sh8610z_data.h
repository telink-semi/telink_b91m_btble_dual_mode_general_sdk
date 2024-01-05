/********************************************************************************************************
 * @file	tlkdrv_sh8610z_data.h
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
typedef struct{
	uint32 oled_addr;//oled operating register address
	uint08 oled_data;//oled data written to operating register address
}tlkdrv_sh8610z_dataCfg_t;

////rgb configuration parameter
volatile const tlkdrv_sh8610z_dataCfg_t scTlkDrvSh8610zRgb565Cfg = {0x003A00, 0x55};
volatile const tlkdrv_sh8610z_dataCfg_t scTlkDrvSh8610zRgb666Cfg = {0x003A00, 0x66};
volatile const tlkdrv_sh8610z_dataCfg_t scTlkDrvSh8610zRgb888Cfg = {0x003A00, 0x77};


/********************************************************************************************************
 * @file     ext_pm.h
 *
 * @brief    This is the header file for BTBLE SDK
 *
 * @author	 BTBLE GROUP
 * @date         2,2022
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#ifndef DRIVERS_B91_DRIVER_EXT_EXT_PM_H_
#define DRIVERS_B91_DRIVER_EXT_EXT_PM_H_

#include "../pm.h"
#include "types.h"


static inline unsigned int pm_get_latest_offset_cal_time(void)
{
	return pmcd.offset_cal_tick;
}

extern	unsigned int 			g_sleep_32k_rc_cnt;
extern	unsigned int 			g_sleep_stimer_tick;

extern	unsigned int		ota_program_bootAddr;
extern	unsigned int		ota_program_offset;


/**
 * @brief   internal oscillator calibration for environment change such as voltage, temperature
 * 			to keep some critical PM or RF performance stable
 * 			attention: this is a stack API, user can not call it
 * @param	none
 * @return	none
 */
void mcu_oscillator_crystal_calibration(void);


#endif /* DRIVERS_B91_DRIVER_EXT_EXT_PM_H_ */

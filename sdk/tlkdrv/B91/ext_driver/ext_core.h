/********************************************************************************************************
 * @file     ext_core.h
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
#ifndef EXT_CORE_H
#define EXT_CORE_H

#define	TICK_PER_US					SYSTEM_TIMER_TICK_1US
#define TICK_PER_HALF_US            (TICK_PER_US/2)


typedef enum{
	SYS_CHIP_CLOCK_96M = 1,
	SYS_CHIP_CLOCK_48M = 2,
	SYS_CHIP_CLOCK_32M = 3,
	SYS_CHIP_CLOCK_24M = 4,
	SYS_CHIP_CLOCK_16M = 5,
}SYS_CHIP_CLOCK_ENUM;



void tlksys_switch_clock(SYS_CHIP_CLOCK_ENUM clock);


void tlksys_interrupt_enable(void);


void tlksys_disable_interrupt(void);
void tlksys_restore_interrupt(void);

void tlksys_enter_critical(void);
void tlksys_leave_critical(void);



#endif //EXT_CORE_H


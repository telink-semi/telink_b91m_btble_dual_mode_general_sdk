/********************************************************************************************************
 * @file	ll_pm.h
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
#ifndef LL_PM_H_
#define LL_PM_H_

#include "drivers.h"




void blc_ll_recoverDeepRetention(void);




/**
 * @brief	for user to initialize low power mode
 * @param	none
 * @return	none
 */
void 		blc_ll_initPowerManagement_module(void);



/**
 * @brief	for user to set latency manually for save power
 * @param	latency - bltPm.user_latency
 * @return	none
 */
void 		bls_pm_setManualLatency(u16 latency); //manual set latency to save power



/**
 * @brief	for user to set the threshold of sleep tick for entering deep retention mode
 * @param	adv_thres_ms - the threshold of sleep tick for advertisement state
 * @param	conn_thres_ms - the threshold of sleep tick for connection state
 * @return  none.
 */
void 		blc_pm_setDeepsleepRetentionThreshold(u32 adv_thres_ms, u32 conn_thres_ms);

/**
 * @brief	for user to set early wake up tick for deep retention mode
 * @param	earlyWakeup_us - early wake up tick for deep retention mode
 * @return  none.
 */
void 		blc_pm_setDeepsleepRetentionEarlyWakeupTiming(u32 earlyWakeup_us);

/**
 * @brief	for user to set the type of deep retention mode
 * @param	sleep_type - the type of deep retention mode
 * @return  none.
 */
//void 		blc_pm_setDeepsleepRetentionType(pm_sleep_mode_e sleep_type);






#endif /* LL_PM_H_ */

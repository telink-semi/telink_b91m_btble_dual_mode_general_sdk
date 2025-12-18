/********************************************************************************************************
 * @file	btble_pm.h
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
#ifndef STACK_BTBLE_BTBLE_PM_H_
#define STACK_BTBLE_BTBLE_PM_H_


#include "btble_scheduler.h"





/**
 *  @brief BTBLE controller base_band & link_layer low power state element, user can select one of them or combine some state
 *
 *  for example:
 *  SLEEP_DISABLE
 *  SLEEP_BT_ACL_SLAVE
 *  SLEEP_BLE_LEG_ADV | SLEEP_BLE_ACL_SLAVE
 *	SLEEP_BT_ACL_SLAVE | SLEEP_BLE_LEG_ADV | SLEEP_BLE_ACL_SLAVE
 */
typedef enum {
	SLEEP_DISABLE				= 0,

	/* BT base_band state */
	SLEEP_BT_INQUIRY_SCAN		= BIT(0),	//now not support
	SLEEP_BT_PAGE_SCAN 			= BIT(1),	//now not support
	SLEEP_BT_ACL_SLAVE			= BIT(2),	//now only support: slave enter sniff mode
//	SLEEP_BB_SCO				= BIT(3),
//	SLEEP_BB_INQUIRY	 		= BIT(4),
//	SLEEP_BB_ACL_MASTER 		= BIT(6),

	/* BLE link_layer state */
	SLEEP_BLE_LEG_ADV			= BIT(8),
	SLEEP_BLE_ACL_SLAVE			= BIT(9),
//	SLEEP_LL_LEG_SCAN 			= BIT(10),	//now not support
//	SLEEP_LL_ACL_MASTER 		= BIT(11),	//now not support
}sleep_state_cfg_t;




/**
 *  @brief sleep type for user to select
 *  @attention  1. if user set "SLEEP_DEEPSLEEP_RETENTION", MCU not always enter this sleep mode, it will meet some other
 *                 condition for this mode, e.g. stack support this mode, and sleep timing is big enough
 *              2. deepsleep mode(deepsleep without retention) is not maintained by stack PM module, if user want use
 *                 this sleep type, should call driver API "cpu_sleep_wakeup" directly in main_loop
 *
 */
typedef enum {
	SLEEP_TYPE_SUSPEND 				= 0x00,
	SLEEP_TYPE_DEEPSLEEP_RETENTION 	= 0x01,
}sleep_type_t;




/**
 * @brief	Telink defined application wake up low power mode process callback function
 */
typedef 	void (*pm_appWakeupLowPower_callback_t)(int);




/**
 * @brief	for user to initialize low power mode
 * @param	none
 * @return	none
 */
void 		btble_pm_initPowerManagement_module(void);


/**
 * @brief	for user to configure low power combination for BTBLT controller state machine
 * @param	slp_cfg - BT baseband and BLE link_layer sleep state combination
 * @return	none
 */
void 		btble_pm_setSleepEnable (sleep_state_cfg_t slp_cfg);


/**
 * @brief	for user to select sleep type
 * @param	slp_type - low power mode mask
 * @return	none
 */
void 		btble_pm_setSleepType (sleep_type_t slp_type);




/**
 * @brief	for user to set low power mode wake up source
 * @param	source - low power mode wake up source, now only "PM_WAKEUP_PAD" works
 * @return	none
 */
void 		btble_pm_setWakeupSource(pm_sleep_wakeup_src_e source);



/**
 * @brief	for user to get low power mode wake up time
 * @param	none
 * @return	bltPm.current_wakeup_tick
 */
uint32_t 	btble_pm_getSystemWakeupTick(void);







/**
 * @brief	for user to set application wake up low power mode
 * @param	wakeup_tick - low power mode wake up time
 * @param	enable - low power mode application wake up enable
 * @return	none
 */
void 		btble_pm_setAppWakeupLowPower(uint32_t wakeup_tick, uint8_t enable);

/**
 * @brief	for user to register the callback for application wake up low power mode process
 * @param	cb - the pointer of callback function
 * @return  none.
 */
void 		btble_pm_registerAppWakeupLowPowerCb(pm_appWakeupLowPower_callback_t cb);







#endif /* STACK_BTBLE_BTBLE_PM_H_ */

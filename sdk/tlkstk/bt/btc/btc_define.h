/********************************************************************************************************
 * @file	btc_define.h
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
#ifndef BTC_DEFINE_H
#define BTC_DEFINE_H



enum iscan_low_power_state {
    ISCAN_LOW_POWER_ENABLE = 1,
    ISCAN_LOW_POWER_DONE = 2,
};

enum pscan_low_power_state
{
    PSCAN_LOW_POWER_ENABLE = 1,
    PSCAN_LOW_POWER_PAGE2ACL = 2,
	PSCAN_LOW_POWER_DONE = 4,
};

typedef enum {
    BT_SNIFF_LP_MODE_SUSPEND = 0,
    BT_SNIFF_LP_MODE_DEEP_RET,
} bt_sniff_lp_mode_t;





#endif /* BTC_DEFINE_H */


/********************************************************************************************************
 * @file	ble_config.h
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

//////////////////////////////////////////////////////////////////////////////
/**
 *  @brief  Definition for Device info
 */
#include "drivers.h"
#include "tlkstk/inner/tlkstk_inner.h"




///////////////////  Feature ////////////////////////////
#ifndef	LL_MULTI_SLAVE_MAC_ENABLE
#define	LL_MULTI_SLAVE_MAC_ENABLE				    				0
#endif

//conn param update/map update
#ifndef	BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE
#define BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE					1
#endif

#ifndef LE_AUTHENTICATED_PAYLOAD_TIMEOUT_SUPPORT_EN
#define LE_AUTHENTICATED_PAYLOAD_TIMEOUT_SUPPORT_EN					0
#endif

//Link layer feature enable flag default setting
#ifndef LL_FEATURE_SUPPORT_LE_DATA_LENGTH_EXTENSION
#define LL_FEATURE_SUPPORT_LE_DATA_LENGTH_EXTENSION					1
#endif

#ifndef LL_FEATURE_SUPPORT_LL_PRIVACY
#define LL_FEATURE_SUPPORT_LL_PRIVACY								0
#endif

#ifndef LL_FEATURE_SUPPORT_LE_2M_PHY
#define LL_FEATURE_SUPPORT_LE_2M_PHY								1
#endif

#ifndef LL_FEATURE_SUPPORT_LE_CODED_PHY
#define LL_FEATURE_SUPPORT_LE_CODED_PHY								1
#endif

#ifndef LL_FEATURE_SUPPORT_LE_AOA_AOD
#define LL_FEATURE_SUPPORT_LE_AOA_AOD								0
#endif

#ifndef LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING
#define LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING					0
#endif

#ifndef LL_FEATURE_SUPPORT_LE_EXTENDED_SCANNING
#define LL_FEATURE_SUPPORT_LE_EXTENDED_SCANNING						0
#endif

#ifndef LL_FEATURE_SUPPORT_LE_EXTENDED_INITIATE
#define LL_FEATURE_SUPPORT_LE_EXTENDED_INITIATE						0
#endif

#ifndef LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING
#define LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING					0
#endif

#ifndef LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING_SYNC
#define LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING_SYNC				0
#endif

#ifndef LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2
#define LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2				0
#endif

//core_5.2 feature begin
#ifndef LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_MASTER
#define LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_MASTER		0
#endif

#ifndef LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_SLAVE
#define LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_SLAVE		0
#endif

#ifndef LL_FEATURE_SUPPORT_ISOCHRONOUS_BROADCASTER
#define LL_FEATURE_SUPPORT_ISOCHRONOUS_BROADCASTER					0
#endif

#ifndef LL_FEATURE_SUPPORT_SYNCHRONIZED_RECEIVER
#define LL_FEATURE_SUPPORT_SYNCHRONIZED_RECEIVER					0
#endif

#ifndef LL_FEATURE_SUPPORT_ISOCHRONOUS_CHANNELS
#define LL_FEATURE_SUPPORT_ISOCHRONOUS_CHANNELS						0
#endif

#ifndef	LL_FEATURE_SUPPORT_ISOCHRONOUS_TEST_MODE
#define	LL_FEATURE_SUPPORT_ISOCHRONOUS_TEST_MODE					0
#endif
//core_5.2 feature end



#ifndef L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN
#define L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN     				0
#endif

//gen p256, dhkey supported by controller
#ifndef	CONTROLLER_GEN_P256KEY_ENABLE
#define	CONTROLLER_GEN_P256KEY_ENABLE								0
#endif

#ifndef	APP_DUMP_EN
#define	APP_DUMP_EN													0
#endif


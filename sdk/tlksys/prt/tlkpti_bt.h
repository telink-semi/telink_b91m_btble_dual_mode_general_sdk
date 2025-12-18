/********************************************************************************************************
 * @file	tlkpti_bt.h
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
#ifndef TLKPTI_BT_H
#define TLKPTI_BT_H


enum TLKPTI_BT_MSGID_ENUM{
	TLKPTI_BT_MSGID_NONE = 0x00,
	TLKPTI_BT_MSGID_SET_HFP_VOLUME,
	TLKPTI_BT_MSGID_SET_AVRCP_VOLUME,
	TLKPTI_BT_MSGID_SET_AVRCP_DEF_VOL,
	TLKPTI_BT_MSGID_ENABLE_SCO,
	TLKPTI_BT_MSGID_ENABLE_DSCO,
	TLKPTI_BT_MSGID_DISABLE_SCO,
	TLKPTI_BT_MSGID_CLOSE_PAIR,
};






#endif //TLKPTI_BT_H


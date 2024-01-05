/********************************************************************************************************
 * @file	tlkpti_phone.h
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
#ifndef TLKPTI_PHONE_H
#define TLKPTI_PHONE_H



enum TLKPTI_PHONE_MSGID_ENUM{
	TLKPTI_PHONE_MSGID_NONE = 0x00,
	TLKPTI_PHONE_MSGID_SYNC_BOOK_CMD,
	TLKPTI_PHONE_MSGID_CANCEL_SYNC_CMD,
	
	TLKPTI_PHONE_MSGID_CALL_CLOSE_EVT,
	TLKPTI_PHONE_MSGID_CALL_START_EVT,
	TLKPTI_PHONE_MSGID_CALL_ALART_EVT,
	TLKPTI_PHONE_MSGID_CALL_ACTIVE_EVT,
	TLKPTI_PHONE_MSGID_CALL_WAIT_EVT,
	TLKPTI_PHONE_MSGID_CALL_HOLD_EVT,
	TLKPTI_PHONE_MSGID_CALL_RESUME_EVT,
	TLKPTI_PHONE_MSGID_CALL_SCO_UPDATE_EVT,
};




#endif //TLKPTI_PHONE_H


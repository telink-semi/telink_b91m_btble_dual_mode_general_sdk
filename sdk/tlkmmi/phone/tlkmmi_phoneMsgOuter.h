/********************************************************************************************************
 * @file	tlkmmi_phoneMsgOuter.h
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
#ifndef TLKMMI_PHONE_MSG_OUTER_H
#define TLKMMI_PHONE_MSG_OUTER_H

#if (TLKMMI_PHONE_ENABLE)

typedef enum{
	TLKMMI_THREE_WAY_NONE = 0x00,
	TLKMMI_THREE_WAY_REJ_WAIT_KEEP_ACTIVE,
	TLKMMI_THREE_WAY_ACC_WAIT_HOLD_ACTIVE,
	TLKMMI_THREE_WAY_HUNGUP_ACTIVE_RESUME_HOLD
}TLKMMI_THREE_WAY_CALL_STATUS;

int tlkmmi_phone_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen);



#endif //#if (TLKMMI_PHONE_ENABLE)

#endif //TLKMMI_PHONE_MSG_OUTER_H


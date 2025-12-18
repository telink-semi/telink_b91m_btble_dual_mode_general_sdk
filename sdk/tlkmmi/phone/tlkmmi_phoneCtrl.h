/********************************************************************************************************
 * @file	tlkmmi_phoneCtrl.h
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
#ifndef TLKMMI_PHONE_CTRL_H
#define TLKMMI_PHONE_CTRL_H

#if (TLKMMI_PHONE_ENABLE)


#define TLKMMI_PHONE_TIMEOUT          50000 //100ms
#define TLKMMI_PHONE_TIMEOUT_MS       50
#define TLKMMI_PHONE_WAIT_TIMEOUT     (200000/TLKMMI_BTMGR_TIMEOUT)



typedef enum{
	TLKMMI_PHONE_CALL_STATUS_IDLE = 0,
	TLKMMI_PHONE_CALL_STATUS_INCOMING, //incoming
	TLKMMI_PHONE_CALL_STATUS_OUTGOING, //outgoing
	TLKMMI_PHONE_CALL_STATUS_ACTIVE,
}TLKMMI_PHONE_CALL_STATUS_ENUM;
typedef enum{
	TLKMMI_PHONE_MANUAL_CODE_NONE = 0,
	TLKMMI_PHONE_MANUAL_CODE_DIAL,
	TLKMMI_PHONE_MANUAL_CODE_ANSWER,
	TLKMMI_PHONE_MANUAL_CODE_HUNGUP,
}TLKMMI_PHONE_MANUAL_CODE_ENUM;


typedef struct{
	uint08 hfStatus;
	uint08 hfOpcode;
	uint16 aclHandle;
	uint16 scoHandle;
	uint32 optTimer;
	tlkapi_timer_t timer;
}tlkmmi_phone_ctrl_t;


/******************************************************************************
 * Function: tlkmmi_phone_ctrlInit
 * Descript: 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_ctrlInit(void);

void tlkmmi_phone_setHfCallStatus(uint16 aclHandle, uint08 hfStatus);
void tlkmmi_phone_setHfManualCode(uint16 aclHandle, uint08 hfOpcode);




#endif //#if (TLKMMI_PHONE_ENABLE)

#endif //TLKMMI_PHONE_CTRL_H

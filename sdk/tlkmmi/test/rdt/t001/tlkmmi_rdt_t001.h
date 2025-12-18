/********************************************************************************************************
 * @file	tlkmmi_rdt_t001.h
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
#ifndef TLKMMI_RDT_T001_H
#define TLKMMI_RDT_T001_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T001_ENABLE)


typedef enum{
	TLKMMI_RDT_T001_MSGID_NONE = 0,
	TLKMMI_RDT_T001_MSGID_SET_PEER_INFO = 1,
}TLKMMI_RDT_T001_MSGID_ENUM;


typedef struct{
	uint08 state;
	uint08 stage;
	uint08 devRole;
	tlkapi_timer_t timer;
}tlkmmi_rdt_t001_t;


int tlkmmi_rdt_t001GetRole(void);
int tlkmmi_rdt_t001GetState(void);



#endif //#if (TLKMMI_RDT_CASE_T001_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T001_H


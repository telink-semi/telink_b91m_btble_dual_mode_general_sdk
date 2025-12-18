/********************************************************************************************************
 * @file	tlkmmi_rdt_t002.h
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
#ifndef TLKMMI_RDT_T002_H
#define TLKMMI_RDT_T002_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T002_ENABLE)



#define TLKMMI_RDT_T002_TIMEOUT       200000 //200ms

#define TLKMMI_RDT_T002_BTNAME_DUT         "RDT-T002-DUT"
#define TLKMMI_RDT_T002_BTNAME_AUT         "RDT-T002-AUT"
#define TLKMMI_RDT_T002_BTNAME_AUT2        "RDT-T002-AUT2"

#define TLKMMI_RDT_T002_BTADDR_DUT         {0x02, 0xD1, 0xD1, 0xD1, 0xD1, 0x02}
#define TLKMMI_RDT_T002_BTADDR_AUT         {0x02, 0xA1, 0xA1, 0xA1, 0xA1, 0x02}
#define TLKMMI_RDT_T002_BTADDR_AUT2        {0x02, 0xA2, 0xA2, 0xA2, 0xA2, 0x02}

#define TLKMMI_RDT_T002_BTDEV_CLASS_AUT    0x240404
#define TLKMMI_RDT_T002_BTDEV_CLASS_DUT    0x240404
#define TLKMMI_RDT_T002_BTDEV_CLASS_AUT2   0x240404


typedef enum{
	TLKMMI_RDT_T002_FLAG_NONE = 0x00,
	TLKMMI_RDT_T002_FLAG_CRYPT = 0x01,
	TLKMMI_RDT_T002_FLAG_SCO_CONN = 0x02,
}TLKMMI_RDT_T002_FLAG_ENUM;
typedef enum{
	TLKMMI_RDT_T002_BUSY_NONE = 0x00,
}TLKMMI_RDT_T002_BUSY_ENUM;

typedef enum{
	TLKMMI_RDT_T002_MSGID_NONE = 0,
	TLKMMI_RDT_T002_MSGID_SET_ACL_SWITCH = 0x01,
	TLKMMI_RDT_T002_MSGID_SET_SCO_SWITCH = 0x01,
}TLKMMI_RDT_T002_MSGID_ENUM;



typedef struct{
	uint08 state;
	uint08 drole;
	uint16 reserve;
	tlkapi_timer_t timer;
}tlkmmi_rdt_t002_t;








#endif //#if (TLKMMI_RDT_CASE_T002_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T002_H


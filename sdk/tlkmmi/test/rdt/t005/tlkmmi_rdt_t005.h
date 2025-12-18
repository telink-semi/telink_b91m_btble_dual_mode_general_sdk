/********************************************************************************************************
 * @file	tlkmmi_rdt_t005.h
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
#ifndef TLKMMI_RDT_T005_H
#define TLKMMI_RDT_T005_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T005_ENABLE)


#define TLKMMI_RDT_T005_TIMEOUT       200000 //200ms

#define TLKMMI_RDT_T005_BTNAME_DUT         "RDT-T005-DUT"
#define TLKMMI_RDT_T005_BTNAME_AUT         "RDT-T005-AUT"
#define TLKMMI_RDT_T005_BTNAME_AUT2        "RDT-T005-AUT2"

#define TLKMMI_RDT_T005_BTADDR_DUT         {0x03, 0xD5, 0xD5, 0xD5, 0xD5, 0x03}
#define TLKMMI_RDT_T005_BTADDR_AUT         {0xCB, 0xC0, 0xF3, 0x80, 0xBF, 0xF4} //Mate 20
#define TLKMMI_RDT_T005_BTADDR_AUT2        {0xCB, 0xC0, 0xF3, 0x80, 0xBF, 0xF4}

#define TLKMMI_RDT_T005_BTDEV_CLASS_AUT    0x240404
#define TLKMMI_RDT_T005_BTDEV_CLASS_DUT    0x240404

#define TLKMMI_RDT_T005_SEND_BUFF_LEN      650


#define TLKMMI_RDT_T005_METHOD_UNFIX_LENGTH_MASK      0x01
#define TLKMMI_RDT_T005_METHOD_UNFIX_INTERVAL_MASK    0x02


typedef enum{
	TLKMMI_RDT_T005_MSGID_NONE = 0x00,
	TLKMMI_RDT_T005_MSGID_SET_SEND_START = 0x01,
	TLKMMI_RDT_T005_MSGID_SET_SEND_STOP  = 0x02,
	TLKMMI_RDT_T005_MSGID_SET_SWITCH_PARAM = 0x03,
}TLKMMI_RDT_T005_MSGID_ENUM;

typedef enum{
	TLKMMI_RDT_T005_FLAG_NONE = 0x00,
	TLKMMI_RDT_T005_FLAG_SDP  = 0x01,
	TLKMMI_RDT_T005_FLAG_RFC  = 0x02,
}TLKMMI_RDT_T005_FLAG_ENUM;
typedef enum{
	TLKMMI_RDT_T005_BUSY_NONE = 0x00,
}TLKMMI_RDT_T005_BUSY_ENUM;

typedef enum{
	TLKMMI_RDT_T005_METHOD_FIXED_LENGTH_FIXED_INTERVAL = 0x00,
	TLKMMI_RDT_T005_METHOD_UNFIX_LENGTH_FIXED_INTERVAL = 0x01, //TLKMMI_RDT_T005_METHOD_UNFIX_LENGTH_MASK,
	TLKMMI_RDT_T005_METHOD_FIXED_LENGTH_UNFIX_INTERVAL = 0x02, //TLKMMI_RDT_T005_METHOD_UNFIX_INTERVAL_MASK,
	TLKMMI_RDT_T005_METHOD_UNFIX_LENGTH_UNFIX_INTERVAL = 0x03, //TLKMMI_RDT_T005_METHOD_UNFIX_LENGTH_MASK | TLKMMI_RDT_T005_METHOD_UNFIX_INTERVAL_MASK,
}TLKMMI_RDT_T005_METHOD_ENUM;


typedef struct{
	uint08 state;
	uint08 drole;
	uint16 reserve;
	tlkapi_timer_t timer;
}tlkmmi_rdt_t005_t;






#endif //#if (TLKMMI_RDT_CASE_T005_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T005_H


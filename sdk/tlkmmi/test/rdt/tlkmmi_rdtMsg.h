/********************************************************************************************************
 * @file	tlkmmi_rdtMsg.h
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
#ifndef TLKMMI_RDT_MSG_H
#define TLKMMI_RDT_MSG_H

#if (TLK_TEST_RDT_ENABLE)



typedef enum{
	TLKMMI_RDT_FUNC_TYPE_NONE = 0x00,
	TLKMMI_RDT_FUNC_TYPE_BTH  = 0x01,
	TLKMMI_RDT_FUNC_TYPE_BTP  = 0x02,
	TLKMMI_RDT_FUNC_TYPE_LEH  = 0x03,
	TLKMMI_RDT_FUNC_TYPE_LEP  = 0x04,
}TLKMMI_RDT_FUNC_TYPE_ENUM;

typedef enum{
	TLKMMI_RDT_MSGID_NONE = 0x00,
	TLKMMI_RDT_MSGID_REBOOT = 0xF1,
	TLKMMI_RDT_MSGID_START  = 0xF2,
	TLKMMI_RDT_MSGID_PAUSE  = 0xF3,
	TLKMMI_RDT_MSGID_CLOSE  = 0xF4,
	TLKMMI_RDT_MSGID_CALL_FUNC     = 0xD0,
	TLKMMI_RDT_MSGID_CALL_BTH_FUNC = 0xD1,
	TLKMMI_RDT_MSGID_CALL_BTP_FUNC = 0xD2,
	TLKMMI_RDT_MSGID_CALL_LEH_FUNC = 0xD3,
	TLKMMI_RDT_MSGID_CALL_LEP_FUNC = 0xD4,
	TLKMMI_RDT_MSGID_USER_START = 0x01,
	TLKMMI_RDT_MSGID_USER_END   = 0xA0,
}TLKMMI_RDT_MSGID_ENUM;



int tlkmmi_rdt_recvMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen);



#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_MSG_H


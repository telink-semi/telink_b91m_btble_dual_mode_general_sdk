/********************************************************************************************************
 * @file	tlkmmi_ptsMsg.h
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
#ifndef TLKMMI_PTS_MSG_H
#define TLKMMI_PTS_MSG_H

#if (TLK_TEST_PTS_ENABLE)


typedef enum{
	TLKMMI_PTS_FUNC_TYPE_NONE = 0x00,
	TLKMMI_PTS_FUNC_TYPE_BTH  = 0x01,
	TLKMMI_PTS_FUNC_TYPE_BTP  = 0x02,
}TLKMMI_PTS_FUNC_TYPE_ENUM;

typedef enum{
	TLKMMI_PTS_MSGID_NONE = 0x00,
	TLKMMI_PTS_MSGID_REBOOT = 0xF1,
	TLKMMI_PTS_MSGID_START  = 0xF2,
	TLKMMI_PTS_MSGID_PAUSE  = 0xF3,
	TLKMMI_PTS_MSGID_CLOSE  = 0xF4,
	TLKMMI_PTS_MSGID_CALL_FUNC     = 0xD0,
	TLKMMI_PTS_MSGID_CALL_BTH_FUNC = 0xD1,
	TLKMMI_PTS_MSGID_CALL_BTP_FUNC = 0xD2,
}TLKMMI_PTS_MSGID_ENUM;


int tlkmmi_pts_recvMsgHandler(uint08 msgID, uint08 *pData, uint16 dataLen);





#endif //#if (TLK_TEST_PTS_ENABLE)

#endif //TLKMMI_PTS_MSG_H


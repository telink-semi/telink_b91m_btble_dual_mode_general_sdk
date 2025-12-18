/********************************************************************************************************
 * @file	leh_funcs.h
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
#ifndef LEH_FUNC_SET_H
#define LEH_FUNC_SET_H

#if (TLK_STK_LE_ENABLE)



enum LEH_FUNCID_SET_ENUM{
	LEH_FUNCID_NONE = 0x0000,
	//Common
	LEH_FUNCID_COMM_START = 0x0000,
	LEH_FUNCID_COMM_START_ADV = 0x01 + LEH_FUNCID_COMM_START,
	LEH_FUNCID_COMM_CLOSE_ADV = 0x02 + LEH_FUNCID_COMM_START,
	LEH_FUNCID_COMM_CLS_PEER_INFO = 0x03 + LEH_FUNCID_COMM_START,
	//ACL Start
	LEH_FUNCID_ACL_START = 0x0100,
 	//Dev Start
 	LEH_FUNCID_DEV_START = 0x0200,
	//HCI-CMD Start
	LEH_FUNCID_CMD_START = 0x0300,
	//HCI-EVT Start
	LEH_FUNCID_EVT_START = 0x0400,
	//L2cap Start
	LEH_FUNCID_L2C_START = 0x0500,
	//Signal Start
	LEH_FUNCID_SIG_START = 0x0600,
	//Att Start
	LEH_FUNCID_ATT_START = 0x0700,
};



typedef struct{
	uint16 funID; //Refer to LEH_FUNC_ID_ENUM.
	int(*Func)(uint08 *pData, uint16 dataLen);
}leh_func_item_t;



int  leh_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
void leh_func_setAclHandle(uint16 aclHandle);
void leh_func_setPeerAddr(uint08 peerAddr[6]);



static int leh_func_commStartAdv(uint08 *pData, uint16 dataLen);
static int leh_func_commCloseAdv(uint08 *pData, uint16 dataLen);
static int leh_func_commClsPeerInfo(uint08 *pData, uint16 dataLen);



#endif //#if (TLK_STK_LE_ENABLE)

#endif //LEH_FUNC_SET_H


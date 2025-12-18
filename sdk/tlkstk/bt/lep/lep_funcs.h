/********************************************************************************************************
 * @file	lep_funcs.h
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
#ifndef LEP_FUNC_SET_H
#define LEP_FUNC_SET_H

#if (TLK_STK_LE_ENABLE)



enum LEP_FUNCID_SET_ENUM{
	LEP_FUNCID_NONE = 0x0000,
	//Common
	LEP_FUNCID_COMM_START = 0x0000,
	//ACL Start
	LEP_FUNCID_GAP_START = 0x0100,
	LEP_FUNCID_GAP_UPDATE_CONN_PARAM = 0x01+LEP_FUNCID_GAP_START,
 	//Dev Start
 	LEP_FUNCID_GATT_START = 0x0200,
};



typedef struct{
	uint16 funID; //Refer to LEP_FUNC_ID_ENUM.
	int(*Func)(uint08 *pData, uint16 dataLen);
}lep_func_item_t;



int  lep_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);
void lep_func_setAclHandle(uint16 aclHandle);


static int lep_func_commUpdateConnParam(uint08 *pData, uint16 dataLen);




#endif //#if (TLK_STK_LE_ENABLE)

#endif //LEP_FUNC_SET_H


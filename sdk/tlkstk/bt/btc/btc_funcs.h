/********************************************************************************************************
 * @file	btc_funcs.h
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
#ifndef BTH_FUNC_SET_H
#define BTH_FUNC_SET_H

#include "types.h"

typedef struct{
	uint8_t type; //funcType
	uint16_t funID;
	int(*Func)(uint8_t *pData, uint16_t dataLen);
}btc_func_item_t;

typedef enum
{
	TLK_FUNC_TYPE_BTC = 0,
	TLK_FUNC_TYPE_BTC_LMP,
	TLK_FUNC_TYPE_BTC_ACL,
	TLK_FUNC_TYPE_BTC_SCO,
	TLK_FUNC_TYPE_BTC_HCI,
	TLK_FUNC_TYPE_BTC_EVT,
	TLK_FUNC_TYPE_BTC_ENC,
	TLK_FUNC_TYPE_BTC_RSW,
	TLK_FUNC_TYPE_BTC_QOS,
	TLK_FUNC_TYPE_BTC_AFH,
	TLK_FUNC_TYPE_BTC_CHN,
	TLK_FUNC_TYPE_BTC_AUTH,
	TLK_FUNC_TYPE_BTC_PAIR,
	TLK_FUNC_TYPE_BTC_TASK,
	TLK_FUNC_TYPE_BTC_NAME,
	TLK_FUNC_TYPE_BTC_PAGE,
	TLK_FUNC_TYPE_BTC_SNIFF,
	TLK_FUNC_TYPE_BTC_TIMER,
	TLK_FUNC_TYPE_BTC_RESET,
	TLK_FUNC_TYPE_BTC_DETACH,
	TLK_FUNC_TYPE_BTC_MAX,
}TLK_FUNC_BTC_TYPE_ENUM;


int btc_FuncCall(uint8_t funcType, uint8_t funcID, uint8_t *pData, uint16_t dataLen);





#endif //BTH_FUNC_SET_H


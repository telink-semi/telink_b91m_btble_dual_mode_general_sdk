/********************************************************************************************************
 * @file     tlkmdi_btsco.h
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
#ifndef TLKMDI_BTSCO_H
#define TLKMDI_BTSCO_H

#if (TLK_STK_BT_ENABLE)

#define TLKMDI_BTSCO_MAX_NUM		2
#define TLKMDI_BTSCO_CODEC_ID_CVSD          1
#define TLKMDI_BTSCO_CODEC_ID_MSBC          2

enum TLKMDI_BTSCO_ROLE_ENUM{
	TLKMDI_BTSCO_ROLE_NONE = 0,
	TLKMDI_BTSCO_ROLE_SCO,
	TLKMDI_BTSCO_ROLE_DSCO,
};

typedef void(*TlkMdiBtScoConnCB)(uint16 aclHandle, uint16 scoHandle, bool isConn);


typedef struct{
	uint16 aclHandle;
	uint16 scoHandle;
	uint08 codec;
}tlkmdi_btsco_handle_t;

typedef struct{
	uint08 scoCnt;
	uint08 curRole;
	tlkmdi_btsco_handle_t pItem[TLKMDI_BTSCO_MAX_NUM];
}tlkmdi_btsco_ctrl_t;

int tlkmdi_btsco_init(void);

/******************************************************************************
 * Function: tlkmdi_audsco_regCB
 * Descript: Register SCO connection status switch callback. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_btsco_regCB(TlkMdiBtScoConnCB connCB);


void tlkmdi_btsco_setCurRole(uint08 role);
uint08 tlkmdi_btsco_getCurRole();

#endif //#if (TLK_STK_BT_ENABLE)

#endif //TLKMDI_BTSCO_H


/********************************************************************************************************
 * @file	tlkmmi_btmgrCtrl.h
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
#ifndef TLKMMI_BTMGR_CTRL_H
#define TLKMMI_BTMGR_CTRL_H




typedef struct{
	uint08 btaddr[6];
	uint08 btname[TLK_CFG_FLASH_BT_NAME_LENS];
}tlkmmi_btmgr_ctrl_t;



extern tlkmmi_btmgr_ctrl_t gTlkMmiBtmgrCtrl;

/******************************************************************************
 * Function: tlkmmi_btmgr_ctrlInit
 * Descript: Initial the Bt manager ctrl block, and read local name and 
 *           Bt address. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_ctrlInit(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_getName
 * Descript: Get BT Name.
 * Params:
 * Return: Return Bt name is success.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_btmgr_getName(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_getAddr
 * Descript: Get the Bt address. 
 * Params:
 * Return: Return Bt Address.
 * Others: None.
*******************************************************************************/
uint08 *tlkmmi_btmgr_getAddr(void);

int tlkmmi_btmgr_setName(uint08 *pName, uint08 nameLen);
int tlkmmi_btmgr_setAddr(uint08 *pAddr);


int tlkmmi_btmgr_ctrlVolInc(void);
int tlkmmi_btmgr_ctrlVolDec(void);



#endif //TLKMMI_BTMGR_CTRL_H


/********************************************************************************************************
 * @file	tlkmmi_lemgrCtrl.h
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
#ifndef TLKMMI_LEMGR_CTRL_H
#define TLKMMI_LEMGR_CTRL_H

#if (TLKMMI_LEMGR_ENABLE)



typedef struct{
	uint08 nameLen;
	uint08 leaddr[12];
	uint08 lename[TLK_CFG_FLASH_LE_NAME_LENS];
}tlkmmi_lemgr_ctrl_t;


int tlkmmi_lemgr_ctrlInit(void);

uint08 tlkmmi_lemgr_getNameLen(void);

uint08 *tlkmmi_lemgr_getName(void);
uint08 *tlkmmi_lemgr_getAddr(void);

int tlkmmi_lemgr_setName(uint08 *pName, uint08 nameLen);
int tlkmmi_lemgr_setAddr(uint08 *pAddr);
int tlkmmi_lemgr_setAddr1(uint08 *pPubAddr, uint08 *pRndAddr);

bool tlkmmi_lemgr_volumeInc(void);
bool tlkmmi_lemgr_volumeDec(void);



#endif //TLKMMI_LEMGR_ENABLE

#endif //TLKMMI_LEMGR_CTRL_H


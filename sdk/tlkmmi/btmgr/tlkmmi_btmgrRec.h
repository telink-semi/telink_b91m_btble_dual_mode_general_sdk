/********************************************************************************************************
 * @file	tlkmmi_btmgrRec.h
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
#ifndef TLKMMI_BTMGR_REC_H
#define TLKMMI_BTMGR_REC_H

#if (TLKMMI_BTMGR_BTREC_ENABLE)



/******************************************************************************
 * Function: tlkmmi_btmgr_recInit
 * Descript: Trigger the BT page initial. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_recInit(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_recStart
 * Descript: Trigger start BT page. 
 * Params:
 *         @pDevAddr[IN]--The device bt address.
 *         @devClass[IN]--The device class type.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_recStart(uint08 *pDevAddr, uint32 devClass, bool enPage, bool enScan);

/******************************************************************************
 * Function: tlkmmi_btmgr_recClose
 * Descript: Trigger stop BT page. 
 * Params: None.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_recClose(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_recIsBusy
 * Descript: Check the BT is busy. 
 * Params: None.
 * Return: Return true is busy, false is idle.
 * Others: None.
*******************************************************************************/
bool tlkmmi_btmgr_recIsBusy(void);

bool tlkmmi_btmgr_recIsPage(void);
bool tlkmmi_btmgr_recIsScan(void);

uint08 *tlkmmi_btmgr_recPageAddr(void);


#endif //#if (TLKMMI_BTMGR_BTREC_ENABLE)

#endif //TLKMMI_BTMGR_REC_H


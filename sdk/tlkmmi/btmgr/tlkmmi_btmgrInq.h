/********************************************************************************************************
 * @file	tlkmmi_btmgrInq.h
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
#ifndef TLKMMI_BTMGR_INQ_H
#define TLKMMI_BTMGR_INQ_H

#if (TLKMMI_BTMGR_BTINQ_ENABLE)



/******************************************************************************
 * Function: tlkmmi_btmgr_inqInit
 * Descript: Trigger the BT Inquiry initial. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_inqInit(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_startInquiry
 * Descript: Trigger the BT start to Inquiry. 
 * Params:
 *        @inqType[IN]--The inquiry type.
 *        @rssiThd[IN]--The rssi of signal.
 *        @maxNumb[IN]--How many devices should bt update.
 *        @timeout[IN]--The inquiry timeout.
 *        @isGetName[IN]--whether get the device name or not.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_startInquiry(uint08 inqType, uint08 rssiThd, uint08 maxNumb, uint08 inqWind, uint08 inqNumb, bool isGetName);

/******************************************************************************
 * Function: tlkmmi_btmgr_closeInquiry
 * Descript: Trigger the BT stop to Inquiry. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_closeInquiry(void);




#endif //#if (TLKMMI_BTMGR_BTINQ_ENABLE)

#endif //TLKMMI_BTMGR_INQ_H


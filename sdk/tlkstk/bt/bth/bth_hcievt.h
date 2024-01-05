/********************************************************************************************************
 * @file	bth_hcievt.h
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
#ifndef BTH_HCI_EVT_H
#define BTH_HCI_EVT_H


#define BTH_HCI_EIR_DT_NAME_PART 0x08 /* Shortened local name */
#define BTH_HCI_EIR_DT_NAME      0x09 /* Complete local name */


/******************************************************************************
 * Function: bth_hcievt_init
 * Descript: Initial the hci event callback.
 * Params: None.
 * Return: TLK_ENONE is success, other value if false.
 *******************************************************************************/
int bth_hcievt_init(void);


#endif // BTH_HCI_EVT_H

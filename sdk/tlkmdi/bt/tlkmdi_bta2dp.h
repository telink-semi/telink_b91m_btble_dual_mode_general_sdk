/********************************************************************************************************
 * @file	tlkmdi_bta2dp.h
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
#ifndef TLKMDI_BT_A2DP_H
#define TLKMDI_BT_A2DP_H

#if (TLK_MDI_BTA2DP_ENABLE)



/******************************************************************************
 * Function: tlkmdi_bta2dp_init.
 * Descript: Trigger to Initial the A2DP control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_bta2dp_init(void);

void tlkmdi_bta2dp_connectEvt(uint16 aclHandle, uint08 usrID);



#endif //#if (TLK_MDI_BTA2DP_ENABLE)

#endif //TLKMDI_BT_A2DP_H


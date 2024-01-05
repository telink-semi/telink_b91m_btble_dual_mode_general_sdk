/********************************************************************************************************
 * @file	tlkmmi_phone.h
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
#ifndef TLKMMI_PHONE_H
#define TLKMMI_PHONE_H

#if (TLKMMI_PHONE_ENABLE)


#define TLKMMI_PHONE_DBG_FLAG       ((TLK_MAJOR_DBGID_MMI_PHONE << 24) | (TLK_MINOR_DBGID_MMI_PHONE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_PHONE_DBG_SIGN       "[PHONE]"

#define TLKMMI_PHONE_SCOMGR_METHOD_NONE             0
#define TLKMMI_PHONE_SCOMGR_METHOD_ONLY_MINE        1 //In any case, when the call is connected, go our way
#define TLKMMI_PHONE_SCOMGR_METHOD_INITIATOR_DEAL   2 //The audio is carried away from the initiator
#define TLKMMI_PHONE_SCOMGR_METHOD                  TLKMMI_PHONE_SCOMGR_METHOD_NONE



/******************************************************************************
 * Function: tlkmmi_phone_startSyncBook
 * Descript: Use to start to sync the pb info. 
 * Params:
 *        @isForce[IN]--is force to sync the pb info.
 *        @pBtAddr[IN]--The device's Bt address
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_phone_startSyncBook(uint16 aclHandle, uint08 *pBtAddr, bool isForce);

/******************************************************************************
 * Function: tlkmmi_phone_closeBookSync
 * Descript: Use to stop sync pb info. 
 *        @isForce[IN]--is force to sync the pb info.
 *        @pBtAddr[IN]--The device's Bt address
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
extern int tlkmmi_phone_closeSyncBook(uint16 aclHandle);



#endif //#if (TLKMMI_PHONE_ENABLE)

#endif //TLKMMI_PHONE_H


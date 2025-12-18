/********************************************************************************************************
 * @file	tlkmmi_atsCtrl.h
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
#ifndef TLKMMI_ATS_CTRL_H
#define TLKMMI_ATS_CTRL_H

#if (TLK_TEST_ATS_ENABLE)




int tlkmmi_ats_ctrlInit(void);


void tlkmmi_ats_ctrlRecvCmdDeal(uint16 msgID, uint08 *pData, uint16 dataLen);


#endif //#if (TLK_TEST_ATS_ENABLE)

#endif //TLKMMI_ATS_CTRL_H


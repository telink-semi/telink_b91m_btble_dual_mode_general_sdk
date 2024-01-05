/********************************************************************************************************
 * @file	tlkmmi_rdtDef.h
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
#ifndef TLKMMI_RDT_DEF_H
#define TLKMMI_RDT_DEF_H

#if (TLK_TEST_RDT_ENABLE)



typedef enum{
	TLKMMI_RDT_ROLE_NONE = 0x00,
	TLKMMI_RDT_ROLE_DUT  = 0x01,
	TLKMMI_RDT_ROLE_AUT  = 0x02,
	TLKMMI_RDT_ROLE_AUT1 = 0x02,
	TLKMMI_RDT_ROLE_AUT2 = 0x03,
	TLKMMI_RDT_ROLE_AUT3 = 0x04,
	TLKMMI_RDT_ROLE_AUT4 = 0x05,
	TLKMMI_RDT_ROLE_AUT5 = 0x06,
}TLKMMI_RDT_ROLE_ENUM;




#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_DEF_H


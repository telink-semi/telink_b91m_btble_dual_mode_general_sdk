/********************************************************************************************************
 * @file	tlkats.h
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
#ifndef TLKMMI_ATS_H
#define TLKMMI_ATS_H

#if (TLK_TEST_ATS_ENABLE)





#define TLKMMI_ATS_DBG_FLAG       ((TLK_MAJOR_DBGID_MMI_TEST << 24) | (TLK_MINOR_DBGID_MMI_TEST_ATS << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_ATS_DBG_SIGN       "[ATS]"


typedef enum{
	TLKMMI_ATS_BUSY_NONE   = 0x0000,
}TLKMMI_ATS_BUSYS_ENUM;







#endif //#if (TLK_TEST_ATS_ENABLE)

#endif //TLKMMI_ATS_H


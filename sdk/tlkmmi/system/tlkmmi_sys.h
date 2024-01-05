/********************************************************************************************************
 * @file	tlkmmi_sys.h
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
#ifndef TLKMMI_SYS_H
#define TLKMMI_SYS_H

#if (TLKMMI_SYSTEM_ENABLE)


#define TLKMMI_SYS_DBG_FLAG         ((TLK_MAJOR_DBGID_MMI_SYSTEM << 24) | (TLK_MINOR_DBGID_MMI_SYSTEM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_SYS_DBG_SIGN         "[SYS]"


#define TLKMMI_SYS_TIMEOUT			100000




#endif //TLKMMI_SYSTEM_ENABLE

#endif //TLKMMI_SYS_H


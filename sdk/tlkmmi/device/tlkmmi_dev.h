/********************************************************************************************************
 * @file	tlkmmi_device.h
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
#ifndef TLKMMI_DEVICE_H
#define TLKMMI_DEVICE_H

#if (TLKMMI_DEVICE_ENABLE)


#define TLKMMI_DEV_DBG_FLAG         ((TLK_MAJOR_DBGID_MMI_DEVICE << 24) | (TLK_MINOR_DBGID_MMI_DEVICE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_DEV_DBG_SIGN         "[MDEV]"



#endif //#if (TLKMMI_DEVICE_ENABLE)

#endif //TLKMMI_DEVICE_H


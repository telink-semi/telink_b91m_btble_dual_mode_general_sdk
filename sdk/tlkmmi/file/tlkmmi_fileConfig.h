/********************************************************************************************************
 * @file	tlkmmi_fileConfig.h
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
#ifndef TLKMMI_FILE_CONFIG_H
#define TLKMMI_FILE_CONFIG_H

#if (TLKMMI_FILE_ENABLE)



#define TLKMMI_FILE_CHN_SERIAL_ENABLE          1
#define TLKMMI_FILE_CHN_BT_SPP_ENABLE          (1 && TLK_STK_BT_ENABLE)
#define TLKMMI_FILE_CHN_BT_ATT_ENABLE          (1 && TLK_STK_BT_ENABLE)
#define TLKMMI_FILE_CHN_LE_ATT_ENABLE          (0 && TLK_STK_LE_ENABLE)







#endif //TLKMMI_FILE_ENABLE

#endif //TLKMMI_FILE_CONFIG_H


/********************************************************************************************************
 * @file	tlkmem_config.h
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
#ifndef TLKMEM_CONFIG_H
#define TLKMEM_CONFIG_H

#define TLKMEM_DBG_FLAG           ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_COMM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMEM_DBG_SIGN           "[MEM]"

#define TLKMEM_CFG_MEM_MODE         1
typedef enum{
	TLKMEM_MODTYPE_NONE = 0,
	TLKMEM_MODTYPE_COMM,
	TLKMEM_MODTYPE_RTOS,
}TLKMEM_MODTYPE_ENUM;



#endif //TLKMEM_CONFIG_H


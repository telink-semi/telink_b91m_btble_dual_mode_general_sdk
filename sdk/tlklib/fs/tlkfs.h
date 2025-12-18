/********************************************************************************************************
 * @file	tlkfs.h
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
#ifndef TLKFS_H
#define TLKFS_H

#if (TLK_CFG_FS_ENABLE)


#if (TLK_FS_FAT_ENABLE)
#include "tlklib/fs/fatfs/ff.h"
#endif
#include "tlklib/fs/tlkfs_disk.h"








#endif //#if (TLK_CFG_FS_ENABLE)

#endif //TLKFS_H

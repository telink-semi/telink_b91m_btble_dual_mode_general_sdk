/********************************************************************************************************
 * @file	tlkfs_disk.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_CFG_FS_ENABLE)
#include "tlkfs_disk.h"



static tlkfs_disk_t *spTlkFsDisks[TLKFS_DISK_MAX_NUMB] = {0};




tlkfs_disk_t *tlkfs_getDisk(uint08 volNum)
{
	if(volNum >= TLKFS_DISK_MAX_NUMB) return nullptr;
	return spTlkFsDisks[volNum];
}



int tlkfs_addDisk(tlkfs_disk_t *pDisk, uint08 volNum)
{
	if(pDisk == nullptr || pDisk->blkSize == 0 || (pDisk->blkSize & 0x1F) != 0
		|| pDisk->blkCount < 20 || pDisk->Read == nullptr){
		return -TLK_EPARAM;
	}
	if(volNum >= TLKFS_DISK_MAX_NUMB) return -TLK_EQUOTA;
	spTlkFsDisks[volNum] = pDisk;
	return TLK_ENONE;
}
void tlkfs_delDisk(tlkfs_disk_t *pDisk)
{
	uint08 index;
	if(pDisk == nullptr) return;
	for(index=0; index<TLKFS_DISK_MAX_NUMB; index++){
		if(spTlkFsDisks[index] == pDisk) break;
	}
	if(index == TLKFS_DISK_MAX_NUMB) return;
	spTlkFsDisks[index] = nullptr;
}
void tlkfs_clrDisk(void)
{
	uint08 index;
	for(index=0; index<TLKFS_DISK_MAX_NUMB; index++){
		spTlkFsDisks[index] = nullptr;
	}
}



#endif //#if (TLK_CFG_FS_ENABLE)


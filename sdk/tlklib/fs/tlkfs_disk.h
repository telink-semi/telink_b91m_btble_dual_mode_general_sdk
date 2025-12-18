/********************************************************************************************************
 * @file	tlkfs_disk.h
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
#ifndef TLKFS_DISK_H
#define TLKFS_DISK_H

#if (TLK_CFG_FS_ENABLE)



#define TLKFS_DISK_MAX_NUMB        4


typedef struct{
	uint08 isReady;
	uint08 reserve;
	uint16 blkSize;
	uint32 blkCount;
	int(*Init)(void);
	int(*Read)(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
	int(*Write)(uint08 *pData, uint32 blkOffs, uint16 blkNumb);
}tlkfs_disk_t;


tlkfs_disk_t *tlkfs_getDisk(uint08 volNum);

int  tlkfs_addDisk(tlkfs_disk_t *pDisk, uint08 volNumb);
void tlkfs_delDisk(tlkfs_disk_t *pDisk);
void tlkfs_clrDisk(void);



#endif //#if (TLK_CFG_FS_ENABLE)

#endif //TLKFS_DISK_H


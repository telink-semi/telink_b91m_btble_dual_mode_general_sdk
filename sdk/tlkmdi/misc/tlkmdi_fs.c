/********************************************************************************************************
 * @file	tlkmdi_fs.c
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
#if (TLK_MDI_FS_ENABLE)
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"
#include "tlkmdi/misc/tlkmdi_fs.h"
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/tlkdev.h"
#endif

#if (TLK_DEV_STORE_ENABLE)
static const tlkfs_disk_t sTlkMdiFsDisk;
#endif


int tlkmdi_fs_init(void)
{
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_open(TLKDEV_DID_SDCARD0, 0);
	tlkfs_addDisk((tlkfs_disk_t*)&sTlkMdiFsDisk, 1);
	tlkapi_file_mount(L"1:", 1);
	tlkdev_close(TLKDEV_DID_SDCARD0);
	#endif
	
	return TLK_ENONE;
}


#if (TLK_DEV_STORE_ENABLE)
int tlkmdi_fs_diskInit(void)
{
	return TLK_ENONE;
}
int tlkmdi_fs_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	if(tlkdev_state(TLKDEV_DID_SDCARD0) == TLKDEV_STATE_CLOSED){
		tlkdev_open(TLKDEV_DID_SDCARD0, 0);
	}
	return tlkdev_read(TLKDEV_DID_SDCARD0, blkOffs<<9, pBuff, blkNumb<<9); //addr=blkOffs<<9=blkOffs*sTlkMdiFsDisk.blkSize
}
int tlkmdi_fs_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	if(tlkdev_state(TLKDEV_DID_SDCARD0) == TLKDEV_STATE_CLOSED){
		tlkdev_open(TLKDEV_DID_SDCARD0, 0);
	}
	return tlkdev_write(TLKDEV_DID_SDCARD0, blkOffs<<9, pData, blkNumb<<9); //addr=blkOffs<<9=blkOffs*sTlkMdiFsDisk.blkSize
}
static const tlkfs_disk_t sTlkMdiFsDisk = {
	true,//isReady;
	0,//reserve;
	512,//blkSize;
	0x0003E000,//blkCount; 128M-2M=126M
	tlkmdi_fs_diskInit,//int(*Init)(void);
	tlkmdi_fs_diskRead,//int(*Read)(uint32 blkOffs, uint16 blkNumb, uint08 *pBuff);
	tlkmdi_fs_diskWrite,//int(*Write)(uint32 blkOffs, uint16 blkNumb, uint08 *pData);
};
#endif


#endif //#if (TLK_MDI_FS_ENABLE)


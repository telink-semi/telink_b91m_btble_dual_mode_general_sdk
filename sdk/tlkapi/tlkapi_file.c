/********************************************************************************************************
 * @file	tlkapi_file.c
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
#include "drivers.h"
#include "tlklib/fs/tlkfs.h"
#include "tlkapi/tlkapi_file.h"

#if (TLK_FS_FAT_ENABLE)
FATFS gTlkFileFatFs;
#endif

/******************************************************************************
 * Function: tlkapi_file_init
 * Descript: Initial the file system. Reserved interface, not used.
 * Params: None.
 * Return: Return the TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkapi_file_init(void)
{
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_file_strlen
 * Descript: Gets the number of bytes used by the file path.
 * Params: 
 *     @pPath[IN]--The control block of the file object.
 * Return: The number of bytes used by the file path.
 *****************************************************************************/
int tlkapi_file_strlen(FCHAR *pPath)
{
	uint08 length = 0;
	FCHAR *ptr = pPath;
	if(pPath == NULL) return 0;
	while((*ptr++) != '\0'){
		#if (TLK_FS_FAT_ENABLE)
		length += 2;
		#else
		length += 1;
		#endif
		if(length >= 0xFD) break;
	}	
	return length;
}

/******************************************************************************
 * Function: tlkapi_file_size
 * Descript: Get the File size.
 * Params: 
 *     @pFile[IN]--The control block of the file object.
 * Return: The size of the file.
 * Notice: 
 *     1.Call this interface after calling "tlkapi_file_open()" successfully.
 *****************************************************************************/
int tlkapi_file_size(FIL *pFile)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_size(pFile);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_open
 * Descript: Open or create a file.
 * Params:
 *     @pFile[IN]--The control block of the file object.
 *     @path[IN]--Pointer to the file path.
 *     @mode[IN]--Access mode and open mode flags.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *     2.For fat32 systems, the "mode" can refer to "FA_READ/FA_WRITE/..." or
 *       "TLKAPI_FM_READ/TLKAPI_FM_WRITE/...".
 *****************************************************************************/
int tlkapi_file_open(FIL *pFile, const FCHAR *pPath, uint08 mode)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_open(pFile, pPath, mode); //int
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_close
 * Descript: Close an open file object.
 * Params: 
 *     @pFile[IN]--The control block of the file object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_close(FIL *pFile)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_close(pFile);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_seek
 * Descript: Move file pointer of the file object.
 * Params: 
 *     @pFile[IN]--The control block of the file object.
 *     @offset[IN]--The offset need to seek.
 * Return: Return the result.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_seek(FIL *pFile, uint32 offset)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_lseek(pFile, offset);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_read
 * Descript: Read data from the file.
 * Params: 
 *     @pFile[IN]--The control block of the file object.
 *     @pBuff[IN]--The buff stored the read data.
 *     @buffLen[IN]--Num of bytes to read.
 *     @pReadLen[OUT]--Num of bytes actually to read.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_read(FIL *pFile, void *pBuff, uint32 buffLen, uint32 *pReadLen)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_read(pFile, pBuff, buffLen, (uint*)pReadLen);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_write
 * Descript: Write data into the file.
 * Params: 
 *     @pFile[IN]--The control block of the file object.
 *     @pData[IN]--The buff stored the write data.
 *     @dataLen[IN]--Num of bytes to write.
 *     @pWriteLen[OUT]--Num of bytes actually to write.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_write(FIL *pFile, const void *pData, uint32 dataLen, uint32 *pWriteLen)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_write(pFile, pData, dataLen, (uint*)pWriteLen);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

#if (TLK_FS_FAT_ENABLE)

/******************************************************************************
 * Function: tlkapi_file_opendir
 * Descript: Open a directory.
 * Params: 
 *     @pDir[IN]--The control block of the directory object.
 *     @pPath[IN]--Pointer to the file path.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_opendir(DIR *pDir, const FCHAR *pPath)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_opendir(pDir, pPath);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_closedir
 * Descript: Close an open directory.
 * Params: 
 *     @pDir[IN]--The control block of the directory object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_closedir(DIR *pDir)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_closedir(pDir);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_readdir
 * Descript: Read a directory item.
 * Params: 
 *     @pDir[IN]--The control block of the directory object.
 *     @pFileInfo[IN]--The file info.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_readdir(DIR *pDir, FILINFO *pFileInfo)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_readdir(pDir, (FILINFO*)pFileInfo);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_findfirst
 * Descript: Find the first file in directory.
 * Params: 
 *     @pDir[IN]--The control block of the directory object.
 *     @pFileInfo[IN]--The file info.
 *     @pPath[IN]--Pointer to the file path.
 *     @pPattern[IN]--The file pattern.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_findfirst(DIR *pDir, FILINFO *pFileInfo, const FCHAR *pPath, const FCHAR *pPattern)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_findfirst(pDir, pFileInfo, pPath, pPattern);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_findnext
 * Descript: Find the next file in directory.
 * Params: 
 *     @pDir[IN]--The control block of the directory object.
 *     @pFileInfo[IN]--The file info.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_findnext(DIR *pDir, FILINFO *pFileInfo)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_findnext(pDir, pFileInfo);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}
#endif

/******************************************************************************
 * Function: tlkapi_file_mkdir
 * Descript: Create a sub directory.
 * Params: 
 *     @path[IN]--Pointer to the file path.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_mkdir(const FCHAR *pPath)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_mkdir(pPath);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_unlink
 * Descript: Delete an existing file or directory .
 * Params: 
 *     @pPath[IN]--Pointer to the file path.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_unlink(const FCHAR *pPath)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_unlink(pPath);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_rename
 * Descript: Rename/Move a file or directory.
 * Params: 
 *     @pOldPath[IN]--The old file path.
 *     @pNewPath[IN]--The new file path.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_rename(const FCHAR *pOldPath, const FCHAR *pNewPath)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_rename(pOldPath, pNewPath);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_mount
 * Descript: Mount/Unmount a logical drive.
 * Params: 
 *     @pPath[IN]--The file path.
 *     @opt[IN]--The cmd of mount/unmount.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_mount(const FCHAR *pPath, uint08 opt)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_mount(&gTlkFileFatFs, pPath, opt);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_mount
 * Descript: Create a FAT volume.
 * Params: 
 *     @pPath[IN]--Pointer to the file path.
 *     @pOpt[IN]--Format options. Refer to MKFS_PARAM.
 *     @pWork[IN]--Pointer to working buffer (null: use heap memory).
 *     @len[IN]--Size of working buffer [byte].
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_mkfs(const FCHAR *pPath, const void *pOpt, void *pWork, uint32 len)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_mkfs(pPath, pOpt, pWork, len);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_file_fdisk
 * Descript: Divide a physical drive into some partitions.
 * Params: 
 *     @pdrv[IN]--Physical drive number.
 *     @ptbl[IN]--Pointer to the size table for each partitions.
 *     @pWork[IN]--Pointer to the working buffer (null: use heap memory).
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_fdisk(uint08 pdrv, const uint32 ptbl[], void *pWork)
{
	int ret;
	tlksys_enter_critical();
	#if (TLK_FS_FAT_ENABLE)
		ret = f_fdisk(pdrv, (const LBA_t*)ptbl, pWork);
	#else
		ret = -TLK_ENOSUPPORT;
	#endif
	tlksys_leave_critical();
	return ret;
}



#endif


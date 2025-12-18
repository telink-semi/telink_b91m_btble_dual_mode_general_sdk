/********************************************************************************************************
 * @file	tlkapi_file.h
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
#ifndef TLKAPI_FILE_H
#define TLKAPI_FILE_H


#if (TLK_CFG_FS_ENABLE)



/******************************************************************************
 * Object: TLKAPI_FM_READ/TLKAPI_FM_WRITE/...
 * Brief : The mode of the "tlkapi_file_open()".
 *****************************************************************************/
#define	TLKAPI_FM_READ                  0x01
#define	TLKAPI_FM_WRITE                 0x02
#define	TLKAPI_FM_OPEN_EXISTING         0x00
#define	TLKAPI_FM_CREATE_NEW            0x04
#define	TLKAPI_FM_CREATE_ALWAYS         0x08
#define	TLKAPI_FM_OPEN_ALWAYS           0x10
#define	TLKAPI_FM_OPEN_APPEND           0x30


#if (TLK_FS_FAT_ENABLE)
	#define FCHAR      uint16
#else
	#define FCHAR      char
	#define FIL        void
#endif



/******************************************************************************
 * Function: tlkapi_file_size
 * Descript: Get the File size.
 * Params: 
 *     @pFile[IN]--The control block of the file object.
 * Return: The size of the file.
 * Notice: 
 *     1.Call this interface after calling "tlkapi_file_open()" successfully.
 *****************************************************************************/
int tlkapi_file_size(FIL *pFile);

/******************************************************************************
 * Function: tlkapi_file_strlen
 * Descript: Gets the number of bytes used by the file path.
 * Params: 
 *     @pPath[IN]--The control block of the file object.
 * Return: The number of bytes used by the file path.
 *****************************************************************************/
int tlkapi_file_strlen(FCHAR *pPath);

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
int tlkapi_file_open(FIL *pFile, const FCHAR *pPath, uint08 mode);

/******************************************************************************
 * Function: tlkapi_file_close
 * Descript: Close an open file object.
 * Params: 
 *     @pFile[IN]--The control block of the file object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_close(FIL *pFile);

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
int tlkapi_file_seek(FIL *pFile, uint32 offset);

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
int tlkapi_file_read(FIL *pFile, void *pBuff, uint32 buffLen, uint32 *pReadLen);

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
int tlkapi_file_write(FIL *pFile, const void *pData, uint32 dataLen, uint32 *pWriteLen);


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
int tlkapi_file_opendir(DIR *pDir, const FCHAR *pPath);

/******************************************************************************
 * Function: tlkapi_file_closedir
 * Descript: Close an open directory.
 * Params: 
 *     @pDir[IN]--The control block of the directory object.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_closedir(DIR *pDir);

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
int tlkapi_file_readdir(DIR *pDir, FILINFO *pFileInfo);

/******************************************************************************
 * Function: tlkapi_file_findfirst
 * Descript: Find the first file in directory.
 * Params: 
 *     @pDir[IN]--The control block of the directory object.
 *     @fno[IN]--The file info.
 *     @pPath[IN]--Pointer to the file path.
 *     @pPattern[IN]--The file pattern.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_findfirst(DIR *pDir, FILINFO *pFileInfo, const FCHAR *pPath, const FCHAR *pPattern);

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
int tlkapi_file_findnext(DIR *pDir, FILINFO *pFileInfo);

#endif	// (TLK_FS_FAT_ENABLE)

/******************************************************************************
 * Function: tlkapi_file_mkdir
 * Descript: Create a sub directory.
 * Params: 
 *     @pPath[IN]--Pointer to the file path.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_mkdir(const FCHAR *pPath);

/******************************************************************************
 * Function: tlkapi_file_unlink
 * Descript: Delete an existing file or directory .
 * Params: 
 *     @pPath[IN]--Pointer to the file path.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.For fat32 systems, the operation error code can refer to "FRESULT".
 *****************************************************************************/
int tlkapi_file_unlink(const FCHAR *pPath);

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
int tlkapi_file_rename(const FCHAR *pOldPath, const FCHAR *pNewPath);

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
int tlkapi_file_mount(const FCHAR *pPath, uint08 opt);

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
int tlkapi_file_mkfs(const FCHAR *pPath, const void *pOpt, void *pWork, uint32 len);

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
int tlkapi_file_fdisk(uint08 pdrv, const uint32 ptbl[], void *pWork);



#endif

#endif //TLKAPI_FILE_H


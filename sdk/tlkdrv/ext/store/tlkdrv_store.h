/********************************************************************************************************
 * @file	tlkdrv_store.h
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
#ifndef TLKDRV_STORE_H
#define TLKDRV_STORE_H


#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T293A33_V1_0) ||\
    (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1)
	#define TLKDRV_STORE_XTSD01G_ENABLE        1
#else
	#define TLKDRV_STORE_XTSD01G_ENABLE        0
#endif


typedef enum{
	TLKDRV_STORE_DEV_XTSD01G = 0,
	TLKDRV_STORE_DEV_MAX,
}TLKDRV_STORE_DEV_ENUM;

typedef enum{
	TLKDRV_STOREOPCODE_NONE = 0,
	TLKDRV_STORE_OPCODE_SD_FORMAT,
	TLKDRV_STORE_OPCODE_GET_PAGE_SIZE,
	TLKDRV_STORE_OPCODE_GET_PAGE_NUMB,
	TLKDRV_STORE_OPCODE_GET_SECTOR_SIZE,
	TLKDRV_STORE_OPCODE_GET_SECTOR_NUMB,
	TLKDRV_STORE_OPCODE_GET_BLOCK_SIZE, //For SD Interface
	TLKDRV_STORE_OPCODE_GET_BLOCK_NUMB, //For SD Interface
}TLKDRV_STORE_OPCODE_ENUM;


typedef struct{
	bool(*IsOpen)(void);
	int(*Init)(void);
	int(*Open)(void);
	int(*Close)(void);
	int(*Erase)(uint32 addr, uint32 size);
	int(*Read)(uint32 addr, uint08 *pBuff, uint32 buffLen);
	int(*Write)(uint32 addr, uint08 *pData, uint32 dataLen);
	int(*SDInit)(void);
	int(*SDRead)(uint08 *pBuff, uint32 blkOffs, uint32 blkNumb);
	int(*SDWrite)(uint08 *pData, uint32 blkOffs, uint32 blkNumb);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
	void(*ShutDown)(void);
}tlkdrv_store_modinf_t;


bool tlkdrv_store_isOpen(TLKDRV_STORE_DEV_ENUM dev);

int tlkdrv_store_init(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_open(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_close(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_erase(TLKDRV_STORE_DEV_ENUM dev, uint32 addr, uint32 size);
int tlkdrv_store_read(TLKDRV_STORE_DEV_ENUM dev, uint32 addr, uint08 *pBuff, uint16 buffLen);
int tlkdrv_store_write(TLKDRV_STORE_DEV_ENUM dev, uint32 addr, uint08 *pData, uint16 dataLen);
int tlkdrv_store_sdInit(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_format(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_sdread(TLKDRV_STORE_DEV_ENUM dev, uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
int tlkdrv_store_sdwrite(TLKDRV_STORE_DEV_ENUM dev, uint08 *pData, uint32 blkOffs, uint16 blkNumb);
int tlkdrv_store_handler(TLKDRV_STORE_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);
void tlkdrv_store_shutDown(TLKDRV_STORE_DEV_ENUM dev);

int tlkdrv_store_getPageSize(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_getPageNumb(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_getSectorSize(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_getSectorNumb(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_getSdBlockSize(TLKDRV_STORE_DEV_ENUM dev);
int tlkdrv_store_getSdBlockNumb(TLKDRV_STORE_DEV_ENUM dev);


#endif //TLKDRV_STORE_H


/********************************************************************************************************
 * @file	tlkdrv_sram.h
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
#ifndef TLKDRV_SRAM_H
#define TLKDRV_SRAM_H


#define TLKDRV_SRAM_ASP1604_ENABLE      1


typedef enum{
	TLKDRV_SRAM_DEV_ASP1604 = 0,
	TLKDRV_SRAM_DEV_MAX,
}TLKDRV_SRAM_DEV_ENUM;


typedef struct{
	bool(*IsOpen)(void);
	int(*Init)(void);
	int(*Open)(void);
	int(*Close)(void);
	int(*Read)(uint32 addr, uint08 *pBuff, uint16 buffLen);
	int(*Write)(uint32 addr, uint08 *pData, uint16 dataLen);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
}tlkdrv_sram_modinf_t;


bool tlkdrv_sram_isOpen(TLKDRV_SRAM_DEV_ENUM dev);

int tlkdrv_sram_init(TLKDRV_SRAM_DEV_ENUM dev);
int tlkdrv_sram_open(TLKDRV_SRAM_DEV_ENUM dev);
int tlkdrv_sram_close(TLKDRV_SRAM_DEV_ENUM dev);
int tlkdrv_sram_read(TLKDRV_SRAM_DEV_ENUM dev, uint32 addr, uint08 *pBuff, uint16 buffLen);
int tlkdrv_sram_write(TLKDRV_SRAM_DEV_ENUM dev, uint32 addr, uint08 *pData, uint16 dataLen);
int tlkdrv_sram_handler(TLKDRV_SRAM_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);



#endif //TLKDRV_SRAM_H


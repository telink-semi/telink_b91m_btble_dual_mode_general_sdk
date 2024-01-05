/********************************************************************************************************
 * @file	tlkdev_sram.h
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
#ifndef TLKDEV_SRAM_H
#define TLKDEV_SRAM_H

#if (TLK_DEV_SRAM_ENABLE)



bool tlkdev_sram_isOpen(void);

int tlkdev_sram_init(void);
int tlkdev_sram_open(void);
int tlkdev_sram_close(void);
int tlkdev_sram_read(uint32 addr, uint08 *pBuff, uint16 buffLen);
int tlkdev_sram_write(TLKDRV_SRAM_DEV_ENUM dev, uint32 addr, uint08 *pData, uint16 dataLen);




#endif //TLK_DEV_SRAM_ENABLE

#endif //TLKDEV_SRAM_H



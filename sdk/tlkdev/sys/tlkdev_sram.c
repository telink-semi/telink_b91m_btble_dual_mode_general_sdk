/********************************************************************************************************
 * @file	tlkdev_sram.c
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
#if (TLK_DEV_SRAM_ENABLE)
#include "tlkdev/tlkdev_stdio.h"
#include "tlkdev/sys/tlkdev_sram.h"
#include "tlkdrv/ext/sram/tlkdrv_sram.h"



#define TLKDEV_SRAM_DEV        TLKDRV_SRAM_DEV_ASP1604



bool tlkdev_sram_isOpen(void)
{
	return tlkdrv_sram_isOpen(TLKDEV_SRAM_DEV);
}

int tlkdev_sram_init(void)
{
	return tlkdrv_sram_init(TLKDEV_SRAM_DEV);
}
int tlkdev_sram_open(void)
{
	return tlkdrv_sram_open(TLKDEV_SRAM_DEV);
}
int tlkdev_sram_close(void)
{
	return tlkdrv_sram_close(TLKDEV_SRAM_DEV);
}
int tlkdev_sram_read(uint32 addr, uint08 *pBuff, uint16 buffLen)
{
	return tlkdrv_sram_read(TLKDEV_SRAM_DEV, addr, pBuff, buffLen);
}
int tlkdev_sram_write(TLKDRV_SRAM_DEV_ENUM dev, uint32 addr, uint08 *pData, uint16 dataLen)
{
	return tlkdrv_sram_read(TLKDEV_SRAM_DEV, addr, pData, dataLen);
}





#endif //#if (TLK_DEV_SRAM_ENABLE)


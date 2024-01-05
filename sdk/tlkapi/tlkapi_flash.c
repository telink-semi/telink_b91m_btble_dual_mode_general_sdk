/********************************************************************************************************
 * @file	tlkapi_flash.c
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
#include "drivers.h"
#include "tlk_config.h"
#include "tlkapi_type.h"
#include "tlkapi_error.h"
#include "tlkapi_flash.h"




/******************************************************************************
 * Function: tlkapi_flash_init
 * Descript: Reserved interface, not used now.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkapi_flash_init(void)
{
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_flash_read
 * Descript: Reads data from on-chip flash.
 * Params: 
 *     @addr[IN]--The address of the flash to be read.
 *     @pBuff[OUT]--The buffer stored the data to be read.
 *     @buffLen[IN]--The length of the data to be read.
 * Return: None.
 *     1.This interface is equivalent to "flash_read_page()".
 *****************************************************************************/
void tlkapi_flash_read(uint32 addr, uint08 *pBuff, uint32 buffLen)
{
	flash_read_page(addr, buffLen, pBuff);
}

/******************************************************************************
 * Function: tlkapi_flash_write
 * Descript: Writes data into on-chip flash.
 * Params: 
 *     @addr[IN]--The address of the flash to be wrote.
 *     @pData[IN]--The data to write.
 *     @buffLen[IN]--The write data length.
 * Return: None.
 * Notice:
 *     1.This interface calls "flash_write_page()", but unlike 
 *       "flash_write_page()", this interface can write multiple pages at the 
 *       same time
 *****************************************************************************/
void tlkapi_flash_write(uint32 addr, uint08 *pData, uint32 dataLen)
{
	uint32 rl = 0;
	uint08 wl = addr&0xff;

	if(wl)
	{
		rl = 0x100 - wl;
		if(dataLen <= rl)
		{
			flash_write_page(addr, dataLen ,pData);
			return;
		}
		else
		{			
			flash_write_page(addr, rl ,pData);
			dataLen -= rl;
			pData += rl;
			addr += rl;
		}
	}
	while(dataLen)
	{
		if(dataLen > 0x100)
		{
			flash_write_page(addr, 0x100, pData);
			dataLen -= 0x100;
			pData += 0x100;
			addr += 0x100;
		}
		else
		{			
			flash_write_page(addr, dataLen, pData);
			break;
		}
	}
}

/******************************************************************************
 * Function: tlkapi_flash_writeByte
 * Descript: Writes data into on-chip flash.
 * Params: 
 *     @addr[IN]--The address of the flash to be wrote.
 *     @byte[IN]--The one byte of the data to be wrote.
 * Return: None.
 *****************************************************************************/
void tlkapi_flash_writeByte(uint32 addr, uint08 byte)
{
	flash_write_page(addr, 1, &byte);
}

/******************************************************************************
 * Function: tlkapi_flash_eraseSector
 * Descript: Erases a sector of flash on the chip.
 * Params: 
 *     @addr[IN]--The address of the sector in flash.
 * Return: None.
 *****************************************************************************/
void tlkapi_flash_eraseSector(uint32 addr)
{
	flash_erase_sector(addr);
}


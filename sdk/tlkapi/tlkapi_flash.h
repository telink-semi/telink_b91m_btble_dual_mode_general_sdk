/********************************************************************************************************
 * @file	tlkapi_flash.h
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
#ifndef TLKAPI_FLASH_H
#define TLKAPI_FLASH_H


/******************************************************************************
 * Function: tlkapi_flash_init
 * Descript: Reserved interface, not used now.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkapi_flash_init(void);

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
void tlkapi_flash_read(uint32 addr, uint08 *pBuff, uint32 buffLen);

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
void tlkapi_flash_write(uint32 addr, uint08 *pData, uint32 dataLen);

/******************************************************************************
 * Function: tlkapi_flash_writeByte
 * Descript: Writes data into on-chip flash.
 * Params: 
 *     @addr[IN]--The address of the flash to be wrote.
 *     @byte[IN]--The one byte of the data to be wrote.
 * Return: None.
 *****************************************************************************/
void tlkapi_flash_writeByte(uint32 addr, uint08 byte);

/******************************************************************************
 * Function: tlkapi_flash_eraseSector
 * Descript: Erases a sector of flash on the chip.
 * Params: 
 *     @addr[IN]--The address of the sector in flash.
 * Return: None.
 *****************************************************************************/
void tlkapi_flash_eraseSector(uint32 addr);



#endif //TLKAPI_FLASH_H


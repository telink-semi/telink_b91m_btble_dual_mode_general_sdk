/********************************************************************************************************
 * @file	tlkdrv_p25q32h.h
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
#ifndef TLKDEV_P25Q32H_H
#define TLKDEV_P25Q32H_H


#if (TLKDEV_EXT_PUYA_P25Q32H_ENABLE)


#define P25Q32H_SPI_CS_GPIO            GPIO_PD2

#define P25Q32H_PAGE_SIZE              256
#define P25Q32H_SECTOR_SIZE            4096
#define P25Q32H_TOTAL_SIZE             (16 * 1024 * 1024)
#define P25Q32H_SECTOR_COUNT           (P25Q32H_TOTAL_SIZE / P25Q32H_SECTOR_SIZE)
#define P25Q32H_BUSY_TIMEOUT_VALUE     (0x50000)



#define TLKDEV_P25Q32H_DISK_BLOCK_SIZE       512
#define TLKDEV_P25Q32H_DISK_BLOCK_NUMB       0x00008000 //16M
//#define TLKDEV_P25Q32H_DISK_BLOCK_NUMB       0x00002000 //4M
#define TLKDEV_P25Q32H_DISK_CLUSTER_SIZE     (32*1024)
#define TLKDEV_P25Q32H_DISK_CLUSTER_NUMB     (TLKDEV_P25Q32H_DISK_CLUSTER_SIZE/TLKDEV_P25Q32H_DISK_BLOCK_SIZE)

#define TLKDEV_P25Q32H_DISK_DBR_OFFSET       (4096) //(2*1024*1024)/512
#define TLKDEV_P25Q32H_DISK_FAT_COPIES       2

#define TLKDEV_P25Q32H_DISK_RSV_NUMB         8 //RSV - Reserve
#define TLKDEV_P25Q32H_DISK_FAT_NUMB         TLKDEV_P25Q32H_DISK_CLUSTER_NUMB


typedef enum{
	P25Q32H_CMDID_WRITE_STATUS       = 0x01,
	P25Q32H_CMDID_PAGE_PROGRAM       = 0x02,
	P25Q32H_CMDID_READ_DATA          = 0x03,
	P25Q32H_CMDID_WRITE_DISABLE      = 0x04,
	P25Q32H_CMDID_READ_STATUS        = 0x05,
	P25Q32H_CMDID_WRITE_ENABLE       = 0x06,
	P25Q32H_CMDID_FAST_READ_DATA     = 0x0B,
	P25Q32H_CMDID_SECTOR_ERASE       = 0x20,
	P25Q32H_CMDID_BLOCK32K_ERASE     = 0x52,
	P25Q32H_CMDID_BLOCK64K_ERASE     = 0xD8,
	P25Q32H_CMDID_PAGE_ERASE         = 0x81,
	P25Q32H_CMDID_MANUFACT_DEVICE_ID = 0x90,
	P25Q32H_CMDID_JEDEC_DEVICE_ID    = 0x9F,
	P25Q32H_CMDID_RELEASE_POWER_DOWN = 0xAB,
	P25Q32H_CMDID_POWER_DOWN         = 0xB9,
	P25Q32H_CMDID_CHIP_ERASE         = 0xC7,
}P25Q32H_CMDID_ENUM;




int p25q32h_init(void);

uint32 p25q32h_read(uint32 addr, uint08 *pdata, uint32 length);
uint32 p25q32h_write(uint32 addr, uint08 *pdata, uint32 length);
bool p25q32h_writePage(uint32 addr, uint08 *pdata, uint32 length);

void p25q32h_erasePage(uint32 addr);
void p25q32h_eraseSector(uint32 addr);
void p25q32h_eraseBlock32K(uint32 addr);
void p25q32h_eraseBlock64K(uint32 addr);
void p25q32h_eraseChip(void);



int tlkdev_p25q32h_diskInit(void);
int tlkdev_p25q32h_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
int tlkdev_p25q32h_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb);
int tlkdev_p25q32h_diskFormat(void);






#endif //#if (TLKDEV_EXT_PUYA_P25Q32H_ENABLE)

#endif //TLKDEV_P25Q32H_H


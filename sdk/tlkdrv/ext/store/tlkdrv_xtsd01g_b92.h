/********************************************************************************************************
 * @file	tlkdrv_xtsd01g.h
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
#ifndef TLKDRV_XTSD01G_B92_H
#define TLKDRV_XTSD01G_B92_H

#if (MCU_CORE_TYPE == MCU_CORE_B92)
#if (TLKDRV_STORE_XTSD01G_ENABLE)


#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
#define TLKDRV_XTSD01G_POWER_PIN    GPIO_PB7
#endif
#if (TLKHW_TYPE == TLKHW_TYPE_C1T293A33_V1_0)
#define TLKDRV_XTSD01G_POWER_PIN    GPIO_PA4
#endif
#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2 || TLKHW_TYPE == TLKHW_TYPE_C1T293A33_V1_0) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
#define TLKDRV_XTSD01G_CS_PIN       GPIO_PA0
#define TLKDRV_XTSD01G_CLK_PIN      GPIO_PB2
#define TLKDRV_XTSD01G_DO_PIN       GPIO_PB4
#define TLKDRV_XTSD01G_DI_PIN       GPIO_PB3
#endif


#define TLKDRV_XTSD01G_DMA_CHN      DMA6

#define TLKDRV_XTSD01G_SPI_MODULE   GSPI_MODULE


#define TLKDRV_XTSD01G_FPP_CLK      12000000
#define TLKDRV_XTSD01G_FOD_CLK      240000


#define TLKDRV_XTSD01G_TYPE_NONE    0X00
#define TLKDRV_XTSD01G_TYPE_MMC     0X01
#define TLKDRV_XTSD01G_TYPE_V1      0X02
#define TLKDRV_XTSD01G_TYPE_V2      0X04
#define TLKDRV_XTSD01G_TYPE_V2HC    0X06

#define TLKDRV_XTSD01G_CMD_00       0
#define TLKDRV_XTSD01G_CMD_01       1
#define TLKDRV_XTSD01G_CMD_02       2
#define TLKDRV_XTSD01G_CMD_08       8
#define TLKDRV_XTSD01G_CMD_09       9
#define TLKDRV_XTSD01G_CMD_10       10
#define TLKDRV_XTSD01G_CMD_12       12
#define TLKDRV_XTSD01G_CMD_16       16
#define TLKDRV_XTSD01G_CMD_17       17
#define TLKDRV_XTSD01G_CMD_18       18
#define TLKDRV_XTSD01G_CMD_23       23
#define TLKDRV_XTSD01G_CMD_24       24
#define TLKDRV_XTSD01G_CMD_25       25
#define TLKDRV_XTSD01G_CMD_32       32
#define TLKDRV_XTSD01G_CMD_33       33
#define TLKDRV_XTSD01G_CMD_38       38
#define TLKDRV_XTSD01G_CMD_41       41
#define TLKDRV_XTSD01G_CMD_55       55
#define TLKDRV_XTSD01G_CMD_58       58
#define TLKDRV_XTSD01G_CMD_59       59

#define TLKDRV_XTSD01G_MSD_DATA_OK              0x05
#define TLKDRV_XTSD01G_MSD_DATA_CRC_ERR         0x0B
#define TLKDRV_XTSD01G_MSD_DATA_WRITE_ERR       0x0D
#define TLKDRV_XTSD01G_MSD_DATA_OTHER_ERR       0xFF

#define TLKDRV_XTSD01G_MSD_RESPONSE_NO_ERR      0x00
#define TLKDRV_XTSD01G_MSD_IN_IDLE_STATE        0x01
#define TLKDRV_XTSD01G_MSD_ERASE_RESET          0x02
#define TLKDRV_XTSD01G_MSD_ILLEGAL_COMMAND      0x04
#define TLKDRV_XTSD01G_MSD_COM_CRC_ERR          0x08
#define TLKDRV_XTSD01G_MSD_ERASE_SEQUENCE_ERR   0x10
#define TLKDRV_XTSD01G_MSD_ADDRESS_ERR          0x20
#define TLKDRV_XTSD01G_MSD_PARAMETER_ERR        0x40
#define TLKDRV_XTSD01G_MSD_RESPONSE_FAILURE     0xFF



typedef struct{
	uint08 isInit;
	uint08 isOpen;
	uint08 sdType; //Refer TLKDRV_XTSD01G_TYPE_V2HC.
}tlkdrv_xtsd01g_t;



bool tlkdrv_xtsd01g_waitReady(uint32 waitTimeUs, uint08 value, uint08 chkMsk);
void tlkdrv_xtsd01g_readBytes(uint08 bytesNumb);
void tlkdrv_xtsd01g_writeBytes(uint08 *pData, uint08 dataLen);
bool tlkdrv_xtsd01g_readData(uint08 *pBuff, uint32 buffLen);
bool tlkdrv_xtsd01g_writeData(uint08 *pData, uint32 dataLen);

bool tlkdrv_xtsd01g_sendCmd(uint08 cmd, uint32 arg, uint08 crc, uint08 rspBytes, uint32 waitUs);

int tlkdrv_xtsd01g_readBlock(uint08 *pBuff, uint32 blkOffs);
int tlkdrv_xtsd01g_readBlocks(uint08 *pBuff, uint32 blkOffs, uint32 blockNumb);
int tlkdrv_xtsd01g_writeBlock(uint08 *pData, uint32 blkOffs);
int tlkdrv_xtsd01g_writeBlocks(uint08 *pData, uint32 blkOffs, uint32 blockNumb);


static bool tlkdrv_xtsd01g_isOpen(void);
static int tlkdrv_xtsd01g_init(void);
static int tlkdrv_xtsd01g_open(void);
static int tlkdrv_xtsd01g_close(void);
static int tlkdrv_xtsd01g_earse(uint32 addr, uint32 size);
static int tlkdrv_xtsd01g_read(uint32 addr, uint08 *pBuff, uint32 buffLen);
static int tlkdrv_xtsd01g_write(uint32 addr, uint08 *pData, uint32 dataLen);
static int tlkdrv_xtsd01g_sdinit(void);
static int tlkdrv_xtsd01g_sdread(uint08 *pBuff, uint32 blkOffs, uint32 blkNumb);
static int tlkdrv_xtsd01g_sdwrite(uint08 *pData, uint32 blkOffs, uint32 blkNumb);
static int tlkdrv_xtsd01g_handler(uint16 opcode, uint32 param0, uint32 param1);
static void tlkdrv_xtsd01g_shutDown(void);



#define TLKDRV_XTSD01G_DISK_BLOCK_SIZE       512
#define TLKDRV_XTSD01G_DISK_BLOCK_NUMB       0x0003E000 //128M-2M=126M
#define TLKDRV_XTSD01G_DISK_CLUSTER_SIZE     (32*1024)
#define TLKDRV_XTSD01G_DISK_CLUSTER_NUMB     (TLKDRV_XTSD01G_DISK_CLUSTER_SIZE/TLKDRV_XTSD01G_DISK_BLOCK_SIZE) //Block number per cluster
#define TLKDRV_XTSD01G_DISK_CLUSTER_COUNT    ((TLKDRV_XTSD01G_DISK_BLOCK_NUMB-8)/TLKDRV_XTSD01G_DISK_CLUSTER_NUMB) //cluster in disk

#define TLKDRV_XTSD01G_DISK_DBR_OFFSET       (4096) //(2*1024*1024)/512
#define TLKDRV_XTSD01G_DISK_FAT_COPIES       2

#define TLKDRV_XTSD01G_DISK_RSV_BLOCK_NUMB   8
#define TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB   TLKDRV_XTSD01G_DISK_CLUSTER_NUMB

int tlkdrv_xtsd01g_diskInit(void);
int tlkdrv_xtsd01g_diskRead(uint08 *pBuff, uint32 blkOffs);
int tlkdrv_xtsd01g_diskWrite(uint08 *pData, uint32 blkOffs);
int tlkdrv_xtsd01g_diskFormat(void);
int tlkdrv_xtsd01g_diskFormatFat16(void);
int tlkdrv_xtsd01g_diskFormatFat32(void);



#endif //#if (TLKDRV_STORE_XTSD01G_ENABLE)
#endif //#if (MCU_CORE_TYPE == MCU_CORE_B92)

#endif //TLKDRV_XTSD01G_B92_H


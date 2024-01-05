/********************************************************************************************************
 * @file	tlkdrv_p25q32h.c
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
#if (TLKDEV_EXT_PUYA_P25Q32H_ENABLE)
#include "tlkdev/ext/puya/tlkdev_p25q32h.h"


static void p25q32s_Interface_Init(SPI_GPIO_GroupTypeDef pin);
static void p25q32s_Interface_Read(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length);
static void p25q32s_Interface_Write(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length);

void p25q32h_setPowerState(bool isRelease);
bool p25q32h_writePage(uint32 addr, uint08 *pdata, uint32 length);

uint08 p25q32h_get_status(void);
bool p25q32h_busy_wait(void);
void p25q32h_write_command(uint08 command);
void p25q32h_write_enable(void);
void p25q32h_write_disable(void);
uint16 p25q32h_getChipId(void);
uint32 p25q32h_getJedecId(void);

static uint08 sTlkDevP25q32hIsReady = false;

int p25q32h_init(void)
{
	p25q32s_Interface_Init(SPI_GPIO_GROUP_A2A3A4D6);
	p25q32h_setPowerState(true);
	sleep_ms(200);
	sTlkDevP25q32hIsReady = true;
	return TLK_ENONE;
}

uint32 p25q32h_read(uint32 addr, uint08 *pdata, uint32 length)
{
	 uint08 cmd_data[] = {P25Q32H_CMDID_READ_DATA, addr >> 16, addr >> 8, addr >> 0,};
	 p25q32s_Interface_Read(cmd_data, sizeof(cmd_data), pdata, length);
	 return length;
}
uint32 p25q32h_write(uint32 addr, uint08 *pdata, uint32 length)
{
	 uint32 feasible_size    = length;
	 uint32 currentAddress   = addr;
	 uint32 currentEndOfPage = (currentAddress / P25Q32H_PAGE_SIZE + 1) * P25Q32H_PAGE_SIZE - 1;

	 if(length > (P25Q32H_TOTAL_SIZE - addr)) feasible_size = P25Q32H_TOTAL_SIZE - addr;

	 uint32 bytes_left_to_send = feasible_size;
	 uint32 bytes_written      = 0;

	 while(bytes_written < feasible_size){
		if(currentAddress + bytes_left_to_send > currentEndOfPage) bytes_left_to_send = currentEndOfPage - currentAddress + 1;

		if(p25q32h_writePage(currentAddress, pdata + bytes_written, bytes_left_to_send) == false) return 0;

		bytes_written     += bytes_left_to_send;
		currentAddress     = currentEndOfPage + 1;
		currentEndOfPage  += P25Q32H_PAGE_SIZE;
		bytes_left_to_send = feasible_size - bytes_written;
	 }
	 return bytes_written;
}
bool p25q32h_writePage(uint32 addr, uint08 *pdata, uint32 length)
{
	 p25q32h_write_enable();

	 uint08 cmd_data[] = {P25Q32H_CMDID_PAGE_PROGRAM, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), pdata, length);
	 return p25q32h_busy_wait();
}


void p25q32h_erasePage(uint32 addr)
{
	 p25q32h_write_enable();

	 uint08 cmd_data[] = {P25Q32H_CMDID_PAGE_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 p25q32h_busy_wait();
}
void p25q32h_eraseSector(uint32 addr)
{
	 p25q32h_write_enable();

	 uint08 cmd_data[] = {P25Q32H_CMDID_SECTOR_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 p25q32h_busy_wait();
}
void p25q32h_eraseBlock32K(uint32 addr)
{
	 p25q32h_write_enable();

	 uint08 cmd_data[] = {P25Q32H_CMDID_BLOCK32K_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 p25q32h_busy_wait();
}
void p25q32h_eraseBlock64K(uint32 addr)
{
	 p25q32h_write_enable();

	 uint08 cmd_data[] = {P25Q32H_CMDID_BLOCK64K_ERASE, addr >> 16, addr >> 8, addr >> 0, };
	 p25q32s_Interface_Write(cmd_data, sizeof(cmd_data), NULL , 0);

	 p25q32h_busy_wait();
}


void p25q32h_eraseChip(void)
{
	 p25q32h_write_enable();
	 p25q32h_write_command(P25Q32H_CMDID_CHIP_ERASE);
	 while(p25q32h_get_status() & 0x01);
}



uint08 p25q32h_get_status(void)
{
	 uint08 status;
	 uint08 command = P25Q32H_CMDID_READ_STATUS;
	 p25q32s_Interface_Read(&command, sizeof(command), &status, sizeof(status));
	 return status;
}

bool p25q32h_busy_wait(void)
{
	 for(int index = 0; index < P25Q32H_BUSY_TIMEOUT_VALUE; index ++){
		 if(!(p25q32h_get_status() & 0x01)){
		 	 return true;
		 }
	 }
	 return false;
}

void p25q32h_write_command(uint08 command)
{
	 p25q32s_Interface_Write(&command, sizeof(command), NULL, 0);
	 p25q32h_busy_wait();
}

void p25q32h_write_enable(void)
{
	 p25q32h_write_command(P25Q32H_CMDID_WRITE_ENABLE);
}

void p25q32h_write_disable(void)
{
	 p25q32h_write_command(P25Q32H_CMDID_WRITE_DISABLE);
}

uint16 p25q32h_getChipId(void)
{
     uint08 cmd_data[] = {P25Q32H_CMDID_MANUFACT_DEVICE_ID, 0x00, 0x00, 0x00,};
     uint08 chip_id[2]; // [0] manufacturer_id  [1] device_id
     p25q32s_Interface_Read(cmd_data, sizeof(cmd_data), chip_id, sizeof(chip_id));
     return (uint16)((chip_id[0] << 8) | chip_id[1]);
}

uint32 p25q32h_getJedecId(void)
{
     uint08 cmd_data[] = {P25Q32H_CMDID_JEDEC_DEVICE_ID, 0x00, 0x00, 0x00,};
     uint08 jedec_id[3]; // [0] manufacturer_id  [1] memory_type_id  [2] capacity_id
     p25q32s_Interface_Read(cmd_data, sizeof(cmd_data), jedec_id, sizeof(jedec_id));
     return (uint32)((jedec_id[0] << 16) | (jedec_id[1] << 8) | jedec_id[2]);
}

void p25q32h_setPowerState(bool isRelease)
{
	 if(isRelease){
		 p25q32h_write_command(P25Q32H_CMDID_RELEASE_POWER_DOWN);
	 }else{
		 p25q32h_write_command(P25Q32H_CMDID_POWER_DOWN);
	 }
}








static void p25q32s_Interface_Init(SPI_GPIO_GroupTypeDef pin)
{
	spi_master_init(0x00,SPI_MODE0);
	spi_master_gpio_set(pin);
	spi_masterCSpin_select(P25Q32H_SPI_CS_GPIO);
	gpio_setup_up_down_resistor(P25Q32H_SPI_CS_GPIO,PM_PIN_PULLUP_1M);
}
static void p25q32s_Interface_Write(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length)
{
	spi_write(cmd_data, cmd_length, pdata, length, P25Q32H_SPI_CS_GPIO);
}
static void p25q32s_Interface_Read(uint08 *cmd_data, uint32 cmd_length, uint08 *pdata, uint32 length)
{
	 spi_read(cmd_data, cmd_length, pdata, length, P25Q32H_SPI_CS_GPIO);
}







///////////////////////////////////////////////////////////////////////////////////////////////////

/********** The time format (16bits) is:**************
Bits15 ~ 11 represents hours, which can be 0 ~ 23;
bits10 ~ 5 represents minutes, which can be 0 ~ 59;
bits4 ~ 0 represents seconds, which can be 0 ~ 29, and each unit is 2 seconds, which means that the actual second value is 2 times of the value.
*/

/********* The date format (16bits) is:*************
 Bits15 ~ 9 represents the year, which can be 0 ~ 127. It represents the difference from 1980,
 That is to say, the actual year is this value plus 1980, which can be expressed up to 2107;

 Bits8 ~ 5 represents the month, which can be 1 ~ 12;
 Bits4 ~ 0 represents the number and can be 1 ~ 31.
*/

//Find the high byte of 16 bit time format
#define TLKDEV_P25Q32H_DISK_TIME_HB(H,M,S)    (unsigned char)(((((H)<<11))|((M)<<5)|(S))>>8)
//Find the low byte of 16 bit time format
#define TLKDEV_P25Q32H_DISK_TIME_LB(H,M,S)    (unsigned char)((((H)<<11))|((M)<<5)|(S))
//Find the high byte of 16 bit date format
#define TLKDEV_P25Q32H_DISK_DATE_HB(Y,M,D)    (unsigned char)(((((Y)-1980)<<9)|((M)<<5)|(D))>>8)
//Find the low byte of 16 bit date format
#define TLKDEV_P25Q32H_DISK_DATE_LB(Y,M,D)    (unsigned char)((((Y)-1980)<<9)|((M)<<5)|(D))

static const char scTlkDevP25q32hDiskReadMe[] = "This is DualMode SDK Fat16 demo\r\n";

static const uint08 scTlkDevP25q32hDiskMBR[16] =
{
	0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff, (TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff000000)>>24, 0x00, 0x00, 0x00, 0x00,
};
static const uint08 scTlkDevP25q32hDiskDBR[64] =
{
	0xEB, 0x3C, 0x90, //Jump instruction, cannot be changed to 0, otherwise it prompts that it is not formatted (0~2)
	'M','S','D','O','S','5','.','0', //File system and version information "MSDOS5.0" (3~10)
	TLKDEV_P25Q32H_DISK_BLOCK_SIZE & 0xFF, (TLKDEV_P25Q32H_DISK_BLOCK_SIZE & 0xFF00) >> 8, //Number of bytes per sector, 512 bytes (11~12)
	TLKDEV_P25Q32H_DISK_CLUSTER_NUMB, //Number of sectors per cluster, 64 sectors (13)
	TLKDEV_P25Q32H_DISK_RSV_NUMB & 0xFF, (TLKDEV_P25Q32H_DISK_RSV_NUMB & 0xFF00) >> 8, //Number of reserved sectors, 8 (14~15)
	TLKDEV_P25Q32H_DISK_FAT_COPIES, //The number of FAT copies of this partition, which is 2 (16)
	(TLKDEV_P25Q32H_DISK_CLUSTER_SIZE >> 5) & 0xFF, (TLKDEV_P25Q32H_DISK_CLUSTER_SIZE >> 13) & 0xFF, //Number of root directory entries, 1024(00 04) entries (17~18). Unit:32Bytes
	(TLKDEV_P25Q32H_DISK_BLOCK_NUMB & 0xff), (TLKDEV_P25Q32H_DISK_BLOCK_NUMB & 0xff00)>>8, //The number of small sectors (<=32M), here is 0, which means to read the value from the large sector field (19~20)
	0xF8, //Media descriptor, 0xF8 means hard disk (21)
	(TLKDEV_P25Q32H_DISK_FAT_NUMB & 0xFF), (TLKDEV_P25Q32H_DISK_FAT_NUMB & 0xFF00)>>8, //Number of sectors per FAT, 64 (22~23)
	(TLKDEV_P25Q32H_DISK_CLUSTER_NUMB & 0xFF), (TLKDEV_P25Q32H_DISK_CLUSTER_NUMB & 0xFF00)>>8, //Number of sectors per track, 64 (24~25)
	(TLKDEV_P25Q32H_DISK_CLUSTER_NUMB & 0xFF), (TLKDEV_P25Q32H_DISK_CLUSTER_NUMB & 0xFF00)>>8, //The number of heads is 256M/32K-0x200 (16M) (reserve 512 32k sectors for program writing) (26~27)
	(TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff), (TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDEV_P25Q32H_DISK_DBR_OFFSET & 0xff000000)>>24, //Number of hidden sectors There is no hidden sector here, it is 0 (28~31)
	0x00, 0x00, 0x00, 0x00,  //Number of large sectors, the total number of sectors, 256k is 0x200 (32~35)
	0x80, //Disk drive parameters, 80 means hard disk (36)
	0x00, //Reserved (37)
	0x29, //Extended boot tag, 0x29 means the next three fields are available (38)
	0x34, 0x12, 0x00, 0x00, //Label serial number (39~42)
	'T', 'L', 'K', '-', 'F', 'S', '-', 'D', 'E', 'M', 'O', //Disk label volume (43~53)
	'F', 'A', 'T', '1', '6', 0x20, 0x20, 0x20, 0x00, 0x00, //File system type information, the string "FAT16" (54~63)
};
static const uint08 scTlkDevP25q32hDiskFat[6] =
{
	//------------- Block1: FAT16 Table -------------//
	0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF// // first 2 entries must be F8FF, third entry is cluster end of readme file };
};
static const uint08 scTlkDevP25q32hDiskRoot[64] = //------------- Block2: Root Directory -------------//
{
	// first entry is volume label
	'T' , 'L' , 'K' , '-' , 'F' , 'S' , '-' , 'D' , 'E' , 'M' , 'O',
	0x08, //File property, indicating the label volume of the disk
	0x00, //Reserved
	0x00, //Create time millisecond timestamp

	//File creation time, 15:27:35
	TLKDEV_P25Q32H_DISK_TIME_LB(15,27,35), TLKDEV_P25Q32H_DISK_TIME_HB(15,27,35), //0x00, 0x00,
	//File creation date, August 19, 2008
	TLKDEV_P25Q32H_DISK_DATE_LB(2008,8,19), TLKDEV_P25Q32H_DISK_DATE_HB(2008,8,19), //0x00, 0x00,
	//Last visit date, August 20, 2008
	TLKDEV_P25Q32H_DISK_DATE_LB(2008,8,20), TLKDEV_P25Q32H_DISK_DATE_HB(2008,8,20), //0x00, 0x00,
	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:36:47
	TLKDEV_P25Q32H_DISK_TIME_LB(15,36,47), TLKDEV_P25Q32H_DISK_TIME_HB(15,36,47), //0x4F, 0x6D,
	//Last revision date, August 19, 2008
	TLKDEV_P25Q32H_DISK_DATE_LB(2008,8,19), TLKDEV_P25Q32H_DISK_DATE_HB(2008,8,19), //0x65, 0x43,

	0x00, 0x00,  //Start cluster low word
	0x00, 0x00, 0x00, 0x00, //file length

	// second entry is readme file
	'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' ,
	0x00, //File properties, representing read-write files
	0x00, //Reserved
	0x00, //0xC6, //0x00-Create time millisecond timestamp

	//File creation time, 15:48:26
	TLKDEV_P25Q32H_DISK_TIME_LB(15,48,26), TLKDEV_P25Q32H_DISK_TIME_HB(15,48,26),
	//File creation date, August 19, 2008
	TLKDEV_P25Q32H_DISK_DATE_LB(2008,8,19), TLKDEV_P25Q32H_DISK_DATE_HB(2008,8,19),
	//Last visit date
	TLKDEV_P25Q32H_DISK_DATE_LB(2008,8,20), TLKDEV_P25Q32H_DISK_DATE_HB(2008,8,20),

	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:50:33
	TLKDEV_P25Q32H_DISK_TIME_LB(15,50,33), TLKDEV_P25Q32H_DISK_TIME_HB(15,50,33), //0x88, 0x6D,
	//Last revision date, August 19, 2008
	TLKDEV_P25Q32H_DISK_DATE_LB(2008,8,19), TLKDEV_P25Q32H_DISK_DATE_HB(2008,8,19), //0x65, 0x43,

	0x02, 0x00, //Start cluster low word, cluster 2.
	sizeof(scTlkDevP25q32hDiskReadMe)-1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
};



int tlkdev_p25q32h_diskInit(void)
{
	int ret;
	uint32 blkCount;
	uint08 temp[512];

	if(!sTlkDevP25q32hIsReady) return -TLK_ENOREADY;

	ret = tlkdev_p25q32h_diskRead(temp, 0, 1);
	if(ret != TLK_ENONE) return TLK_ENONE;
	if((temp[510] != 0x55) || (temp[511] != 0xaa)) return -TLK_EFAIL;

	blkCount = (temp[461]<<24) + (temp[460]<<16) + (temp[459]<<8) + temp[458];
	if(blkCount != TLKDEV_P25Q32H_DISK_BLOCK_NUMB){
		ret = tlkdev_p25q32h_diskFormat();
	}
	return ret;
}
int tlkdev_p25q32h_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	uint08 index;
	uint32 baseAddr = blkOffs*TLKDEV_P25Q32H_DISK_BLOCK_SIZE;
	uint08 *pData = pBuff;
	for(index=0; index<blkNumb; index++){
		p25q32h_read(baseAddr, pData, TLKDEV_P25Q32H_DISK_BLOCK_SIZE);
		baseAddr += TLKDEV_P25Q32H_DISK_BLOCK_SIZE;
		pData    += TLKDEV_P25Q32H_DISK_BLOCK_SIZE;
	}
	return TLK_ENONE;
}
int tlkdev_p25q32h_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	uint08 index;
	uint32 pageNum;
	uint08 buffer[4096];
	uint08 *pWrite;
	uint32 sectAddr;
	uint32 baseAddr = blkOffs*TLKDEV_P25Q32H_DISK_BLOCK_SIZE;
	for(index=0; index<blkNumb; index++){
		pWrite   = buffer;
		sectAddr = baseAddr & 0xFFFFF000;
		p25q32h_read(baseAddr & 0xFFFFF000, buffer, 4096);
		p25q32h_eraseSector(baseAddr);
		memcpy(buffer+(baseAddr & 0xFFF), pData, TLKDEV_P25Q32H_DISK_BLOCK_SIZE);
		baseAddr += TLKDEV_P25Q32H_DISK_BLOCK_SIZE;
		pData    += TLKDEV_P25Q32H_DISK_BLOCK_SIZE;
		for(pageNum=0; pageNum<(4096/P25Q32H_PAGE_SIZE); pageNum++){
			p25q32h_writePage(sectAddr, pWrite, P25Q32H_PAGE_SIZE);
			sectAddr += P25Q32H_PAGE_SIZE;
			pWrite   += P25Q32H_PAGE_SIZE;
		}
	}
	return TLK_ENONE;
}
int tlkdev_p25q32h_diskFormat(void)
{
	uint08 index;
	uint08 fatNum;
	uint32 offset;
	uint08 block[TLKDEV_P25Q32H_DISK_BLOCK_SIZE];

	if(!sTlkDevP25q32hIsReady) return -TLK_ENOREADY;

	offset = TLKDEV_P25Q32H_DISK_DBR_OFFSET;

	//write DBR
	memset(block, 0, TLKDEV_P25Q32H_DISK_BLOCK_SIZE);
	memcpy(block, scTlkDevP25q32hDiskDBR, sizeof(scTlkDevP25q32hDiskDBR));
	block[510] = 0x55;
	block[511] = 0xaa;
	tlkdev_p25q32h_diskWrite(block, offset, 1);
	memset(block, 0, TLKDEV_P25Q32H_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDEV_P25Q32H_DISK_RSV_NUMB; index ++){
		tlkdev_p25q32h_diskWrite(block, offset+index, 1);
	}
	offset += index;

	//write FAT
	for(fatNum = 0; fatNum < TLKDEV_P25Q32H_DISK_FAT_COPIES; fatNum++){
		memcpy(block, scTlkDevP25q32hDiskFat, sizeof(scTlkDevP25q32hDiskFat));
		tlkdev_p25q32h_diskWrite(block, offset, 1);
		memset(block, 0, TLKDEV_P25Q32H_DISK_BLOCK_SIZE);
		for(index = 1; index < TLKDEV_P25Q32H_DISK_FAT_NUMB; index ++){
			tlkdev_p25q32h_diskWrite(block, offset+index, 1);
		}
		offset += index;
	}

	//write root dir
	memcpy(block, scTlkDevP25q32hDiskRoot, sizeof(scTlkDevP25q32hDiskRoot));
	tlkdev_p25q32h_diskWrite(block, offset, 1);
	memset(block, 0, TLKDEV_P25Q32H_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDEV_P25Q32H_DISK_CLUSTER_NUMB; index ++){
		tlkdev_p25q32h_diskWrite(block, offset+index, 1);
	}
	offset += index;

	//write file
	memcpy(block, scTlkDevP25q32hDiskReadMe, sizeof(scTlkDevP25q32hDiskReadMe));
	tlkdev_p25q32h_diskWrite(block, offset, 1);
	offset ++;

	//write MBR
	memset(block, 0, TLKDEV_P25Q32H_DISK_BLOCK_SIZE);
	memcpy(&block[446], scTlkDevP25q32hDiskMBR, sizeof(scTlkDevP25q32hDiskMBR));
	block[461] = (TLKDEV_P25Q32H_DISK_BLOCK_NUMB & 0xff000000)>>24;
	block[460] = (TLKDEV_P25Q32H_DISK_BLOCK_NUMB & 0xff0000)>>16;
	block[459] = (TLKDEV_P25Q32H_DISK_BLOCK_NUMB & 0xff00)>>8;
	block[458] = (TLKDEV_P25Q32H_DISK_BLOCK_NUMB & 0xff);
	block[510] = 0x55;
	block[511] = 0xaa;
	tlkdev_p25q32h_diskWrite(block, 0, 1);

	return TLK_ENONE;
}




#endif //#if (TLKDEV_EXT_PUYA_TLKDEV_P25Q32H_ENABLE)


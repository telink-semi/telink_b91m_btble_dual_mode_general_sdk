/********************************************************************************************************
 * @file	tlkdrv_xtsd01g.c
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
#if (MCU_CORE_TYPE == MCU_CORE_B91)
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdrv_store.h"
#if (TLKDRV_STORE_XTSD01G_ENABLE)
#include "tlkdrv_xtsd01g_b91.h"

#include "drivers.h"



#define TLKDRV_XTSD01G_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_XTSD01G_DBG_SIGN     "[DRV]"



#define TLKDRV_XTSD01G_IRQ_DISABLE()     uint32 rie = core_disable_interrupt()
#define TLKDRV_XTSD01G_IRQ_RESTORE()     core_restore_interrupt(rie)

extern void pspi_set_pin_mux(pspi_pin_def_e pin);
extern void spi_set_dma(dma_chn_e spi_dma_chn, unsigned int src_addr, unsigned int dst_addr, unsigned int len);

static void tlkdrv_xtsd01g_gpioConfig(void);

static void tlkdrv_xtsd01g_spiCsLow(void);
static void tlkdrv_xtsd01g_spiCsHigh(void);

static uint08 tlkdrv_xtsd01g_spiSelect(void);
static void   tlkdrv_xtsd01g_spiDisSelect(void);

static uint08 tlkdrv_xtsd01g_spiGetResponse(uint08 Response);
static uint08 tlkdrv_xtsd01g_spiRecvData(uint08 *pBuff, uint16 buffLen);
static uint08 tlkdrv_xtsd01g_spiSendData(uint08 *pData, uint08 dataCmd);

//static void tlkdrv_xtsd01g_switchLowSpeed(void);
static void tlkdrv_xtsd01g_switchHighSpeed(void);

static uint08 tlkdrv_xtsd01g_sendCmd(uint08 cmd, uint32 arg, uint08 crc);


static uint08 tlkdrv_xtsd01g_waitReady(void);

static void tlkdrv_xtsd01g_writeByte(uint08 TxData);
static uint08 tlkdrv_xtsd01g_readByte(void);

static void tlkdrv_xtsd01g_spiRead(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen);
static void tlkdrv_xtsd01g_spiReadDma(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen);

static void tlkdrv_xtsd01g_readData(uint08 *pBuff, uint32 buffLen);
static void tlkdrv_xtsd01g_writeData(uint08 *pData, uint32 dataLen);



//extern unsigned char s_hspi_tx_dma_chn;
extern unsigned char s_hspi_rx_dma_chn;
//extern unsigned char s_pspi_tx_dma_chn;
extern unsigned char s_pspi_rx_dma_chn;

#if (TLKDRV_XTSD01G_SPI_MODULE == TLKDRV_XTSD01G_SPI_PSPI)
static pspi_pin_config_t sTlkDevXtsd04gPspiPinCfg = {
	.pspi_clk_pin		= PSPI_CLK_PC5,
	.pspi_csn_pin 		= PSPI_NONE_PIN,
	.pspi_mosi_io0_pin  = PSPI_MOSI_IO0_PC7,
	.pspi_miso_io1_pin  = PSPI_MISO_IO1_PC6,//3line mode set none
};
#elif (TLKDRV_XTSD01G_SPI_MODULE == TLKDRV_XTSD01G_SPI_HSPI)
static hspi_pin_config_t sTlkDevXtsd04gHspiPinCfg = {
	.hspi_clk_pin		= HSPI_CLK_PB4,
	.hspi_csn_pin 		= HSPI_NONE_PIN,
	.hspi_mosi_io0_pin  = HSPI_MOSI_IO0_PB3,
	.hspi_miso_io1_pin  = HSPI_MISO_IO1_PB2,//3line mode set none
	.hspi_wp_io2_pin    = HSPI_NONE_PIN,//set quad mode otherwise set none
	.hspi_hold_io3_pin  = HSPI_NONE_PIN,//set quad mode otherwise set none
};
#endif

static uint08 sTlkDevXtsd01gIsOK = true;
static uint08 sTlkDevXtsd01gType = 0;
static uint08 sTlkDevXtsd01gIsEnter = 0;
static uint08 sTlkDevXtsd01gIsReady = false;
static uint08 sTlkDevXtsd01gPowerIsOn = false;


const tlkdrv_store_modinf_t gcTlkDrvXtsd01gInf = {
	tlkdrv_xtsd01g_isOpen, //IsOpen
	tlkdrv_xtsd01g_init, //Init
	tlkdrv_xtsd01g_open, //Open
	tlkdrv_xtsd01g_close, //Close
	tlkdrv_xtsd01g_earse, //Close
	tlkdrv_xtsd01g_read, //Read
	tlkdrv_xtsd01g_write, //Write
	tlkdrv_xtsd01g_sdinit, //SDInit
	tlkdrv_xtsd01g_sdread, //SDRead
	tlkdrv_xtsd01g_sdwrite, //SDWrite
	tlkdrv_xtsd01g_handler, //Handler
	tlkdrv_xtsd01g_shutDown, //ShutDown
};


static bool tlkdrv_xtsd01g_isOpen(void)
{
	return (sTlkDevXtsd01gIsReady && sTlkDevXtsd01gPowerIsOn);
}
static int tlkdrv_xtsd01g_init(void)
{
	
	return TLK_ENONE;
}
static int tlkdrv_xtsd01g_open(void)
{
	uint08 r1;
    uint16 retry;
    uint08 buf[10];
	uint16 i;
	
	if(!sTlkDevXtsd01gIsOK) return -TLK_EFAIL;
	
	sTlkDevXtsd01gIsEnter = 1;
	
	tlkdrv_xtsd01g_powerOn();
	delay_ms(10); // delay_ms(50); delay 250ms for voltage setup.
	
	tlkdrv_xtsd01g_gpioConfig();

	//74 clock
 	for(i=0; i<10; i++){
 		tlkdrv_xtsd01g_writeByte(0xFF);
 	}
	
	retry=0X10;
	//enable spi mode
	while(--retry){
		r1 = tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_00, 0, 0x95);
		if(r1 == 0x01) break;
		delay_us(100);
	}
	if(r1 != 0x01){
		#if(TLKDRV_XTSD01G_DEBUG_ENABLE)
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "sd_nand_flash_init");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "enable spi mode fail");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_handler(); /* open this for usb log printf*/
		#endif
		tlkdrv_xtsd01g_spiDisSelect();
		sTlkDevXtsd01gIsEnter = 0;
		return -TLK_EFAIL;
	}
	
	//wait for busy
	sTlkDevXtsd01gType=0;
	retry=500;
	do{
		tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_55,0,1);
		delay_us(10);
		r1 = tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_41, 0x40000000, 0X01);
		delay_ms(1);
	}while(r1 && retry--);
	
	if(retry == 0){
		sTlkDevXtsd01gIsOK = false;
		#if(TLKDRV_XTSD01G_DEBUG_ENABLE)
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "sd_nand_flash_init");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "wait for busy fail");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_handler(); /* open this for usb log printf*/
		#endif
		tlkdrv_xtsd01g_spiDisSelect();
		sTlkDevXtsd01gIsEnter = 0;
		return -TLK_EFAIL;
	}
	
	r1 = tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_58,0,0X01);
	if(r1==0){
		for(i=0;i<4;i++)buf[i]=tlkdrv_xtsd01g_readByte();	//80 FF 80 00
		if(buf[0]&0x40)sTlkDevXtsd01gType=TLKDRV_XTSD01G_TYPE_V2HC;
		else sTlkDevXtsd01gType=TLKDRV_XTSD01G_TYPE_V2;
	}else{
		tlkdrv_xtsd01g_spiDisSelect();
		sTlkDevXtsd01gIsEnter = 0;
		return -TLK_EFAIL;
	}
	
	tlkdrv_xtsd01g_spiDisSelect();
	tlkdrv_xtsd01g_switchHighSpeed();
	
	sTlkDevXtsd01gIsEnter = 0;
	sTlkDevXtsd01gIsReady = true;

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "sd_nand_flash_init: %d", sTlkDevXtsd01gType);
	
	return TLK_ENONE;
}
static int tlkdrv_xtsd01g_close(void)
{
	tlkdrv_xtsd01g_powerOff();
	
	return TLK_ENONE;
}

static int tlkdrv_xtsd01g_earse(uint32 addr, uint32 size)
{
	return TLK_ENONE;
}
static int tlkdrv_xtsd01g_read(uint32 addr, uint08 *pBuff, uint32 buffLen)
{
	uint32 blkOffs;
	uint32 blkNumb;
	
	blkOffs = addr >> 9;
	blkNumb = buffLen >> 9;
	if(tlkdrv_xtsd01g_sdread(pBuff, blkOffs, blkNumb) == TLK_ENONE){
		return (blkNumb << 9);
	}else{
		return -TLK_EFAIL;
	}
}
static int tlkdrv_xtsd01g_write(uint32 addr, uint08 *pData, uint32 dataLen)
{
	uint32 blkOffs;
	uint32 blkNumb;
	
	blkOffs = addr >> 9;
	blkNumb = dataLen >> 9;
	if(tlkdrv_xtsd01g_sdwrite(pData, blkOffs, blkNumb) == TLK_ENONE){
		return (blkNumb << 9);
	}else{
		return -TLK_EFAIL;
	}
}
static int tlkdrv_xtsd01g_sdinit(void)
{
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_sdinit 01");
	if(!tlkdrv_xtsd01g_isOpen()) return -TLK_ENOREADY;
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_sdinit 02");
	return tlkdrv_xtsd01g_diskInit();
}
static int tlkdrv_xtsd01g_sdread(uint08 *pBuff, uint32 blkOffs, uint32 blkNumb)
{
	uint16 index;
	if(!tlkdrv_xtsd01g_isOpen()) return -TLK_ENOREADY;
//	if(blkNumb == 1) return tlkdrv_xtsd01g_readBlock(pBuff, blkOffs);
//	else return tlkdrv_xtsd01g_readBlocks(pBuff, blkOffs, blkNumb);
	for(index=0; index<blkNumb; index++){
		if(tlkdrv_xtsd01g_readBlock(pBuff, blkOffs+index) != TLK_ENONE) break;
		pBuff += 512;
	}
	if(index == blkNumb) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkdrv_xtsd01g_sdwrite(uint08 *pData, uint32 blkOffs, uint32 blkNumb)
{
	uint16 index;
	if(!tlkdrv_xtsd01g_isOpen()) return -TLK_ENOREADY;
//	if(blkNumb == 1) return tlkdrv_xtsd01g_writeBlock(pData, blkOffs);
//	else return tlkdrv_xtsd01g_writeBlocks(pData, blkOffs, blkNumb);
	for(index=0; index<blkNumb; index++){
		if(tlkdrv_xtsd01g_writeBlock(pData, blkOffs+index) != TLK_ENONE) break;
		pData += 512;
	}
	if(index == blkNumb) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkdrv_xtsd01g_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	if(!tlkdrv_xtsd01g_isOpen()) return -TLK_ENOREADY;
	switch(opcode){
		case TLKDRV_STORE_OPCODE_SD_FORMAT:
			return tlkdrv_xtsd01g_diskFormat();
		case TLKDRV_STORE_OPCODE_GET_PAGE_SIZE:
			return -TLK_ENOSUPPORT;
		case TLKDRV_STORE_OPCODE_GET_PAGE_NUMB:
			return -TLK_ENOSUPPORT;
		case TLKDRV_STORE_OPCODE_GET_SECTOR_SIZE:
			return -TLK_ENOSUPPORT;
		case TLKDRV_STORE_OPCODE_GET_SECTOR_NUMB:
			return -TLK_ENOSUPPORT;
		case TLKDRV_STORE_OPCODE_GET_BLOCK_SIZE:
			return TLKDRV_STORE_OPCODE_GET_BLOCK_SIZE;
		case TLKDRV_STORE_OPCODE_GET_BLOCK_NUMB:
			return TLKDRV_STORE_OPCODE_GET_BLOCK_NUMB;
	}
	return -TLK_ENOSUPPORT;
}
static void tlkdrv_xtsd01g_shutDown(void)
{
	gpio_shutdown(GPIO_PC4|GPIO_PC5|GPIO_PC6|GPIO_PC7);	
	tlkdrv_xtsd01g_powerOff();
}



/******************************************************************************
 * Function: tlkdrv_xtsd01g_powerOn, tlkdrv_xtsd01g_powerOff.
 * Descript: .
 * Params: None.
 * Return: None.
*******************************************************************************/
bool tlkdrv_xtsd01g_powerIsOn(void)
{
	return sTlkDevXtsd01gPowerIsOn;
}
void tlkdrv_xtsd01g_powerOn(void)
{
	if(sTlkDevXtsd01gPowerIsOn) return;
	
	gpio_function_en(TLKDRV_XTSD01G_POWER_PIN);
	gpio_output_en(TLKDRV_XTSD01G_POWER_PIN);
	
	gpio_set_low_level(TLKDRV_XTSD01G_POWER_PIN);		//low active
	gpio_set_up_down_res(TLKDRV_XTSD01G_POWER_PIN, GPIO_PIN_PULLDOWN_100K);
	
	sTlkDevXtsd01gPowerIsOn = true;
}
void tlkdrv_xtsd01g_powerOff(void)
{
	if(!sTlkDevXtsd01gPowerIsOn) return;
	
	gpio_function_en(TLKDRV_XTSD01G_POWER_PIN);
	gpio_output_en(TLKDRV_XTSD01G_POWER_PIN);
	
	gpio_set_high_level(TLKDRV_XTSD01G_POWER_PIN);	//high active
	gpio_set_up_down_res(TLKDRV_XTSD01G_POWER_PIN, GPIO_PIN_PULLUP_1M);

	sTlkDevXtsd01gPowerIsOn = false;
}




int tlkdrv_xtsd01g_readBlock(uint08 *pBuff, uint32 blkOffs)
{
	uint08 r1;
	uint08 buffer[512] = {0};
	if(sTlkDevXtsd01gType!=TLKDRV_XTSD01G_TYPE_V2HC) blkOffs <<= 9;

	r1 = tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_17, blkOffs, 0X01);
	if(r1==0){
		r1 = tlkdrv_xtsd01g_spiRecvData(buffer, 512);
		if(r1==0x00) tmemcpy(pBuff, buffer, 512);//in order to align(4) for dma
	}
	tlkdrv_xtsd01g_spiDisSelect();
	if(r1 != 0) return -TLK_EFAIL;	
	return TLK_ENONE;
}
int tlkdrv_xtsd01g_writeBlock(uint08 *pData, uint32 blkOffs)
{
	uint08 r1;
	uint08 buffer[512] = {0};
	if(sTlkDevXtsd01gType!=TLKDRV_XTSD01G_TYPE_V2HC) blkOffs *= 512;

	r1 = tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_24, blkOffs, 0X01);
	if(r1 == 0){
		tmemcpy(buffer, pData, 512);//in order to align(4) for dma
		r1 = tlkdrv_xtsd01g_spiSendData(buffer, 0xFE);
	}
	tlkdrv_xtsd01g_spiDisSelect();
	if(r1 != 0) return -TLK_EFAIL;	
	return TLK_ENONE;
}
int tlkdrv_xtsd01g_readBlocks(uint08 *pBuff, uint32 blkOffs, uint32 blkNumb)
{
	int ret;

	if(!sTlkDevXtsd01gIsReady) return -TLK_EFAIL;
	if(sTlkDevXtsd01gIsEnter != 0){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_readBlocks Reentry");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		return -TLK_EREPEAT;
	}

	sTlkDevXtsd01gIsEnter = 1;
		
	do{
		ret = tlkdrv_xtsd01g_readBlock(pBuff, blkOffs);
		pBuff += 512;
		blkOffs ++;
	}while(--blkNumb && ret == TLK_ENONE);
	
	sTlkDevXtsd01gIsEnter = 0;
	
	return ret;
}
int tlkdrv_xtsd01g_writeBlocks(uint08 *pData, uint32 blkOffs, uint32 blkNumb)
{
	int ret;
	
	if(!sTlkDevXtsd01gIsReady) return -TLK_EFAIL;
	
	if(sTlkDevXtsd01gIsEnter != 0){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlocks Reentry");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		return -TLK_EREPEAT;
	}
	
	sTlkDevXtsd01gIsEnter = 1;
	
	do{
		ret = tlkdrv_xtsd01g_writeBlock(pData, blkOffs);
		pData += 512;
		blkOffs ++;
	}while(--blkNumb && ret == TLK_ENONE);
	sTlkDevXtsd01gIsEnter = 0;
	
	return ret;
}




bool tlkdrv_xtsd01g_getCID(uint08 *cid_data)
{
    uint08 r1;
	uint08 buffer[32];

    r1 = tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_10,0,0x01);
    if(r1==0x00){
		r1 = tlkdrv_xtsd01g_spiRecvData(buffer,16);
		if(r1==0x00) tmemcpy(cid_data,buffer,16);//in order to align(4) for dma
    }
	tlkdrv_xtsd01g_spiDisSelect();
	if(r1) return false;
	else return true;
}

bool tlkdrv_xtsd01g_getCSD(uint08 *csd_data)
{
    uint08 r1;
	uint08 buffer[32];
    r1=tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_09,0,0x01);
    if(r1==0){
		r1 = tlkdrv_xtsd01g_spiRecvData(buffer,16);
		if(r1==0x00) tmemcpy(csd_data,buffer,16);//in order to align(4) for dma
    }
	tlkdrv_xtsd01g_spiDisSelect();
	if(r1) return false;
	else return true;
}


uint tlkdrv_xtsd01g_getBlockCount(void)
{
	if(sTlkDevXtsd01gIsEnter != 0)
	{
		#if(TLKDRV_XTSD01G_DEBUG_ENABLE)
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_getBlockCount Reentry");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some logic error is happen, the function is Reentry when sd nand flash is using in other function ");
		while(1) tlkapi_debug_handler(); /* open this for usb log printf*/
		#endif
		return 0x11;
	}
	
	sTlkDevXtsd01gIsEnter = 1;
	
	uint08 csd[16] = {0};

    uint32 Capacity;
    uint08 n;
	uint16 csize;

	memset(csd, 0, 16);

    if(tlkdrv_xtsd01g_getCSD(csd)!=0) return 0;

    if((csd[0]&0xC0)==0x40)
    {
		csize = csd[9] + ((uint16)csd[8] << 8) + 1;
		Capacity = (uint32)csize << 10;
    }else{
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16)csd[7] << 2) + ((uint16)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32)csize << (n - 9);
    }
    sTlkDevXtsd01gIsEnter = 0;
    return Capacity;
}


void tlkdrv_xtsd01g_eraseBlocks(uint32 blkOffs, uint32 blkNumb)
{
	uint08 resp;
	uint32 start_addr;
	uint32 end_addr;

	start_addr = blkOffs;
	end_addr = (blkOffs+blkNumb-1);
	if(sTlkDevXtsd01gType  != TLKDRV_XTSD01G_TYPE_V2HC){
		start_addr <<= 9;
		end_addr <<= 9;
	}
	tlkdrv_xtsd01g_spiCsLow();
	resp=tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_32, start_addr, 0x01);
	if(resp != 0){
		tlkdrv_xtsd01g_spiCsHigh();
		return;
	}
	
	resp=tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_33, end_addr, 0x01);
	if(resp != 0){
		tlkdrv_xtsd01g_spiCsHigh();
		return;
	}
		
	tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_38, 0x0, 0x01);	//resp = 0xC0
	for(uint08 i=0;i<10;i++){
		tlkdrv_xtsd01g_writeByte(0xFF);
	}
	
	uint16 cnt=0;
	uint16 total=15000; //wait 15000 * 200 us = 3 s for check ready.
	do{
		if(tlkdrv_xtsd01g_readByte() == 0xFF){
			tlkdrv_xtsd01g_spiCsHigh();
			return;
		}
		cnt++;
		delay_us(200);
	}while(cnt < total);
	tlkdrv_xtsd01g_spiCsHigh();
}



//static void tlkdrv_xtsd01g_switchLowSpeed(void)
//{
//	spi_master_init(TLKDRV_XTSD01G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDRV_XTSD01G_FOD_CLK) - 1, SPI_MODE0);
//}
static void tlkdrv_xtsd01g_switchHighSpeed(void)
{
	spi_master_init(TLKDRV_XTSD01G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDRV_XTSD01G_FPP_CLK) - 1, SPI_MODE0);
}

static void tlkdrv_xtsd01g_gpioConfig(void)
{
	gpio_output_en(TLKDRV_XTSD01G_SPI_CS_PIN);
	gpio_input_dis(TLKDRV_XTSD01G_SPI_CS_PIN);
	gpio_set_high_level(TLKDRV_XTSD01G_SPI_CS_PIN);
	gpio_function_en(TLKDRV_XTSD01G_SPI_CS_PIN);
	
	
	#if(TLKDRV_XTSD01G_SPI_MODULE == TLKDRV_XTSD01G_SPI_PSPI)
	pspi_set_pin(&sTlkDevXtsd04gPspiPinCfg);
	gpio_input_dis(GPIO_PC7);
	gpio_set_up_down_res(GPIO_PC6,GPIO_PIN_PULLUP_1M);
	spi_master_init(TLKDRV_XTSD01G_SPI_MODULE, sys_clk.pclk * 1000000 / (2 * TLKDRV_XTSD01G_FOD_CLK) - 1, SPI_MODE0);
	#else
	hspi_set_pin(&sTlkDevXtsd04gHspiPinCfg);
	spi_master_init(TLKDRV_XTSD01G_SPI_MODULE, sys_clk.hclk * 1000000 / (2 * TLKDRV_XTSD01G_FOD_CLK) - 1, SPI_MODE0);
	#endif
	
	spi_master_config(TLKDRV_XTSD01G_SPI_MODULE, SPI_NORMAL);
}

static void tlkdrv_xtsd01g_spiCsLow(void)
{
	gpio_set_low_level(TLKDRV_XTSD01G_SPI_CS_PIN);
}
static void tlkdrv_xtsd01g_spiCsHigh(void)
{
	gpio_set_high_level(TLKDRV_XTSD01G_SPI_CS_PIN);
}
static uint08 tlkdrv_xtsd01g_spiSelect(void)
{
	tlkdrv_xtsd01g_spiCsLow();
	if(tlkdrv_xtsd01g_waitReady()==0)return 0;
	tlkdrv_xtsd01g_spiDisSelect();
	return 1;
}
static void tlkdrv_xtsd01g_spiDisSelect(void)
{
	tlkdrv_xtsd01g_spiCsHigh();
	tlkdrv_xtsd01g_writeByte(0xff);
}

static uint08 tlkdrv_xtsd01g_spiGetResponse(uint08 Response)
{
	uint32 t=0;
	uint32 Count = 0x3F;
	do{
		if(tlkdrv_xtsd01g_readByte() == Response) return TLKDRV_XTSD01G_MSD_RESPONSE_NO_ERR;
		delay_us(100);
		t ++;
	}while(t < Count);
	#if(TLKDRV_XTSD01G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_spiGetResponse");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_handler(); /* open this for usb log printf*/
	}
	#endif
	return TLKDRV_XTSD01G_MSD_RESPONSE_FAILURE;
}
static uint08 tlkdrv_xtsd01g_spiRecvData(uint08 *pBuff, uint16 buffLen)
{
	if(tlkdrv_xtsd01g_spiGetResponse(0xFE)) return 1;
	
	tlkdrv_xtsd01g_readData(pBuff, buffLen);
    tlkdrv_xtsd01g_readByte();
    tlkdrv_xtsd01g_readByte();
	
    return 0;
}
static uint08 tlkdrv_xtsd01g_spiSendData(uint08 *pData, uint08 dataCmd)
{
	uint16 t;
	if(tlkdrv_xtsd01g_waitReady()) return 1;
	tlkdrv_xtsd01g_writeByte(dataCmd);
	if(dataCmd !=0XFD )
	{
		tlkdrv_xtsd01g_writeData(pData, 512);
		tlkdrv_xtsd01g_writeByte(0xFF);	//CRC
		tlkdrv_xtsd01g_writeByte(0xFF);	//CRC
		t=tlkdrv_xtsd01g_readByte();
		if((t&0x1F)!=0x05)return 2;

		uint16 cnt=0;
		uint16 total=2500;	//wait 2500x100 us = 250 ms for check ready.
		do
		{
			if(tlkdrv_xtsd01g_readByte() == 0xFF)
				return 0;
			cnt++;
			delay_us(100);
		}while(cnt<total);
	}
    return 1;
}


static uint08 tlkdrv_xtsd01g_sendCmd(uint08 cmd, uint32 arg, uint08 crc)
{
	uint08 r1;
	uint32 t=0;
	uint32 count=0XFF;
	
	tlkdrv_xtsd01g_spiDisSelect();
	if(tlkdrv_xtsd01g_spiSelect()) return 0XFF;

	tlkdrv_xtsd01g_writeByte(cmd | 0x40);
	tlkdrv_xtsd01g_writeByte(arg >> 24);
	tlkdrv_xtsd01g_writeByte(arg >> 16);
	tlkdrv_xtsd01g_writeByte(arg >> 8);
	tlkdrv_xtsd01g_writeByte(arg);
	tlkdrv_xtsd01g_writeByte(crc);
	if(cmd==TLKDRV_XTSD01G_CMD_12) tlkdrv_xtsd01g_writeByte(0xff);
		
	do{
		r1 = tlkdrv_xtsd01g_readByte();
		if(r1 != 0xff) break;
		t ++;
		delay_us(10);
	}while(t<count);
	#if(TLKDRV_XTSD01G_DEBUG_ENABLE)
	if(t == count){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_sendCmd");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_handler(); /* open this for usb log printf*/
	}
	#endif
	return r1;
}


static void tlkdrv_xtsd01g_spiRead(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen)
{
	spi_rx_dma_dis(spi_sel);
	spi_rx_fifo_clr(spi_sel);
	spi_set_transmode(spi_sel, SPI_MODE_READ_ONLY);
	spi_rx_cnt(spi_sel, buffLen);
	spi_set_cmd(spi_sel, 0x00);//when  cmd  disable that  will not sent cmd,just trigger spi send .
	spi_read(spi_sel, (unsigned char*)pBuff, buffLen);
	while(spi_is_busy(spi_sel));
}
static void tlkdrv_xtsd01g_spiReadDma(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen)
{
	unsigned char  rx_dma_chn;
	spi_rx_fifo_clr(spi_sel);
	spi_rx_dma_en(spi_sel);
	spi_rx_cnt(spi_sel, buffLen);
	spi_set_transmode(spi_sel, SPI_MODE_READ_ONLY);
	if(HSPI_MODULE == spi_sel) rx_dma_chn = s_hspi_rx_dma_chn;
	else rx_dma_chn = s_pspi_rx_dma_chn;
	spi_set_dma(rx_dma_chn, reg_spi_data_buf_adr(spi_sel), (unsigned int)convert_ram_addr_cpu2bus(pBuff), buffLen);
	spi_set_cmd(spi_sel, 0x00);//when  cmd  disable that  will not sent cmd,just trigger spi send .
}


static uint08 tlkdrv_xtsd01g_waitReady(void)
{
	uint32 t=0;
	uint32 Count=0x2FF;
	do{
		if(tlkdrv_xtsd01g_readByte()==0XFF)return 0;
		delay_us(100);
		t++;
	}while(t<Count);
	#if(TLKDRV_XTSD01G_DEBUG_ENABLE)
	if(t == Count){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "sd_nand_flash");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_waitReady");
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "maybe some error is happen,Please keep the site and contact telink");
		while(1) tlkapi_debug_handler(); /* open this for usb log printf*/
	}
	#endif
	return 1;
}



static void tlkdrv_xtsd01g_writeByte(uint08 TxData)
{
	tlkdrv_xtsd01g_writeData(&TxData, 1);
}
static uint08 tlkdrv_xtsd01g_readByte(void)
{
	uint08 temp[1];
	tlkdrv_xtsd01g_readData(temp,1);
	return temp[0];
}

static void tlkdrv_xtsd01g_readData(uint08 *pBuff, uint32 buffLen)
{
	while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
	#if TLKDRV_XTSD01G_DMA_ENABLE
	if(((buffLen % 4) == 0) && (buffLen>=4))
	{
		spi_tx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
		pspi_set_rx_dma_config(TLKDRV_XTSD01G_DMA_CHN);
		tlkdrv_xtsd01g_spiReadDma(TLKDRV_XTSD01G_SPI_MODULE, pBuff, buffLen);
	}
	else
	#endif
	{
		TLKDRV_XTSD01G_IRQ_DISABLE();
		tlkdrv_xtsd01g_spiRead(TLKDRV_XTSD01G_SPI_MODULE, pBuff, buffLen);
		TLKDRV_XTSD01G_IRQ_RESTORE();
	}
	while (spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
}
static void tlkdrv_xtsd01g_writeData(uint08 *pData, uint32 dataLen)
{
	pspi_set_pin_mux(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
	
	while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));

	#if TLKDRV_XTSD01G_DMA_ENABLE
	if(((dataLen % 4) == 0) && (dataLen>=4))
	{
		spi_rx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
		pspi_set_tx_dma_config(TLKDRV_XTSD01G_DMA_CHN);
		spi_master_write_dma(TLKDRV_XTSD01G_SPI_MODULE, pData, dataLen);
	}
	else
	#endif
	{
		spi_tx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
		TLKDRV_XTSD01G_IRQ_DISABLE();
		spi_master_write(TLKDRV_XTSD01G_SPI_MODULE, pData, dataLen);
		TLKDRV_XTSD01G_IRQ_RESTORE();
	}
	while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
	gpio_set_high_level(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
	gpio_function_en(sTlkDevXtsd04gPspiPinCfg.pspi_mosi_io0_pin);
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
#define TLKDRV_XTSD01G_DISK_TIME_HB(H,M,S)    (unsigned char)(((((H)<<11))|((M)<<5)|(S))>>8)
//Find the low byte of 16 bit time format
#define TLKDRV_XTSD01G_DISK_TIME_LB(H,M,S)    (unsigned char)((((H)<<11))|((M)<<5)|(S))
//Find the high byte of 16 bit date format
#define TLKDRV_XTSD01G_DISK_DATE_HB(Y,M,D)    (unsigned char)(((((Y)-1980)<<9)|((M)<<5)|(D))>>8)
//Find the low byte of 16 bit date format
#define TLKDRV_XTSD01G_DISK_DATE_LB(Y,M,D)    (unsigned char)((((Y)-1980)<<9)|((M)<<5)|(D))

static const char scTlkDevXtsd01gDiskReadMe[] = "This is DualMode SDK Fat16 demo\r\n";

static const uint08 scTlkDevXtsd01gDiskMBR[16] =
{
	0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff, (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff000000)>>24, 0x00, 0x00, 0x00, 0x00,
};
static const uint08 scTlkDevXtsd01gDiskDBR[64] =
{
	0xEB, 0x3C, 0x90, //Jump instruction, cannot be changed to 0, otherwise it prompts that it is not formatted (0~2)
	'M','S','D','O','S','5','.','0', //File system and version information "MSDOS5.0" (3~10)
	TLKDRV_XTSD01G_DISK_BLOCK_SIZE & 0xFF, (TLKDRV_XTSD01G_DISK_BLOCK_SIZE & 0xFF00) >> 8, //Number of bytes per sector, 512 bytes (11~12)
	TLKDRV_XTSD01G_DISK_CLUSTER_NUMB, //Number of sectors per cluster, 64 sectors (13)
	TLKDRV_XTSD01G_DISK_RSV_BLOCK_NUMB & 0xFF, (TLKDRV_XTSD01G_DISK_RSV_BLOCK_NUMB & 0xFF00) >> 8, //Number of reserved sectors, 8 (14~15)
	TLKDRV_XTSD01G_DISK_FAT_COPIES, //The number of FAT copies of this partition, which is 2 (16)
	(TLKDRV_XTSD01G_DISK_CLUSTER_SIZE >> 5) & 0xFF, (TLKDRV_XTSD01G_DISK_CLUSTER_SIZE >> 13) & 0xFF, //Number of root directory entries, 1024(00 04) entries (17~18). Unit:32Bytes
	0x00, 0x00, //The number of small sectors (<=32M), here is 0, which means to read the value from the large sector field (19~20)
	0xF8, //Media descriptor, 0xF8 means hard disk (21)
	(TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB & 0xFF), (TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB & 0xFF00)>>8, //Number of sectors per FAT, 64 (22~23)
	(TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF), (TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF00)>>8, //Number of sectors per track, 64 (24~25)
	(TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF), (TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF00)>>8, //The number of heads is 256M/32K-0x200 (16M) (reserve 512 32k sectors for program writing) (26~27)
	(TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff), (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff000000)>>24, //Number of hidden sectors There is no hidden sector here, it is 0 (28~31)
	(TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff), (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff00)>>8, (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff0000)>>16, (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff000000)>>24,  //Number of large sectors, the total number of sectors, 256k is 0x200 (32~35)
	0x80, //Disk drive parameters, 80 means hard disk (36)
	0x00, //Reserved (37)
	0x29, //Extended boot tag, 0x29 means the next three fields are available (38)
	0x34, 0x12, 0x00, 0x00, //Label serial number (39~42)
	'T', 'L', 'K', '-', 'F', 'S', '-', 'D', 'E', 'M', 'O', //Disk label volume (43~53)
	'F', 'A', 'T', '1', '6', 0x20, 0x20, 0x20, 0x00, 0x00, //File system type information, the string "FAT16" (54~63)
};
static const uint08 scTlkDevXtsd01gDiskFat16[6] =
{
	//------------- Block1: FAT16 Table -------------//
	0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF// // first 2 entries must be F8FF, third entry is cluster end of readme file };
};
static const uint08 scTlkDevXtsd01gDiskRoot[64] = //------------- Block2: Root Directory -------------//
{
	// first entry is volume label
	'T' , 'L' , 'K' , '-' , 'F' , 'S' , '-' , 'D' , 'E' , 'M' , 'O',
	0x08, //File property, indicating the label volume of the disk
	0x00, //Reserved
	0x00, //Create time millisecond timestamp

	//File creation time, 15:27:35
	TLKDRV_XTSD01G_DISK_TIME_LB(15,27,35), TLKDRV_XTSD01G_DISK_TIME_HB(15,27,35), //0x00, 0x00,
	//File creation date, August 19, 2008
	TLKDRV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDRV_XTSD01G_DISK_DATE_HB(2008,8,19), //0x00, 0x00,
	//Last visit date, August 20, 2008
	TLKDRV_XTSD01G_DISK_DATE_LB(2008,8,20), TLKDRV_XTSD01G_DISK_DATE_HB(2008,8,20), //0x00, 0x00,
	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:36:47
	TLKDRV_XTSD01G_DISK_TIME_LB(15,36,47), TLKDRV_XTSD01G_DISK_TIME_HB(15,36,47), //0x4F, 0x6D,
	//Last revision date, August 19, 2008
	TLKDRV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDRV_XTSD01G_DISK_DATE_HB(2008,8,19), //0x65, 0x43,

	0x00, 0x00,  //Start cluster low word
	0x00, 0x00, 0x00, 0x00, //file length
	
	// second entry is readme file
	'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 
	0x00, //File properties, representing read-write files
	0x00, //Reserved
	0x00, //0xC6, //0x00-Create time millisecond timestamp
	
	//File creation time, 15:48:26
	TLKDRV_XTSD01G_DISK_TIME_LB(15,48,26), TLKDRV_XTSD01G_DISK_TIME_HB(15,48,26),
	//File creation date, August 19, 2008
	TLKDRV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDRV_XTSD01G_DISK_DATE_HB(2008,8,19),
	//Last visit date
	TLKDRV_XTSD01G_DISK_DATE_LB(2008,8,20), TLKDRV_XTSD01G_DISK_DATE_HB(2008,8,20),

	0x00, 0x00, //High byte of starting cluster number, FAT12 / 16 must be 0

	//Last modified time, 15:50:33
	TLKDRV_XTSD01G_DISK_TIME_LB(15,50,33), TLKDRV_XTSD01G_DISK_TIME_HB(15,50,33), //0x88, 0x6D,
	//Last revision date, August 19, 2008
	TLKDRV_XTSD01G_DISK_DATE_LB(2008,8,19), TLKDRV_XTSD01G_DISK_DATE_HB(2008,8,19), //0x65, 0x43, 
	
	0x02, 0x00, //Start cluster low word, cluster 2.
	sizeof(scTlkDevXtsd01gDiskReadMe)-1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
};

int tlkdrv_xtsd01g_diskInit(void)
{
	int ret;
	uint32 blkCount;
	uint08 temp[512];

	if(!tlkdrv_xtsd01g_isOpen()) return -TLK_ENOREADY;
	
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskInit 01");
	ret = tlkdrv_xtsd01g_diskRead(temp, 0);
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskInit 02: %d", ret);
	if(ret != TLK_ENONE) return -TLK_EFAIL;
//	if((temp[510] != 0x55) || (temp[511] != 0xaa)) return -TLK_EFAIL;
	
	blkCount = (temp[461]<<24) + (temp[460]<<16) + (temp[459]<<8) + temp[458];
	if(temp[510] != 0x55 || temp[511] != 0xaa || blkCount != TLKDRV_XTSD01G_DISK_BLOCK_NUMB){
		ret = tlkdrv_xtsd01g_diskFormat();
	}
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskInit 03: %d", ret);
	return ret;
}
int tlkdrv_xtsd01g_diskRead(uint08 *pBuff, uint32 blkOffs)
{
	if(!tlkdrv_xtsd01g_isOpen()) return -TLK_ENOREADY;
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskRead");
	return tlkdrv_xtsd01g_readBlock(pBuff, blkOffs);
}
int tlkdrv_xtsd01g_diskWrite(uint08 *pData, uint32 blkOffs)
{
	if(!tlkdrv_xtsd01g_isOpen()) return -TLK_ENOREADY;
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskWrite");
	return tlkdrv_xtsd01g_writeBlock(pData, blkOffs);
}
int tlkdrv_xtsd01g_diskFormat(void)
{
	uint08 index;
	uint08 fatNum;
	uint32 offset;
	uint08 block[TLKDRV_XTSD01G_DISK_BLOCK_SIZE];

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskFormat");
	offset = TLKDRV_XTSD01G_DISK_DBR_OFFSET;

	//write DBR
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	tmemcpy(block, scTlkDevXtsd01gDiskDBR, sizeof(scTlkDevXtsd01gDiskDBR));
	block[510] = 0x55;
	block[511] = 0xaa;
	tlkdrv_xtsd01g_diskWrite(block, offset);
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDRV_XTSD01G_DISK_RSV_BLOCK_NUMB; index ++){
		tlkdrv_xtsd01g_diskWrite(block, offset+index);
	}
	offset += index;
	
	//write FAT
	for(fatNum = 0; fatNum < TLKDRV_XTSD01G_DISK_FAT_COPIES; fatNum++){
		tmemcpy(block, scTlkDevXtsd01gDiskFat16, sizeof(scTlkDevXtsd01gDiskFat16));
		tlkdrv_xtsd01g_diskWrite(block, offset);
		tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
		for(index = 1; index < TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB; index ++){
			tlkdrv_xtsd01g_diskWrite(block, offset+index);
		}
		offset += index;
	}
	
	//write root dir
	tmemcpy(block, scTlkDevXtsd01gDiskRoot, sizeof(scTlkDevXtsd01gDiskRoot));
	tlkdrv_xtsd01g_diskWrite(block, offset);
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	for(index = 1; index < TLKDRV_XTSD01G_DISK_CLUSTER_NUMB; index ++){
		tlkdrv_xtsd01g_diskWrite(block, offset+index);
	}
	offset += index;

	//write file
	tmemcpy(block, scTlkDevXtsd01gDiskReadMe, sizeof(scTlkDevXtsd01gDiskReadMe));
	tlkdrv_xtsd01g_diskWrite(block, offset);
	offset ++;

	//write MBR
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	tmemcpy(&block[446], scTlkDevXtsd01gDiskMBR, sizeof(scTlkDevXtsd01gDiskMBR));
	block[461] = (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff000000)>>24;
	block[460] = (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff0000)>>16;
	block[459] = (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff00)>>8;
	block[458] = (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff);
	block[510] = 0x55;
	block[511] = 0xaa;
	tlkdrv_xtsd01g_diskWrite(block, 0);
	
	return TLK_ENONE;
}



#endif
#endif //#if (MCU_CORE_TYPE == MCU_CORE_B91)


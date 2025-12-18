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
#if (MCU_CORE_TYPE == MCU_CORE_B92)
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdrv_store.h"
#if (TLKDRV_STORE_XTSD01G_ENABLE)
#include "tlkdrv_xtsd01g_b92.h"
#include "drivers.h"


#define TLKDRV_XTSD01G_DBG_FLAG     0//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_XTSD01G_DBG_SIGN     "[DRV]"

#define TLKDRV_XTSD01G_CS_SOFT_ENABLE    0

#if (TLKDRV_XTSD01G_CS_SOFT_ENABLE)
#define TLKDRV_XTSD01G_CS_LOW      gpio_set_low_level(TLKDRV_XTSD01G_CS_PIN)
#define TLKDRV_XTSD01G_CS_HIGH     gpio_set_high_level(TLKDRV_XTSD01G_CS_PIN)
#else
#define TLKDRV_XTSD01G_CS_LOW      //gpio_set_low_level(TLKDRV_XTSD01G_CS_PIN)
#define TLKDRV_XTSD01G_CS_HIGH     //gpio_set_high_level(TLKDRV_XTSD01G_CS_PIN)
#endif

extern void spi_set_dma(dma_chn_e spi_dma_chn, unsigned int src_addr, unsigned int dst_addr, unsigned int len);


static int tlkdrv_xtsd01g_config(void);
void tlkdrv_xtsd01g_spiRead(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen);
void tlkdrv_xtsd01g_spiWrite(spi_sel_e spi_sel, uint08 *pData, uint32 dataLen);
void tlkdrv_xtsd01g_spiReadDma(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen);
void tlkdrv_xtsd01g_spiWriteDma(spi_sel_e spi_sel, unsigned char *src_addr, unsigned int len);
void tlkdrv_xtsd01g_spiReadDmaPlus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *dst_addr, unsigned int data_len, spi_rd_tans_mode_e rd_mode);
void tlkdrv_xtsd01g_spiWriteDmaPlus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *data, unsigned int data_len, spi_wr_tans_mode_e wr_mode);


int tlkdrv_xtsd01g_readBlock(uint08 *pBuff, uint32 blkOffs);
int tlkdrv_xtsd01g_writeBlock(uint08 *pData, uint32 blkOffs);


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
static tlkdrv_xtsd01g_t sTlkDrvXtsd01gCtrl = {0};
uint08 __attribute__((aligned(4)))  sTlkXtsd01gTxBuffer[16] = { 0x00 };
uint08 __attribute__((aligned(4)))  sTlkXtsd01gRxBuffer[512] = { 0x00 };



static bool tlkdrv_xtsd01g_isOpen(void)
{
	return sTlkDrvXtsd01gCtrl.isOpen;
}
static int tlkdrv_xtsd01g_init(void)
{
	if(sTlkDrvXtsd01gCtrl.isInit) return -TLK_EREPEAT;

	sTlkDrvXtsd01gCtrl.isInit = true;
	sTlkDrvXtsd01gCtrl.isOpen = false;
	sTlkDrvXtsd01gCtrl.sdType = 0;

	gpio_function_en(TLKDRV_XTSD01G_POWER_PIN);
	gpio_set_up_down_res(TLKDRV_XTSD01G_POWER_PIN, GPIO_PIN_UP_DOWN_FLOAT);
	gpio_set_low_level(TLKDRV_XTSD01G_POWER_PIN); //Close
	gpio_output_dis(TLKDRV_XTSD01G_POWER_PIN);
	gpio_input_dis(TLKDRV_XTSD01G_POWER_PIN);
		
#if (TLKDRV_XTSD01G_CS_SOFT_ENABLE)
	gpio_function_en(TLKDRV_XTSD01G_CS_PIN);
	gpio_output_en(TLKDRV_XTSD01G_CS_PIN);
	gpio_set_low_level(TLKDRV_XTSD01G_CS_PIN);
	gpio_set_up_down_res(TLKDRV_XTSD01G_CS_PIN, GPIO_PIN_PULLDOWN_100K);
#endif
	
	return TLK_ENONE;
}


static int tlkdrv_xtsd01g_open(void)
{
	static gspi_pin_config_t pinCfg = {
		.spi_clk_pin		= TLKDRV_XTSD01G_CLK_PIN,
		.spi_csn_pin 		= TLKDRV_XTSD01G_CS_PIN,
		.spi_mosi_io0_pin   = TLKDRV_XTSD01G_DI_PIN, //TLKDRV_XTSD01G_DO_PIN,
		.spi_miso_io1_pin   = TLKDRV_XTSD01G_DO_PIN, //TLKDRV_XTSD01G_DI_PIN,//3line mode set 0
		.spi_io2_pin     	= 0,//set quad mode otherwise set 0
		.spi_io3_pin   		= 0,//set quad mode otherwise set 0
	};
	spi_wr_rd_config_t slaveCfg = {
		.spi_io_mode 		= SPI_SINGLE_MODE,
		.spi_dummy_cnt 		= 0,
		.spi_cmd_en 		= 0,
		.spi_addr_en 		= 0,
		.spi_addr_len 		= 0,//when hspi_addr_en = 0,invalid set.
		.spi_cmd_fmt_en 	= 0,//when hspi_cmd_en = 0,invalid set.
		.spi_addr_fmt_en 	= 0,//when hspi_addr_en=0,invalid set.
	};

	
	if(!sTlkDrvXtsd01gCtrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvXtsd01gCtrl.isOpen) return -TLK_EREPEAT;
	
	//PowerOn
	gpio_output_en(TLKDRV_XTSD01G_POWER_PIN);
	gpio_set_up_down_res(TLKDRV_XTSD01G_POWER_PIN, GPIO_PIN_PULLDOWN_100K);
	gpio_set_low_level(TLKDRV_XTSD01G_POWER_PIN); //Close

	//The initialization frequency is relatively low, usually 100k to 400k.
	spi_master_init(TLKDRV_XTSD01G_SPI_MODULE, sys_clk.pll_clk*1000000/(TLKDRV_XTSD01G_FOD_CLK), SPI_MODE0);
	gspi_set_pin(&pinCfg);
	spi_master_config_plus(TLKDRV_XTSD01G_SPI_MODULE, &slaveCfg);
	
	delay_ms(20); // delay_ms(50); delay 250ms for voltage setup.
	sTlkDrvXtsd01gCtrl.isOpen = true;
	
	if(tlkdrv_xtsd01g_config() != TLK_ENONE){
		tlkdrv_xtsd01g_close();
		return -TLK_EFAIL;
	}
	
	//Switch to high frequency clock for data access
	spi_master_init(TLKDRV_XTSD01G_SPI_MODULE, sys_clk.pll_clk*1000000/TLKDRV_XTSD01G_FPP_CLK, SPI_MODE0);
	gspi_set_pin(&pinCfg);
	spi_master_config_plus(TLKDRV_XTSD01G_SPI_MODULE, &slaveCfg);
	tmemset(sTlkXtsd01gTxBuffer, 0xFF, 10);
	tlkdrv_xtsd01g_writeBytes(sTlkXtsd01gTxBuffer, 10);
	delay_ms(20);
	
	return TLK_ENONE;
}
static int tlkdrv_xtsd01g_close(void)
{
	if(!sTlkDrvXtsd01gCtrl.isOpen) return TLK_ENONE;

	sTlkDrvXtsd01gCtrl.isOpen = false;

	//PowerOff
	gpio_set_up_down_res(TLKDRV_XTSD01G_POWER_PIN, GPIO_PIN_UP_DOWN_FLOAT);
	gpio_set_low_level(TLKDRV_XTSD01G_POWER_PIN); //Close
	gpio_output_dis(TLKDRV_XTSD01G_POWER_PIN);
	gpio_input_dis(TLKDRV_XTSD01G_POWER_PIN);

	spi_cmd_dis(TLKDRV_XTSD01G_SPI_MODULE);
	spi_cmd_fmt_dis(TLKDRV_XTSD01G_SPI_MODULE);
	spi_addr_dis(TLKDRV_XTSD01G_SPI_MODULE);
	spi_addr_fmt_dis(TLKDRV_XTSD01G_SPI_MODULE);
		
	gpio_function_en(TLKDRV_XTSD01G_CS_PIN);
	gpio_function_en(TLKDRV_XTSD01G_CLK_PIN);
	gpio_function_en(TLKDRV_XTSD01G_DO_PIN);
	gpio_function_en(TLKDRV_XTSD01G_DI_PIN);
	gpio_output_dis(TLKDRV_XTSD01G_CS_PIN);
	gpio_input_dis(TLKDRV_XTSD01G_CS_PIN);
	gpio_output_dis(TLKDRV_XTSD01G_CLK_PIN);
	gpio_input_dis(TLKDRV_XTSD01G_CLK_PIN);
	gpio_output_dis(TLKDRV_XTSD01G_DO_PIN);
	gpio_input_dis(TLKDRV_XTSD01G_DO_PIN);
	gpio_output_dis(TLKDRV_XTSD01G_DI_PIN);
	gpio_input_dis(TLKDRV_XTSD01G_DI_PIN);
	gpio_set_up_down_res(TLKDRV_XTSD01G_CS_PIN, GPIO_PIN_UP_DOWN_FLOAT);
	gpio_set_up_down_res(TLKDRV_XTSD01G_CLK_PIN, GPIO_PIN_UP_DOWN_FLOAT);
	gpio_set_up_down_res(TLKDRV_XTSD01G_DO_PIN, GPIO_PIN_UP_DOWN_FLOAT);
	gpio_set_up_down_res(TLKDRV_XTSD01G_DI_PIN, GPIO_PIN_UP_DOWN_FLOAT);

#if (TLKDRV_XTSD01G_CS_SOFT_ENABLE)
	gpio_function_en(TLKDRV_XTSD01G_CS_PIN);
	gpio_output_en(TLKDRV_XTSD01G_CS_PIN);
	gpio_set_low_level(TLKDRV_XTSD01G_CS_PIN);
	gpio_set_up_down_res(TLKDRV_XTSD01G_CS_PIN, GPIO_PIN_PULLDOWN_100K);
#endif
		
	return TLK_ENONE;
}
static int tlkdrv_xtsd01g_earse(uint32 addr, uint32 size)
{
	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;
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
	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_sdinit 02");
	return tlkdrv_xtsd01g_diskInit();
}
static int tlkdrv_xtsd01g_sdread(uint08 *pBuff, uint32 blkOffs, uint32 blkNumb)
{
	uint16 index;
	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;
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
	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;
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
	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;
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

	tlkdrv_xtsd01g_close();
//	gpio_set_up_down_res(TLKDRV_XTSD01G_POWER_PIN, GPIO_PIN_UP_DOWN_FLOAT);

//	gpio_shutdown(TLKDRV_XTSD01G_CS_PIN);
//	gpio_shutdown(TLKDRV_XTSD01G_CLK_PIN);
//	gpio_shutdown(TLKDRV_XTSD01G_DO_PIN);
//	gpio_shutdown(TLKDRV_XTSD01G_DI_PIN);
}



static int tlkdrv_xtsd01g_config(void)
{
    uint16 retry;

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_config 01");

	retry = 10;
	while(--retry){
		//The initialization delay is at least 74 clock until the SD card completes the internal operation.
		tmemset(sTlkXtsd01gTxBuffer, 0xFF, 10);
		tlkdrv_xtsd01g_writeBytes(sTlkXtsd01gTxBuffer, 8);
		delay_ms(2);
		//Send the command CMD0 to make the card device in idle state.
		if(tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_00, 0x00000000, 0x95, 1, 0)
			&& sTlkXtsd01gRxBuffer[0] == 0x01){
			break;
		}
		delay_ms(3);
	}
	
	if(retry == 0x00) return -TLK_EFAIL;

	delay_ms(2);
	//Check whether the voltage is supported. Here is 2.7~3.6v
	tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_08, 0x000001AA, 0x87, 5, 100);
	if(sTlkXtsd01gRxBuffer[0] == 0x01 && sTlkXtsd01gRxBuffer[1] == 0x00 && sTlkXtsd01gRxBuffer[2] == 0x00
		&& sTlkXtsd01gRxBuffer[3] == 0x01 && sTlkXtsd01gRxBuffer[4] == 0xAA){
		tlkapi_info(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "Here is an SD card version 2.0 or above");
	}else{
		tlkapi_info(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "Here are the SD cards below version 2.0");
	}
	delay_ms(2);

	//The circular transmission command CMD55+ACMD41 is used to detect whether the operating voltage of the card device meets the requirements of the host end.
	retry = 100;
	while(--retry){
		tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_55, 0, 1, 1, 500);
		delay_us(10);
		if(tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_41, 0x40000000, 0x01, 1, 500) && sTlkXtsd01gRxBuffer[0] == 0x00){
			break;
		}
		delay_ms(1);
	}
	if(retry == 0) return -TLK_EFAIL;

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "SD power is ready");

	if(!tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_58, 0, 0X00, 5, 200)){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "The SD card model cannot be identified");
		return -TLK_EFAIL;
	}

	if((sTlkXtsd01gRxBuffer[1] & 0x40) == 0x40){
		sTlkDrvXtsd01gCtrl.sdType = TLKDRV_XTSD01G_TYPE_V2HC;
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "The card is a High Capacity SD Memory Card.");
	}else{
		sTlkDrvXtsd01gCtrl.sdType = TLKDRV_XTSD01G_TYPE_V2;
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "The card is a Standard Capacity SD Memory Card.");
	}
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "SD initialization is ready");
		
	return TLK_ENONE;
}





bool tlkdrv_xtsd01g_waitReady(uint32 waitTimeUs, uint08 value, uint08 chkMsk)
{
	bool isReady = false;
	uint32 timer = clock_time();
	TLKDRV_XTSD01G_CS_LOW;
	spi_set_master_rx_dma_config(TLKDRV_XTSD01G_SPI_MODULE, TLKDRV_XTSD01G_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_XTSD01G_DMA_CHN, DMA_BURST_1_WORD);
	gspi_reset();
	while(waitTimeUs == 0 || !clock_time_exceed(timer, waitTimeUs)){
		tlkdrv_xtsd01g_spiReadDma(TLKDRV_XTSD01G_SPI_MODULE, sTlkXtsd01gRxBuffer, 1);
		while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
		gspi_reset();
		if((sTlkXtsd01gRxBuffer[0] & chkMsk) == value){ isReady = true; break; }
		delay_us(10);
	}
	spi_rx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
	TLKDRV_XTSD01G_CS_HIGH;
	return isReady;
}
void tlkdrv_xtsd01g_readBytes(uint08 bytesNumb)
{
	if(bytesNumb > 16) bytesNumb = 16;
	spi_set_master_rx_dma_config(TLKDRV_XTSD01G_SPI_MODULE, TLKDRV_XTSD01G_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_XTSD01G_DMA_CHN, DMA_BURST_1_WORD);
	tlkdrv_xtsd01g_spiReadDma(TLKDRV_XTSD01G_SPI_MODULE, sTlkXtsd01gRxBuffer, bytesNumb);
	while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
	gspi_reset();
	spi_rx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
}
void tlkdrv_xtsd01g_writeBytes(uint08 *pData, uint08 dataLen)
{
	spi_set_tx_dma_config(TLKDRV_XTSD01G_SPI_MODULE, TLKDRV_XTSD01G_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_XTSD01G_DMA_CHN, DMA_BURST_1_WORD);
	tlkdrv_xtsd01g_spiWriteDma(TLKDRV_XTSD01G_SPI_MODULE, pData, dataLen);
	while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
	gspi_reset();
	spi_tx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
}
bool tlkdrv_xtsd01g_readData(uint08 *pBuff, uint32 buffLen)
{
	spi_set_master_rx_dma_config(TLKDRV_XTSD01G_SPI_MODULE, TLKDRV_XTSD01G_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_XTSD01G_DMA_CHN, DMA_BURST_1_WORD);
	if(((uint32)pBuff & 0x03) == 0){
		tlkdrv_xtsd01g_spiReadDma(TLKDRV_XTSD01G_SPI_MODULE, pBuff, buffLen);
		while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
		gspi_reset();
	}else{
		uint32 offset = 0;
		uint32 length = 0;
		while(offset < buffLen){
			if(offset+512 <= buffLen) length = 512;
			else length = buffLen-offset;
			tlkdrv_xtsd01g_spiReadDma(TLKDRV_XTSD01G_SPI_MODULE, sTlkXtsd01gRxBuffer, length);
			while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
			gspi_reset();
			tmemcpy(pBuff+offset, sTlkXtsd01gRxBuffer, length);
			offset += length;
		}
	}
	spi_rx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
	return true;
}
bool tlkdrv_xtsd01g_writeData(uint08 *pData, uint32 dataLen)
{
	spi_set_tx_dma_config(TLKDRV_XTSD01G_SPI_MODULE, TLKDRV_XTSD01G_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_XTSD01G_DMA_CHN, DMA_BURST_1_WORD);
	if(((uint32)pData & 0x03) == 0){
		tlkdrv_xtsd01g_spiWriteDma(TLKDRV_XTSD01G_SPI_MODULE, pData, dataLen);
		while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
		gspi_reset();
	}else{
		uint32 offset = 0;
		uint32 length = 0;
		while(offset < dataLen){
			if(offset+512 <= dataLen) length = 512;
			else length = dataLen-offset;
			tmemcpy(sTlkXtsd01gRxBuffer, pData+offset, length);
			tlkdrv_xtsd01g_spiWriteDma(TLKDRV_XTSD01G_SPI_MODULE, sTlkXtsd01gRxBuffer, length);
			while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
			gspi_reset();
			offset += length;
		}
	}
	spi_tx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
	return true;
}
bool tlkdrv_xtsd01g_sendCmd(uint08 cmd, uint32 arg, uint08 crc, uint08 rspBytes, uint32 waitUs)
{
	uint recvCnt;
	uint32 timer;

	recvCnt = 0;
	if(rspBytes == 0 || rspBytes > 32) return false; 

	if(waitUs != 0 && !tlkdrv_xtsd01g_waitReady(waitUs, 0xFF, 0xFF)){
		tlkapi_error(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_sendCmd: no ready");
		return false;
	}

	sTlkXtsd01gTxBuffer[0] = cmd | 0x40;
	sTlkXtsd01gTxBuffer[1] = (arg >> 24) & 0xFF;
	sTlkXtsd01gTxBuffer[2] = (arg >> 16) & 0xFF;
	sTlkXtsd01gTxBuffer[3] = (arg >> 8) & 0xFF;
	sTlkXtsd01gTxBuffer[4] = arg & 0xFF;
	sTlkXtsd01gTxBuffer[5] = crc;
	TLKDRV_XTSD01G_CS_LOW;
	spi_set_tx_dma_config(TLKDRV_XTSD01G_SPI_MODULE, TLKDRV_XTSD01G_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_XTSD01G_DMA_CHN, DMA_BURST_1_WORD);
	tlkdrv_xtsd01g_spiWriteDma(TLKDRV_XTSD01G_SPI_MODULE, sTlkXtsd01gTxBuffer, 6);
	while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
	gspi_reset();
	spi_tx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
	
	timer = clock_time();
	spi_set_master_rx_dma_config(TLKDRV_XTSD01G_SPI_MODULE, TLKDRV_XTSD01G_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_XTSD01G_DMA_CHN, DMA_BURST_1_WORD);
	while(recvCnt < rspBytes && !clock_time_exceed(timer, 20000)){
		tlkdrv_xtsd01g_spiReadDma(TLKDRV_XTSD01G_SPI_MODULE, sTlkXtsd01gTxBuffer, 1);
		while(spi_is_busy(TLKDRV_XTSD01G_SPI_MODULE));
		gspi_reset();
		if(recvCnt != 0 || sTlkXtsd01gTxBuffer[0] != 0xFF){
			sTlkXtsd01gRxBuffer[recvCnt++] = sTlkXtsd01gTxBuffer[0];
		}else{
			delay_us(10);
		}
	}
	spi_rx_dma_dis(TLKDRV_XTSD01G_SPI_MODULE);
	TLKDRV_XTSD01G_CS_HIGH;
	if(recvCnt == rspBytes) return true;
	tlkapi_error(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_sendCmd: no response");
	return false;
}

int tlkdrv_xtsd01g_readBlock(uint08 *pBuff, uint32 blkOffs)
{
	if(pBuff == nullptr) return -TLK_EPARAM;
	if(sTlkDrvXtsd01gCtrl.sdType != TLKDRV_XTSD01G_TYPE_V2HC) blkOffs <<= 9;
		
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_readBlock 01: %d 0x%x", blkOffs, blkOffs << 9);
	//First send CMD17
	if(!tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_17, blkOffs, 0X01, 1, 200)){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_readBlock fail: send CMD failure");
		return -TLK_EFAIL;
	}
	//After sending CMD17, 0x00 is received, indicating that the card is ready
	if(sTlkXtsd01gRxBuffer[0] != 0x00 && !tlkdrv_xtsd01g_waitReady(1000, 0x00, 0xFF)){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_readBlock fail: cmd no ready");
		return -TLK_EFAIL;
	}
	//Read continuously until the beginning byte 0xFE is read
	if(!tlkdrv_xtsd01g_waitReady(5000, 0xFE, 0xFF)){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_readBlock fail: no 0xFE incoming");
		return -TLK_EFAIL;
	}
	
	TLKDRV_XTSD01G_CS_LOW;
	//Read 512 bytes
	tlkdrv_xtsd01g_readData(pBuff, 512);
	//Read two Byte CRCS
	tlkdrv_xtsd01g_readData(sTlkXtsd01gRxBuffer, 2); //two bytes for CRC
	TLKDRV_XTSD01G_CS_HIGH;
	
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_readBlock succ: %x %x %x %x %x %x %x %x",
		pBuff[0], pBuff[1], pBuff[2], pBuff[3], pBuff[508], pBuff[509], pBuff[510], pBuff[511]);
		
	return TLK_ENONE;
}
int tlkdrv_xtsd01g_readBlocks(uint08 *pBuff, uint32 blkOffs, uint32 blockNumb)
{
	uint08 *pTemp;

	if(pBuff == nullptr || blockNumb == 0) return -TLK_EPARAM;
	if(sTlkDrvXtsd01gCtrl.sdType != TLKDRV_XTSD01G_TYPE_V2HC) blkOffs <<= 9;

	//First send CMD18
	if(!tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_18, blkOffs, 0x01, 1, 200)){
		return -TLK_EFAIL;
	}
	//After sending CMD18, 0x00 is received, indicating that the card is ready
	if(sTlkXtsd01gRxBuffer[0] != 0x00 && !tlkdrv_xtsd01g_waitReady(3000, 0x00, 0xFF)){
		return -TLK_EFAIL;
	}

	pTemp = pBuff;
	while(blockNumb != 0){
		//Read continuously until the beginning byte 0xFE is read
		if(!tlkdrv_xtsd01g_waitReady(5000, 0xFE, 0xFF)) break;
		TLKDRV_XTSD01G_CS_LOW;
		//Read 512 bytes
		tlkdrv_xtsd01g_readData(pTemp, 512);
		//Read two Byte CRCS
		tlkdrv_xtsd01g_readData(sTlkXtsd01gRxBuffer, 2); //two bytes for CRC
		TLKDRV_XTSD01G_CS_HIGH;
		blockNumb --;
		pTemp += 512;
	}
	if(blockNumb != 0) return -TLK_EFAIL;
	
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_readBlocks 03: %x %x %x %x",
		pBuff[0], pBuff[1], pBuff[512], pBuff[513]);
	
	return TLK_ENONE;
}

int tlkdrv_xtsd01g_writeBlock(uint08 *pData, uint32 blkOffs)
{
	if(pData == nullptr) return -TLK_EPARAM;

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlock: %d 0x%x", blkOffs, blkOffs << 9);
	if(sTlkDrvXtsd01gCtrl.sdType != TLKDRV_XTSD01G_TYPE_V2HC) blkOffs <<= 9;

	//First send CMD24
	if(!tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_24, blkOffs, 0x01, 1, 200)){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlock fail: send CMD failure");
		return -TLK_EFAIL;
	}
	//After sending CMD24, 0x00 is received, indicating that the card is ready
	if(sTlkXtsd01gRxBuffer[0] != 0x00 && !tlkdrv_xtsd01g_waitReady(3000, 0x00, 0xFF)){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlock fail: CMD not ready");
		return -TLK_EFAIL;
	}

	TLKDRV_XTSD01G_CS_LOW;
	//Send a number of clocks
	sTlkXtsd01gRxBuffer[0] = 0xFF;
	sTlkXtsd01gRxBuffer[1] = 0xFF;
	sTlkXtsd01gRxBuffer[2] = 0xFF;
	tlkdrv_xtsd01g_writeData(sTlkXtsd01gRxBuffer, 3);
	//Send write single block start byte 0xFE
	sTlkXtsd01gRxBuffer[0] = 0xFE;
	tlkdrv_xtsd01g_writeData(sTlkXtsd01gRxBuffer, 1);
	//Write 512 bytes of data
	tlkdrv_xtsd01g_writeData(pData, 512);
	//Send 2-byte CRC (either 0xff)
	sTlkXtsd01gRxBuffer[0] = 0xFF;
	sTlkXtsd01gRxBuffer[1] = 0xFF;
	tlkdrv_xtsd01g_writeData(sTlkXtsd01gRxBuffer, 2);
	TLKDRV_XTSD01G_CS_HIGH;
	//Read data continuously until XXX00101 is read
	if(!tlkdrv_xtsd01g_waitReady(5000, 0x05, 0x1F)){
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlock fail: wait 0x05");
		return -TLK_EFAIL;
	}
	//Read on for busy detection (0x00 indicates that the SD card is busy), and read 0xff indicates that the write operation is complete
	if(!tlkdrv_xtsd01g_waitReady(100000, 0xFF, 0xFF)){ //The maximum waiting time is found to be 50ms
		tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlock fail: wait 0xFF");
		return -TLK_EFAIL;
	}
	
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlock: success");
	
	return TLK_ENONE;
}
int tlkdrv_xtsd01g_writeBlocks(uint08 *pData, uint32 blkOffs, uint32 blockNumb)
{
	uint08 *pTemp;
	
	if(pData == nullptr || blockNumb == 0) return -TLK_EPARAM;
	if(sTlkDrvXtsd01gCtrl.sdType != TLKDRV_XTSD01G_TYPE_V2HC) blkOffs <<= 9;
	//First send CMD25
	if(!tlkdrv_xtsd01g_sendCmd(TLKDRV_XTSD01G_CMD_25, blkOffs, 0X01, 1, 200)){
		return -TLK_EFAIL;
	}
	//After sending CMD24, 0x00 is received, indicating that the card is ready
	if(sTlkXtsd01gRxBuffer[0] != 0x00 && !tlkdrv_xtsd01g_waitReady(3000, 0x00, 0xFF)){
		return -TLK_EFAIL;
	}

	TLKDRV_XTSD01G_CS_LOW;		
	pTemp = pData;
	while(blockNumb != 0){
		//Send a number of clocks
		tlkdrv_xtsd01g_readData(sTlkXtsd01gRxBuffer, 3);
		//Send write multiple block start byte 0xFC
		sTlkXtsd01gRxBuffer[0] = 0xFC;
		tlkdrv_xtsd01g_writeData(sTlkXtsd01gRxBuffer, 1);
		//Write 512 bytes of data
		tlkdrv_xtsd01g_writeData(pData, 512);
		//Send 2-byte CRC (either 0xff)
		sTlkXtsd01gRxBuffer[0] = 0xFF;
		sTlkXtsd01gRxBuffer[1] = 0xFF;
		tlkdrv_xtsd01g_writeData(sTlkXtsd01gRxBuffer, 2);
		//Read data continuously until XXX00101 is read
		if(!tlkdrv_xtsd01g_waitReady(3000, 0x05, 0x1F)) break;
		//Read on for busy detection (0x00 indicates that the SD card is busy), and read 0xff indicates that the write operation is complete
		if(!tlkdrv_xtsd01g_waitReady(100000, 0xFF, 0xFF)) break; //The maximum waiting time is found to be 50ms
		pTemp += 512;
		blockNumb --;
	}
	//Send Write multiple block stop bytes 0xFD to stop the write operation
	sTlkXtsd01gRxBuffer[0] = 0xFD;
	tlkdrv_xtsd01g_writeData(sTlkXtsd01gRxBuffer, 1);
	TLKDRV_XTSD01G_CS_HIGH;
	if(blockNumb != 0) return -TLK_EFAIL;
	//Perform busy detection until 0xFF is read
	if(!tlkdrv_xtsd01g_waitReady(5000, 0xFF, 0xFF)) return -TLK_EFAIL;

//	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_writeBlocks 03: success");
	
	return TLK_ENONE;
}

void tlkdrv_xtsd01g_spiRead(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen)
{
	spi_rx_dma_dis(spi_sel);
	spi_rx_fifo_clr(spi_sel);
	spi_set_transmode(spi_sel, SPI_MODE_READ_ONLY);
	spi_rx_cnt(spi_sel, buffLen);
	spi_set_cmd(spi_sel, 0x00);//when  cmd  disable that  will not sent cmd,just trigger spi send .
	spi_read(spi_sel, (unsigned char*)pBuff, buffLen);
	while(spi_is_busy(spi_sel));
	gspi_reset();
}
void tlkdrv_xtsd01g_spiWrite(spi_sel_e spi_sel, uint08 *pData, uint32 dataLen)
{
	spi_tx_dma_dis(spi_sel);
	spi_tx_fifo_clr(spi_sel);
	spi_tx_cnt(spi_sel,dataLen);
	spi_set_transmode(spi_sel,SPI_MODE_WRITE_ONLY);
	spi_set_cmd(spi_sel,0x00);//when  cmd  disable that  will not sent cmd,just trigger spi send .
	spi_write(spi_sel,(unsigned char *)pData, dataLen);
	while(spi_is_busy(spi_sel));
	gspi_reset();
}

void tlkdrv_xtsd01g_spiReadDma(spi_sel_e spi_sel, uint08 *pBuff, uint32 buffLen)
{
	spi_rx_fifo_clr(spi_sel);
	spi_rx_dma_en(spi_sel);
	spi_rx_cnt(spi_sel, buffLen);
	spi_set_transmode(spi_sel, SPI_MODE_READ_ONLY);
	spi_set_dma(TLKDRV_XTSD01G_DMA_CHN, reg_spi_data_buf_adr(spi_sel), (unsigned int)(pBuff), buffLen);
	spi_set_cmd(spi_sel, 0x00);//when  cmd  disable that  will not sent cmd,just trigger spi send .
}
void tlkdrv_xtsd01g_spiWriteDma(spi_sel_e spi_sel, unsigned char *src_addr, unsigned int len)
{
	spi_tx_fifo_clr(spi_sel);
	spi_tx_dma_en(spi_sel);
	spi_tx_cnt(spi_sel, len);
	spi_set_transmode(spi_sel, SPI_MODE_WRITE_ONLY);
	spi_set_dma(TLKDRV_XTSD01G_DMA_CHN, (unsigned int)src_addr, reg_spi_data_buf_adr(spi_sel), len);
	spi_set_cmd(spi_sel, 0x00);
}
void tlkdrv_xtsd01g_spiReadDmaPlus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *dst_addr, unsigned int data_len, spi_rd_tans_mode_e rd_mode)
{
	spi_rx_fifo_clr(spi_sel);
	spi_rx_dma_en(spi_sel);
	spi_set_transmode(spi_sel, rd_mode);
	spi_rx_cnt(spi_sel, data_len);
	spi_set_address(spi_sel,addr);
	spi_set_dma(TLKDRV_XTSD01G_DMA_CHN, reg_spi_data_buf_adr(spi_sel), (unsigned int)(dst_addr), data_len);
	spi_set_cmd(spi_sel, cmd);
}
void tlkdrv_xtsd01g_spiWriteDmaPlus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char *data, unsigned int data_len, spi_wr_tans_mode_e wr_mode)
{
	spi_tx_fifo_clr(spi_sel);
	spi_tx_dma_en(spi_sel);
	spi_tx_cnt(spi_sel, data_len);
	spi_set_transmode(spi_sel, wr_mode);
	spi_set_address(spi_sel,addr);
	spi_set_dma(TLKDRV_XTSD01G_DMA_CHN, (unsigned int)data, reg_spi_data_buf_adr(spi_sel), data_len);
	spi_set_cmd(spi_sel, cmd);
}




#define TLKFS_FORMATE_FAT16     1
#define TLKFS_FORMATE_FAT32     2
#define TLKFS_FORMATE           TLKFS_FORMATE_FAT16


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

static const uint08 scTlkDevXtsd01gDiskMBREntry[16] =
{
	0x00, //BootIndicator: A sign indicating whether it can be started
	0x00, //StartHead: Number of the start head of the partition
	0x00, //StartSector: The upper 2 digits are the start cylinder number and the lower 6 digits are the start sector number
	0x00, //StartCylinder: The lower 8 digits of the starting cylinder number.
	0x0E, //PartitionType: Partition type
	0x00, //EndHead: The partition terminating head number
	0x00, //EndSector: The upper 2 digits are the end cylinder number and the lower 6 digits are the end sector number
	0x00, //EndCylinder: The lower 8 digits of the terminal cylinder number
	(TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff), //StartLBA: Start sector number
	(TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff00)>>8, 
	(TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff0000)>>16, 
	(TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff000000)>>24,
	(TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff), //TotalSector: Partition size (Total number of sectors
	(TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff00)>>8, 
	(TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff0000)>>16, 
	(TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff000000)>>24,
};
static const uint08 scTlkDevXtsd01gDiskFat32DBR[96] =
{
	0xEB, 0x58, 0x90, //Jump instruction, cannot be changed to 0, otherwise it prompts that it is not formatted (0~2)
	'M','S','D','O','S','5','.','0', //File system and version information "MSDOS5.0" (3~10)
	TLKDRV_XTSD01G_DISK_BLOCK_SIZE & 0xFF, (TLKDRV_XTSD01G_DISK_BLOCK_SIZE & 0xFF00) >> 8, //Number of bytes per sector, 512 bytes (0x0B~0x0C)
	TLKDRV_XTSD01G_DISK_CLUSTER_NUMB, //Number of sectors per cluster, 64 sectors (0x0D)
	TLKDRV_XTSD01G_DISK_RSV_BLOCK_NUMB & 0xFF, (TLKDRV_XTSD01G_DISK_RSV_BLOCK_NUMB & 0xFF00) >> 8, //Number of reserved sectors, 8 (0x0E~0x0F)
	TLKDRV_XTSD01G_DISK_FAT_COPIES, //The number of FAT copies of this partition, which is 2 (0x10)
	0x00, 0x00, //Number of root directory entries. (0x11~0x12)
	0x00, 0x00, //The number of small sectors (small volume, <=32M), here is 0, which means to read the value from the large sector field (0x13~0x14)
	0xF8, //Media descriptor, 0xF8 means hard disk (0x15)
	0x00, 0x00, //Number of sectors per FAT, (small volume) (0x16~0x17)
	(TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF), (TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF00)>>8, //Number of sectors per track, 64 (0x18~0x19)
	(TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF), (TLKDRV_XTSD01G_DISK_CLUSTER_NUMB & 0xFF00)>>8, //The number of heads is 256M/32K-0x200 (16M) (reserve 512 32k sectors for program writing) (0x1A~0x1B)
	(TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff), (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff00)>>8, (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff0000)>>16, (TLKDRV_XTSD01G_DISK_DBR_OFFSET & 0xff000000)>>24, //Number of hidden sectors There is no hidden sector here, it is 0 (0x1C~0x1F)
	(TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff), (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff00)>>8, (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff0000)>>16, (TLKDRV_XTSD01G_DISK_BLOCK_NUMB & 0xff000000)>>24,  //Number of large sectors, the total number of sectors, 256k is 0x200 (0x20~0x23)
	(TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB & 0xFF), (TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB & 0xFF00)>>8, (TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB & 0xFF0000)>>16, (TLKDRV_XTSD01G_DISK_FAT_BLOCK_NUMB & 0xFF000000)>>24,  //Sectors per FAT (0x24~0x27)
	0x00, //Extended flags (0x28)
	0x00, //FAT mirroring disabled? (0x29)
	0x00, 0x00, //Version (usually 0) (0x2A~0x2B)
	0x02, 0x00, 0x00, 0x00, //Root dir 1st cluster (0x2C~0x2F)
	0x01, 0x00, //FSInfo sector (0x30~0x31)
	0x06, 0x00, //Backup boot sector (0x32~0x33)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Reserved (0x34~0x3F)
	0x80, //Disk drive parameters, 80 means hard disk (0x40)
	0x00, //Reserved (0x41)
	0x29, //Extended boot tag, 0x29 means the next three fields are available (0x42)
	0x34, 0x12, 0x00, 0x00, //Label serial number (0x43~0x46)
	'T', 'L', 'K', '-', 'F', 'S', '-', 'D', 'E', 'M', 'O', //Disk label volume (0x47~0x51)
	'F', 'A', 'T', '3', '2', 0x20, 0x20, 0x20, //File system type information, the string "FAT32" (0x52~0x59)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //supplement content
};
static const uint08 scTlkDevXtsd01gDiskFat16DBR[64] =
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
static const uint08 scTlkDevXtsd01gDiskFat32[12] =
{
	//------------- Block1: FAT32 Table -------------//
	0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF // // first 4 entries must be F8FFFFFF, third entry is cluster end of readme file };
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

	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskInit 01");
	ret = tlkdrv_xtsd01g_diskRead(temp, 0);
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskInit 02: %d", ret);
	if(ret != TLK_ENONE) return -TLK_EFAIL;
//	if((temp[510] != 0x55) || (temp[511] != 0xaa)) return -TLK_EFAIL;
	
	blkCount = (temp[0x1CD]<<24) + (temp[0x1CC]<<16) + (temp[0x1CB]<<8) + temp[0x1CA];
	if(temp[0x1FE] != 0x55 || temp[0x1FF] != 0xaa || blkCount != TLKDRV_XTSD01G_DISK_BLOCK_NUMB){
		ret = tlkdrv_xtsd01g_diskFormat();
	}else{
		ret = tlkdrv_xtsd01g_diskRead(temp, TLKDRV_XTSD01G_DISK_DBR_OFFSET);
		if(ret != TLK_ENONE) return TLK_ENONE;
		#if (TLKFS_FORMATE == TLKFS_FORMATE_FAT32)
		if(tmemcmp(temp + 82, "FAT32   ", 8) != 0){
			ret = tlkdev_p25q32h_diskFormat();
		}
		#else
		if(tmemcmp(temp + 54, "FAT16   ", 8) != 0){
			ret = tlkdrv_xtsd01g_diskFormat();
		}
		#endif
	}
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskInit 03: %d", ret);
	return ret;
}
int tlkdrv_xtsd01g_diskRead(uint08 *pBuff, uint32 blkOffs)
{
	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskRead");
	return tlkdrv_xtsd01g_readBlock(pBuff, blkOffs);
}
int tlkdrv_xtsd01g_diskWrite(uint08 *pData, uint32 blkOffs)
{
	if(!sTlkDrvXtsd01gCtrl.isOpen) return -TLK_ENOREADY;
	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskWrite");
	return tlkdrv_xtsd01g_writeBlock(pData, blkOffs);
}
int tlkdrv_xtsd01g_diskFormat(void)
{
	#if (TLKFS_FORMATE == TLKFS_FORMATE_FAT32)
	return tlkdrv_xtsd01g_diskFormatFat32();
	#else
	return tlkdrv_xtsd01g_diskFormatFat16();
	#endif
}
int tlkdrv_xtsd01g_diskFormatFat16(void)
{
	uint08 index;
	uint08 fatNum;
	uint32 offset;
	uint08 block[TLKDRV_XTSD01G_DISK_BLOCK_SIZE];

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskFormat");
	offset = TLKDRV_XTSD01G_DISK_DBR_OFFSET;

	//write DBR
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	tmemcpy(block, scTlkDevXtsd01gDiskFat16DBR, sizeof(scTlkDevXtsd01gDiskFat16DBR));
	block[510] = 0x55; //FSINFO ending signature (0x55 0xAA)
	block[511] = 0xAA;
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
	tmemcpy(&block[0x1BE], scTlkDevXtsd01gDiskMBREntry, sizeof(scTlkDevXtsd01gDiskMBREntry));
	block[0x1FE] = 0x55; //FSINFO ending signature (0x55 0xAA)
	block[0x1FF] = 0xAA;
	tlkdrv_xtsd01g_diskWrite(block, 0);
	
	return TLK_ENONE;
}

int tlkdrv_xtsd01g_diskFormatFat32(void)
{
	uint08 index;
	uint08 fatNum;
	uint32 offset;
	uint08 block[TLKDRV_XTSD01G_DISK_BLOCK_SIZE];

	tlkapi_trace(TLKDRV_XTSD01G_DBG_FLAG, TLKDRV_XTSD01G_DBG_SIGN, "tlkdrv_xtsd01g_diskFormat");
	offset = TLKDRV_XTSD01G_DISK_DBR_OFFSET;

	//write DBR
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	tmemcpy(block, scTlkDevXtsd01gDiskFat32DBR, sizeof(scTlkDevXtsd01gDiskFat32DBR));
	block[0x1FE] = 0x55;
	block[0x1FF] = 0xaa;
	tlkdrv_xtsd01g_diskWrite(block, offset);
	//Write backup DBR
	tlkdrv_xtsd01g_diskWrite(block, offset+6);

	//Write FS info
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	block[0x000] = 0x52; //Extended boot signature (RRaA)
	block[0x001] = 0x52;
	block[0x002] = 0x61;
	block[0x003] = 0x41;
	block[0x1E4] = 0x72; //FS info signature (rrAa)
	block[0x1E5] = 0x72;
	block[0x1E6] = 0x41;
	block[0x1E7] = 0x61;
	block[0x1E8] = ((TLKDRV_XTSD01G_DISK_CLUSTER_COUNT - 3) & 0xFF); //Free cluster count, 2(for FAT1 and FAT2) + 1(Readme.txt)
	block[0x1E9] = ((TLKDRV_XTSD01G_DISK_CLUSTER_COUNT - 3) & 0xFF00) >> 8;
	block[0x1EA] = ((TLKDRV_XTSD01G_DISK_CLUSTER_COUNT - 3) & 0xFF0000) >> 16;
	block[0x1EB] = ((TLKDRV_XTSD01G_DISK_CLUSTER_COUNT - 3) & 0xFF000000) >> 24;
	block[0x1EC] = 0x03; //Next free cluster
	block[0x1ED] = 0x00;
	block[0x1EE] = 0x00;
	block[0x1EF] = 0x00;
	block[0x1FE] = 0x55; //FSINFO ending signature (0x55 0xAA)
	block[0x1FF] = 0xAA;
	tlkdrv_xtsd01g_diskWrite(block, offset+1);
	
	//Wite Reserve Sector
	tmemset(block, 0, TLKDRV_XTSD01G_DISK_BLOCK_SIZE);
	for(index = 2; index < 6; index ++){
		tlkdrv_xtsd01g_diskWrite(block, offset+index);
	}
	tlkdrv_xtsd01g_diskWrite(block, offset+7);
	offset += 8;
	
	//write FAT
	for(fatNum = 0; fatNum < TLKDRV_XTSD01G_DISK_FAT_COPIES; fatNum++){
		tmemcpy(block, scTlkDevXtsd01gDiskFat32, sizeof(scTlkDevXtsd01gDiskFat32));
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
	tmemcpy(&block[0x1BE], scTlkDevXtsd01gDiskMBREntry, sizeof(scTlkDevXtsd01gDiskMBREntry));
	block[0x1FE] = 0x55; //FSINFO ending signature (0x55 0xAA)
	block[0x1FF] = 0xAA;
	tlkdrv_xtsd01g_diskWrite(block, 0);
	
	return TLK_ENONE;
}


#endif //#if (TLKDRV_STORE_XTSD01G_ENABLE)
#endif //#if (MCU_CORE_TYPE == MCU_CORE_B92)


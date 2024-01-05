/********************************************************************************************************
 * @file	tlkdrv_st77903.c
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
#include "tlkdrv_lcd.h"
#if (TLKDRV_LCD_ST77903_ENABLE)
#include "tlkdrv_st77903.h"
#include "drivers.h"
#include "./st77903_data/tlkdrv_st77903_backBuffer.h"
#include "./st77903_data/tlkdrv_st77903_foreBuffer.h"


#define TLKDRV_ST77903_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_ST77903_DBG_SIGN     "[DRV]"



const tlkdrv_lcd_modinf_t gcTlkDrvSt77903Infs = {
	tlkdrv_st77903_isOpen, //IsOpen
	tlkdrv_st77903_init,  //Init
	tlkdrv_st77903_open,  //Open
	tlkdrv_st77903_close, //Close
	tlkdrv_st77903_reset, //Reset
	tlkdrv_st77903_flush, //Flush
	tlkdrv_st77903_clean, //Clean
	tlkdrv_st77903_brush, //Brush
	tlkdrv_st77903_point, //Point
	tlkdrv_st77903_paint, //Paint
	tlkdrv_st77903_image, //Image
	tlkdrv_st77903_handler, //Handler
};
static tlkdrv_st77903_t sTlkDrvSt77903Ctrl = {0};


static bool tlkdrv_st77903_isOpen(void)
{
	return sTlkDrvSt77903Ctrl.isOpen;
}
static int tlkdrv_st77903_init(void)
{
	sTlkDrvSt77903Ctrl.isInit = true;
	sTlkDrvSt77903Ctrl.isOpen = false;

	gpio_output_en(TLKDRV_ST77903_RESET_PIN); 		//enable output
	gpio_input_dis(TLKDRV_ST77903_RESET_PIN);		//disable input
	gpio_set_high_level(TLKDRV_ST77903_RESET_PIN);
	gpio_function_en(TLKDRV_ST77903_RESET_PIN);
	
	gpio_output_en(TLKDRV_ST77903_POWER_ON_PIN); 		//enable output
	gpio_input_dis(TLKDRV_ST77903_POWER_ON_PIN);		//disable input
	gpio_set_high_level(TLKDRV_ST77903_POWER_ON_PIN);
	gpio_function_en(TLKDRV_ST77903_POWER_ON_PIN);
	gpio_set_up_down_res(TLKDRV_ST77903_POWER_ON_PIN, GPIO_PIN_PULLDOWN_100K);

	tlkdrv_st77903_setRgb(565);
	

	return TLK_ENONE;
}

static int tlkdrv_st77903_open(void)
{
	uint08 index;
	spi_pin_config_t pinCfg = {
		.spi_csn_pin 		= LSPI_CSN_PE0_PIN,
		.spi_clk_pin		= LSPI_CLK_PE1_PIN,
		.spi_mosi_io0_pin   = LSPI_MOSI_IO0_PE2_PIN,
		.spi_miso_io1_pin   = LSPI_MISO_IO1_PE3_PIN,//3line mode set 0
		.spi_io2_pin    	= LSPI_IO2_PE4_PIN,//set quad mode otherwise set 0
		.spi_io3_pin  		= LSPI_IO3_PE5_PIN,//set quad mode otherwise set 0
	};
	spi_wr_rd_config_t protCfg = {
		#if(TLKDRV_ST77903_DATA_LINE_MODE == TLKDRV_ST77903_CMD4X_ADDR4X_DATA4X)
		.spi_io_mode		= SPI_QUAD_MODE,
		#else
		.spi_io_mode		= SPI_SINGLE_MODE,
		#endif
		.spi_dummy_cnt  	= 2,
		.spi_cmd_en 		= 1,
		.spi_addr_en 		= 1,
		.spi_addr_len 		= 3,//when spi_addr_en = 0,invalid set.
		#if(TLKDRV_ST77903_DATA_LINE_MODE == TLKDRV_ST77903_CMD4X_ADDR4X_DATA4X)
		.spi_cmd_fmt_en  	= 1,//when spi_cmd_en = 0,invalid set.
		.spi_addr_fmt_en 	= 1,//when spi_addr_en = 0,invalid set.
		#else
		.spi_cmd_fmt_en  	= 0,//when spi_cmd_en = 0,invalid set.
		.spi_addr_fmt_en 	= 0,//when spi_addr_en = 0,invalid set.
		#endif
	};
	
	tlkapi_trace(TLKDRV_ST77903_DBG_FLAG, TLKDRV_ST77903_DBG_SIGN, "tlkdrv_st77903_open 01");

	if(!sTlkDrvSt77903Ctrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvSt77903Ctrl.isOpen) return -TLK_EREPEAT;

	gpio_set_low_level(TLKDRV_ST77903_POWER_ON_PIN);
	sTlkDrvSt77903Ctrl.isOpen = true;
	
	for(index=0; index<16; index++){
		if(sTlkDrvSt77903Ctrl.colorTable[index] != 0){
			lspi_lcd_set_lut(index, sTlkDrvSt77903Ctrl.colorTable[index]);
		}
	}
	
	tlkapi_trace(TLKDRV_ST77903_DBG_FLAG, TLKDRV_ST77903_DBG_SIGN, "tlkdrv_st77903_open 02");
	
	lspi_set_pin(&pinCfg);
	spi_master_init(LSPI_MODULE, sys_clk.pll_clk*1000000/TLKDRV_ST77903_SPI_CLK, SPI_MODE0);//48M TLKDRV_ST77903_SPI_CLK

//	lspi_lcd_clr_irq_status(FLD_LSPI_LCD_FRAME_INT_STA);//clr
//	lspi_lcd_set_irq_mask(FLD_LSPI_LCD_FRAME_INT_EN);//endint_en
//	spi_tx_irq_trig_cnt(LSPI_MODULE,20);
//	plic_interrupt_enable(IRQ22_LSPI);
	
	spi_master_config_plus(LSPI_MODULE, &protCfg);
	tlkdrv_st77903_config();
		
#if (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB888)
	tlkdrv_st77903_cfgScanMode((unsigned int)gTlkdrvSt77903ForeImage,(unsigned int)gTlkdrvSt77903BackImageRGB888, TLKDRV_ST77903_WIDTH, TLKDRV_ST77903_HEIGHT);
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB666)
	tlkdrv_st77903_cfgScanMode((unsigned int)gTlkdrvSt77903ForeImage,(unsigned int)gTlkdrvSt77903BackImageRGB666, TLKDRV_ST77903_WIDTH, TLKDRV_ST77903_HEIGHT);
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB565)
	tlkdrv_st77903_cfgScanMode((unsigned int)gTlkdrvSt77903ForeImage,(unsigned int)gTlkdrvSt77903BackImageRGB565, TLKDRV_ST77903_WIDTH, TLKDRV_ST77903_HEIGHT);
#endif
//
//	for( unsigned int i = 0; i<(sizeof(gTlkdrvSt77903ForeImage)/sizeof(gTlkdrvSt77903ForeImage[0])); i++)
//		gTlkdrvSt77903ForeImage[i] = 0;

//	ramless_lcd_fill_rectangle(100,100,100,100, LUT_GREEN,(unsigned char *)image_4bitram_ivo1_6);
	
	tlkapi_trace(TLKDRV_ST77903_DBG_FLAG, TLKDRV_ST77903_DBG_SIGN, "tlkdrv_st77903_open 03");
	
	return TLK_ENONE;
}
static int tlkdrv_st77903_close(void)
{
	if(!sTlkDrvSt77903Ctrl.isOpen) return TLK_ENONE;

	sTlkDrvSt77903Ctrl.isOpen = false;
	gpio_set_high_level(TLKDRV_ST77903_POWER_ON_PIN);
	return TLK_ENONE;
}
static int tlkdrv_st77903_reset(void)
{
	gpio_set_low_level(TLKDRV_ST77903_RESET_PIN);
	delay_ms(30);//20ms
	gpio_set_high_level(TLKDRV_ST77903_RESET_PIN);
	delay_ms(180);//120ms
	return TLK_ENONE;
}
static int tlkdrv_st77903_flush(void)
{
	return TLK_ENONE;
}
static int tlkdrv_st77903_clean(void)
{
	tmemset(gTlkdrvSt77903ForeImage, 0, 60800);
	return TLK_ENONE;
}
static int tlkdrv_st77903_brush(tlkdrv_lcd_brush_t *pBrush)
{
	if(pBrush == nullptr) return -TLK_EPARAM;
#if (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB888)
	sTlkDrvSt77903Ctrl.brushSize  = pBrush->w;
	sTlkDrvSt77903Ctrl.brushAlpha = pBrush->a;
	sTlkDrvSt77903Ctrl.brushColor = ((pBrush->r & 0xFF) << 16) | ((pBrush->g & 0xFF) << 8) | (pBrush->b & 0xFF);
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB666)
	sTlkDrvSt77903Ctrl.brushSize  = pBrush->w;
	sTlkDrvSt77903Ctrl.brushAlpha = pBrush->a;
	sTlkDrvSt77903Ctrl.brushColor = ((pBrush->r & 0x3F) << 12) | ((pBrush->g & 0x3F) << 6) | (pBrush->b & 0x3F);
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB565)
	sTlkDrvSt77903Ctrl.brushSize  = pBrush->w;
	sTlkDrvSt77903Ctrl.brushAlpha = pBrush->a;
	sTlkDrvSt77903Ctrl.brushColor = ((pBrush->r & 0x1F) << 3) | ((pBrush->g & 0x3F) << 5) | ((pBrush->b & 0x1F) << 8);
#endif
	sTlkDrvSt77903Ctrl.brushColor = TLKDRV_ST77903_LUT_RED;
	return TLK_ENONE;
}

static int tlkdrv_st77903_point(uint16 x, uint16 y)
{
	uint32 offset;
	
	if(x > TLKDRV_ST77903_WIDTH || y > TLKDRV_ST77903_HEIGHT) return -TLK_EOVERFLOW;

	if(sTlkDrvSt77903Ctrl.brushSize == 0 || sTlkDrvSt77903Ctrl.brushSize == 1){
		offset = (y * TLKDRV_ST77903_WIDTH + x)>>1;
		gTlkdrvSt77903ForeImage[offset] = (x % 2) ? ((gTlkdrvSt77903ForeImage[offset]&0x0f) | (sTlkDrvSt77903Ctrl.brushColor<<4))
			: ((gTlkdrvSt77903ForeImage[offset]&0xf0) | sTlkDrvSt77903Ctrl.brushColor);
	}else{
		uint16 i, j;
		for(i=0; i<sTlkDrvSt77903Ctrl.brushSize; i++){
			for(j=0; j<sTlkDrvSt77903Ctrl.brushSize; j++){
				offset = ((y+j) * TLKDRV_ST77903_WIDTH + (x+i))>>1;
				gTlkdrvSt77903ForeImage[offset] = ((x+i) % 2) 
					? ((gTlkdrvSt77903ForeImage[offset]&0x0f) | (sTlkDrvSt77903Ctrl.brushColor<<4))
					: ((gTlkdrvSt77903ForeImage[offset]&0xf0) | sTlkDrvSt77903Ctrl.brushColor);
			}
		}
	}	
	return TLK_ENONE;
}
static int tlkdrv_st77903_paint(uint16 opcode, void *pParam, uint16 paramLen)
{
	switch(opcode){
//		case TLKDRV_LCD_PAINT_LINE:
//			break;
//		case TLKDRV_LCD_PAINT_H_LINE:
//			break;
//		case TLKDRV_LCD_PAINT_V_LINE:
//			break;
//		case TLKDRV_LCD_PAINT_SECTOR:
//			break;
//		case TLKDRV_LCD_PAINT_RECT:
//			break;
//		case TLKDRV_LCD_PAINT_ROUNDED_RECT:
//			break;
//		case TLKDRV_LCD_PAINT_CHAMFER_RECT:
//			break;
//		case TLKDRV_LCD_PAINT_ELLIPSE:
//			break;
//		case TLKDRV_LCD_PAINT_CIRCLE:
//			break;
//		case TLKDRV_LCD_PAINT_FILL_SECTOR:
//			break;
//		case TLKDRV_LCD_PAINT_FILL_RECT:
//			break;
//		case TLKDRV_LCD_PAINT_FILL_ROUNDED_RECT:
//			break;
//		case TLKDRV_LCD_PAINT_FILL_CHAMFER_RECT:
//			break;
//		case TLKDRV_LCD_PAINT_FILL_ELLIPSE:
//			break;
//		case TLKDRV_LCD_PAINT_FILL_CIRCLE:
//			break;
		default:
			return -TLK_ENOSUPPORT;
	}
	
	return TLK_ENONE;
}
static int tlkdrv_st77903_image(tlkdrv_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_st77903_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	switch(opcode){
		case TLKDRV_LCD_OPCODE_GET_COLOR:
			return tlkdrv_st77903_getColorDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_GET_WIDTH:
			return tlkdrv_st77903_getWidthDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_GET_HEIGHT:
			return tlkdrv_st77903_getHeightDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_SET_FOREGROUND:
			return tlkdrv_st77903_setForegroundDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_SET_BACKGROUND:
			return tlkdrv_st77903_setBackgroundDeal(param0, param1);
	}
	return -TLK_ENOSUPPORT;
}


static int tlkdrv_st77903_getColorDeal(uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_st77903_getWidthDeal(uint32 param0, uint32 param1)
{
	return TLKDRV_ST77903_WIDTH;
}
static int tlkdrv_st77903_getHeightDeal(uint32 param0, uint32 param1)
{
	return TLKDRV_ST77903_HEIGHT;
}
static int tlkdrv_st77903_setForegroundDeal(uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_st77903_setBackgroundDeal(uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}

















static void tlkdrv_st77903_setRgb(uint16 RGB)
{
	tmemset(sTlkDrvSt77903Ctrl.colorTable, 0, sizeof(sTlkDrvSt77903Ctrl.colorTable));
	if(RGB == 565){
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_RED]   = 0x00F8;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_GREEN] = 0xE007;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_BLUE]  = 0x1F00;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_WHITE] = 0xFFFF;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_BLACK] = 0x0000;
	}else if(RGB == 666){
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_RED]   = 0x0000FC;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_GREEN] = 0x00FC00;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_BLUE]  = 0xFC0000;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_WHITE] = 0xFCFCFC;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_BLACK] = 0x000000;
	}else if(RGB == 888){
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_RED]   = 0x0000FF;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_GREEN] = 0x00FF00;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_BLUE]  = 0xFF0000;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_WHITE] = 0xFFFFFF;
		sTlkDrvSt77903Ctrl.colorTable[TLKDRV_ST77903_LUT_BLACK] = 0x000000;
	}
}


/******************************************************************************
 * Function: tlkdrv_st77903_writeData
 * Descript: This function serves to write "cmd(0xde) + address" to ramless lcd.
 * Params: 
 *		@addr[IN]--the operating address of ramless lcd.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_st77903_writeCmd(uint32 addr)
{
	spi_master_write_plus(LSPI_MODULE, 0xde, addr, 0, 0, SPI_MODE_NONE_DATA);
}
/******************************************************************************
 * Function: tlkdrv_st77903_writeData
 * Descript: This function serves to write "cmd(0xde) + address + data" to ramless lcd.
 * Params: 
 *		@addr[IN]--the operating address of ramless lcd.
 *		@pData[IN]--pointer to the data need to write.
 *		@dataLen[IN]--length in byte of the data need to write.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_st77903_writeData(uint32 addr, uint08 *pData, uint32 dataLen)
{
	spi_master_write_plus(LSPI_MODULE, 0xde, addr, pData, dataLen, SPI_MODE_WR_WRITE_ONLY);
}
/******************************************************************************
 * Function: tlkdrv_st77903_writeData
 * Descript: This function servers to config ramless lcd.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_st77903_config(void)
{
	unsigned char data[20];

	tlkdrv_st77903_reset();

	data[0] = 0xC3;
	tlkdrv_st77903_writeData(0x00f000, data, 1);

	data[0] = 0x96;
	tlkdrv_st77903_writeData(0x00f000, data, 1);

	data[0] = 0xa5;
	tlkdrv_st77903_writeData(0x00f000, data, 1);

	data[0] = 0x11;
	data[1] = 0x09;
	data[2] = 0xaa;
    data[3] = 0x11;
	tlkdrv_st77903_writeData(0x00c100, data, 4);

	data[0] = 0x11;
	data[1] = 0x09;
	data[2] = 0xaa;
	data[3] = 0x11;
	tlkdrv_st77903_writeData(0x00c200, data, 4);

	data[0] = 0x44;
	data[1] = 0x03;
	data[2] = 0x33;
	data[3] = 0x04;
	tlkdrv_st77903_writeData(0x00c300, data, 4);

	data[0] = 0x44;
	data[1] = 0x03;
	data[2] = 0x33;
	data[3] = 0x04;
	tlkdrv_st77903_writeData(0x00c400, data, 4);

	data[0] = 0x4d;
	tlkdrv_st77903_writeData(0x00c500, data, 1);

	data[0] = 0x00;
	tlkdrv_st77903_writeData(0x00d600, data, 1);

	data[0] = 0x00;
	tlkdrv_st77903_writeData(0x00d700, data, 1);

	data[0] = 0xf0;
	data[1] = 0x0d;
	data[2] = 0x14;
	data[3] = 0x0a;
	data[4] = 0x09;
	data[5] = 0x06;
	data[6] = 0x3a;
	data[7] = 0x43;
	data[8] = 0x51;
	data[9] = 0x07;
	data[10] = 0x14;
	data[11] = 0x15;
	data[12] = 0x30;
	data[13] = 0x35;
	tlkdrv_st77903_writeData(0x00e000, data, 14);

	data[0] = 0xf0;
	data[1] = 0x0d;
	data[2] = 0x13;
	data[3] = 0x0b;
	data[4] = 0x09;
	data[5] = 0x07;
	data[6] = 0x3b;
	data[7] = 0x43;
	data[8] = 0x50;
	data[9] = 0x08;
	data[10] = 0x14;
	data[11] = 0x15;
	data[12] = 0x30;
	data[13] = 0x35;
	tlkdrv_st77903_writeData(0x00e100, data, 14);

	data[0] = 0xb9;
	data[1] = 0xf5;
	data[2] = 0xb5;
	data[3] = 0x55;
	data[4] = 0x22;
	data[5] = 0x25;
	data[6] = 0x10;
	data[7] = 0x22;
	data[8] = 0x22;
	data[9] = 0x22;
	data[10] = 0x22;
	data[11] = 0x22;
	data[12] = 0x22;
	data[13] = 0x22;
	tlkdrv_st77903_writeData(0x00e500, data, 14);

	data[0] = 0xb9;
	data[1] = 0xf5;
	data[2] = 0xb5;
	data[3] = 0x55;
	data[4] = 0x22;
	data[5] = 0x25;
	data[6] = 0x10;
	data[7] = 0x22;
	data[8] = 0x22;
	data[9] = 0x22;
	data[10] = 0x22;
	data[11] = 0x22;
	data[12] = 0x22;
	data[13] = 0x22;
	tlkdrv_st77903_writeData(0x00e600, data, 14);

	data[0] = 0x80;
	data[1] = 0x77;
	data[2] = 0x1f;
	data[3] = 0xcc;
	tlkdrv_st77903_writeData(0x00e700, data, 4);

	data[0] = 0x00;
	data[1] = 0x55;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x88;
	tlkdrv_st77903_writeData(0x00ec00, data, 6);

#if (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB888)
	data[0] = 0x07;
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB666)
	data[0] = 0x06;
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB565)
	data[0] = 0x05;
#endif
	tlkdrv_st77903_writeData(0x003a00, data, 1);

	data[0] = 0xfe;
	data[1] = 0xdf;
	tlkdrv_st77903_writeData(0x00b100, data, 2);

	data[0] = 0x09;
	tlkdrv_st77903_writeData(0x00b200, data, 1);

	data[0] = 0x01;
	tlkdrv_st77903_writeData(0x00b300, data, 1);

	data[0] = 0x01;
	tlkdrv_st77903_writeData(0x00b400, data, 1);

	data[0] = 0x00;
	data[1] = 0x08;
	data[2] = 0x00;
	data[3] = 0x08;
	tlkdrv_st77903_writeData(0x00b500, data, 4);

	data[0] = 0xbd;
	data[1] = 0x27;
	tlkdrv_st77903_writeData(0x00b600, data, 2);

	data[0] = 0xc0;
	data[1] = 0x6b;
	tlkdrv_st77903_writeData(0x00a400, data, 2);

	data[0] = 0x11;
	data[1] = 0x53;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x20;
	data[5] = 0x15;
	data[6] = 0x2a;
	data[7] = 0xba;
	data[8] = 0x02;
	tlkdrv_st77903_writeData(0x00a500, data, 9);

	data[0] = 0x11;
	data[1] = 0x53;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x20;
	data[5] = 0x15;
	data[6] = 0x2a;
	data[7] = 0xba;
	data[8] = 0x02;
	tlkdrv_st77903_writeData(0x00a600, data, 9);

	data[0] = 0x58;
	data[1] = 0x0a;
	data[2] = 0x34;
	data[3] = 0x10;
	data[4] = 0x22;
	data[5] = 0x01;
	data[6] = 0x00;
	tlkdrv_st77903_writeData(0x00ba00, data, 7);

	data[0] = 0x00;
	data[1] = 0x33;
	data[2] = 0x00;
	data[3] = 0x2c;
	data[4] = 0x83;
	data[5] = 0x07;
	data[6] = 0x18;
	data[7] = 0x00;
	tlkdrv_st77903_writeData(0x00bb00, data, 8);

	data[0] = 0x00;
	data[1] = 0x33;
	data[2] = 0x00;
	data[3] = 0x2c;
	data[4] = 0x83;
	data[5] = 0x07;
	data[6] = 0x18;
	data[7] = 0x00;
	tlkdrv_st77903_writeData(0x00bc00, data, 8);

	data[0] = 0x21;
	data[1] = 0x12;
	data[2] = 0xff;
	data[3] = 0xff;
	data[4] = 0x67;
	data[5] = 0x58;
	data[6] = 0x85;
	data[7] = 0x76;
	data[8] = 0xab;
	data[9] = 0xff;
	data[10] = 0x03;
	tlkdrv_st77903_writeData(0x00bd00, data, 11);


	data[0] = 0xc3;
	tlkdrv_st77903_writeData(0x00ed00, data, 1);

	data[0] = 0x08;
	data[1] = 0x2f;
	tlkdrv_st77903_writeData(0x00e400, data, 2);

	data[0] = 0x22;
	tlkdrv_st77903_writeData(0x00d900, data, 1);

	data[0] = 0x0c;
	tlkdrv_st77903_writeData(0x003600, data, 1);

	tlkdrv_st77903_writeCmd(0x002100);
	tlkdrv_st77903_writeCmd(0x001100);

	delay_ms(120);//120ms

	tlkdrv_st77903_writeCmd(0x002900);
}
static void tlkdrv_st77903_cfgScanMode(uint32 foreImgAddr, uint32 backImgAddr, uint16 nCols, uint16 nRows)
{
#if(TLKDRV_ST77903_DATA_LINE_MODE == TLKDRV_ST77903_CMD1X_ADDR1X_DATA1X)
	lspi_lcd_set_cmd(0xD8);
	spi_set_normal_mode(LSPI_MODULE);
#elif(TLKDRV_ST77903_DATA_LINE_MODE == TLKDRV_ST77903_CMD1X_ADDR1X_DATA2X)
	lspi_lcd_set_cmd(0xDB);
	spi_set_dual_mode(LSPI_MODULE);
#elif(TLKDRV_ST77903_DATA_LINE_MODE == TLKDRV_ST77903_CMD1X_ADDR1X_DATA4X)
	lspi_lcd_set_cmd(0xDE);
	spi_set_quad_mode(LSPI_MODULE);
#elif(TLKDRV_ST77903_DATA_LINE_MODE == TLKDRV_ST77903_CMD4X_ADDR4X_DATA4X)
	lspi_lcd_set_cmd(0xDE);
	spi_set_quad_mode(LSPI_MODULE);
#endif

#if (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB888) || (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB666)
	lspi_lcd_set_display_transmode(SPI_MODE_WRITE_ONLY);//write only for Display dummy
	lspi_lcd_set_porch_transmode(SPI_MODE_NONE_DATA); ////nodata for CMD_DUMMY
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB565)
#if(TLKDRV_ST77903_DATA_LINE_MODE == TLKDRV_ST77903_CMD4X_ADDR4X_DATA4X)
	lspi_lcd_set_display_transmode(SPI_MODE_DUMMY_WRITE);//write only for Display dummy
	lspi_lcd_set_porch_transmode(SPI_MODE_NONE_DATA); ////nodata for CMD_DUMMY
#else
	lspi_lcd_set_display_transmode(SPI_MODE_WRITE_ONLY);//write only for Display dummy
	lspi_lcd_set_porch_transmode(SPI_MODE_NONE_DATA); ////nodata for CMD_DUMMY
#endif
#endif

	lspi_lcd_rgb_little_endian_mode();
//vbp vfp given by the manufacturer test
	lspi_lcd_vbp_line_cnt(TLKDRV_ST77903_VBP_CNT);
	lspi_lcd_vfp_line_cnt(TLKDRV_ST77903_VFP_CNT);
	
	lspi_lcd_bimage_start_addr(convert_ram_addr_cpu2bus(backImgAddr));
	lspi_lcd_fimage_start_addr(convert_ram_addr_cpu2bus(foreImgAddr));

#if (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB888)
	lspi_lcd_rgb_mode(LSPI_LCD_RGB888); // 1:565   2��666   3:888
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB666)
	lspi_lcd_rgb_mode(LSPI_LCD_RGB666);
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB565)
	lspi_lcd_rgb_mode(LSPI_LCD_RGB565);
#endif
//the minimum time of porch and display need meet 40us, and the delay between line and line not be less than 500ns.
#if( TLKDRV_ST77903_SPI_CLK == 48000000)
	lspi_lcd_porch_min_line_time(1920);
	lspi_lcd_display_min_line_time(1920);
#elif( TLKDRV_ST77903_SPI_CLK == 38400000 )
	lspi_lcd_porch_min_line_time(1539);
	lspi_lcd_display_min_line_time(1539);
#elif( TLKDRV_ST77903_SPI_CLK == 32000000 )
	lspi_lcd_porch_min_line_time(1320);
	lspi_lcd_display_min_line_time(1320);
#else
	lspi_lcd_porch_min_line_time(1920);
	lspi_lcd_display_min_line_time(1920);
#endif

	spi_set_cs_high_time(LSPI_MODULE,26);

	lspi_lcd_set_res(nRows,nCols);
	lspi_lcd_single_color_mode_dis();
	
	lspi_lcd_scan_en();
}



#endif //#if (TLKDRV_LCD_ST77903_ENABLE)


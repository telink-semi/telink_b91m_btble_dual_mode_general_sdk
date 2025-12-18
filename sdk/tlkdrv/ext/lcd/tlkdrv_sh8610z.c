/********************************************************************************************************
 * @file	tlkdrv_sh8610z.c
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
#if (TLKDRV_LCD_SH8610Z_ENABLE)
#include "drivers.h"
#include "tlkdrv_sh8610z.h"
#include "image/tlkdrv_lcd_image320x380.h"
#include "sh8610z_data/tlkdrv_sh8610z_data.h"


#define TLKDRV_SH8610Z_DBG_FLAG     ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_SH8610Z_DBG_SIGN     "[DRV]"


const tlkdrv_lcd_modinf_t gcTlkDrvSh8610zInfs = {
	tlkdrv_sh8610z_isOpen, //IsOpen
	tlkdrv_sh8610z_init,  //Init
	tlkdrv_sh8610z_open,  //Open
	tlkdrv_sh8610z_close, //Close
	tlkdrv_sh8610z_reset, //Reset
	tlkdrv_sh8610z_flush, //Flush
	tlkdrv_sh8610z_clean, //Clean
	tlkdrv_sh8610z_brush, //Brush
	tlkdrv_sh8610z_point, //Point
	tlkdrv_sh8610z_paint, //Paint
	tlkdrv_sh8610z_image, //Image
	tlkdrv_sh8610z_handler, //Handler
};
static tlkdrv_sh8610z_t sTlkDrvSh8610zCtrl = {0};

////oled configuration parameter
//volatile
const tlkdrv_sh8610z_config_t scTlkDrvSh8610zDataCfg[TLKDRV_SH8610Z_DATA_CFG_NUMB] = {
	{4, 0x002A00, {0,0,0x01,0x6F}},
	{4, 0x002B00, {0,0,0x01,0xBF}},
	{2, 0x004400, {0x01,0xBF}},
	{1, 0x003500, {0x00}},
	{1, 0x005300, {0x20}},
	{1, 0x00c400, {0x82}},
};


_attribute_ram_code_sec_ void lspi_irq_handler(void)
{
	if(spi_get_irq_status(LSPI_MODULE,SPI_END_INT))
	{
		spi_clr_irq_status(LSPI_MODULE, FLD_SPI_END_INT);//clr
		if( !spi_is_busy(LSPI_MODULE) )
		{
#if (TLKGUI_LVGL_ENABLE)
			extern void lv_ready_irq(void);
			lv_ready_irq();
#endif
		}
	}
}

static bool tlkdrv_sh8610z_isOpen(void)
{
	return sTlkDrvSh8610zCtrl.isOpen;
}
static int tlkdrv_sh8610z_init(void)
{
	tmemset(&sTlkDrvSh8610zCtrl, 0, sizeof(tlkdrv_sh8610z_t));
	sTlkDrvSh8610zCtrl.isInit = true;
	sTlkDrvSh8610zCtrl.isOpen = false;

	gpio_output_en(TLKDRV_SH8610Z_RESET_PIN);
	gpio_input_dis(TLKDRV_SH8610Z_RESET_PIN);
	gpio_set_high_level(TLKDRV_SH8610Z_RESET_PIN);
	gpio_function_en(TLKDRV_SH8610Z_RESET_PIN);

	gpio_output_en(TLKDRV_SH8610Z_POWER_ON_PIN);
	gpio_input_dis(TLKDRV_SH8610Z_POWER_ON_PIN);
	gpio_set_high_level(TLKDRV_SH8610Z_POWER_ON_PIN);
	gpio_function_en(TLKDRV_SH8610Z_POWER_ON_PIN);
	
	sTlkDrvSh8610zCtrl.brushSize = 2;
	#if (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB565)
	sTlkDrvSh8610zCtrl.brushColor = TLKDRV_SH8610Z_565_BLUE;
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB666)
	sTlkDrvSh8610zCtrl.brushColor = TLKDRV_SH8610Z_666_BLUE;
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB888)
	sTlkDrvSh8610zCtrl.brushColor = TLKDRV_SH8610Z_888_BLUE;
	#endif
	sTlkDrvSh8610zCtrl.brushAlpha = 0;
		
	return TLK_ENONE;
}

static int tlkdrv_sh8610z_open(void)
{
	uint08 index;
	spi_pin_config_t lspiPinConfig = {
		.spi_csn_pin 		= LSPI_CSN_PE0_PIN,
		.spi_clk_pin		= LSPI_CLK_PE1_PIN,
		.spi_mosi_io0_pin   = LSPI_MOSI_IO0_PE2_PIN,
		.spi_miso_io1_pin   = LSPI_MISO_IO1_PE3_PIN,//3line mode set 0
		.spi_io2_pin    	= LSPI_IO2_PE4_PIN,//set quad mode otherwise set 0
		.spi_io3_pin  		= LSPI_IO3_PE5_PIN,//set quad mode otherwise set 0
	};
	spi_wr_rd_config_t lspiProtocolConfig = {
		.spi_io_mode	= SPI_SINGLE_MODE,
		.spi_dummy_cnt 	= 0,
		.spi_cmd_en		= 1,
		.spi_addr_en	= 1,//must
		.spi_addr_len	= 3,//when spi_addr_en = false, invalid set.
		.spi_cmd_fmt_en	= 0,//when spi_cmd_en = false, invalid set.
		.spi_addr_fmt_en= 1,//when spi_addr_en = false, invalid set.
	};
	
	if(!sTlkDrvSh8610zCtrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvSh8610zCtrl.isOpen) return -TLK_EREPEAT;
	
	gpio_set_low_level(TLKDRV_SH8610Z_POWER_ON_PIN);
	tlkdrv_sh8610z_reset();
	
	spi_master_init(LSPI_MODULE, sys_clk.pll_clk * 1000000 / TLKDRV_SH8610Z_SPI_CLK, SPI_MODE0);
	lspi_set_pin(&lspiPinConfig);
	
	//enable irq
	spi_clr_irq_status(LSPI_MODULE, FLD_SPI_END_INT);//clr
	spi_set_irq_mask(LSPI_MODULE, SPI_END_INT_EN);

	plic_interrupt_enable(IRQ22_LSPI);
	plic_set_priority(IRQ22_LSPI, IRQ_PRI_LEV1);

	#if (TLKDRV_SH8610Z_DMA_ENABLE)
	spi_set_tx_dma_config(LSPI_MODULE,TLKDRV_SH8610Z_DMA_CHN);
	dma_set_spi_burst_size(TLKDRV_SH8610Z_DMA_CHN, DMA_BURST_4_WORD);
	#endif
	spi_master_config_plus(LSPI_MODULE, &lspiProtocolConfig);
	
	tlkdrv_sh8610z_spiWriteCmd(0x001100);//Sleep Out
	delay_ms(120);
	for(index = 0; index < TLKDRV_SH8610Z_DATA_CFG_NUMB; index++){
		tlkdrv_sh8610z_spiWriteData(scTlkDrvSh8610zDataCfg[index].oled_addr,
			(uint08*)scTlkDrvSh8610zDataCfg[index].oled_data, scTlkDrvSh8610zDataCfg[index].dataLen);
	}

	#if (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB565)
	tlkdrv_sh8610z_spiWriteData(scTlkDrvSh8610zRgb565Cfg.oled_addr, (uint08*)&scTlkDrvSh8610zRgb565Cfg.oled_data, 1);
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB666)
	tlkdrv_sh8610z_spiWriteData(scTlkDrvSh8610zRgb666Cfg.oled_addr, &scTlkDrvSh8610zRgb666Cfg.oled_data, 1);
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB888)
	tlkdrv_sh8610z_spiWriteData(scTlkDrvSh8610zRgb888Cfg.oled_addr, &scTlkDrvSh8610zRgb888Cfg.oled_data, 1);
	#endif
	tlkdrv_sh8610z_spiWriteCmd(0x002900);//Display On
	
	//show a picture in oled
	#if (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB565)
	tlkdrv_sh8610z_showImage(70, 40, 320, 380, (uint08*)scTlkDrvLcdRgb565Size320x380Image, 243200);
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB666)
	tlkdrv_sh8610z_showImage(70, 40, 320, 380, (uint08*)scTlkDrvLcdRgb666Size320x380Image, 364800);
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB888)
	tlkdrv_sh8610z_showImage(70, 40, 320, 380, (uint08*)scTlkDrvLcdRgb888Size320x380Image, 364800);
	#endif

	sTlkDrvSh8610zCtrl.isOpen = true;

	tlkdrv_sh8610z_showFillRect(0,0,367,447,TLKDRV_SH8610Z_565_BLACK);
	delay_us(50);
	//fill a rectangle with red color in oled
	tlkdrv_sh8610z_showFillRect(60,150,50,200,TLKDRV_SH8610Z_565_BLUE);
	delay_ms(50);

	return TLK_ENONE;
}
static int tlkdrv_sh8610z_close(void)
{
	if(!sTlkDrvSh8610zCtrl.isOpen) return TLK_ENONE;

	sTlkDrvSh8610zCtrl.isOpen = false;
	gpio_set_high_level(TLKDRV_SH8610Z_POWER_ON_PIN);
	return TLK_ENONE;
}
static int tlkdrv_sh8610z_reset(void)
{
	if(!sTlkDrvSh8610zCtrl.isInit) return -TLK_ENOREADY;
	gpio_set_low_level(TLKDRV_SH8610Z_RESET_PIN);
	delay_ms(120);
	gpio_set_high_level(TLKDRV_SH8610Z_RESET_PIN);
	delay_ms(120);
	return TLK_ENONE;
}
static int tlkdrv_sh8610z_flush(void)
{
	return TLK_ENONE;
}
static int tlkdrv_sh8610z_clean(void)
{
	return TLK_ENONE;
}
static int tlkdrv_sh8610z_brush(tlkdrv_lcd_brush_t *pBrush)
{
	if(pBrush == nullptr) return -TLK_EPARAM;

	pBrush->w &= 0xFE;
	if(pBrush->w < 2) pBrush->w = 2;
	sTlkDrvSh8610zCtrl.brushSize = pBrush->w;
	sTlkDrvSh8610zCtrl.brushColor = 0;
	#if (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB565)
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->r & 0x1F) << 3;
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->g & 0x07) << 10;
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->g & 0x38) >> 3;
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->b & 0x1F) << 8;
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB666)
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->r & 0x3F) << 2;
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->g & 0x3F) << 10;
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->b & 0x3F) << 18;
	sTlkDrvSh8610zCtrl.brushColor |= ;
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB888)
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->r & 0xFF) << 0;
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->g & 0xFF) << 8;
	sTlkDrvSh8610zCtrl.brushColor |= (pBrush->g & 0xFF) << 16;
	#endif
	sTlkDrvSh8610zCtrl.brushAlpha = pBrush->a;
	
	return TLK_ENONE;
}

static int tlkdrv_sh8610z_point(uint16 x, uint16 y)
{
	if(x > TLKDRV_SH8610Z_WIDTH || y > TLKDRV_SH8610Z_HEIGHT) return -TLK_EOVERFLOW;
	tlkdrv_sh8610z_showFillRect(x, y, sTlkDrvSh8610zCtrl.brushSize, sTlkDrvSh8610zCtrl.brushSize, sTlkDrvSh8610zCtrl.brushColor);
	return TLK_ENONE;
}
static int tlkdrv_sh8610z_paint(uint16 opcode, void *pParam, uint16 paramLen)
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
		case TLKDRV_LCD_PAINT_FILL_RECT:
			return tlkdrv_sh8610z_paintFillRectDeal(pParam, paramLen);
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
static int tlkdrv_sh8610z_image(tlkdrv_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen)
{
	tlkdrv_sh8610z_showImage(pRegion->x, pRegion->y, pRegion->w, pRegion->h, pData, dataLen);
	return TLK_ENONE;
}
static int tlkdrv_sh8610z_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	switch(opcode){
		case TLKDRV_LCD_OPCODE_GET_COLOR:
			return tlkdrv_sh8610z_getColorDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_GET_WIDTH:
			return tlkdrv_sh8610z_getWidthDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_GET_HEIGHT:
			return tlkdrv_sh8610z_getHeightDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_SET_FOREGROUND:
			return tlkdrv_sh8610z_setForegroundDeal(param0, param1);
		case TLKDRV_LCD_OPCODE_SET_BACKGROUND:
			return tlkdrv_sh8610z_setBackgroundDeal(param0, param1);
	}
	return -TLK_ENOSUPPORT;
}

static int tlkdrv_sh8610z_getColorDeal(uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_sh8610z_getWidthDeal(uint32 param0, uint32 param1)
{
	return TLKDRV_SH8610Z_WIDTH;
}
static int tlkdrv_sh8610z_getHeightDeal(uint32 param0, uint32 param1)
{
	return TLKDRV_SH8610Z_HEIGHT;
}
static int tlkdrv_sh8610z_setForegroundDeal(uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_sh8610z_setBackgroundDeal(uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}


static int tlkdrv_sh8610z_paintFillRectDeal(void *pParam, uint16 paramLen)
{
	tlkdrv_lcd_rect_t *pRectParam;

	pRectParam = (tlkdrv_lcd_rect_t*)pParam;
	tlkdrv_sh8610z_showFillRect(pRectParam->x, pRectParam->y, pRectParam->w, pRectParam->h,
		sTlkDrvSh8610zCtrl.brushColor);
	
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkdrv_sh8610z_spiWriteCmd
 * Descript: This function serves to write  "cmd(0x02) + address" to oled.
 * Params:
 *     @addr[IN]--the operating address of oled.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_sh8610z_spiWriteCmd(uint32 addr)
{
	spi_master_write_plus(LSPI_MODULE,TLKDRV_SH8610Z_CMD_SINGLE_WRITE, addr, 0, 0, SPI_MODE_NONE_DATA);
}
/******************************************************************************
 * Function: tlkdrv_sh8610z_spiWriteData
 * Descript: This function serves to write "cmd(0x02) + address + data" to oled.
 * Params:
 *     @addr[IN]--the operating address of oled.
 *     @pData[IN]--pointer to the data need to write.
 *     @dataLen[IN]--length in byte of the data need to write.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_sh8610z_spiWriteData(uint32 addr, uint08 *pData, uint32 dataLen)
{
	spi_master_write_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_SINGLE_WRITE, addr, pData, dataLen, SPI_MODE_WR_WRITE_ONLY);
}

/******************************************************************************
 * Function: tlkdrv_sh8610z_setShowArea
 * Descript: This function serves to write operating memory address and space 
 *           size to oled.
 * Params:
 *     @x[IN]--abscissa of address.
 *     @y[IN]--ordinate of address.
 *     @w[IN]--horizontal width of space.
 *     @h[IN]--vertical width of space.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkdrv_sh8610z_setShowArea(uint16 x, uint16 y, uint16 w, uint16 h)
{
	uint08 data[4];
	
	while(spi_is_busy(LSPI_MODULE));
	spi_set_normal_mode(LSPI_MODULE);
	
	data[0] = x>>8;
	data[1] = x&0xff;
	data[2] = (x+w-1)>>8;
	data[3] = (x+w-1)&0xff;
	tlkdrv_sh8610z_spiWriteData(0x002A00, data, 4);
	data[0] = y>>8;
	data[1] = y&0xff;
	data[2] = (y+h-1)>>8;
	data[3] = (y+h-1)&0xff;
	tlkdrv_sh8610z_spiWriteData(0x002B00, data, 4);
}

/******************************************************************************
 * Function: tlkdrv_sh8610z_setShowArea
 * Descript: This function serves to fill a rectangular space of oled with 
 *           single color.
 * Params:
 *     @x[IN]--abscissa of address.
 *     @y[IN]--ordinate of address.
 *     @w[IN]--horizontal width of space.
 *     @h[IN]--vertical width of space.
 *     @color[IN]--the single color.
 * Return: None.
 * Others: For rm69330 oled param x, y, x_delta, y_delta must be multiple of 2.
*******************************************************************************/
static void tlkdrv_sh8610z_showFillRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 color)
{
	unsigned int display_size;
	
	tlkdrv_sh8610z_setShowArea(x, y, w, h);
	display_size = w * h;

#if (TLKDRV_SH8610Z_DATA_LANE_MODE == TLKDRV_SH8610Z_4DATA_LANE)
	spi_set_quad_mode(LSPI_MODULE);
	#if (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB565)
	spi_master_write_repeat_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_QUAD_ADDR_WRITE, 0x002C00, (unsigned char*)&color, 2, display_size, SPI_MODE_WR_WRITE_ONLY);
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB666 || TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB888) //rgb_666 & RGB_888
	spi_master_write_repeat_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_QUAD_ADDR_WRITE, 0x002C00, (unsigned char*)&color, 3, display_size, SPI_MODE_WR_WRITE_ONLY);
	#endif
#else
	#if (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB565)
	spi_master_write_repeat_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_SINGLE_WRITE, 0x002C00, (unsigned char*)&color, 2, display_size, SPI_MODE_WR_WRITE_ONLY);
	#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB666 || TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB888) //rgb_666 & RGB_888
	spi_master_write_repeat_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_SINGLE_WRITE, 0x002C00, (unsigned char*)&color, 3, display_size, SPI_MODE_WR_WRITE_ONLY);
	#endif
#endif
}

/******************************************************************************
 * Function: tlkdrv_sh8610z_showImage
 * Descript: This function serves to show a rectangular picture in oled.
 * Params:
 *     @x[IN]--abscissa of address.
 *     @y[IN]--ordinate of address.
 *     @w[IN]--horizontal width of space.
 *     @h[IN]--vertical width of space.
 *     @pData - address of picture buffer.
 *     @dataLen - length of picture buffer.
 * Return: None.
 * Others: For rm69330 oled param x, y, x_delta, y_delta must be multiple of 2.
*******************************************************************************/
static void tlkdrv_sh8610z_showImage(uint16 x, uint16 y, uint16 w, uint16 h, uint08 *pData, uint32 dataLen)
{
	uint32 needLen;
	
	tlkdrv_sh8610z_setShowArea(x, y, w, h);

#if (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB565)
	needLen=2*w * h;
#elif (TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB666 || TLKDRV_SH8610Z_RGB_SELECT == TLKDRV_SH8610Z_RGB888) //rgb_666 & RGB_888
	needLen=3*w * h;
#endif

#if (TLKDRV_SH8610Z_DATA_LANE_MODE == TLKDRV_SH8610Z_4DATA_LANE)
	spi_set_quad_mode(LSPI_MODULE);
	#if (TLKDRV_SH8610Z_DMA_ENABLE)
	spi_master_write_dma_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_QUAD_ADDR_WRITE, 0x002C00, pData, needLen, SPI_MODE_WR_WRITE_ONLY);
	#else
	spi_master_write_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_QUAD_ADDR_WRITE, 0x002C00, pData, needLen, SPI_MODE_WR_WRITE_ONLY);
	#endif
#else
	#if (TLKDRV_SH8610Z_DMA_ENABLE)
	spi_master_write_dma_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_SINGLE_WRITE, 0x002C00, pData, needLen, SPI_MODE_WR_WRITE_ONLY);
	#else
	spi_master_write_plus(LSPI_MODULE, TLKDRV_SH8610Z_CMD_SINGLE_WRITE, 0x002C00, pData, needLen, SPI_MODE_WR_WRITE_ONLY);
	#endif
#endif
}




#endif //#if (TLKDRV_LCD_SH8610Z_ENABLE)


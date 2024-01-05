/********************************************************************************************************
 * @file	tlkdrv_st77903.h
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
#ifndef TLKDRV_ST77903_H
#define TLKDRV_ST77903_H

#if (TLKDRV_LCD_ST77903_ENABLE)



//#define TLKDRV_ST77903_SPI_CLK 				48000000   		//According to IC design, the fastest SPI clock is 48M
//#define TLKDRV_ST77903_SPI_CLK				38400000		//for st77903, when sending Line,cs should be pulled down at least 35us,so spi_clk should be at the highest 38.4m,otherwise picture jitters will occur
#define TLKDRV_ST77903_SPI_CLK					32000000




#define TLKDRV_ST77903_RESET_PIN			GPIO_PE6
#define TLKDRV_ST77903_POWER_ON_PIN         GPIO_PF7
#define TLKDRV_ST77903_WIDTH                320
#define TLKDRV_ST77903_HEIGHT               380
#define TLKDRV_ST77903_VBP_CNT      		6
#define TLKDRV_ST77903_VFP_CNT      		16


#define TLKDRV_ST77903_RGB565           1
#define TLKDRV_ST77903_RGB666           2
#define TLKDRV_ST77903_RGB888           3
#define TLKDRV_ST77903_RGB_SELECT       TLKDRV_ST77903_RGB565
#if (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB565)
#define TLKDRV_ST77903_COLOR_RED        0x00F8
#define TLKDRV_ST77903_COLOR_GREEN      0xE007
#define TLKDRV_ST77903_COLOR_BLUE       0x1F00
#define TLKDRV_ST77903_COLOR_WHITE      0xFFFF
#define TLKDRV_ST77903_COLOR_BLACK      0x0000
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB888)
#define TLKDRV_ST77903_COLOR_RED        0x0000FF
#define TLKDRV_ST77903_COLOR_GREEN      0x00FF00
#define TLKDRV_ST77903_COLOR_BLUE       0xFF0000
#define TLKDRV_ST77903_COLOR_WHITE      0xFFFFFF
#define TLKDRV_ST77903_COLOR_BLACK  0x000000
#elif (TLKDRV_ST77903_RGB_SELECT == TLKDRV_ST77903_RGB666)
#define TLKDRV_ST77903_COLOR_RED        0x0000FC
#define TLKDRV_ST77903_COLOR_GREEN      0x00FC00
#define TLKDRV_ST77903_COLOR_BLUE       0xFC0000
#define TLKDRV_ST77903_COLOR_WHITE      0xFCFCFC
#define TLKDRV_ST77903_COLOR_BLACK      0x000000
#endif

#define TLKDRV_ST77903_LUT_RED    0x01
#define TLKDRV_ST77903_LUT_GREEN  0x02
#define TLKDRV_ST77903_LUT_BLUE   0x0C
#define TLKDRV_ST77903_LUT_WHITE  0x0D
#define TLKDRV_ST77903_LUT_BLACK  0x0F


/*
 * BOE1.72 : since the screen does not draw out the IM0,IM1 pins,can only be the default value (0,0).
 */
#define	TLKDRV_ST77903_CMD1X_ADDR1X_DATA1X      1//(IM1,IM0) = (0,0)
#define	TLKDRV_ST77903_CMD1X_ADDR1X_DATA2X      2//(IM1,IM0) = (0,0)
#define	TLKDRV_ST77903_CMD1X_ADDR1X_DATA4X      3//(IM1,IM0) = (0,0)
#define	TLKDRV_ST77903_CMD4X_ADDR4X_DATA4X      4//(IM1,IM0) = (0,1)

#define TLKDRV_ST77903_DATA_LINE_MODE           TLKDRV_ST77903_CMD1X_ADDR1X_DATA4X



typedef struct{
	uint08 isInit;
	uint08 isOpen;
	uint32 colorTable[16];

	uint08 brushSize;
	uint08 brushAlpha;
	uint32 brushColor;
}tlkdrv_st77903_t;


static bool tlkdrv_st77903_isOpen(void);
static int tlkdrv_st77903_init(void);
static int tlkdrv_st77903_open(void);
static int tlkdrv_st77903_close(void);
static int tlkdrv_st77903_reset(void);
static int tlkdrv_st77903_flush(void);
static int tlkdrv_st77903_clean(void);
static int tlkdrv_st77903_brush(tlkdrv_lcd_brush_t *pBrush);
static int tlkdrv_st77903_point(uint16 x, uint16 y);
static int tlkdrv_st77903_paint(uint16 opcode, void *pParam, uint16 paramLen);
static int tlkdrv_st77903_image(tlkdrv_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen);
static int tlkdrv_st77903_handler(uint16 opcode, uint32 param0, uint32 param1);


static void tlkdrv_st77903_writeCmd(uint32 addr);
static void tlkdrv_st77903_writeData(uint32 addr, uint08 *pData, uint32 dataLen);
static void tlkdrv_st77903_setRgb(uint16 RGB);
static void tlkdrv_st77903_config(void);
static void tlkdrv_st77903_cfgScanMode(uint32 foreImgAddr, uint32 backImgAddr, uint16 nCols, uint16 nRows);


static int tlkdrv_st77903_getColorDeal(uint32 param0, uint32 param1);
static int tlkdrv_st77903_getWidthDeal(uint32 param0, uint32 param1);
static int tlkdrv_st77903_getHeightDeal(uint32 param0, uint32 param1);
static int tlkdrv_st77903_setForegroundDeal(uint32 param0, uint32 param1);
static int tlkdrv_st77903_setBackgroundDeal(uint32 param0, uint32 param1);



#endif //#if (TLKDRV_LCD_ST77903_ENABLE)

#endif //TLKDRV_ST77903_H


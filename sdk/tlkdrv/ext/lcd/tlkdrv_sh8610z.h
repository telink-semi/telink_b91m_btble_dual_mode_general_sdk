/********************************************************************************************************
 * @file	tlkdrv_sh8610z.h
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
#ifndef TLKDRV_SH8610Z_H
#define TLKDRV_SH8610Z_H

#if (TLKDRV_LCD_SH8610Z_ENABLE)


#define TLKDRV_SH8610Z_RESET_PIN			GPIO_PE6
#define TLKDRV_SH8610Z_POWER_ON_PIN         GPIO_PF7

#define TLKDRV_SH8610Z_SPI_CLK              48000000

#define TLKDRV_SH8610Z_DMA_ENABLE           1
#define TLKDRV_SH8610Z_DMA_CHN              DMA7

#define	TLKDRV_SH8610Z_1DATA_LANE           1
#define	TLKDRV_SH8610Z_4DATA_LANE           2
#define TLKDRV_SH8610Z_DATA_LANE_MODE       TLKDRV_SH8610Z_4DATA_LANE

#define TLKDRV_SH8610Z_CMD_SINGLE_WRITE         0x02
#define TLKDRV_SH8610Z_CMD_SINGLE_READ          0x03
#define TLKDRV_SH8610Z_CMD_QUAD_ADDR_WRITE      0x12
#define TLKDRV_SH8610Z_CMD_ADDR_QUAD_WRITE      0x32


#define TLKDRV_SH8610Z_WIDTH          480
#define TLKDRV_SH8610Z_HEIGHT         480

#define TLKDRV_SH8610Z_RGB565         1
#define TLKDRV_SH8610Z_RGB666         2
#define TLKDRV_SH8610Z_RGB888         3
#define TLKDRV_SH8610Z_RGB_SELECT     TLKDRV_SH8610Z_RGB565

#define TLKDRV_SH8610Z_565_RED        0x00F8
#define TLKDRV_SH8610Z_565_GREEN      0xE007
#define TLKDRV_SH8610Z_565_BLUE       0x1F00
#define TLKDRV_SH8610Z_565_WHITE      0xFFFF
#define TLKDRV_SH8610Z_565_BLACK      0x0000

#define TLKDRV_SH8610Z_888_RED        0x0000FF
#define TLKDRV_SH8610Z_888_GREEN      0x00FF00
#define TLKDRV_SH8610Z_888_BLUE       0xFF0000
#define TLKDRV_SH8610Z_888_WHITE      0xFFFFFF
#define TLKDRV_SH8610Z_888_BLACK      0x000000

#define TLKDRV_SH8610Z_666_RED        0x0000FC
#define TLKDRV_SH8610Z_666_GREEN      0x00FC00
#define TLKDRV_SH8610Z_666_BLUE       0xFC0000
#define TLKDRV_SH8610Z_666_WHITE      0xFCFCFC
#define TLKDRV_SH8610Z_666_BLACK      0x000000

#define TLKDRV_SH8610Z_DATA_CFG_NUMB       6


typedef struct{
	uint08 isInit;
	uint08 isOpen;
	uint32 colorTable[16];

	uint08 brushSize;
	uint08 brushAlpha;
	uint32 brushColor;
}tlkdrv_sh8610z_t;

typedef struct{
	uint08 dataLen;
	uint32 oled_addr;
	uint08 oled_data[4];
}tlkdrv_sh8610z_config_t;


static bool tlkdrv_sh8610z_isOpen(void);
static int tlkdrv_sh8610z_init(void);
static int tlkdrv_sh8610z_open(void);
static int tlkdrv_sh8610z_close(void);
static int tlkdrv_sh8610z_reset(void);
static int tlkdrv_sh8610z_flush(void);
static int tlkdrv_sh8610z_clean(void);
static int tlkdrv_sh8610z_brush(tlkdrv_lcd_brush_t *pBrush);
static int tlkdrv_sh8610z_point(uint16 x, uint16 y);
static int tlkdrv_sh8610z_paint(uint16 opcode, void *pParam, uint16 paramLen);
static int tlkdrv_sh8610z_image(tlkdrv_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen);
static int tlkdrv_sh8610z_handler(uint16 opcode, uint32 param0, uint32 param1);

static int tlkdrv_sh8610z_getColorDeal(uint32 param0, uint32 param1);
static int tlkdrv_sh8610z_getWidthDeal(uint32 param0, uint32 param1);
static int tlkdrv_sh8610z_getHeightDeal(uint32 param0, uint32 param1);
static int tlkdrv_sh8610z_setForegroundDeal(uint32 param0, uint32 param1);
static int tlkdrv_sh8610z_setBackgroundDeal(uint32 param0, uint32 param1);

static int tlkdrv_sh8610z_paintFillRectDeal(void *pParam, uint16 paramLen);

static void tlkdrv_sh8610z_spiWriteCmd(uint32 addr);
static void tlkdrv_sh8610z_spiWriteData(uint32 addr, uint08 *pData, uint32 dataLen);
static void tlkdrv_sh8610z_setShowArea(uint16 x, uint16 y, uint16 w, uint16 h);
static void tlkdrv_sh8610z_showFillRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 color);
static void tlkdrv_sh8610z_showImage(uint16 x, uint16 y, uint16 w, uint16 h, uint08 *pData, uint32 dataLen);



#endif //#if (TLKDRV_LCD_SH8610Z_ENABLE)

#endif //TLKDRV_SH8610Z_H



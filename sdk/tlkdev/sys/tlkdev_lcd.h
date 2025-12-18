/********************************************************************************************************
 * @file	tlkdev_lcd.h
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
#ifndef TLKDEV_LCD_H
#define TLKDEV_LCD_H

#if (TLK_DEV_LCD_ENABLE)


typedef struct{
	uint08 r;
	uint08 g;
	uint08 b;
}tlkdev_lcd_color_t;
typedef struct{
	uint08 r;
	uint08 g;
	uint08 b;
	uint08 a;
	uint08 w; //The width of the brush
}tlkdev_lcd_brush_t;
typedef struct{
	uint16 x;
	uint16 y;
	uint16 w;
	uint16 h;
}tlkdev_lcd_region_t;



bool tlkdev_lcd_isOpen(void);

int tlkdev_lcd_init(void);
int tlkdev_lcd_open(void);
int tlkdev_lcd_close(void);
int tlkdev_lcd_reset(void);
int tlkdev_lcd_flush(void);
int tlkdev_lcd_clean(void);

int tlkdev_lcd_getWidth(void);
int tlkdev_lcd_getHeight(void);
int tlkdev_lcd_setBrush(tlkdev_lcd_brush_t *pBrush);
int tlkdev_lcd_setForeground(uint08 *pBuffer, uint32 buffLen);
int tlkdev_lcd_setBackground(uint08 *pBuffer, uint32 buffLen);

int tlkdev_lcd_paintPoint(uint16 x, uint16 y, bool isFlush);
int tlkdev_lcd_paintImage(tlkdev_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen, bool isFlush);
int tlkdev_lcd_paintLine(uint16 x0, uint16 y0, uint16 x1, uint16 y1, bool isFlush);
int tlkdev_lcd_paintHLine(uint16 x, uint16 y, uint16 w, bool isFlush);
int tlkdev_lcd_paintVLine(uint16 x, uint16 y, uint16 h, bool isFlush);
int tlkdev_lcd_paintArc(uint16 x0, uint16 y0, uint16 r, uint16 sAngle, uint16 eAngle, bool isFlush);
int tlkdev_lcd_paintRing(uint16 x0, uint16 y0, uint16 sr, uint16 er, bool isFlush);
int tlkdev_lcd_paintRect(uint16 x, uint16 y, uint16 w, uint16 h, bool isFlush);
int tlkdev_lcd_paintRoundedRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 rounded, bool isFlush);
int tlkdev_lcd_paintChamferRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 chamfer, bool isFlush);
int tlkdev_lcd_paintEllipse(uint16 x0, uint16 y0, uint16 a, uint16 b, bool isFlush);
int tlkdev_lcd_paintCircle(uint16 x0, uint16 y0, uint16 r, bool isFlush);
int tlkdev_lcd_paintSector(uint16 x0, uint16 y0, uint16 r, uint16 sAngle, uint16 eAngle, bool isFlush);
int tlkdev_lcd_paintFillRing(uint16 x0, uint16 y0, uint16 sr, uint16 er, bool isFlush);
int tlkdev_lcd_paintFillRect(uint16 x, uint16 y, uint16 w, uint16 h, bool isFlush);
int tlkdev_lcd_paintFillRoundedRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 rounded, bool isFlush);
int tlkdev_lcd_paintFillChamferRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 chamfer, bool isFlush);
int tlkdev_lcd_paintFillEllipse(uint16 x0, uint16 y0, uint16 a, uint16 b, bool isFlush);
int tlkdev_lcd_paintFillCircle(uint16 x0, uint16 y0, uint16 r, bool isFlush);
int tlkdev_lcd_paintFillSector(uint16 x0, uint16 y0, uint16 r, uint16 sAngle, uint16 eAngle, bool isFlush);



#endif //TLK_DEV_LCD_ENABLE

#endif //TLKDEV_LCD_H


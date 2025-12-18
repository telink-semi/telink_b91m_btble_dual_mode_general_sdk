/********************************************************************************************************
 * @file	tlkdrv_lcd.h
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
#ifndef TLKDRV_LCD_H
#define TLKDRV_LCD_H


#define TLKDRV_LCD_ST77903_ENABLE      0
#define TLKDRV_LCD_SH8610Z_ENABLE      0



typedef enum{
	TLKDRV_LCD_DEV_ST77903 = 0,
	TLKDRV_LCD_DEV_SH8610Z,
	TLKDRV_LCD_DEV_MAX,
}TLKDRV_LCD_DEV_ENUM;
typedef enum{
	TLKDRV_LCD_OPCODE_NONE = 0,
	TLKDRV_LCD_OPCODE_GET_COLOR,
	TLKDRV_LCD_OPCODE_GET_WIDTH,
	TLKDRV_LCD_OPCODE_GET_HEIGHT,
	TLKDRV_LCD_OPCODE_SET_FOREGROUND,
	TLKDRV_LCD_OPCODE_SET_BACKGROUND,
}TLKDRV_LCD_OPCODE_ENUM;
typedef enum{
	TLKDRV_LCD_PAINT_NONE = 0,
	TLKDRV_LCD_PAINT_LINE,
	TLKDRV_LCD_PAINT_H_LINE, //horizontal line
	TLKDRV_LCD_PAINT_V_LINE, //vertical line
	//Null Graph
	TLKDRV_LCD_PAINT_RING,
	TLKDRV_LCD_PAINT_RECT, //Rectangle
	TLKDRV_LCD_PAINT_ROUNDED_RECT, //Rounded Rectangle
	TLKDRV_LCD_PAINT_CHAMFER_RECT, //Edges and corners rectangular
	TLKDRV_LCD_PAINT_ELLIPSE,
	TLKDRV_LCD_PAINT_CIRCLE,
	TLKDRV_LCD_PAINT_SECTOR,
	//Fill Graph
	TLKDRV_LCD_PAINT_FILL_RING,
	TLKDRV_LCD_PAINT_FILL_RECT,
	TLKDRV_LCD_PAINT_FILL_ROUNDED_RECT,
	TLKDRV_LCD_PAINT_FILL_CHAMFER_RECT,
	TLKDRV_LCD_PAINT_FILL_ELLIPSE,
	TLKDRV_LCD_PAINT_FILL_CIRCLE,
	TLKDRV_LCD_PAINT_FILL_SECTOR,
}TLKDRV_LCD_PAINT_ENUM;

typedef enum{
	TLKDRV_LCD_COLOR_565 = 565,
	TLKDRV_LCD_COLOR_666 = 666,
	TLKDRV_LCD_COLOR_888 = 888,
}TLKDRV_LCD_COLOR_ENUM;



typedef struct{
	uint08 r;
	uint08 g;
	uint08 b;
}tlkdrv_lcd_color_t;
typedef struct{
	uint08 r;
	uint08 g;
	uint08 b;
	uint08 a;
	uint08 w; //The width of the brush
}tlkdrv_lcd_brush_t;

typedef struct{
	uint16 x;
	uint16 y;
}tlkdrv_lcd_point_t;
typedef struct{
	uint16 x;
	uint16 y;
	uint16 w;
	uint16 h;
}tlkdrv_lcd_region_t;


typedef struct{
	uint16 x1;
	uint16 y1; 
	uint16 x2;
	uint16 y2;
}tlkdrv_lcd_line_t;
typedef struct{
	uint16 x;
	uint16 y; 
	uint16 w;
}tlkdrv_lcd_hline_t;
typedef struct{
	uint16 x;
	uint16 y; 
	uint16 h;
}tlkdrv_lcd_vline_t;
typedef struct{
	uint16 x0;
	uint16 y0; 
	uint16 r;
	uint16 sAngle;
	uint16 eAngle;
}tlkdrv_lcd_arc_t;
typedef struct{
	uint16 x0;
	uint16 y0; 
	uint16 sr;
	uint16 er;
}tlkdrv_lcd_ring_t;
typedef struct{
	uint16 x;
	uint16 y; 
	uint16 w;
	uint16 h;
}tlkdrv_lcd_rect_t;
typedef struct{
	uint16 x;
	uint16 y; 
	uint16 w;
	uint16 h;
	uint16 rounded;
}tlkdrv_lcd_roundedRect_t;
typedef struct{
	uint16 x;
	uint16 y; 
	uint16 w;
	uint16 h;
	uint16 chamfer;
}tlkdrv_lcd_chamferRect_t;
typedef struct{
	uint16 x0;
	uint16 y0; 
	uint16 a;
	uint16 b;
}tlkdrv_lcd_ellipse_t;
typedef struct{
	uint16 x0;
	uint16 y0; 
	uint16 r;
}tlkdrv_lcd_circle_t;
typedef struct{
	uint16 x0;
	uint16 y0; 
	uint16 r;
	uint16 sAngle;
	uint16 eAngle;
}tlkdrv_lcd_sector_t;


typedef int(*TlkdrvLcdPointFunc)(uint16 x, uint16 y);


typedef struct{
	bool(*IsOpen)(void);
	int(*Init)(void);
	int(*Open)(void);
	int(*Close)(void);
	int(*Reset)(void);
	int(*Flush)(void);
	int(*Clean)(void);
	int(*Brush)(tlkdrv_lcd_brush_t *pBrush);
	int(*Point)(uint16 x, uint16 y);
	int(*Paint)(uint16 opcode, void *pParam, uint16 paramLen);
	int(*Image)(tlkdrv_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
}tlkdrv_lcd_modinf_t;

//alpha==100, operate background
bool tlkdrv_lcd_isOpen(TLKDRV_LCD_DEV_ENUM dev);

int tlkdrv_lcd_init(TLKDRV_LCD_DEV_ENUM dev);
int tlkdrv_lcd_open(TLKDRV_LCD_DEV_ENUM dev);
int tlkdrv_lcd_close(TLKDRV_LCD_DEV_ENUM dev);
int tlkdrv_lcd_reset(TLKDRV_LCD_DEV_ENUM dev);
int tlkdrv_lcd_flush(TLKDRV_LCD_DEV_ENUM dev);
int tlkdrv_lcd_clean(TLKDRV_LCD_DEV_ENUM dev);
int tlkdrv_lcd_brush(TLKDRV_LCD_DEV_ENUM dev, tlkdrv_lcd_brush_t *pBrush);
int tlkdrv_lcd_point(TLKDRV_LCD_DEV_ENUM dev, uint16 x, uint16 y);
int tlkdrv_lcd_paint(TLKDRV_LCD_DEV_ENUM dev, uint16 opcode, void *pParam, uint16 paramLen);
int tlkdrv_lcd_image(TLKDRV_LCD_DEV_ENUM dev, tlkdrv_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen);
int tlkdrv_lcd_handler(TLKDRV_LCD_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);

int tlkdrv_lcd_getWidth(TLKDRV_LCD_DEV_ENUM dev);
int tlkdrv_lcd_getHeight(TLKDRV_LCD_DEV_ENUM dev);

int tlkdrv_lcd_paintLine(tlkdrv_lcd_line_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintHLine(tlkdrv_lcd_hline_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintVLine(tlkdrv_lcd_vline_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintArc(tlkdrv_lcd_arc_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintRing(tlkdrv_lcd_ring_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintRect(tlkdrv_lcd_rect_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintRoundedRect(tlkdrv_lcd_roundedRect_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintChamferRect(tlkdrv_lcd_chamferRect_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintEllipse(tlkdrv_lcd_ellipse_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintCircle(tlkdrv_lcd_circle_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintSector(tlkdrv_lcd_sector_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintFillRing(tlkdrv_lcd_ring_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintFillRect(tlkdrv_lcd_rect_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintFillRoundedRect(tlkdrv_lcd_roundedRect_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintFillChamferRect(tlkdrv_lcd_chamferRect_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintFillEllipse(tlkdrv_lcd_ellipse_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintFillCircle(tlkdrv_lcd_circle_t *pParam, TlkdrvLcdPointFunc paint);
int tlkdrv_lcd_paintFillSector(tlkdrv_lcd_sector_t *pParam, TlkdrvLcdPointFunc paint);




//static void tlkdrv_lcd_paintEllipsePoints(int x0, int y0, int x, int y, TlkdrvLcdPointFunc paint);




#endif //TLKDRV_LCD_H


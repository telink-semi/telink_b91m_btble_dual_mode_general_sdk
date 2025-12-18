/********************************************************************************************************
 * @file	tlkdev_lcd.c
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
#if (TLK_DEV_LCD_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlkdev/sys/tlkdev_lcd.h"
#include "tlkdrv/ext/lcd/tlkdrv_lcd.h"



#define TLKDEV_LCD_DEV        TLKDRV_LCD_DEV_SH8610Z

static int tlkdev_lcd_paintPoint1(uint16 x, uint16 y);


bool tlkdev_lcd_isOpen(void)
{
	return tlkdrv_lcd_isOpen(TLKDEV_LCD_DEV);
}

int tlkdev_lcd_init(void)
{
	int ret;
	tlkdrv_lcd_brush_t brush;

	brush.r = 0x00;
	brush.g = 0xFF;
	brush.b = 0x00;
	brush.a = 0;
	brush.w = 3;
	ret = tlkdrv_lcd_init(TLKDEV_LCD_DEV);
	if(ret == TLK_ENONE) ret = tlkdrv_lcd_brush(TLKDEV_LCD_DEV, &brush);
		
	return ret;
}
int tlkdev_lcd_open(void)
{
	return tlkdrv_lcd_open(TLKDEV_LCD_DEV);
}
int tlkdev_lcd_close(void)
{
	return tlkdrv_lcd_close(TLKDEV_LCD_DEV);
}
int tlkdev_lcd_reset(void)
{
	return tlkdrv_lcd_reset(TLKDEV_LCD_DEV);
}
int tlkdev_lcd_flush(void)
{
	return tlkdrv_lcd_flush(TLKDEV_LCD_DEV);
}
int tlkdev_lcd_clean(void)
{
	return tlkdrv_lcd_clean(TLKDEV_LCD_DEV);
}

int tlkdev_lcd_getWidth(void)
{
	return tlkdrv_lcd_handler(TLKDEV_LCD_DEV, TLKDRV_LCD_OPCODE_GET_WIDTH, 0, 0);
}
int tlkdev_lcd_getHeight(void)
{
	return tlkdrv_lcd_handler(TLKDEV_LCD_DEV, TLKDRV_LCD_OPCODE_GET_HEIGHT, 0, 0);
}
int tlkdev_lcd_setBrush(tlkdev_lcd_brush_t *pBrush)
{
	return tlkdrv_lcd_brush(TLKDEV_LCD_DEV, (tlkdrv_lcd_brush_t*)pBrush);
}
int tlkdev_lcd_setForeground(uint08 *pBuffer, uint32 buffLen)
{
	return tlkdrv_lcd_handler(TLKDEV_LCD_DEV, TLKDRV_LCD_OPCODE_SET_FOREGROUND, (uint32)pBuffer, buffLen);
}
int tlkdev_lcd_setBackground(uint08 *pBuffer, uint32 buffLen)
{
	return tlkdrv_lcd_handler(TLKDEV_LCD_DEV, TLKDRV_LCD_OPCODE_SET_BACKGROUND, (uint32)pBuffer, buffLen);
}
int tlkdev_lcd_paintPoint(uint16 x, uint16 y, bool isFlush)
{
	int ret = tlkdrv_lcd_point(TLKDEV_LCD_DEV, x, y);
	if(ret == TLK_ENONE && isFlush) tlkdrv_lcd_flush(TLKDEV_LCD_DEV);
	return ret;
}
int tlkdev_lcd_paintImage(tlkdev_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen, bool isFlush)
{
	int ret = tlkdrv_lcd_image(TLKDEV_LCD_DEV, (tlkdrv_lcd_region_t*)pRegion, pData, dataLen);
	if(ret == TLK_ENONE && isFlush) tlkdrv_lcd_flush(TLKDEV_LCD_DEV);
	return ret;
}
int tlkdev_lcd_paintLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2, bool isFlush)
{
	int ret;
	tlkdrv_lcd_line_t param;
	param.x1 = x1;
	param.y1 = y1;
	param.x2 = x2;
	param.y2 = y2;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_LINE, &param, sizeof(tlkdrv_lcd_line_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintLine(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintHLine(uint16 x, uint16 y, uint16 w, bool isFlush)
{
	int ret;
	tlkdrv_lcd_hline_t param;
	param.x = x;
	param.y = y;
	param.w = w;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_H_LINE, &param, sizeof(tlkdrv_lcd_hline_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintHLine(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintArc(uint16 x0, uint16 y0, uint16 r, uint16 sAngle, uint16 eAngle, bool isFlush)
{
	int ret;
	tlkdrv_lcd_arc_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.r = r;
	param.sAngle = sAngle;
	param.eAngle = eAngle;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_SECTOR, &param, sizeof(tlkdrv_lcd_sector_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintArc(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintVLine(uint16 x, uint16 y, uint16 h, bool isFlush)
{
	int ret;
	tlkdrv_lcd_vline_t param;
	param.x = x;
	param.y = y;
	param.h = h;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_V_LINE, &param, sizeof(tlkdrv_lcd_vline_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintVLine(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintRing(uint16 x0, uint16 y0, uint16 sr, uint16 er, bool isFlush)
{
	int ret;
	tlkdrv_lcd_ring_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.sr = sr;
	param.er = er;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_RING, &param, sizeof(tlkdrv_lcd_ring_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintRing(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintRect(uint16 x, uint16 y, uint16 w, uint16 h, bool isFlush)
{
	int ret;
	tlkdrv_lcd_rect_t param;
	param.x = x;
	param.y = y;
	param.w = w;
	param.h = h;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_RECT, &param, sizeof(tlkdrv_lcd_rect_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintRect(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintRoundedRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 rounded, bool isFlush)
{
	int ret;
	tlkdrv_lcd_roundedRect_t param;
	param.x = x;
	param.y = y;
	param.w = w;
	param.h = h;
	param.rounded = rounded;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_ROUNDED_RECT, &param, sizeof(tlkdrv_lcd_roundedRect_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintRoundedRect(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintChamferRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 chamfer, bool isFlush)
{
	int ret;
	tlkdrv_lcd_chamferRect_t param;
	param.x = x;
	param.y = y;
	param.w = w;
	param.h = h;
	param.chamfer = chamfer;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_CHAMFER_RECT, &param, sizeof(tlkdrv_lcd_chamferRect_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintChamferRect(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintEllipse(uint16 x0, uint16 y0, uint16 a, uint16 b, bool isFlush)
{
	int ret;
	tlkdrv_lcd_ellipse_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.a = a;
	param.b = b;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_ELLIPSE, &param, sizeof(tlkdrv_lcd_ellipse_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintEllipse(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintCircle(uint16 x0, uint16 y0, uint16 r, bool isFlush)
{
	int ret;
	tlkdrv_lcd_circle_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.r = r;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_CIRCLE, &param, sizeof(tlkdrv_lcd_circle_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintCircle(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintSector(uint16 x0, uint16 y0, uint16 r, uint16 sAngle, uint16 eAngle, bool isFlush)
{
	int ret;
	tlkdrv_lcd_sector_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.r = r;
	param.sAngle = sAngle;
	param.eAngle = eAngle;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_SECTOR, &param, sizeof(tlkdrv_lcd_sector_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintSector(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}


int tlkdev_lcd_paintFillRing(uint16 x0, uint16 y0, uint16 sr, uint16 er, bool isFlush)
{
	int ret;
	tlkdrv_lcd_ring_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.sr = sr;
	param.er = er;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_FILL_RING, &param, sizeof(tlkdrv_lcd_ring_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintFillRing(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintFillRect(uint16 x, uint16 y, uint16 w, uint16 h, bool isFlush)
{
	int ret;
	tlkdrv_lcd_rect_t param;
	param.x = x;
	param.y = y;
	param.w = w;
	param.h = h;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_FILL_RECT, &param, sizeof(tlkdrv_lcd_rect_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintFillRect(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintFillRoundedRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 rounded, bool isFlush)
{
	int ret;
	tlkdrv_lcd_roundedRect_t param;
	param.x = x;
	param.y = y;
	param.w = w;
	param.h = h;
	param.rounded = rounded;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_FILL_ROUNDED_RECT, &param, sizeof(tlkdrv_lcd_roundedRect_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintFillRoundedRect(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintFillChamferRect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 chamfer, bool isFlush)
{
	int ret;
	tlkdrv_lcd_chamferRect_t param;
	param.x = x;
	param.y = y;
	param.w = w;
	param.h = h;
	param.chamfer = chamfer;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_FILL_CHAMFER_RECT, &param, sizeof(tlkdrv_lcd_chamferRect_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintFillChamferRect(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintFillEllipse(uint16 x0, uint16 y0, uint16 a, uint16 b, bool isFlush)
{
	int ret;
	tlkdrv_lcd_ellipse_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.a = a;
	param.b = b;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_FILL_ELLIPSE, &param, sizeof(tlkdrv_lcd_ellipse_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintFillEllipse(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintFillCircle(uint16 x0, uint16 y0, uint16 r, bool isFlush)
{
	int ret;
	tlkdrv_lcd_circle_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.r = r;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_FILL_CIRCLE, &param, sizeof(tlkdrv_lcd_circle_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintFillCircle(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}
int tlkdev_lcd_paintFillSector(uint16 x0, uint16 y0, uint16 r, uint16 sAngle, uint16 eAngle, bool isFlush)
{
	int ret;
	tlkdrv_lcd_sector_t param;
	param.x0 = x0;
	param.y0 = y0;
	param.r = r;
	param.sAngle = sAngle;
	param.eAngle = eAngle;
	ret = tlkdrv_lcd_paint(TLKDEV_LCD_DEV, TLKDRV_LCD_PAINT_FILL_SECTOR, &param, sizeof(tlkdrv_lcd_sector_t));
	if(ret != TLK_ENONE) ret = tlkdrv_lcd_paintFillSector(&param, tlkdev_lcd_paintPoint1);
	if(ret == TLK_ENONE && isFlush) tlkdev_lcd_flush();
	return ret;
}


static int tlkdev_lcd_paintPoint1(uint16 x, uint16 y)
{
	return tlkdrv_lcd_point(TLKDEV_LCD_DEV, x, y);
}





#if (0)
static void tlkapp_debug_usbHandler(uint08 *pData, uint16 dataLen)
{
	tlkapi_array(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkapp_debug_usbHandler receive", pData, dataLen);

	if(pData[0] != 0x11) return;

	switch(pData[1])
	{
		case 0x00:
			tlkdev_lcd_open();
			break;
		case 0x01:
			tlkdev_lcd_clean();
			break;
		case 0x02:
			tlkdev_lcd_paintLine(10, 10, 100, 100, true);
			break;
		case 0x03:
			tlkdev_lcd_paintHLine(10, 10, 100, true);
			break;
		case 0x04:
			tlkdev_lcd_paintVLine(10, 10, 100, true);
			break;
		case 0x05:
			tlkdev_lcd_paintRect(50, 50, 100, 100, true);
			break;
		case 0x06:
			tlkdev_lcd_paintFillRect(80, 80, 100, 100, true);
			break;
		case 0x07:
			tlkdev_lcd_paintCircle(160, 160, 155, true);
			break;
		case 0x08:
			tlkdev_lcd_paintFillCircle(160, 160, 140, true);
			break;
		case 0x10:
			tlkdev_lcd_paintEllipse(160, 160, 120, 155, true);
			break;
		case 0x11:
			tlkdev_lcd_paintFillEllipse(160, 160, 100, 140, true);
			break;
		case 0x12:
			tlkdev_lcd_paintRing(160, 160, 140, 120, true);
			break;
		case 0x13:
			tlkdev_lcd_paintFillRing(160, 160, 100, 80, true);
			break;
		case 0x14:
			tlkdev_lcd_paintRoundedRect(60, 60, 200, 200, 20, true);
			break;
		case 0x15:
			tlkdev_lcd_paintFillRoundedRect(80, 80, 160, 160, 20, true);
			break;
		case 0x16:
			tlkdev_lcd_paintFillRoundedRect(80, 80, 160, 200, 80, true);
			break;
		case 0x17:
			tlkdev_lcd_paintFillRoundedRect(80, 80, 200, 160, 80, true);
			break;
		case 0x20:
			tlkdev_lcd_paintChamferRect(60, 60, 200, 200, 20, true);
			break;
		case 0x21:
			tlkdev_lcd_paintFillChamferRect(80, 80, 160, 140, 30, true);
			break;
		case 0x22:
			tlkdev_lcd_paintFillChamferRect(80, 80, 140, 160, 30, true);
			break;
		case 0x30:
			tlkdev_lcd_open();
			tlkdev_lcd_clean();
			break;
		case 0x31:
			tlkdev_lcd_paintSector(160, 160, 130, 30, 80, true);
			break;
		case 0x32:
			tlkdev_lcd_paintFillSector(160, 160, 130, 30, 80, true);
			break;
		
		case 0xf2:
			#if (TLK_DEV_STORE_ENABLE)
			tlkdev_store_format();
			tlkapi_trace(TLKAPP_DBG_FLAG, TLKAPP_DBG_SIGN, "tlkdev_store_format");
			#endif
			break;
		break;
	    case 0xfe:
	    {
	    	tlkapp_debug_cmdGetPhoneBookHandler(pData+2,7);
	    }
	        break;

		case 0xFF:
		{

		}
			break;

	    default:
	        break;
	}

}
#endif


#endif //#if (TLK_DEV_LCD_ENABLE)


/********************************************************************************************************
 * @file	tlkdrv_lcd.c
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
#include "stdlib.h"


static void tlkdrv_lcd_drawQuadrantPoint(uint16 x0, uint16 y0, uint16 x, uint16 y, TlkdrvLcdPointFunc paint);
static void tlkdrv_lcd_calcXYAxisByAngle(uint16 x0, uint16 y0, uint16 r, uint16 angle, uint16 *pX, uint16 *pY);
static bool tlkdrv_lcd_pointInCircleIsFront(uint16 x0, uint16 y0, uint16 x, uint16 y, uint16 rx, uint16 ry);

static const tlkdrv_lcd_modinf_t *tlkdrv_lcd_getDev(TLKDRV_LCD_DEV_ENUM dev);

extern const tlkdrv_lcd_modinf_t gcTlkDrvSt77903Infs;
extern const tlkdrv_lcd_modinf_t gcTlkDrvSh8610zInfs;
static const tlkdrv_lcd_modinf_t *spTlkDrvLcdModInfs[TLKDRV_LCD_DEV_MAX] = {
	#if (TLKDRV_LCD_ST77903_ENABLE)
	&gcTlkDrvSt77903Infs,
	#else
	nullptr,
	#endif
	#if (TLKDRV_LCD_SH8610Z_ENABLE)
	&gcTlkDrvSh8610zInfs,
	#else
	nullptr,
	#endif
};

static uint08 sTlkDrvLcdBrushSize = 1;

bool tlkdrv_lcd_isOpen(TLKDRV_LCD_DEV_ENUM dev)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsOpen();
}

int tlkdrv_lcd_init(TLKDRV_LCD_DEV_ENUM dev)
{
	int ret;
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	ret = pModInf->Init();
	if(ret == TLK_ENONE){
		sTlkDrvLcdBrushSize = 1;
	}
	return ret;
}
int tlkdrv_lcd_open(TLKDRV_LCD_DEV_ENUM dev)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Open();
}
int tlkdrv_lcd_close(TLKDRV_LCD_DEV_ENUM dev)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close();
}
int tlkdrv_lcd_reset(TLKDRV_LCD_DEV_ENUM dev)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Reset == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Reset();
}
int tlkdrv_lcd_flush(TLKDRV_LCD_DEV_ENUM dev)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Flush == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Flush();
}
int tlkdrv_lcd_clean(TLKDRV_LCD_DEV_ENUM dev)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Clean == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Clean();
}
int tlkdrv_lcd_width(TLKDRV_LCD_DEV_ENUM dev)
{
	return tlkdrv_lcd_handler(dev, TLKDRV_LCD_OPCODE_GET_WIDTH, 0, 0);
}
int tlkdrv_lcd_height(TLKDRV_LCD_DEV_ENUM dev)
{
	return tlkdrv_lcd_handler(dev, TLKDRV_LCD_OPCODE_GET_HEIGHT, 0, 0);
}
int tlkdrv_lcd_brush(TLKDRV_LCD_DEV_ENUM dev, tlkdrv_lcd_brush_t *pBrush)
{
	int ret;
	const tlkdrv_lcd_modinf_t *pModInf;

	if(pBrush == nullptr || pBrush->w == 0) return -TLK_EPARAM;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Brush == nullptr) return -TLK_ENOSUPPORT;
	ret = pModInf->Brush(pBrush);
	if(ret == TLK_ENONE) sTlkDrvLcdBrushSize = pBrush->w;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkdrv_lcd_point
 * Descript: This function serves to draw a n*n paint in ramless lcd with single color.
 * Params: 
 *     @dev[IN]--;
 *     @x[IN]--abscissa of address;
 *     @y[IN]--ordinate of address;
 * Return: TLK_ENONE is success,other value is false .
*******************************************************************************/
int tlkdrv_lcd_point(TLKDRV_LCD_DEV_ENUM dev, uint16 x, uint16 y)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Point == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Point(x, y);
}
int tlkdrv_lcd_paint(TLKDRV_LCD_DEV_ENUM dev, uint16 opcode, void *pParam, uint16 paramLen)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Paint == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Paint(opcode, pParam, paramLen);
}
int tlkdrv_lcd_image(TLKDRV_LCD_DEV_ENUM dev, tlkdrv_lcd_region_t *pRegion, uint08 *pData, uint32 dataLen)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Image == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Image(pRegion, pData, dataLen);
}
int tlkdrv_lcd_handler(TLKDRV_LCD_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_lcd_modinf_t *pModInf;
	pModInf = tlkdrv_lcd_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(opcode, param0, param1);
}

int tlkdrv_lcd_paintLine(tlkdrv_lcd_line_t *pParam, TlkdrvLcdPointFunc paint)
{
	if(pParam == nullptr || paint == nullptr) return -TLK_EPARAM;

    int dx = abs(pParam->x2 - pParam->x1);
    int dy = abs(pParam->y2 - pParam->y1);
    int yy = 0;
 	
    if(dx < dy){
		int temp;
        yy = 1;
		temp = pParam->x1;
		pParam->x1 = pParam->y1;
		pParam->y1 = temp;
        temp = pParam->x2;
		pParam->x2 = pParam->y2;
		pParam->y2 = temp;
		temp = dx;
		dx = dy;
		dy = temp;
    }
 
    int ix = (pParam->x2 - pParam->x1) > 0 ? 1 : -1;
    int iy = (pParam->y2 - pParam->y1) > 0 ? 1 : -1;
    int cx = pParam->x1;
    int cy = pParam->y1;
    int n2dy = dy * 2;
    int n2dydx = (dy - dx) * 2;
    int d = dy * 2 - dx;
 	
    if(yy){ //The Angle between the line and the X-axis is greater than 45 degrees
        while(cx != pParam->x2){
            if(d < 0){
                d += n2dy;
            } else {
                cy += iy;
                d += n2dydx;
            }
            paint(cy, cx);
            cx += ix;
        }
    }else{ //The Angle between the line and the X-axis is less than 45 degrees
        while(cx != pParam->x2){
            if(d < 0){
                d += n2dy;
            }else{
                cy += iy;
                d += n2dydx;
            }
            paint(cx, cy);
            cx += ix;
        }
    }
		
	return TLK_ENONE;
}
int tlkdrv_lcd_paintHLine(tlkdrv_lcd_hline_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint16 x;
	if(pParam == nullptr || paint == nullptr/* || pParam->w < sTlkDrvLcdBrushSize*/){
		return -TLK_EPARAM;
	}
	for(x=0; x <= pParam->w - sTlkDrvLcdBrushSize; x += sTlkDrvLcdBrushSize){
		paint(pParam->x+x, pParam->y);
	}
	return TLK_ENONE;
}
int tlkdrv_lcd_paintVLine(tlkdrv_lcd_vline_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint16 y;
	if(pParam == nullptr || paint == nullptr/* || pParam->h < sTlkDrvLcdBrushSize*/){
		return -TLK_EPARAM;
	}
	for(y=0; y <= pParam->h - sTlkDrvLcdBrushSize; y += sTlkDrvLcdBrushSize){
		paint(pParam->x, pParam->y+y);
	}
	return TLK_ENONE;
}
int tlkdrv_lcd_paintArc(tlkdrv_lcd_arc_t *pParam, TlkdrvLcdPointFunc paint)
{
	int d;
	uint16 x;
	uint16 y;
	uint16 sx;
	uint16 sy;
	uint16 ex;
	uint16 ey;
	
	if(pParam == nullptr || paint == nullptr || pParam->r < 1 || pParam->sAngle >= pParam->eAngle
		|| pParam->sAngle >= 360 || pParam->eAngle >= 720){
		return -TLK_EPARAM;
	}
	if(pParam->eAngle - pParam->sAngle >= 360){
		tlkdrv_lcd_circle_t circle;
		circle.x0 = pParam->x0;
		circle.y0 = pParam->y0;
		circle.r = pParam->r;
		return tlkdrv_lcd_paintCircle(&circle, paint);
	}
	if(pParam->eAngle >= 360) pParam->eAngle -= 360;
	
	tlkdrv_lcd_calcXYAxisByAngle(pParam->x0, pParam->y0, pParam->r, pParam->sAngle, &sx, &sy);
	tlkdrv_lcd_calcXYAxisByAngle(pParam->x0, pParam->y0, pParam->r, pParam->eAngle, &ex, &ey);
	
	x = 0;
	y = pParam->r;
	d = 3 - 2 * pParam->r;
	while(x <= y){
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 + y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 + y, sx, sy)){
			paint(pParam->x0 + x, pParam->y0 + y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 + x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 + x, sx, sy)){
			paint(pParam->x0 + y, pParam->y0 + x);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 - y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 - y, sx, sy)){
			paint(pParam->x0 + x, pParam->y0 - y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 + y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 + y, sx, sy)){
			paint(pParam->x0 - x, pParam->y0 + y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 - x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 - x, sx, sy)){
			paint(pParam->x0 + y, pParam->y0 - x);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 - y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 - y, sx, sy)){
			paint(pParam->x0 - x, pParam->y0 - y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 - x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 - x, sx, sy)){
			paint(pParam->x0 - y, pParam->y0 - x);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 + x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 + x, sx, sy)){
			paint(pParam->x0 - y, pParam->y0 + x);
		}
				
		if(d < 0){
			d += 4*x+6;
		}else{
			d += 4*(x-y)+10;
			y--;
		}
		x++;
	}
	
	return TLK_ENONE;	
}
int tlkdrv_lcd_paintRing(tlkdrv_lcd_ring_t *pParam, TlkdrvLcdPointFunc paint)
{
	int ret;
	tlkdrv_lcd_circle_t param;
	
	if(pParam == nullptr || paint == nullptr) return -TLK_EPARAM;
	
	param.x0 = pParam->x0;
	param.y0 = pParam->y0;
	param.r  = pParam->sr;
	ret = tlkdrv_lcd_paintCircle(&param, paint);
	param.x0 = pParam->x0;
	param.y0 = pParam->y0;
	param.r  = pParam->er;
	if(ret == TLK_ENONE) ret = tlkdrv_lcd_paintCircle(&param, paint);
	
	return ret;
}
int tlkdrv_lcd_paintRect(tlkdrv_lcd_rect_t *pParam, TlkdrvLcdPointFunc paint)
{
	tlkdrv_lcd_hline_t hline;
	tlkdrv_lcd_vline_t vline;
	
	if(pParam == nullptr || paint == nullptr || pParam->w == 0 || pParam->h == 0
		|| pParam->w < sTlkDrvLcdBrushSize || pParam->h < sTlkDrvLcdBrushSize){
		return -TLK_EPARAM;
	}
	
	hline.x = pParam->x;
	hline.y = pParam->y;
	hline.w = pParam->w-sTlkDrvLcdBrushSize;
	vline.x = pParam->x;
	vline.y = pParam->y;
	vline.h = pParam->h-sTlkDrvLcdBrushSize;
	tlkdrv_lcd_paintHLine(&hline, paint);
	tlkdrv_lcd_paintVLine(&vline, paint);
	hline.y += pParam->h-sTlkDrvLcdBrushSize;
	tlkdrv_lcd_paintHLine(&hline, paint);
	vline.x += pParam->w-sTlkDrvLcdBrushSize;
	tlkdrv_lcd_paintVLine(&vline, paint);
	
	return TLK_ENONE;
}
int tlkdrv_lcd_paintRoundedRect(tlkdrv_lcd_roundedRect_t *pParam, TlkdrvLcdPointFunc paint)
{
	int d;
	uint16 x;
	uint16 y;
	uint16 x1;
	uint16 y1;
	uint16 x2;
	uint16 y2;
	uint16 x3;
	uint16 y3;
	uint16 x4;
	uint16 y4;
	uint16 size;	
	
	if(pParam == nullptr || paint == nullptr || pParam->w == 0 || pParam->h == 0
		/*|| pParam->w < sTlkDrvLcdBrushSize || pParam->h < sTlkDrvLcdBrushSize*/){
		return -TLK_EPARAM;
	}

	size = sTlkDrvLcdBrushSize << 1;
	if(pParam->w <= size || pParam->h <= size || pParam->rounded == 0){
		tlkdrv_lcd_rect_t rect;
		rect.x = pParam->x;
		rect.y = pParam->y;
		rect.w = pParam->w;
		rect.h = pParam->h;
		return tlkdrv_lcd_paintRect(&rect, paint);
	}
	if(pParam->rounded > (pParam->w >> 1)) pParam->rounded = (pParam->w >> 1);
	if(pParam->rounded > (pParam->h >> 1)) pParam->rounded = (pParam->h >> 1);
	
	x1 = pParam->x + pParam->w - pParam->rounded;
	y1 = pParam->y + pParam->rounded;
	x2 = pParam->x + pParam->rounded;
	y2 = pParam->y + pParam->rounded;
	x3 = pParam->x + pParam->rounded;
	y3 = pParam->y + pParam->h - pParam->rounded;
	x4 = pParam->x + pParam->w - pParam->rounded;
	y4 = pParam->y + pParam->h - pParam->rounded;
	
	x = 0;
	y = pParam->rounded;
	d = 3 - 2 * pParam->rounded;
	while(x <= y){
		//First quadrant
		paint(x1 + x, y1 - y);
		paint(x1 + y, y1 - x);
		//second quadrant
		paint(x2 - x, y2 - y);
		paint(x2 - y, y2 - x);
		//third quadrant
		paint(x3 - x, y3 + y);
		paint(x3 - y, y3 + x);
		//fourth quadrant
		paint(x4 + x, y4 + y);
		paint(x4 + y, y4 + x);
		if(d < 0){
			d += 4*x + 6;
		}else{
			d += 4*(x-y)+10;
			y --;
		}
		x++;
	}

	if((pParam->rounded << 1) != pParam->w){
		tlkdrv_lcd_hline_t param;
		param.x = x2;
		param.y = pParam->y;
		param.w = x1 - x2;
		tlkdrv_lcd_paintHLine(&param, paint);
		param.y = pParam->y + pParam->h;
		tlkdrv_lcd_paintHLine(&param, paint);
	}
	if((pParam->rounded << 1) != pParam->h){
		tlkdrv_lcd_vline_t param;
		param.x = pParam->x;
		param.y = y2;
		param.h = y4 - y1;
		tlkdrv_lcd_paintVLine(&param, paint);
		param.x = pParam->x + pParam->w;
		tlkdrv_lcd_paintVLine(&param, paint);
	}
	
	return TLK_ENONE;
}
int tlkdrv_lcd_paintChamferRect(tlkdrv_lcd_chamferRect_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint16 size;
	tlkdrv_lcd_line_t line;
	
	if(pParam == nullptr || paint == nullptr || pParam->w == 0 || pParam->h == 0
		/*|| pParam->w < sTlkDrvLcdBrushSize || pParam->h < sTlkDrvLcdBrushSize*/){
		return -TLK_EPARAM;
	}

	size = sTlkDrvLcdBrushSize << 1;
	if(pParam->w <= size || pParam->h <= size || pParam->chamfer == 0){
		tlkdrv_lcd_rect_t rect;
		rect.x = pParam->x;
		rect.y = pParam->y;
		rect.w = pParam->w;
		rect.h = pParam->h;
		return tlkdrv_lcd_paintRect(&rect, paint);
	}
	if(pParam->chamfer > (pParam->w >> 1)) pParam->chamfer = (pParam->w >> 1);
	if(pParam->chamfer > (pParam->h >> 1)) pParam->chamfer = (pParam->h >> 1);
	//First quadrant
	line.x1 = pParam->x + pParam->w - pParam->chamfer;
	line.y1 = pParam->y;
	line.x2 = pParam->x + pParam->w;
	line.y2 = pParam->y + pParam->chamfer;
	tlkdrv_lcd_paintLine(&line, paint);
	//second quadrant
	line.x1 = pParam->x;
	line.y1 = pParam->y + pParam->chamfer;
	line.x2 = pParam->x + pParam->chamfer;
	line.y2 = pParam->y;
	tlkdrv_lcd_paintLine(&line, paint);
	//third quadrant
	line.x1 = pParam->x;
	line.y1 = pParam->y + pParam->h - pParam->chamfer;
	line.x2 = pParam->x + pParam->chamfer;
	line.y2 = pParam->y + pParam->h;
	tlkdrv_lcd_paintLine(&line, paint);
	//fourth quadrant
	line.x1 = pParam->x + pParam->w - pParam->chamfer;
	line.y1 = pParam->y + pParam->h;
	line.x2 = pParam->x + pParam->w;
	line.y2 = pParam->y + pParam->h - pParam->chamfer;
	tlkdrv_lcd_paintLine(&line, paint);
	
	if((pParam->chamfer << 1) != pParam->w){
		tlkdrv_lcd_hline_t param;
		param.x = pParam->x + pParam->chamfer;
		param.y = pParam->y;
		param.w = pParam->w - pParam->chamfer - pParam->chamfer;
		tlkdrv_lcd_paintHLine(&param, paint);
		param.y = pParam->y + pParam->h;
		tlkdrv_lcd_paintHLine(&param, paint);
	}
	if((pParam->chamfer << 1) != pParam->h){
		tlkdrv_lcd_vline_t param;
		param.x = pParam->x;
		param.y = pParam->y + pParam->chamfer;
		param.h = pParam->h - pParam->chamfer - pParam->chamfer;
		tlkdrv_lcd_paintVLine(&param, paint);
		param.x = pParam->x + pParam->w;
		tlkdrv_lcd_paintVLine(&param, paint);
	}
	
	return TLK_ENONE;
}
int tlkdrv_lcd_paintEllipse(tlkdrv_lcd_ellipse_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint16 x;
	uint16 y;
	double d;
	double sqa;
	double sqb;
		
	if(pParam == nullptr || paint == nullptr || pParam->a < sTlkDrvLcdBrushSize || pParam->b < sTlkDrvLcdBrushSize){
		return -TLK_EPARAM;
	}

	sqa = pParam->a * pParam->a;
	sqb = pParam->b * pParam->b;

	x = 0;
	y = pParam->b;
	d = sqb + sqa * (-pParam->b + 0.25);
	sqa = pParam->a * pParam->a;
	sqb = pParam->b * pParam->b;
	
	tlkdrv_lcd_drawQuadrantPoint(pParam->x0, pParam->y0, x, y, paint);
	while(sqb * (x + 1) < sqa * (y - 0.5)){
		if(d < 0){
			d += sqb * (2 * x + 3);
		}else{
			d += (sqb * (2 * x + 3) + sqa * (-2 * y + 2));
			y--;
		}
		x++;
		tlkdrv_lcd_drawQuadrantPoint(pParam->x0, pParam->y0, x, y, paint);
	}
	
	d = (pParam->b * (x + 0.5)) * 2 + (pParam->a * (y - 1)) * 2 - (pParam->a * pParam->b) * 2;
	while(y > 0){
		if(d < 0){
			d += sqb * (2 * x + 2) + sqa * (-2 * y + 3);
			x++; 
		}else{
			d += sqa * (-2 * y + 3); 
		}
		y--;
		tlkdrv_lcd_drawQuadrantPoint(pParam->x0, pParam->y0, x, y, paint);
	}
	 
	return TLK_ENONE;
}
int tlkdrv_lcd_paintCircle(tlkdrv_lcd_circle_t *pParam, TlkdrvLcdPointFunc paint)
{
	int d;
	uint16 x;
	uint16 y;

	if(pParam == nullptr || paint == nullptr || pParam->r < sTlkDrvLcdBrushSize){
		return -TLK_EPARAM;
	}
	
	x = 0;
	y = pParam->r;
	d = 3 - 2 * pParam->r;
	while(x <= y){
		paint(pParam->x0 + x, pParam->y0 + y);
		paint(pParam->x0 + y, pParam->y0 + x);
		paint(pParam->x0 + x, pParam->y0 - y);
		paint(pParam->x0 - x, pParam->y0 + y);
		paint(pParam->x0 + y, pParam->y0 - x);
		paint(pParam->x0 - x, pParam->y0 - y);
		paint(pParam->x0 - y, pParam->y0 - x);
		paint(pParam->x0 - y, pParam->y0 + x);
		if(d < 0){
			d += 4*x+6;
		}else{
			d += 4*(x-y)+10;
			y--;
		}
		x++;
	}
	return TLK_ENONE;
}

int tlkdrv_lcd_paintSector(tlkdrv_lcd_sector_t *pParam, TlkdrvLcdPointFunc paint)
{
	int d;
	uint16 x;
	uint16 y;
	uint16 sx;
	uint16 sy;
	uint16 ex;
	uint16 ey;
	tlkdrv_lcd_line_t param;
	
	if(pParam == nullptr || paint == nullptr || pParam->r < 1 || pParam->sAngle >= pParam->eAngle
		|| pParam->sAngle >= 360 || pParam->eAngle >= 720){
		return -TLK_EPARAM;
	}
	if(pParam->eAngle - pParam->sAngle >= 360){
		tlkdrv_lcd_circle_t circle;
		circle.x0 = pParam->x0;
		circle.y0 = pParam->y0;
		circle.r = pParam->r;
		return tlkdrv_lcd_paintCircle(&circle, paint);
	}
	if(pParam->eAngle >= 360) pParam->eAngle -= 360;
	
	tlkdrv_lcd_calcXYAxisByAngle(pParam->x0, pParam->y0, pParam->r, pParam->sAngle, &sx, &sy);
	tlkdrv_lcd_calcXYAxisByAngle(pParam->x0, pParam->y0, pParam->r, pParam->eAngle, &ex, &ey);
	
	x = 0;
	y = pParam->r;
	d = 3 - 2 * pParam->r;
	while(x <= y){
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 + y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 + y, sx, sy)){
			paint(pParam->x0 + x, pParam->y0 + y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 + x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 + x, sx, sy)){
			paint(pParam->x0 + y, pParam->y0 + x);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 - y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 - y, sx, sy)){
			paint(pParam->x0 + x, pParam->y0 - y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 + y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 + y, sx, sy)){
			paint(pParam->x0 - x, pParam->y0 + y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 - x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 - x, sx, sy)){
			paint(pParam->x0 + y, pParam->y0 - x);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 - y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 - y, sx, sy)){
			paint(pParam->x0 - x, pParam->y0 - y);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 - x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 - x, sx, sy)){
			paint(pParam->x0 - y, pParam->y0 - x);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 + x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 + x, sx, sy)){
			paint(pParam->x0 - y, pParam->y0 + x);
		}
				
		if(d < 0){
			d += 4*x+6;
		}else{
			d += 4*(x-y)+10;
			y--;
		}
		x++;
	}

	param.x1 = pParam->x0;
	param.y1 = pParam->y0;
	param.x2 = sx;
	param.y2 = sy;
	tlkdrv_lcd_paintLine(&param, paint);
	param.x2 = ex;
	param.y2 = ey;
	tlkdrv_lcd_paintLine(&param, paint);
	
	return TLK_ENONE;
}


int tlkdrv_lcd_paintFillRing(tlkdrv_lcd_ring_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint08 i;

	if(pParam == nullptr || paint == nullptr || pParam->sr < 1 || pParam->er < 1){
		return -TLK_EPARAM;
	}
	
	if(pParam->sr > pParam->er){
		uint08 temp = pParam->sr;
		pParam->sr = pParam->er;
		pParam->er = temp;
	}
	for(i = pParam->sr; i <= pParam->er; i+=sTlkDrvLcdBrushSize){
		tlkdrv_lcd_circle_t param;
		param.x0 = pParam->x0;
		param.y0 = pParam->y0;
		param.r  = i;
		tlkdrv_lcd_paintCircle(&param, paint);
	}
	return TLK_ENONE;
}
int tlkdrv_lcd_paintFillRect(tlkdrv_lcd_rect_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint08 i;
	uint08 brush;
	
	if(pParam == nullptr || paint == nullptr || pParam->w == 0 || pParam->h == 0
		|| pParam->w < 1 || pParam->h < 1){
		return -TLK_EPARAM;
	}

	brush = sTlkDrvLcdBrushSize;
	sTlkDrvLcdBrushSize = 1;
	if(pParam->w >= pParam->h){
		tlkdrv_lcd_hline_t param;
		param.x = pParam->x;
		param.y = pParam->y;
		param.w = pParam->w;
		for(i = 0; i < pParam->h - sTlkDrvLcdBrushSize; i += sTlkDrvLcdBrushSize){
			param.y += sTlkDrvLcdBrushSize;
			tlkdrv_lcd_paintHLine(&param, paint);
		}
	}else{
		tlkdrv_lcd_vline_t param;
		param.x = pParam->x;
		param.y = pParam->y;
		param.h = pParam->h;
		for(i = 0; i < pParam->w - sTlkDrvLcdBrushSize; i += sTlkDrvLcdBrushSize){
			param.x += sTlkDrvLcdBrushSize;
			tlkdrv_lcd_paintVLine(&param, paint);
		}
	}
	sTlkDrvLcdBrushSize = brush;
	return TLK_ENONE;
}
int tlkdrv_lcd_paintFillRoundedRect(tlkdrv_lcd_roundedRect_t *pParam, TlkdrvLcdPointFunc paint)
{
	int d;
	uint16 x;
	uint16 y;
	uint16 x1;
	uint16 y1;
	uint16 x2;
	uint16 y2;
	uint16 x3;
	uint16 y3;
	uint16 x4;
	uint16 y4;
	uint16 size;
	uint08 brush;
	tlkdrv_lcd_hline_t param;
	
	if(pParam == nullptr || paint == nullptr || pParam->w == 0 || pParam->h == 0
		|| pParam->w < 1 || pParam->h < 1){
		return -TLK_EPARAM;
	}
	
	size = sTlkDrvLcdBrushSize << 1;
	if(pParam->w <= size || pParam->h <= size || pParam->rounded == 0){
		tlkdrv_lcd_rect_t rect;
		rect.x = pParam->x;
		rect.y = pParam->y;
		rect.w = pParam->w;
		rect.h = pParam->h;
		return tlkdrv_lcd_paintFillRect(&rect, paint);
	}
	if(pParam->rounded > (pParam->w >> 1)) pParam->rounded = (pParam->w >> 1);
	if(pParam->rounded > (pParam->h >> 1)) pParam->rounded = (pParam->h >> 1);
	
	brush = sTlkDrvLcdBrushSize;
	sTlkDrvLcdBrushSize = 1;
	x1 = pParam->x + pParam->w - pParam->rounded;
	y1 = pParam->y + pParam->rounded;
	x2 = pParam->x + pParam->rounded;
	y2 = pParam->y + pParam->rounded;
	x3 = pParam->x + pParam->rounded;
	y3 = pParam->y + pParam->h - pParam->rounded;
	x4 = pParam->x + pParam->w - pParam->rounded;
	y4 = pParam->y + pParam->h - pParam->rounded;
	
	x = 0;
	y = pParam->rounded;
	d = 3 - 2 * pParam->rounded;
	while(x <= y){
		//First quadrant and second quadrant
		param.x = x2 - x;
		param.y = y1 - y;
		param.w = (x1 + x) - (x2 - x);
		tlkdrv_lcd_paintHLine(&param, paint);
		param.x = x2 - y;
		param.y = y1 - x;
		param.w = (x1 + y) - (x2 - y);
		tlkdrv_lcd_paintHLine(&param, paint);
		//third quadrant and fourth quadrant
		param.x = x3 - x;
		param.y = y3 + y;
		param.w = (x4 + x) - (x3 - x);
		tlkdrv_lcd_paintHLine(&param, paint);
		param.x = x3 - y;
		param.y = y3 + x;
		param.w = (x4 + y) - (x3 - y);
		tlkdrv_lcd_paintHLine(&param, paint);
		if(d < 0){
			d += 4*x + 6;
		}else{
			d += 4*(x-y)+10;
			y --;
		}
		x++;
	}
	sTlkDrvLcdBrushSize = brush;
	if(y1 != y4){
		tlkdrv_lcd_rect_t rect;
		rect.x = pParam->x;
		rect.y = y2;
		rect.w = pParam->w;
		rect.h = y4 - y1;
		tlkdrv_lcd_paintFillRect(&rect, paint);
	}
	
	return TLK_ENONE;
}
int tlkdrv_lcd_paintFillChamferRect(tlkdrv_lcd_chamferRect_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint16 size;
	uint08 brush;
	
	if(pParam == nullptr || paint == nullptr || pParam->w == 0 || pParam->h == 0
		/*|| pParam->w < sTlkDrvLcdBrushSize || pParam->h < sTlkDrvLcdBrushSize*/){
		return -TLK_EPARAM;
	}

	size = sTlkDrvLcdBrushSize << 1;
	if(pParam->w <= size || pParam->h <= size || pParam->chamfer == 0){
		tlkdrv_lcd_rect_t rect;
		rect.x = pParam->x;
		rect.y = pParam->y;
		rect.w = pParam->w;
		rect.h = pParam->h;
		return tlkdrv_lcd_paintRect(&rect, paint);
	}
	if(pParam->chamfer > (pParam->w >> 1)) pParam->chamfer = (pParam->w >> 1);
	if(pParam->chamfer > (pParam->h >> 1)) pParam->chamfer = (pParam->h >> 1);

	brush = sTlkDrvLcdBrushSize;
	sTlkDrvLcdBrushSize = 1;
	if(pParam->w >= pParam->h){
		uint16 i;
		tlkdrv_lcd_hline_t param;
		for(i=0; i<pParam->h; i++){
			param.y = pParam->y + i;
			if(i < pParam->chamfer){
				param.x = pParam->x + pParam->chamfer - i;
				param.w = pParam->w + i - pParam->chamfer + i - pParam->chamfer;
			}else if(i + pParam->chamfer > pParam->h){
				param.x = pParam->x + i + pParam->chamfer - pParam->h;
				param.w = pParam->w + pParam->h - i - pParam->chamfer + pParam->h - i - pParam->chamfer;
			}else{
				param.x = pParam->x;
				param.w = pParam->w;
			}
			tlkdrv_lcd_paintHLine(&param, paint);
		}
	}else{
		uint16 i;
		tlkdrv_lcd_vline_t param;
		for(i=0; i<pParam->w; i++){
			param.x = pParam->x + i;
			if(i < pParam->chamfer){
				param.y = pParam->y + pParam->chamfer - i;
				param.h = pParam->h + i - pParam->chamfer + i - pParam->chamfer;
			}else if(i + pParam->chamfer > pParam->w){
				param.y = pParam->y + i + pParam->chamfer - pParam->w;
				param.h = pParam->h + pParam->w - i - pParam->chamfer + pParam->w - i - pParam->chamfer;
			}else{
				param.y = pParam->y;
				param.h = pParam->h;
			}
			tlkdrv_lcd_paintVLine(&param, paint);
		}
	}
	sTlkDrvLcdBrushSize = brush;
	
	return TLK_ENONE;
}
int tlkdrv_lcd_paintFillEllipse(tlkdrv_lcd_ellipse_t *pParam, TlkdrvLcdPointFunc paint)
{
	uint16 x;
	uint16 y;
	double d;
	double sqa;
	double sqb;
	uint08 brush;
	tlkdrv_lcd_hline_t param;
	
	if(pParam == nullptr || paint == nullptr || pParam->a < 1 || pParam->b < 1){
		return -TLK_EPARAM;
	}
	
	brush = sTlkDrvLcdBrushSize;
	sTlkDrvLcdBrushSize = 1;

	sqa = pParam->a * pParam->a;
	sqb = pParam->b * pParam->b;

	x = 0;
	y = pParam->b;
	d = sqb + sqa * (-pParam->b + 0.25);
	sqa = pParam->a * pParam->a;
	sqb = pParam->b * pParam->b;

	tlkdrv_lcd_drawQuadrantPoint(pParam->x0, pParam->y0, x, y, paint);
	while(sqb * (x + 1) < sqa * (y - 0.5)){
		if(d < 0){
			d += sqb * (2 * x + 3);
		}else{
			d += (sqb * (2 * x + 3) + sqa * (-2 * y + 2));
			y--;
		}
		x++;
		param.x = pParam->x0-x;
		param.y = pParam->x0+y;
		param.w = 2 * x;
		tlkdrv_lcd_paintHLine(&param, paint);
		param.x = pParam->x0-x;
		param.y = pParam->x0-y;
		param.w = 2 * x;
		tlkdrv_lcd_paintHLine(&param, paint);
	}
	
	d = (pParam->b * (x + 0.5)) * 2 + (pParam->a * (y - 1)) * 2 - (pParam->a * pParam->b) * 2;
	while(y > 0){
		if(d < 0){
			d += sqb * (2 * x + 2) + sqa * (-2 * y + 3);
			x++; 
		}else{
			d += sqa * (-2 * y + 3); 
		}
		y--;
		param.x = pParam->x0-x;
		param.y = pParam->x0+y;
		param.w = 2 * x;
		tlkdrv_lcd_paintHLine(&param, paint);
		param.x = pParam->x0-x;
		param.y = pParam->x0-y;
		param.w = 2 * x;
		tlkdrv_lcd_paintHLine(&param, paint);
	}
	sTlkDrvLcdBrushSize = brush;
	
	return TLK_ENONE;
}
int tlkdrv_lcd_paintFillCircle(tlkdrv_lcd_circle_t *pParam, TlkdrvLcdPointFunc paint)
{
	int d;
	uint16 x;
	uint16 y;
	uint08 brush;
	tlkdrv_lcd_hline_t param;

	if(pParam == nullptr || paint == nullptr || pParam->r < 1){
		return -TLK_EPARAM;
	}

	brush = sTlkDrvLcdBrushSize;
	sTlkDrvLcdBrushSize = 1;
		
	x = 0;
	y = pParam->r;
	d = 3 - 2 * pParam->r;
	while(x < y){
		param.x = pParam->x0 - x;
		param.y = pParam->y0 + y;
		param.w = 2 * x;
		tlkdrv_lcd_paintHLine(&param, paint);
		param.x = pParam->x0 - y;
		param.y = pParam->y0 + x;
		param.w = 2 * y;
		tlkdrv_lcd_paintHLine(&param, paint);
		param.x = pParam->x0 - x;
		param.y = pParam->y0 - y;
		param.w = 2 * x;
		tlkdrv_lcd_paintHLine(&param, paint);
		param.x = pParam->x0 - y;
		param.y = pParam->y0 - x;
		param.w = 2 * y;
		tlkdrv_lcd_paintHLine(&param, paint);
		if(d < 0){
			d += 4*x+6;
		}else{
			d += 4*(x-y)+10;
			y--;
		}
		x++;
	}
	sTlkDrvLcdBrushSize = brush;
	return TLK_ENONE;
}
int tlkdrv_lcd_paintFillSector(tlkdrv_lcd_sector_t *pParam, TlkdrvLcdPointFunc paint)
{
	int d;
	uint16 x;
	uint16 y;
	uint16 sx;
	uint16 sy;
	uint16 ex;
	uint16 ey;
	uint08 brush;
	tlkdrv_lcd_line_t param;
	
	if(pParam == nullptr || paint == nullptr || pParam->r < 1){
		return -TLK_EPARAM;
	}

	brush = sTlkDrvLcdBrushSize;
	sTlkDrvLcdBrushSize = 1;
		
	if(pParam == nullptr || paint == nullptr || pParam->r < 1 || pParam->sAngle >= pParam->eAngle
		|| pParam->sAngle >= 360 || pParam->eAngle >= 720){
		return -TLK_EPARAM;
	}
	if(pParam->eAngle - pParam->sAngle >= 360){
		tlkdrv_lcd_circle_t circle;
		circle.x0 = pParam->x0;
		circle.y0 = pParam->y0;
		circle.r = pParam->r;
		return tlkdrv_lcd_paintCircle(&circle, paint);
	}
	if(pParam->eAngle >= 360) pParam->eAngle -= 360;

	tlkdrv_lcd_calcXYAxisByAngle(pParam->x0, pParam->y0, pParam->r, pParam->sAngle, &sx, &sy);
	tlkdrv_lcd_calcXYAxisByAngle(pParam->x0, pParam->y0, pParam->r, pParam->eAngle, &ex, &ey);

	param.x1 = pParam->x0;
	param.y1 = pParam->y0;
	param.x2 = sx;
	param.y2 = sy;
	tlkdrv_lcd_paintLine(&param, paint);
	
	x = 0;
	y = pParam->r;
	d = 3 - 2 * pParam->r;
	while(x <= y){
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 + y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 + y, sx, sy)){
			param.x2 = pParam->x0 + x;
			param.y2 = pParam->y0 + y;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 + x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 + x, sx, sy)){
			param.x2 = pParam->x0 + y;
			param.y2 = pParam->y0 + x;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 - y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + x, pParam->y0 - y, sx, sy)){
			param.x2 = pParam->x0 + x;
			param.y2 = pParam->y0 - y;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 + y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 + y, sx, sy)){
			param.x2 = pParam->x0 - x;
			param.y2 = pParam->y0 + y;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 - x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 + y, pParam->y0 - x, sx, sy)){
			param.x2 = pParam->x0 + y;
			param.y2 = pParam->y0 - x;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 - y, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - x, pParam->y0 - y, sx, sy)){
			param.x2 = pParam->x0 - x;
			param.y2 = pParam->y0 - y;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 - x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 - x, sx, sy)){
			param.x2 = pParam->x0 - y;
			param.y2 = pParam->y0 - x;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		if(!tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 + x, ex, ey)
			&& tlkdrv_lcd_pointInCircleIsFront(pParam->x0, pParam->y0, pParam->x0 - y, pParam->y0 + x, sx, sy)){
			param.x2 = pParam->x0 - y;
			param.y2 = pParam->y0 + x;
			tlkdrv_lcd_paintLine(&param, paint);
		}
		
		if(d < 0){
			d += 4*x+6;
		}else{
			d += 4*(x-y)+10;
			y--;
		}
		x++;
	}
	
	param.x2 = ex;
	param.y2 = ey;
	tlkdrv_lcd_paintLine(&param, paint);
		
	sTlkDrvLcdBrushSize = brush;
	
	return TLK_ENONE;
}



static const tlkdrv_lcd_modinf_t *tlkdrv_lcd_getDev(TLKDRV_LCD_DEV_ENUM dev)
{
	if(dev >= TLKDRV_LCD_DEV_MAX) return nullptr;
	return spTlkDrvLcdModInfs[dev];
}
static void tlkdrv_lcd_drawQuadrantPoint(uint16 x0, uint16 y0, uint16 x, uint16 y, TlkdrvLcdPointFunc paint)
{
	paint(x0-x, y0+y);
	paint(x0+x, y0+y);
	paint(x0-x, y0-y);
	paint(x0+x, y0-y);
}


static const uint16 sTlkdrvLcdSin90Table[] = {
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16384, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32768
};
static const uint16 sTlkdrvLcdCos90Table[] = {
    32768, 32763, 32748, 32723, 32688, 32643, 32588, 32524, 32449, 32364, 32270, 32166, 32052, 31928, 31794, 31651,
    31498, 31336, 31164, 30983, 30792, 30592, 30382, 30163, 29935, 29678, 29452, 29196, 29932, 28660, 28378, 28088, //16
    27789, 27482, 27166, 26842, 26510, 26170, 25822, 25466, 25102, 24730, 24351, 23965, 23571, 23170, 22763, 22348, //32
    21926, 21498, 21063, 20622, 20174, 19720, 19261, 18795, 18324, 17847, 17364, 16877, 16384, 15886, 15384, 14876, //48
    14365, 13848, 13328, 12803, 12275, 11743, 11207, 10668, 10126,  9580,  9032,  8481,  7927,  7371,  6813,  6252, //64
     5690,  5126,  4560,  3993,  3425,  2856,  2286,  1715,  1144,   572,     0 //80
};

static void tlkdrv_lcd_calcXYAxisByAngle(uint16 x0, uint16 y0, uint16 r, uint16 angle, uint16 *pX, uint16 *pY)
{
	int x;
	int y;
	uint16 tangle;

	if(angle > 360 || pX == nullptr || pY == nullptr) return;
	
	tangle = angle;
	if(angle > 270) tangle = 360 - angle;
	else if(angle > 180) tangle = angle - 180;
	else if(angle > 90) tangle = 180 - angle;
	else tangle = angle;
	
	x = ((uint32)r * sTlkdrvLcdCos90Table[tangle]) >> 15;
	y = ((uint32)r * sTlkdrvLcdSin90Table[tangle]) >> 15;
	if(angle > 90 && angle < 270) *pX = x0 - x;
	else *pX = x0 + x;
	if(angle > 0 && angle < 180) *pY = y0 - y;
	else *pY = y0 + y;
}
static bool tlkdrv_lcd_pointInCircleIsFront(uint16 x0, uint16 y0, uint16 x, uint16 y, uint16 rx, uint16 ry)
{
	if(rx > x0 && ry < y0){ //First quadrant
		if(x > x0 && y < y0 && x > rx && y > ry) return false;
		else return true;
	}else if(rx < x0 && ry < y0){ //second quadrant
		if(y > y0 || (x < x0 && y < y0 && x < rx && y > ry)) return true;
		else return false;
	}else if(rx < x0 && ry > y0){ //third quadrant
		if(y < y0 || (x < x0 && y > y0 && x < rx && y < ry)) return false;
		else return true;
	}else{ //fourth quadrant
		if(x > x0 && y > y0 && x > rx && y < ry) return true;
		else return false;
	}
}


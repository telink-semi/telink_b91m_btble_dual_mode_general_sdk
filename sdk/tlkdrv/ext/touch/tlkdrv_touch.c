/********************************************************************************************************
 * @file	tlkdrv_touch.c
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
#include "tlkdrv_touch.h"

static const tlkdrv_touch_modinf_t *tlkdrv_touch_getDev(TLKDRV_TOUCH_DEV_ENUM dev);

extern const tlkdrv_touch_modinf_t sTlkDrvFt3168ModInf;
static const tlkdrv_touch_modinf_t *spTlkDrvTouchModInf[TLKDRV_TOUCH_DEV_MAX] = {
	#if (TLKDRV_TOUCH_FT3168_ENABLE)
	&sTlkDrvFt3168ModInf,
	#else
	nullptr,
	#endif
};



bool tlkdrv_touch_isOpen(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->IsOpen == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsOpen();
}
bool tlkdrv_touch_isReady(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->IsReady == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->IsReady();
}

int tlkdrv_touch_init(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Init == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Init();
}
int tlkdrv_touch_open(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Open == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Open();
}
int tlkdrv_touch_close(TLKDRV_TOUCH_DEV_ENUM dev)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Close == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Close();
}
int tlkdrv_touch_getPoint(TLKDRV_TOUCH_DEV_ENUM dev, tlkdrv_touch_point_t *pPoint)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->GetPoint == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->GetPoint(pPoint);
}
int tlkdrv_touch_getGesture(TLKDRV_TOUCH_DEV_ENUM dev, tlkdrv_touch_gesture_t *pGesture)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->GetGesture == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->GetGesture(pGesture);
}
int tlkdrv_touch_handler(TLKDRV_TOUCH_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdrv_touch_modinf_t *pModInf;
	pModInf = tlkdrv_touch_getDev(dev);
	if(pModInf == nullptr || pModInf->Handler == nullptr) return -TLK_ENOSUPPORT;
	return pModInf->Handler(opcode, param0, param1);
}


static const tlkdrv_touch_modinf_t *tlkdrv_touch_getDev(TLKDRV_TOUCH_DEV_ENUM dev)
{
	if(dev >= TLKDRV_TOUCH_DEV_MAX) return nullptr;
	return spTlkDrvTouchModInf[dev];
}



#if 0

#define THRD_TP_IND_MAX   80/*100*/ /* the MAX value of default for identify */
#define THRD_TP_IND_WAVE  40/*50*/  /* the WAVE value of default for identify */
#define THRD_TP_IND_MIN    20/*20*/  /* the MIN value of default for identify */

#define THRD_TP_EVT_BEGIN      (1)
#define THRD_TP_EVT_DOWN      (THRD_TP_EVT_BEGIN+0x0)
#define THRD_TP_EVT_UP           (THRD_TP_EVT_BEGIN+0x1)
#define THRD_TP_EVT_LONG       (THRD_TP_EVT_BEGIN+0x2)
#define THRD_TP_EVT_DOUBLE   (THRD_TP_EVT_BEGIN+0x3)
#define THRD_TP_EVT_REPEAT   (THRD_TP_EVT_BEGIN+0x4)
#define THRD_TP_EVT_L2R         (THRD_TP_EVT_BEGIN+0x5)
#define THRD_TP_EVT_R2L         (THRD_TP_EVT_BEGIN+0x6)
#define THRD_TP_EVT_T2B         (THRD_TP_EVT_BEGIN+0x7)
#define THRD_TP_EVT_B2T         (THRD_TP_EVT_BEGIN+0x8)
#define THRD_TP_EVT_ABORT    (THRD_TP_EVT_BEGIN+0x9)


typedef struct{
	ushort x;
	ushort y;
}thrd_tp_msg_t,*p_thrd_tp_msg_t;

typedef struct{ /* donot motify */
	ushort x;
	ushort y;
}tp_point_t,*p_tp_point_t;
typedef struct{
	unsigned stat:1;
	unsigned type:3;
	unsigned len:8;
	unsigned x:10;
	unsigned y:10;
	uint param;
}tp_parse_t,*p_tp_parser_t;

void thrd_tp_parser_init(p_tp_parser_t p_parse,p_tp_point_t p_point);
int thrd_tp_parser(p_tp_parser_t p_parse,p_tp_point_t p_point);

void thrd_tp_event(VM_TOUCH_EVENT event, int x, int y);


typedef struct{
	tp_parse_t tp_parse;
}tp_t,*p_tp_t;

static tp_t s_g_tp={0};

static int thrd_tp_abs(int x){
	if(x < 0){
		x = -x;
	}
	return x;
}

void thrd_tp_parser_init(p_tp_parser_t p_parse,p_tp_point_t p_point)
{
	if(!p_parse || !p_point) return ;
	p_parse->stat=1;
	p_parse->type=0;
	p_parse->type=0;
	p_parse->x=p_point->x;
	p_parse->y=p_point->y;
}

int thrd_tp_parser(p_tp_parser_t p_parse,p_tp_point_t p_point){
	if(p_parse==NULL || p_point==NULL) return THRD_TP_EVT_ABORT;

	if(0==p_parse->stat){
    	return THRD_TP_EVT_ABORT;
  	}
	
	if(!((THRD_TP_IND_MAX>=THRD_TP_IND_WAVE) && (THRD_TP_IND_WAVE>=THRD_TP_IND_MIN))){
   	 	return THRD_TP_EVT_ABORT;
	}
	if((thrd_tp_abs(p_parse->x-p_point->x) < THRD_TP_IND_MIN)
			&& (thrd_tp_abs(p_parse->y-p_point->y) < THRD_TP_IND_MIN)){
		return THRD_TP_EVT_ABORT;
	}
	if((thrd_tp_abs(p_parse->x-p_point->x) >= THRD_TP_IND_WAVE)
			&& (thrd_tp_abs(p_parse->y-p_point->y) >= THRD_TP_IND_WAVE)){
		p_parse->stat=0;
		return THRD_TP_EVT_ABORT;
	}
	if(thrd_tp_abs(p_parse->x-p_point->x) >= THRD_TP_IND_MAX){
		p_parse->stat=0;
		return ((p_parse->x-p_point->x)>=0) ? THRD_TP_EVT_R2L : THRD_TP_EVT_L2R;
	}
	if(thrd_tp_abs(p_parse->y-p_point->y) >= THRD_TP_IND_MAX){
		p_parse->stat=0;
		return ((p_parse->y-p_point->y)>=0) ? THRD_TP_EVT_B2T : THRD_TP_EVT_T2B;
	}
	
	return THRD_TP_EVT_ABORT;
}

void thrd_tp_event(VM_TOUCH_EVENT event, int x, int y){
    uint tpEvtId = VM_TOUCH_EVENT_ABORT;
	 tp_point_t point={x,y};
	 char buff[256];

	 if(event == VM_TOUCH_EVENT_TAP){
		thrd_tp_parser_init(&s_g_tp.tp_parse,&point);
		tpEvtId=THRD_TP_EVT_DOWN;
	 }else if(event == VM_TOUCH_EVENT_RELEASE){
		tpEvtId=THRD_TP_EVT_UP;
	 }else if(event == VM_TOUCH_EVENT_MOVE){
		tpEvtId=thrd_tp_parser(&s_g_tp.tp_parse,&point);		
	 }else if(event == VM_TOUCH_EVENT_LONG_TAP){
		tpEvtId=THRD_TP_EVT_LONG;		
	 }else if(event == VM_TOUCH_EVENT_DOUBLE_CLICK){
		tpEvtId=THRD_TP_EVT_DOUBLE;			
	 }else if(event == VM_TOUCH_EVENT_REPEAT){
		tpEvtId=THRD_TP_EVT_REPEAT;		
	 }else if(event == VM_TOUCH_EVENT_ABORT){
		//tpEvtId=THRD_TP_EVT_ABORT;	
		return ;
	 }else{
		return ;
	 }

	if(tpEvtId==THRD_TP_EVT_ABORT) return ;

	//vm_log_info("touch tpEvtId=%d,touch x=%d,touch y=%d",tpEvtId,point.x,point.y);    /* output log to monitor or catcher */
	//sprintf(buff, "touch tpEvtId = %d, x=%d, y=%d", tpEvtId,point.x, point.y);
	//log_info(2, buff);    /* output log to LCD if have */
}


#endif



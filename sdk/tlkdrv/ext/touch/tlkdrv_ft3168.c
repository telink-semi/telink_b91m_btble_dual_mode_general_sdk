/********************************************************************************************************
 * @file	tlkdrv_ft3168.c
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
#if (TLKDRV_TOUCH_FT3168_ENABLE)
#include "drivers.h"
#include "tlkdrv_ft3168.h"


#define TLKDRV_TOUCH_FT3168_POINT_MAX     2
#define TLKDRV_TOUCH_FT3168_POINT_MASK    0x03


const tlkdrv_touch_modinf_t sTlkDrvFt3168ModInf = {
	tlkdrv_ft3168_isOpen, //IsOpen
	tlkdrv_ft3168_isReady, //IsReady
	tlkdrv_ft3168_init, //Init
	tlkdrv_ft3168_open, //Open
	tlkdrv_ft3168_close, //Close
	tlkdrv_ft3168_getPoint, //GetPoint
	tlkdrv_ft3168_getGesture, //GetGesture
	tlkdrv_ft3168_handler, //Handler
};
static tlkdrv_ft3168_t sTlkDrvFt3168Ctrl = {0};


_attribute_ram_code_sec_noinline_ 
void gpio_irq_handler(void)
{
	gpio_clr_irq_status(FLD_GPIO_IRQ_CLR);
	sTlkDrvFt3168Ctrl.irqRdy = true;
}


static bool tlkdrv_ft3168_isOpen(void)
{
	return sTlkDrvFt3168Ctrl.isOpen;
}
static bool tlkdrv_ft3168_isReady(void)
{
	return (sTlkDrvFt3168Ctrl.isOpen && sTlkDrvFt3168Ctrl.irqRdy);
}

static int tlkdrv_ft3168_init(void)
{
	tmemset(&sTlkDrvFt3168Ctrl, 0, sizeof(tlkdrv_ft3168_t));

    i2c_master_init();
	i2c_set_master_clk((unsigned char)(sys_clk.pclk*1000*1000/(4*6))); //500k
	i2c_master_detect_nack_en();
	i2c_set_pin(TLKDRV_FT3168_I2C_SDA, TLKDRV_FT3168_I2C_SCL);

	//enable irq
	gpio_function_en(TLKDRV_FT3168_INT_PIN);
	gpio_output_dis(TLKDRV_FT3168_INT_PIN);
	gpio_input_en(TLKDRV_FT3168_INT_PIN);
	gpio_set_up_down_res(TLKDRV_FT3168_INT_PIN, GPIO_PIN_PULLUP_10K);

	//reset
	gpio_function_en(TLKDRV_FT3168_RST_PIN);
	gpio_output_en(TLKDRV_FT3168_RST_PIN);
	gpio_input_dis(TLKDRV_FT3168_RST_PIN);
	gpio_set_up_down_res(TLKDRV_FT3168_RST_PIN, GPIO_PIN_PULLUP_10K);
	
	sTlkDrvFt3168Ctrl.isInit = true;
	
	return TLK_ENONE;
}
static int tlkdrv_ft3168_open(void)
{
	if(!sTlkDrvFt3168Ctrl.isInit) return -TLK_ENOREADY;
	if(sTlkDrvFt3168Ctrl.isOpen) return -TLK_EREPEAT;
	
	gpio_set_low_level(TLKDRV_FT3168_RST_PIN);
	delay_ms(10);
	gpio_set_high_level(TLKDRV_FT3168_RST_PIN);
	delay_ms(100); //should delay some time
	
	//enable gesture
	tlkdrv_ft3168_sendByte(0xD1, 0xFF);
	tlkdrv_ft3168_sendByte(0xD2, 0xFF);
	tlkdrv_ft3168_sendByte(0xD5, 0xFF);
	tlkdrv_ft3168_sendByte(0xD6, 0xFF);
	tlkdrv_ft3168_sendByte(0xD7, 0xFF);
	tlkdrv_ft3168_sendByte(0xD8, 0xFF);
	#if (TLKDRV_FT3168_GESTURE_ENABLE)
	tlkdrv_ft3168_sendByte(TLKDRV_FT3168_REG_GESTURE_EN, 1);
	#else
	tlkdrv_ft3168_sendByte(TLKDRV_FT3168_REG_GESTURE_EN, 0);
	#endif
	
	//test: read chip id
	tlkdrv_ft3168_readData(0xA8, &sTlkDrvFt3168Ctrl.chipID, 1);
	
	gpio_set_irq(TLKDRV_FT3168_INT_PIN, INTR_FALLING_EDGE);
	plic_set_priority(IRQ25_GPIO, IRQ_PRI_LEV2);
	plic_interrupt_enable(IRQ25_GPIO);
	
	sTlkDrvFt3168Ctrl.isOpen = true;
	
	return TLK_ENONE;
}
static int tlkdrv_ft3168_close(void)
{
	if(!sTlkDrvFt3168Ctrl.isOpen) return TLK_ENONE;
	
	plic_interrupt_disable(IRQ25_GPIO);
	tlkdrv_ft3168_sendByte(TLKDRV_FT3168_REG_GESTURE_EN, 0);

	sTlkDrvFt3168Ctrl.isOpen = false;
	
	return TLK_ENONE;
}
static int tlkdrv_ft3168_getPoint(tlkdrv_touch_point_t *pPoint)
{
	uint08 index;
	uint08 tryCnt;
	uint08 valSta;
	uint08 pointNum;
	uint16 xSum, ySum;
	uint08 buffer[4] = {0};
	uint08 regAddr[2] = {TLKDRV_FT3168_REG_TOUCH1_XH_ADDR, TLKDRV_FT3168_REG_TOUCH2_XH_ADDR};
	
	if(pPoint == nullptr) return -TLK_EPARAM;
	if(!sTlkDrvFt3168Ctrl.isOpen || !sTlkDrvFt3168Ctrl.irqRdy) return -TLK_ENOREADY;

	sTlkDrvFt3168Ctrl.irqRdy = false;	
	
	//make sure data in buffer is valid
	tryCnt = 0x30;
	while(--tryCnt){
		tlkdrv_ft3168_readData(TLKDRV_FT3168_REG_STATUS, &valSta, 1);
		if(valSta <= TLKDRV_TOUCH_FT3168_POINT_MAX) break;
	}
	if(tryCnt == 0) return -TLK_EFAIL;

	xSum = 0;
	ySum = 0;
	pointNum = (valSta & TLKDRV_TOUCH_FT3168_POINT_MASK);
	if(pointNum == 0 || pointNum > TLKDRV_TOUCH_FT3168_POINT_MAX) pointNum = 1;	
	for(index = 0; index < pointNum; index++){
		tlkdrv_ft3168_readData(regAddr[index], buffer, 4);
		pPoint->evtID = (((buffer[0]>>6) & 0x3)<<8) | ((buffer[2]>>4) & 0x0f); //EvtID[0~7], EvtFlag[8~15]
		xSum += (((unsigned short)(buffer[0]&0x0f))<<8) | buffer[1];
		ySum += (((unsigned short)(buffer[2]&0x0f))<<8) | buffer[3];
	}
	
	if(pointNum == 1){
		pPoint->axisX = xSum;
		pPoint->axisY = ySum;
	}else{
		pPoint->axisX = xSum/pointNum;
		pPoint->axisY = ySum/pointNum;
	}
		
	return TLK_ENONE;
}
static int tlkdrv_ft3168_getGesture(tlkdrv_touch_gesture_t *pGesture)
{
	uint08 en;
	uint08 index;
	uint08 offset;
	uint08 buffer[2+(TLKDRV_TOUCH_FT3168_POINT_MAX<<2)] = {0};

	if(pGesture == nullptr) return -TLK_EPARAM;
	if(!sTlkDrvFt3168Ctrl.isOpen) return -TLK_ENOREADY;

	tlkdrv_ft3168_readData(TLKDRV_FT3168_REG_GESTURE_EN, &en, 1);
	if(en != 1) return -TLK_EFAIL;
	
	tlkdrv_ft3168_readData(TLKDRV_FT3168_REG_GESTURE_OUTPUT_ADDRESS, buffer, 2+(TLKDRV_TOUCH_FT3168_POINT_MAX<<2));
    memset(pGesture, 0, sizeof(tlkdrv_touch_gesture_t));
    pGesture->gestID = buffer[0];
    pGesture->number = buffer[1];
    /* save point data,max:6 */
    for(index = 0; index < TLKDRV_TOUCH_FT3168_POINT_MAX; index++){
        offset = 4 * index + 2;
        pGesture->axisX[index] = (uint16)(((buffer[0 + offset] & 0x0F) << 8) + buffer[1 + offset]);
        pGesture->axisY[index] = (uint16)(((buffer[2 + offset] & 0x0F) << 8) + buffer[3 + offset]);
    }
	return TLK_ENONE;
}
static int tlkdrv_ft3168_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}

static void tlkdrv_ft3168_sendByte(uint08 reg, uint08 byte)
{
	u8 buf[2];

	buf[0] = reg;
	buf[1] = byte;
	i2c_master_write(TLKDRV_FT3168_I2C_SLAVE_ADDR, buf, sizeof(buf));
}

static void tlkdrv_ft3168_readData(uint08 reg, uint08 *pBuff, uint08 buffLen)
{
	i2c_master_write_read(TLKDRV_FT3168_I2C_SLAVE_ADDR, &reg, 1, pBuff, buffLen);
}




#endif //#if (TLKDRV_TOUCH_FT3168_ENABLE)


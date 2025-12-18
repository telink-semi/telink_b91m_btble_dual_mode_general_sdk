/********************************************************************************************************
 * @file	tlkdrv_charge.c
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
#include "tlkdrv/ext/charge/tlkdrv_charge.h"
#include "drivers.h"

#define	TLKDRV_CHARGE_GET_BIT(x, bit)	((x & (1 << bit)) >> bit)


static void tlkdrv_charge_powerInit(void);
static void tlkdrv_charge_iicInit(void);
static uint08 tlkdrv_charge_readRegister(uint08 registerAddr);
static void tlkdrv_charge_writeRegister(uint08 registerAddr, uint08 data);


void tlkdrv_charge_init(void)
{
	tlkdrv_charge_powerInit();
	tlkdrv_charge_iicInit();
}

uint16 tlkdrv_charge_getVBat(void)
{
    uint16 vBat;

    vBat = tlkdrv_charge_readRegister(TLKDRV_CHARGE_VBAT_ADDR_H) << 8 | \
                                tlkdrv_charge_readRegister(TLKDRV_CHARGE_VBAT_ADDR_L);
    return vBat;
}

uint16 tlkdrv_charge_getVBus(void)
{
    uint16 vBus;

    vBus = tlkdrv_charge_readRegister(TLKDRV_CHARGE_VBUS_ADDR_H) << 8 | \
                                tlkdrv_charge_readRegister(TLKDRV_CHARGE_VBUS_ADDR_L);
    return vBus;
}

uint08 tlkdrv_charge_getState(void)
{
    int i;
    uint08 chargeState;
    uint08 tempBuffer[3];

    chargeState = tlkdrv_charge_readRegister(TLKDRV_CHARGE_STATUS_ADDR);

	for (i = 0; i < 3; i++) {
		tempBuffer[i] = TLKDRV_CHARGE_GET_BIT(chargeState, i);
	}
	chargeState = tempBuffer[0] | tempBuffer[1] << 1 | tempBuffer[2] << 2;

    return chargeState;
}


static void tlkdrv_charge_powerInit(void)
{
	gpio_function_en(TLKDRV_CHARGE_EN_PIN);
	gpio_output_en(TLKDRV_CHARGE_EN_PIN);
	gpio_set_output_en(TLKDRV_CHARGE_EN_PIN, 1);
	gpio_write(TLKDRV_CHARGE_EN_PIN, 0);
	
	gpio_function_en(TLKDRV_CHARGE_ILIMIT1_PIN);
	gpio_output_en(TLKDRV_CHARGE_ILIMIT1_PIN);
	gpio_set_output_en(TLKDRV_CHARGE_ILIMIT1_PIN, 0);
	gpio_write(TLKDRV_CHARGE_ILIMIT1_PIN, 0);
	
	gpio_function_en(TLKDRV_CHARGE_ILIMIT2_PIN);
	gpio_output_en(TLKDRV_CHARGE_ILIMIT2_PIN);
	gpio_set_output_en(TLKDRV_CHARGE_ILIMIT2_PIN, 1);
	gpio_write(TLKDRV_CHARGE_ILIMIT2_PIN, 1);
}
static void tlkdrv_charge_iicInit(void)
{
	i2c_master_init();
	i2c_set_master_clk((unsigned char)(sys_clk.pclk * 1000 * 1000 / (4 * 200000) ));	// set i2c frequency 200K.
	i2c_set_pin(TLKDRV_CHARGE_SDA_PIN, TLKDRV_CHARGE_SCL_PIN);

	tlkdrv_charge_writeRegister(0x01, 0x54);
	tlkdrv_charge_writeRegister(0x02, 0x84);
	tlkdrv_charge_writeRegister(0x03, 0x25);
	tlkdrv_charge_writeRegister(0x04, 0x21);
	tlkdrv_charge_writeRegister(0x05, 0x8D);
	tlkdrv_charge_writeRegister(0x06, 0x2F);
	tlkdrv_charge_writeRegister(0x07, 0x02);
	tlkdrv_charge_writeRegister(0x08, 0x0A);
	tlkdrv_charge_writeRegister(0x15, 0xB0);
}

static void tlkdrv_charge_writeRegister(uint08 registerAddr, uint08 data)
{
	uint08 txBuffer[2];

	txBuffer[0] = registerAddr;
	txBuffer[1] = data;
	i2c_master_write(TLKDRV_CHARGE_ADDR, (unsigned char *)txBuffer, 2);
}

static uint08 tlkdrv_charge_readRegister(uint08 registerAddr)
{
	uint08 data = 0;

	i2c_master_write(TLKDRV_CHARGE_ADDR, &registerAddr, 1);
	i2c_master_read(TLKDRV_CHARGE_ADDR, &data, 1);

	return data;
}


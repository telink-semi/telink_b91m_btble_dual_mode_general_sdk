/********************************************************************************************************
 * @file     ext_uart1_dbg.c
 *
 * @brief    This is the source file for BTBLE SDK
 *
 * @author	 BTBLE GROUP
 * @date         2,2022
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#include "clock.h"
#include "compiler.h"
#include "ext_driver/ext_uart1_dbg.h"
#include "reg_include/register.h"
#include "stimer.h"
#include "uart.h"
#include "string.h"
#if (UART_DEBUG_EN)
extern sys_clk_t sys_clk;
extern unsigned char uart_dma_tx_chn[];
extern dma_config_t uart_tx_dma_config[];
_attribute_retention_code_ void uart1_low_power_init ()
{
#if	HYTERA_MODULE_EN
	return;
#endif
	uart_set_pin(UART1_TX_PE0, UART1_RX_PE2);

	uart_reset(UART1);

   	reg_uart_ctrl1(UART1)  &= ~(FLD_UART_PARITY_ENABLE | FLD_UART_STOP_SEL);
    reg_uart_ctrl1(UART1)  |= UART_STOP_BIT_ONE;

	////////// 12M(PCLK) / 2 / 6 = 1 M baud rte
    //reg_uart_ctrl0(UART1) = sys_clk.cclk == 64 ? 7 : sys_clk.cclk == 48 ? 5 : 3; // divide by 6

    reg_uart_ctrl0(UART1) = sys_clk.cclk == 64 ? 15 : sys_clk.cclk == 48 ? 11 : 7; // divide by 6
    //reg_uart_ctrl0(UART1) = sys_clk.cclk == 64 ? 7 : 3; // divide by 6
    reg_uart_clk_div(UART1) = (1 | FLD_UART_CLK_DIV_EN); //set div_clock

	uart_dma_tx_chn[UART1]=DMA5;

	BM_CLR(reg_dma_ctrl(DMA5),BIT_RNG(4,31));

	reg_dma_ctrl(DMA5) |= (*(u32*)(&uart_tx_dma_config[UART1])) << 4;
}

_attribute_retention_code_ void uart1_low_power_print (u8 *p, int len)
{
	dma_set_address(uart_dma_tx_chn[UART1],(u32)convert_ram_addr_cpu2bus(p),reg_uart_data_buf_adr(UART1));
	dma_set_size(uart_dma_tx_chn[UART1],len,DMA_WORD_WIDTH);
	dma_chn_en(uart_dma_tx_chn[UART1]);
	delay_us (len * 10);
}

void uart1_low_power_print_data (u32 d, int len)
{
	dma_set_address(uart_dma_tx_chn[UART1],(u32)convert_ram_addr_cpu2bus(&d),reg_uart_data_buf_adr(UART1));
	dma_set_size(uart_dma_tx_chn[UART1], len, DMA_WORD_WIDTH);
	dma_chn_en(uart_dma_tx_chn[UART1]);
	delay_us (len * 10);
}


void uart1_debug_str(u8 *p, int len)
{
	u8 tbuf[256];
	tmemcpy(tbuf,p,len);

	uart1_low_power_print(tbuf,len);
}

void uart1_debug_num(unsigned char c) {
	unsigned char nib = c >> 4;
		if (nib > 9)	nib = nib + 87;
		else		nib = nib + 48;
		uart1_low_power_print_data (nib,1);

		nib = c & 15;
		if (nib > 9)	nib = nib + 87;
		else		nib = nib + 48;
		uart1_low_power_print_data (nib,1);
}


/**
 * @brief      This function serves to format string.
 * @param[in]  w  -  a integer need to print
 * @return     none.
 */
void uart1_debug_hex(unsigned int w,int len) {
	int i;
	int c = w;

	for(i=len-1;i>=0;i--)
	{
		c = w >>(i*8);
		uart1_debug_num(c);
	}
}

void uart1_debug_hex_array(const u8 *p,unsigned char * buffer, int len)
{
	int i;
	uart1_debug_str((u8 *)p,8);
	for(i=0;i<len;i++)
	{
		uart1_debug_hex(buffer[i],1);
		uart1_debug_str((u8 *)",",1);
	}
}
#endif

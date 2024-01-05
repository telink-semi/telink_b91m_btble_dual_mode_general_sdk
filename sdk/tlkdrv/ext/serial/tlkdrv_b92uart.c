/********************************************************************************************************
 * @file	tlkdrv_b92uart.c
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
#if (TLK_DEV_SERIAL_ENABLE)
#include "tlkdrv_serial.h"
#if (TLKDRV_SERIAL_B92_UART_ENABLE)
#include "tlkdrv_b92uart.h"
#include "drivers.h"


extern void tlkdrv_serial_irqErrDone(uint08 port);
extern void tlkdrv_serial_irqTxDoneWithDma(uint08 port);
extern void tlkdrv_serial_irqTxDoneWithoutDma(uint08 port);
extern void tlkdrv_serial_irqRxDoneWithDma(uint08 port, bool isDma);
extern void tlkdrv_serial_irqRxDoneWithoutDma(uint08 port);



int tlkdrv_b92uart_open(uint08 port, uint32 baudRate, uint16 txPin,
	uint16 rxPin, uint08 txDma, uint08 rxDma, uint16 rtsPin, uint16 ctsPin)
{
	unsigned short div;
	unsigned char bwpc;

	if(port != UART0 && port != UART1){
		return -TLK_EPARAM;
	}
	if(txPin == 0 && rxPin == 0){
		return -TLK_EPARAM;
	}

	if(txPin == 0 && txDma != 0) txDma = 0;
	if(rxPin == 0 && rxDma != 0) rxDma = 0;

	if(port == UART0) plic_interrupt_disable(IRQ19_UART0);
	else if(port == UART1) plic_interrupt_disable(IRQ18_UART1);
	
	uart_reset(port);
	uart_set_pin(port, txPin, rxPin);
	
	uart_cal_div_and_bwpc(baudRate, sys_clk.pclk*1000*1000, &div, &bwpc);
	uart_init(port, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);	
	uart_set_rx_timeout(port, bwpc, 12, UART_BW_MUL2);//uart use rx_timeout to decide the end of a packet,UART_BW_MUL2 means rx_timeout is 2-byte.

	#if (TLKDRV_SERIAL_FLOW_CTRL_ENABLE)
	if(ctsPin != 0){
		uart_cts_config(port, ctsPin, TLKDRV_B92_UART_STOP_VOLT);
		uart_set_cts_en(port);
	}
	if(rtsPin != 0){
		uart_set_rts_en(port);
		uart_rts_config(port, rtsPin, TLKDRV_B92_UART_RTS_INVERT, TLKDRV_B92_UART_RTS_MODE);
		uart_rts_trig_level_auto_mode(port, TLKDRV_B92_UART_RTS_THRESH);
		if(rxDma != 0){
			uart_rts_stop_rxtimeout_en(port);
    		uart_rxdone_rts_en(port);
		}
	}
	#endif
	
	uart_clr_irq_status(port, UART_TXDONE_IRQ_STATUS);
	uart_clr_irq_status(port, UART_RXDONE_IRQ_STATUS);
	uart_clr_irq_status(port, UART_RXBUF_IRQ_STATUS);
	uart_clr_irq_status(port, UART_TXBUF_IRQ_STATUS);
	uart_set_irq_mask(port, UART_ERR_IRQ_MASK);
	uart_rx_irq_trig_level(port, 1);
//	uart_tx_irq_trig_level(port, 1);

	if(rxDma == 0){
		uart_set_irq_mask(port, UART_RX_IRQ_MASK);
	}else{
		uart_set_rx_dma_config(port, rxDma);
		dma_clr_irq_mask(rxDma, TC_MASK|ABT_MASK|ERR_MASK);
		uart_set_irq_mask(port, UART_RXDONE_MASK);
	}
	if(txDma != 0){
		uart_set_tx_dma_config(port, txDma);
		dma_clr_irq_mask(txDma, TC_MASK|ABT_MASK|ERR_MASK);
		uart_set_irq_mask(port, UART_TXDONE_MASK);
	}
	uart_set_irq_mask(port, UART_ERR_IRQ_MASK);

	if(port == UART0){
		if(txDma != 0 || rxDma != 0){
			plic_set_priority(IRQ19_UART0, IRQ_PRI_LEV2);
			plic_interrupt_enable(IRQ19_UART0);			
		}else if(rxPin != 0){
			plic_set_priority(IRQ19_UART0, IRQ_PRI_LEV3);
			plic_interrupt_enable(IRQ19_UART0);
		}
	}else if(port == UART1){
		if(txDma != 0 || rxDma != 0){
			plic_set_priority(IRQ18_UART1, IRQ_PRI_LEV2);
			plic_interrupt_enable(IRQ18_UART1);			
		}else if(rxPin != 0){
			plic_set_priority(IRQ18_UART1, IRQ_PRI_LEV3);
			plic_interrupt_enable(IRQ18_UART1);
		}
	}
	#if (TLKDRV_SERIAL_FLOW_CTRL_ENABLE)
	if(rtsPin != 0 && rxDma != 0){
		dma_set_irq_mask(rxDma, TC_MASK);
		plic_set_priority(IRQ5_DMA, IRQ_PRI_LEV2);
		plic_interrupt_enable(IRQ5_DMA);
	}
	#endif
	
	return TLK_ENONE;
}

int tlkdrv_b92uart_close(uint08 port, uint08 txDma, uint08 rxDma)
{
	uart_reset(port);
	if(port == UART0) plic_interrupt_disable(IRQ19_UART0);
	else if(port == UART1) plic_interrupt_disable(IRQ18_UART1);
	if(txDma != 0){
		dma_chn_dis(txDma);
		dma_clr_irq_mask(txDma, TC_MASK|ABT_MASK|ERR_MASK);
	}
	if(rxDma != 0){
		dma_chn_dis(rxDma);
		dma_clr_irq_mask(rxDma, TC_MASK|ABT_MASK|ERR_MASK);
	}
	uart_clr_irq_status(port, UART_TXDONE_IRQ_STATUS);
	uart_clr_irq_status(port, UART_RXDONE_IRQ_STATUS);
	uart_clr_irq_status(port, UART_RXBUF_IRQ_STATUS);
	uart_clr_irq_status(port, UART_TXBUF_IRQ_STATUS);
	return TLK_ENONE;
}

int tlkdrv_b92uart_sendWithoutDma(uint08 port, uint08 rtsPin, uint08 *pData, uint16 dataLen)
{
	uart_send(port, pData, dataLen);
	return TLK_ENONE;
}


_attribute_ram_code_sec_ 
void tlkdrv_b92uart_irqHandler(uint08 port)
{
	if(uart_get_irq_status(port, UART_TXBUF_IRQ_STATUS)){ //for no dam
		tlkdrv_serial_irqTxDoneWithoutDma(port);
	}
	
	if(uart_get_irq_status(port, UART_TXDONE_IRQ_STATUS)){ //for dma
		uart_clr_irq_status(port, UART_TXDONE_IRQ_STATUS);
		tlkdrv_serial_irqTxDoneWithDma(port);
	}
	if(uart_get_irq_status(port, UART_RXBUF_IRQ_STATUS)){ //for no dam
		tlkdrv_serial_irqRxDoneWithoutDma(port);
	}
	if(uart_get_irq_status(port, UART_RXDONE_IRQ_STATUS)){ //for dma
		tlkdrv_serial_irqRxDoneWithDma(port, false);
	}
	if(uart_get_irq_status(port, UART_RX_ERR)){
		uart_clr_irq_status(port, UART_RXBUF_IRQ_STATUS);
		// it will clear rx_fifo and rx_err_irq ,rx_buff_irq,so it won't enter rx_buff interrupt.
		uart_reset(port); //clear hardware pointer
		uart_clr_rx_index(port); //clear software pointer
		tlkdrv_serial_irqErrDone(port);
	}
}

#if (TLKDRV_SERIAL_FLOW_CTRL_ENABLE)
_attribute_ram_code_sec_
void tlkdrv_b92uart_dmaIrqHandler(uint08 port, uint08 rxDma)
{
	tlkdrv_serial_irqRxDoneWithDma(port, true);
}
#endif


#endif //#if (TLKDRV_SERIAL_B92_UART_ENABLE)
#endif //#if (TLK_DEV_SERIAL_ENABLE)



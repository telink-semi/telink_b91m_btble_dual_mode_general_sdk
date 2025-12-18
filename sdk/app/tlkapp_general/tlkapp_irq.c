/********************************************************************************************************
 * @file	tlkapp_irq.c
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
#include "types.h"
#include "drivers.h"
#include "tlkapp_config.h"


extern void trap_entry(void);

#if (TLK_DEV_SERIAL_ENABLE)
extern void tlkdrv_serial_dmaIrqHandler(void);
#endif



/******************************************************************************
 * Function: dma_irq_handler
 * Descript: DMA interrupt handler.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
_attribute_retention_code_ void dma_irq_handler(void)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdrv_serial_dmaIrqHandler();
	#endif

//	unsigned char uart_dma_irq_src = reg_dma_tc_isr;
//	plic_interrupt_complete(IRQ5_DMA);
//	/*dma 0*/
//	if((uart_dma_irq_src & FLD_DMA_CHANNEL4_IRQ))
//	{
//		reg_dma_tc_isr |= FLD_DMA_CHANNEL4_IRQ;
//	}
//	/*dma 1*/
//	if(( uart_dma_irq_src &FLD_DMA_CHANNEL5_IRQ))
//	{
//		reg_dma_tc_isr |= FLD_DMA_CHANNEL5_IRQ;
//	}
}

/******************************************************************************
 * Function: IRQ Table and interrupt handler
 * Descript:
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
extern void trap_entry(void);
void  entry_null(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_null(void)
{
//	trap_entry ();
}

void stimer_irq_handler();
void  entry_irq1(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq1(void)
{
	plic_isr (stimer_irq_handler, IRQ1_SYSTIMER);
}


//extern void timer1_irq_handler(void);
void  entry_irq3(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq3(void)
{
//	plic_isr (timer1_irq_handler, IRQ3_TIMER1);
}

extern void timer0_irq_handler(void);
void  entry_irq4(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq4(void)
{
	plic_isr (timer0_irq_handler, IRQ4_TIMER0);
}


extern void dma_irq_handler(void);
void  entry_irq5(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq5(void)
{
	plic_isr (dma_irq_handler, IRQ5_DMA);
}


extern void  usb_endpoint_irq_handler (void);
void  entry_irq11(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq11(void)
{
	plic_isr (usb_endpoint_irq_handler, IRQ11_USB_ENDPOINT);
}

#if(MCU_CORE_TYPE == MCU_CORE_B91)
void zb_dm_irq_handler(void);
void  entry_irq12(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq12(void)
{
	plic_isr (zb_dm_irq_handler, IRQ12_ZB_DM);
}
#endif

void zb_bt_irq_handler(void);
void  entry_irq14(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq14(void)
{
	plic_isr (zb_bt_irq_handler, IRQ14_ZB_BT);

}

void rf_irq_handler(void);
void  entry_irq15(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq15(void)
{
	plic_isr (rf_irq_handler, IRQ15_ZB_RT);

}

extern void uart1_irq_handler(void);
void  entry_irq18(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq18(void)
{
	plic_isr (uart1_irq_handler, IRQ18_UART1);

}

extern void uart0_irq_handler(void);
void  entry_irq19(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq19(void)
{
	plic_isr (uart0_irq_handler, IRQ19_UART0);
}

#if(MCU_CORE_TYPE == MCU_CORE_B92)
//extern void lspi_irq_handler(void);
void  entry_irq22(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq22(void)
{
//	plic_isr (lspi_irq_handler, IRQ22_LSPI);
}
#endif

extern void gpio_irq_handler(void);
void  entry_irq25(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq25(void)
{
	plic_isr (gpio_irq_handler, IRQ25_GPIO);

}

extern void gpio_risc0_irq_handler(void);
void  entry_irq26(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq26(void)
{
	plic_isr (gpio_risc0_irq_handler, IRQ26_GPIO2RISC0);

}

extern void gpio_risc1_irq_handler(void);
void  entry_irq27(void) __attribute__ ((interrupt ("machine") , aligned(4)));
_attribute_retention_code_ void  entry_irq27(void)
{
	plic_isr (gpio_risc1_irq_handler, IRQ27_GPIO2RISC1);

}





typedef   void   (* interrupt_ptr_t)(void);
#if(MCU_CORE_TYPE == MCU_CORE_B91)
__attribute__((section(".retention_data"))) interrupt_ptr_t   g_interrupt_tbl[64] __attribute__ ((aligned (256))) = {
		trap_entry,		entry_irq1, 	entry_null, 	entry_irq3, 		entry_irq4, 	entry_irq5, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_irq11, 		entry_null, 	entry_null, 	entry_irq14, 	entry_irq15,
		entry_null,		entry_null, 	entry_irq18, 	entry_irq19, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_irq25, 	entry_irq26, 	entry_irq27, 		entry_null, 	entry_null, 	entry_null, 	entry_null,

		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
};
#endif
#if(MCU_CORE_TYPE == MCU_CORE_B92)
__attribute__((section(".retention_data"))) interrupt_ptr_t   g_interrupt_tbl[64] __attribute__ ((aligned (256))) = {
		trap_entry,		entry_irq1, 	entry_null, 	entry_irq3, 		entry_irq4, 	entry_irq5, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_irq11, 		entry_null, 	entry_null, 	entry_irq14, 	entry_irq15,
		entry_null,		entry_null, 	entry_irq18, 	entry_irq19, 		entry_null, 	entry_null, 	entry_irq22, 	entry_null,
		entry_null,		entry_irq25, 	entry_irq26, 	entry_irq27, 		entry_null, 	entry_null, 	entry_null, 	entry_null,

		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
		entry_null,		entry_null, 	entry_null, 	entry_null, 		entry_null, 	entry_null, 	entry_null, 	entry_null,
};
#endif

/******************************************************************************
 * Function: tlkapp_irq_init
 * Descript: Set the start address of interrupt ISR function table mapping to
 *           MCU hardware interrupt entry
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_irq_init(void)
{
	/*set the start address of interrupt ISR function table 
	mapping to  MCU hardware interrupt entry*/
	write_csr(NDS_MTVEC, (unsigned long)g_interrupt_tbl);
}


/********************************************************************************************************
 * @file	plic.c
 *
 * @brief	This is the source file for B92
 *
 * @author	Driver Group
 * @date	2020
 *
 * @par		Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
 *			All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRANTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapi/tlkapi_debug.h"
#include "drivers.h"


#define TLKDRV_PLIC_DBG_FLAG      ((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_SYS << 16) | TLK_DEBUG_DBG_FLAG_ALL)


#if IRQ_DEBUG_ENABLE
_attribute_data_retention_sec_  volatile EXCEPT_HANDLER_S_T except_handler_b;
#endif 

_attribute_data_retention_sec_ volatile EXCEPT_HANDLER_S_T except_handler_e;
_attribute_data_retention_sec_ unsigned char g_plic_preempt_en=1;
/**
 * @brief    This function serves to config plic when enter some function process such as flash.
 * @param[in]   preempt_en - 1 can disturb by interrupt, 0 can't disturb by interrupt.
 * @param[in]   threshold  - interrupt threshold.when the interrupt priority> interrupt threshold,the function process will be disturb by interrupt.
 * @return  none
*/

volatile uint32 sCoreCriticalCount0 = 0;

#define TLK_CORE_CRITICAL_THRESHOLD      1




_attribute_ram_code_sec_noinline_ 
unsigned int plic_enter_critical_sec(unsigned char preempt_en ,unsigned char threshold)
{
	unsigned int r = 0;
	if(g_plic_preempt_en && preempt_en && threshold != 0)
	{
		unsigned int r_inq = core_disable_interrupt();
		unsigned char thrd = reg_irq_threshold & 0xFF;
		sCoreCriticalCount0 ++;
		if(thrd < threshold){
			plic_set_threshold(threshold);
		}else{
			
		}
		core_restore_interrupt(r_inq);
	}
	else
	{
	   r = core_disable_interrupt();
	}
	return r ;
}

/**
 * @brief    This function serves to config plic when exit some function process such as flash.
 * @param[in]   preempt_en - 1 can disturb by interrupt, 0 can disturb by interrupt.
 * @param[in]    r         - the value of mie register to restore.
 * @return  none
*/
_attribute_ram_code_sec_noinline_ void  plic_exit_critical_sec(unsigned char preempt_en ,unsigned int r)
{
	if(g_plic_preempt_en && preempt_en)
	{
		unsigned int r_inq = core_disable_interrupt();
		if(sCoreCriticalCount0 == 1){
			plic_set_threshold(0);
		}
		if(sCoreCriticalCount0 != 0) sCoreCriticalCount0--;
		core_restore_interrupt(r_inq);
	}
	else
	{
		core_restore_interrupt(r);
	}
}


_attribute_ram_code_sec_noinline_
void default_irq_handler(void)
{
	//printf("Default interrupt handler\n");
}
void  __attribute__((unused)) stimer_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) analog_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) timer1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) timer0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) dma_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) bmc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_ctrl_ep_setup_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_ctrl_ep_data_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_ctrl_ep_status_irq_handler(void)  __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_ctrl_ep_setinf_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_endpoint_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) zb_dm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) rf_ble_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) zb_bt_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) rf_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) pwm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) pke_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) uart1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) uart0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) audio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) i2c_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) hspi_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) pspi_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_pwdn_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) gpio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) gpio_risc0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) gpio_risc1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) soft_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) npe_bus0_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus3_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus4_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_250us_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_reset_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) npe_bus7_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus8_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) npe_bus13_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus14_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) npe_bus15_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus17_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) npe_bus21_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus22_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus23_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus24_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus25_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus26_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus27_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus28_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

void __attribute__((unused)) npe_bus29_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus30_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_bus31_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) npe_comb_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) pm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) eoc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));




_attribute_bt_data_retention_ volatile int irq_stack_overflow=0;

_attribute_retention_code_ void irq_stack_check(uint32_t sp)
{
	volatile uint32_t* sp_ptr = (uint32_t*)sp;

	if(sp_ptr[0]!=0x55555555)
	{
		irq_stack_overflow++;
		//log_tick(SL_APP_SP_EN,SLET_sp_overflow);
	}
}


#if !(TLK_CFG_OS_ENABLE)

extern void tlkdbg_handler(void);
/**
 * @brief  exception handler.this defines an exception handler to handle all the platform pre-defined exceptions.
 * @return none
 */
_attribute_retention_code_ __attribute__((weak)) void except_handler(long cause, long epc,long* reg)
{
	core_disable_interrupt();

	#if (TLK_CFG_DBG_ENABLE)
	tlkdbg_delayForPrint(10000);
	#endif
	
	except_handler_e.pc = epc;
	except_handler_e.lr = reg[0];
	except_handler_e.sp = reg[1];
	except_handler_e.gp = reg[2];
	except_handler_e.cause = cause;

	#if (TLK_CFG_DBG_ENABLE)
	for(volatile unsigned int i = 0; i < 20; i++)
	{		
		tlkdbg_sendU32s(TLKDRV_PLIC_DBG_FLAG,
					"cause",
					except_handler_e.pc, 
					except_handler_e.lr, 
					except_handler_e.sp, 
					except_handler_e.cause);
		tlkdbg_sendU32s(TLKDRV_PLIC_DBG_FLAG,
					"PC,LR,SP,GP",
					except_handler_e.pc, 
					except_handler_e.lr, 
					except_handler_e.sp, 
					except_handler_e.gp);
		tlkdbg_delayForPrint(10000);
	}
	#endif
	while(1){
		#if (TLK_CFG_DBG_ENABLE)
		tlkdbg_handler();
		#endif
		asm("nop");
	}
}

_attribute_retention_code_  __attribute__((weak)) void trap_entry(void) __attribute__ ((interrupt ("machine") , aligned(4)));
void trap_entry(void)
{
	register long ra asm("x1"), sp asm("x2"), gp asm("x3");
	long mcause = read_csr(NDS_MCAUSE);
	long mepc = read_csr(NDS_MEPC);
	long reg[3];
	reg[0] = ra;
	reg[1] = sp;
	reg[2] = gp;
	except_handler(mcause,mepc,reg);
}
#endif
/**
 * @brief system timer interrupt handler.
 * @return none
 */



/// @} DRIVERS

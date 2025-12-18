/********************************************************************************************************
 * @file     ext_core.c
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
#include "compiler.h"
#include "core.h"
#include "lib/include/plic.h"
#include "ext_pm.h"
#include "ext_btreg.h"
#include "ext_core.h"
#include "tlk_config.h"



#if (TLK_STK_BT_ENABLE)
extern void btc_prefetch_set();
#endif

_attribute_data_retention_sec_ volatile unsigned char  sCoreIrqIsEn = 0;
_attribute_data_retention_sec_ volatile unsigned char  sCoreIrqIsSet = 0;
_attribute_data_retention_sec_ volatile unsigned short sCoreIrqCount = 0;
_attribute_data_retention_sec_ volatile unsigned char  sCoreCriticalThrd = 0;
_attribute_data_retention_sec_ volatile unsigned short sCoreCriticalCount = 0;



_attribute_ram_code_sec_noinline_
void core_reboot(void)
{
	sys_reboot();
}

/******************************************************************************
 * Function: tlksys_set_mcuMode
 * Descript: Set the work mode of the MCU.
 * Params:
 *     @mcuMode[IN]--The work mode of the MCU. Refer pm_sleep_mode_e.
 *     @wakeSrc[IN]--The wakeup source of the the MCU. Refer pm_sleep_wakeup_src_e.
 * Return: The number of actual UNICODE characters encoded.
 ********************************************************s*********************/
void tlksys_set_mcuMode(unsigned int mcuMode, unsigned int wakeupSrc)
{
	cpu_sleep_wakeup(mcuMode, wakeupSrc, 0);
}

void tlksys_switch_clock(SYS_CHIP_CLOCK_ENUM clock)
{
#if (TLK_STK_BT_ENABLE)
	sys_pll_div_to_cclk_e  cclk_div;
	sys_cclk_div_to_hclk_e hclk_div;
#endif
	
	/* HCLK & PCLK can not change, because some important peripherals clock is derive from HCLK & PCLK,
	 * such as UART/IIC/SPI. If HCLK & PCLK are changed, make sure that peripherals clock should re_init */
	if(clock == SYS_CHIP_CLOCK_96M){
		CCLK_96M_HCLK_48M_PCLK_24M;
#if (TLK_STK_BT_ENABLE)
		cclk_div = PLL_DIV2_TO_CCLK;
		hclk_div = CCLK_DIV2_TO_HCLK;
#endif
	}else if(clock == SYS_CHIP_CLOCK_48M){
	    CCLK_48M_HCLK_48M_PCLK_24M;
#if (TLK_STK_BT_ENABLE)
		cclk_div = PLL_DIV4_TO_CCLK;
		hclk_div = CCLK_DIV1_TO_HCLK;
#endif
	}else if(clock == SYS_CHIP_CLOCK_32M){
		CCLK_32M_HCLK_32M_PCLK_16M;
#if (TLK_STK_BT_ENABLE)
		cclk_div = PLL_DIV6_TO_CCLK;
		hclk_div = CCLK_DIV1_TO_HCLK;
#endif
	}else if(clock == SYS_CHIP_CLOCK_24M){
		CCLK_24M_HCLK_24M_PCLK_24M;
#if (TLK_STK_BT_ENABLE)
		cclk_div = PLL_DIV8_TO_CCLK;
		hclk_div = CCLK_DIV1_TO_HCLK;
#endif
	}else if(clock == SYS_CHIP_CLOCK_16M){
		CCLK_16M_HCLK_16M_PCLK_16M;
#if (TLK_STK_BT_ENABLE)
		cclk_div = PLL_DIV12_TO_CCLK;
		hclk_div = CCLK_DIV1_TO_HCLK;
#endif
	}else{
		//consider later
		return;
	}

#if (TLK_STK_BT_ENABLE)
	int zclk = 24/(cclk_div*hclk_div) - 1;
	reg_zb_mst_mod = (reg_zb_mst_mod & 0xf0) | zclk;	//baseband clock
	btc_prefetch_set();
#endif
}

/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */
void tlksys_interrupt_enable(void)
{
	sCoreIrqIsEn  = 1;
	sCoreIrqIsSet = 0;
	sCoreIrqCount = 0;
	sCoreCriticalThrd = 0;
	sCoreCriticalCount = 0;
	plic_preempt_feature_en(CORE_PREEMPT_PRI_MODE3);
	core_interrupt_enable();
	fence_iorw;
}
void tlksys_close_interrupt(void)
{
	core_mie_disable(FLD_MIE_MEIE);
}

/******************************************************************************
 * Function: tlksys_disable_interrupt, tlksys_restore_interrupt
 * Descript: Disable and restore the system interrupt.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlksys_disable_interrupt()" and "tlksys_restore_interrupt()" need to
 *       be used in pairs, and "tlksys_disable_interrupt()" needs to be called
 *       first.
 *****************************************************************************/
_attribute_retention_code_ 
void tlksys_disable_interrupt(void)
{
	if(sCoreIrqIsEn){
		if(sCoreIrqCount == 0){
			if((read_csr(NDS_MSTATUS) & FLD_MSTATUS_MIE) != 0){
				clear_csr(NDS_MSTATUS, FLD_MSTATUS_MIE);//global interrupts disable
				fence_iorw; //ensure that MIE bit of mstatus reg is cleared at hardware level
				sCoreIrqIsSet = 1;
			}
		}
		sCoreIrqCount ++;
	}
}
_attribute_retention_code_ 
void tlksys_restore_interrupt(void)
{
	if(sCoreIrqIsEn && sCoreIrqCount != 0){
		sCoreIrqCount --;
		if(sCoreIrqCount == 0 && sCoreIrqIsSet){
			set_csr(NDS_MSTATUS, FLD_MSTATUS_MIE); //global interrupts enable
			fence_iorw;
			sCoreIrqIsSet = 0;
		}
	}
}

/******************************************************************************
 * Function: tlksys_enter_critical, tlksys_leave_critical
 * Descript: Enter and push out the system critical region.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlksys_enter_critical()" and "tlksys_enter_critical()" need to be used
 *       in pairs, and "tlksys_enter_critical()" needs to be called first.
 *     2.The current critical section of the system is 1. Once you enter the
 *       critical section, interrupts with an interrupt priority lower than or
 *       equal to 1 will not come in.
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
void tlksys_enter_critical(void)
{
	tlksys_disable_interrupt();
	unsigned char thrd = reg_irq_threshold & 0xFF;
	sCoreCriticalCount ++;
	if(sCoreCriticalCount == 1){
		sCoreCriticalThrd = thrd;
	}
	if(thrd < TLK_CFG_CRITICAL_THRESHOLD){
		plic_set_threshold(TLK_CFG_CRITICAL_THRESHOLD);
	}
	tlksys_restore_interrupt();
}
_attribute_ram_code_sec_noinline_
void tlksys_leave_critical(void)
{
	tlksys_disable_interrupt();
	if(sCoreCriticalCount == 1){
		plic_set_threshold(sCoreCriticalThrd);
		sCoreCriticalThrd = 0;
	}
	if(sCoreCriticalCount != 0) sCoreCriticalCount--;
	tlksys_restore_interrupt();
}




_attribute_ram_code_sec_noinline_
void default_irq_handler(void)
{
	//printf("Default interrupt handler\n");
}
void __attribute__((unused)) stimer_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
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

void __attribute__((unused)) usb_250us_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) usb_reset_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));


void __attribute__((unused)) pm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void __attribute__((unused)) eoc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));



#include "tlk_config.h"


#if !(TLK_CFG_OS_ENABLE)
/**
 * @brief  exception handler.this defines an exception handler to handle all the platform pre-defined exceptions.
 * @return none
 */
_attribute_retention_code_ void except_handler(long cause, long epc,long* reg)
{
	
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



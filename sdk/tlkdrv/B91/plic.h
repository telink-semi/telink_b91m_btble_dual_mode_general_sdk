/********************************************************************************************************
 * @file     plic.h
 *
 * @brief    This is the header file for BTBLE SDK
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

/**	@page PLIC
 *
 *	Introduction
 *	===============
 *   platform-level interrupt controller (PLIC)
 *
 *	API Reference
 *	===============
 *	Header File: plic.h
 */

#ifndef  INTERRUPT_H
#define  INTERRUPT_H
#include "core.h"
#include "compiler.h"
#include "bit.h"
#include "reg_include/register.h"


#define IRQ_DEBUG_ENABLE			        0


typedef struct
{
	unsigned char preempt_en;
	unsigned char threshold;
}preempt_config_t ;

typedef struct
{
	unsigned char preempt_en;
	unsigned char threshold;
}preempt_config_st ;


extern unsigned char  g_plic_preempt_en;

#define 	flash_is_idle()			(0 == (g_plic_preempt_en & (1UL<<1)))

enum{
	INTCNTL_PRIO_LOW,
	INTCNTL_PRIO_HIGH,
};

typedef enum{
	IRQ0_EXCEPTION ,
	IRQ1_SYSTIMER,
	IRQ2_ALG,
	IRQ3_TIMER1,
	IRQ4_TIMER0,
	IRQ5_DMA,
	IRQ6_BMC,
	IRQ7_USB_CTRL_EP_SETUP,
	IRQ8_USB_CTRL_EP_DATA,
	IRQ9_USB_CTRL_EP_STATUS,
	IRQ10_USB_CTRL_EP_SETINF,
	IRQ11_USB_ENDPOINT,
	IRQ12_ZB_DM,
	IRQ13_ZB_BLE,
	IRQ14_ZB_BT,
	IRQ15_ZB_RT,
	IRQ16_PWM,
	IRQ17_PKE,
	IRQ18_UART1,
	IRQ19_UART0,
	IRQ20_DFIFO,
	IRQ21_I2C,
	IRQ22_SPI_AHB,
	IRQ23_SPI_APB,
	IRQ24_USB_PWDN,
	IRQ25_GPIO,
	IRQ26_GPIO2RISC0,
	IRQ27_GPIO2RISC1,
	IRQ28_SOFT,

	IRQ29_NPE_BUS0,
	IRQ30_NPE_BUS1,
	IRQ31_NPE_BUS2,
	IRQ32_NPE_BUS3,
	IRQ33_NPE_BUS4,

	IRQ34_USB_250US,
	IRQ35_USB_RESET,
	IRQ36_NPE_BUS7,
	IRQ37_NPE_BUS8,

	IRQ42_NPE_BUS13=42,
	IRQ43_NPE_BUS14,
	IRQ44_NPE_BUS15,

	IRQ46_NPE_BUS17=46,

	IRQ50_NPE_BUS21=50,
	IRQ51_NPE_BUS22,
	IRQ52_NPE_BUS23,
	IRQ53_NPE_BUS24,
	IRQ54_NPE_BUS25,
	IRQ55_NPE_BUS26,
	IRQ56_NPE_BUS27,
	IRQ57_NPE_BUS28,
	IRQ58_NPE_BUS29,
	IRQ59_NPE_BUS30,
	IRQ60_NPE_BUS31,

	IRQ61_NPE_COMB,
	IRQ62_PM_TM,
	IRQ63_EOC,

} irq_source_e;

typedef enum{
	IRQ_PRI_LEV0,
	IRQ_PRI_LEV1,
	IRQ_PRI_LEV2,
	IRQ_PRI_LEV3,
}irq_priority_e;


/**
 * @brief    This function serves to set plic feature.
 * @param[in]   feature - preemptive priority interrupt feature and the vector mode.
 * @return  none
 */
static inline void plic_set_feature (feature_e feature)
{
	reg_irq_feature = feature;//enable vectored in PLIC
}

/**
 * @brief    This function serves to enable preemptive priority interrupt feature.
 * @return  none
 */
static inline void plic_preempt_feature_en (void)
{
	reg_irq_feature |= FLD_FEATURE_PREEMPT_PRIORITY_INT_EN;
	 g_plic_preempt_en =1;
}

/**
 * @brief    This function serves to enable preemptive priority interrupt feature.
 * @return  none
 */
static inline void plic_preempt_feature_dis (void)
{
	reg_irq_feature &=(~ FLD_FEATURE_PREEMPT_PRIORITY_INT_EN);
	g_plic_preempt_en=0;
}


/**
 * @brief    This function serves to set plic pending.
 * @param[in]  src - interrupt source.
 * @return  none
 */
static inline void plic_set_pending (irq_source_e src)
{
	reg_irq_pending(src)=BIT(src%32);
}

/**
 * @brief    This function serves to set Priority Threshold,Only active interrupts with priorities strictly greater than the threshold will cause interrupt.
 * @param[in]   threshold -  threshold level.
 * @return  none
 */
static inline void plic_set_threshold (unsigned char threshold)
{
	reg_irq_threshold=threshold;
	NDS_FENCE_IORW;
}

/**
 * @brief    This function serves to set preemptive priority level,The priority value 0 is reserved to mean "never interrupt".
 * the larger the priority value, the higher the interrupt priority.
 * @param[in]   src- interrupt source.
 * @param[in]   priority-  priority level.
 * @return  none
 */
static inline void plic_set_priority (irq_source_e src, irq_priority_e priority)
{
	reg_irq_src_priority(src)=priority;
	NDS_FENCE_IORW;
}


/**
 * @brief    This function serves to enable plic interrupt source.
 * @param[in]   src - interrupt source.
 * @return  none
 */
static inline void plic_interrupt_enable(irq_source_e  src)
{
	reg_irq_src(src) |= BIT(src%32);
	NDS_FENCE_IORW;
}

/**
 * @brief    This function serves to disable plic interrupt source.
 * @param[in]   src - interrupt source.
 * @return  none
 */
static inline void plic_interrupt_disable(irq_source_e  src)
{
	reg_irq_src(src) &= (~ BIT(src%32));
	NDS_FENCE_IORW;
}

/**
 * @brief    This function serves to clear interrupt source has completed.
 * @param[in] src - interrupt source.
 * @return  none
 */
static inline void plic_interrupt_complete(irq_source_e  src)
{
	reg_irq_done = src;
}

/**
 * @brief    This function serves to claim  interrupt source.
 * @param[in]  src - interrupt source.
 * @return  none
 */
static inline  unsigned int plic_interrupt_claim(void)
{
	return reg_irq_done;
}



/**
 * @brief    This function serves to config plic when enter some function process such as flash.
 * @param[in]   preempt_en - 1 can disturb by interrupt, 0 can't disturb by interrupt.
 * @param[in]   threshold  - interrupt threshold.when the interrupt priority> interrupt threshold,the function process will be disturb by interrupt.
 * @return  none
*/
_attribute_ram_code_sec_noinline_ unsigned int plic_enter_critical_sec(unsigned char preempt_en ,unsigned char threshold);



/**
 * @brief    This function serves to config plic when exit some function process such as flash.
 * @param[in]   preempt_en - 1 can disturb by interrupt, 0 can disturb by interrupt.
 * @param[in]    r         - the value of mie register to restore.
 * @return  none
*/
_attribute_ram_code_sec_noinline_   void  plic_exit_critical_sec(unsigned char preempt_en ,unsigned int r);

/*******************************      BLE/BT Use     ******************************/
typedef enum{//todo
	FLD_IRQ_EXCEPTION_EN ,
	FLD_IRQ_SYSTIMER_EN,
	FLD_IRQ_ALG_EN,
	FLD_IRQ_TIMER1_EN,
	FLD_IRQ_TIMER0_EN,
	FLD_IRQ_DMA_EN,
	FLD_IRQ_BMC_EN,
	FLD_IRQ_USB_CTRL_EP_SETUP_EN,
	FLD_IRQ_USB_CTRL_EP_DATA_EN,
	FLD_IRQ_USB_CTRL_EP_STATUS_EN,
	FLD_IRQ_USB_CTRL_EP_SETINF_EN,
	FLD_IRQ_USB_ENDPOINT_EN,
	FLD_IRQ_ZB_DM_EN,
	FLD_IRQ_ZB_BLE_EN,
	FLD_IRQ_ZB_BT_EN,
	FLD_IRQ_ZB_RT_EN,
	FLD_IRQ_PWM_EN,
	FLD_IRQ_PKE_EN,//add
	FLD_IRQ_UART1_EN,
	FLD_IRQ_UART0_EN,
	FLD_IRQ_DFIFO_EN,
	FLD_IRQ_I2C_EN,
	FLD_IRQ_SPI_APB_EN,
	FLD_IRQ_USB_PWDN_EN,
	FLD_IRQ_EN,
	FLD_IRQ_GPIO2RISC0_EN,
	FLD_IRQ_GPIO2RISC1_EN,
	FLD_IRQ_SOFT_EN,

	FLD_IRQ_NPE_BUS0_EN,
	FLD_IRQ_NPE_BUS1_EN,
	FLD_IRQ_NPE_BUS2_EN,
	FLD_IRQ_NPE_BUS3_EN,
	FLD_IRQ_NPE_BUS4_EN,

	FLD_IRQ_USB_250US_EN,
	FLD_IRQ_USB_RESET_EN,
	FLD_IRQ_NPE_BUS7_EN,
	FLD_IRQ_NPE_BUS8_EN,

	FLD_IRQ_NPE_BUS13_EN=42,
	FLD_IRQ_NPE_BUS14_EN,
	FLD_IRQ_NPE_BUS15_EN,

	FLD_IRQ_NPE_BUS17_EN=46,

	FLD_IRQ_NPE_BUS21_EN=50,
	FLD_IRQ_NPE_BUS22_EN,
	FLD_IRQ_NPE_BUS23_EN,
	FLD_IRQ_NPE_BUS24_EN,
	FLD_IRQ_NPE_BUS25_EN,
	FLD_IRQ_NPE_BUS26_EN,
	FLD_IRQ_NPE_BUS27_EN,
	FLD_IRQ_NPE_BUS28_EN,
	FLD_IRQ_NPE_BUS29_EN,
	FLD_IRQ_NPE_BUS30_EN,
	FLD_IRQ_NPE_BUS31_EN,

	FLD_IRQ_NPE_COMB_EN,
	FLD_IRQ_PM_TM_EN,
	FLD_IRQ_EOC_EN,

}FLD_IRQ_ENUM;

/* Machine mode MCAUSE */
#define TRAP_M_I_ACC_FAULT    	1   /* Instruction access fault */
#define TRAP_M_L_ACC_FAULT   	5   /* Data load access fault */
#define TRAP_M_S_ACC_FAULT   	7   /* Data store access fault */
#define TRAP_U_ECALL        	8
#define TRAP_S_ECALL        	9
#define TRAP_H_ECALL        	10
#define TRAP_M_ECALL        	11
#define TRAP_M_I_PAGE_FAULT		12  /* Instruction page fault */
#define TRAP_M_L_PAGE_FAULT		13  /* Data load page fault */
#define TRAP_M_S_PAGE_FAULT		15  /* Data store page fault */
#define TRAP_M_STACKOVF     	32
#define TRAP_M_STACKUDF     	33

typedef struct EXCEPT_HANDLER_S{
	uint32_t pc;
	uint32_t lr;
	uint32_t sp;
	uint32_t gp;
	uint32_t cause;
	uint32_t irq_src;
}EXCEPT_HANDLER_S_T;




#if IRQ_DEBUG_ENABLE
extern _attribute_data_retention_sec_  volatile EXCEPT_HANDLER_S_T except_handler_b;
#endif 

#if IRQ_DEBUG_ENABLE

extern void irq_stack_check(uint32_t sp);

#define plic_isr(func, src)	\
	do {	\
		NESTED_IRQ_ENTER();	\
		register long ra asm("x1"), sp asm("x2"), gp asm("x3");\
		long mcause = read_csr(NDS_MCAUSE);\
		long mepc = read_csr(NDS_MEPC);\
		long reg[3];\
		reg[0] = ra;\
		reg[1] = sp;\
		reg[2] = gp;\
		except_handler_b.pc = mepc;\
		except_handler_b.lr = reg[0];\
		except_handler_b.sp = reg[1];\
		except_handler_b.gp = reg[2];\
		except_handler_b.cause   = mcause;\
		except_handler_b.irq_src = src;\
		{\
			__asm volatile("addi sp,sp,-8");\
			__asm volatile("sw t0,4(sp)");\
			__asm volatile("li t0, 0x55555555");\
			__asm volatile("sw t0,0(sp)");\
			func ();\
			register long sp asm("x2");\
			irq_stack_check(sp);\
			__asm volatile("lw t0,0(sp)");\
			__asm volatile("lw t0,4(sp)");\
			__asm volatile("addi sp,sp,8");\
		}\
		NESTED_IRQ_EXIT();\
		except_handler_b.irq_src = 0xFF; \
		reg_irq_done = src;\
		NDS_FENCE_IORW;\
	} while(0) 
#else
#define plic_isr(func, src)	\
	do {	\
		NESTED_IRQ_ENTER();	\
		func ();\
		NESTED_IRQ_EXIT();\
		reg_irq_done = src;\
		NDS_FENCE_IORW;\
	} while(0)
#endif




#endif

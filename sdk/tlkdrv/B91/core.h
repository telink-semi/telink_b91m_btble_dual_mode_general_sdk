/********************************************************************************************************
 * @file     core.h
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

#ifndef CORE_H
#define CORE_H
#include <nds_intrinsic.h>
#include "types.h"
#include "bit.h"
#define read_csr(reg)		      __nds__csrr(reg)
#define write_csr(reg, val)	      __nds__csrw(val, reg)
#define swap_csr(reg, val)	      __nds__csrrw(val, reg)
#define set_csr(reg, bit)	      __nds__csrrs(bit, reg)
#define clear_csr(reg, bit)	      __nds__csrrc(bit, reg)
/*
 * Inline nested interrupt entry/exit macros
 */
/* Save/Restore macro */
#define SAVE_CSR(r)             long __##r = read_csr(r);
#define RESTORE_CSR(r)          write_csr(r, __##r);

/* Support PowerBrake (Performance Throttling) feature */
#define	SUPPORT_PFT_ARCH		1

#if SUPPORT_PFT_ARCH

#define MSTATUS_UIE         	0x00000001
#define MSTATUS_SIE         	0x00000002
#define MSTATUS_HIE         	0x00000004
#define MSTATUS_MIE         	0x00000008
#define MSTATUS_UPIE        	0x00000010
#define MSTATUS_SPIE        	0x00000020
#define MSTATUS_HPIE        	0x00000040
#define MSTATUS_MPIE        	0x00000080
#define MSTATUS_SPP         	0x00000100
#define MSTATUS_HPP         	0x00000600
#define MSTATUS_MPP         	0x00001800
#define MSTATUS_FS          	0x00006000
#define MSTATUS_XS          	0x00018000
#define MSTATUS_MPRV        	0x00020000
#define MSTATUS_PUM         	0x00040000
#define MSTATUS_MXR				0x00080000
#define MSTATUS_VM          	0x1F000000
#define MSTATUS32_SD        	0x80000000
#define MSTATUS64_SD        	0x8000000000000000

#define SAVE_MXSTATUS()         SAVE_CSR(NDS_MXSTATUS)
#define RESTORE_MXSTATUS()      RESTORE_CSR(NDS_MXSTATUS)

 /* Nested IRQ entry macro : Save CSRs and enable global interrupt. */
#define NESTED_IRQ_ENTER()                              \
	 SAVE_CSR(NDS_MEPC)                              \
	 SAVE_CSR(NDS_MSTATUS)                           \
	 SAVE_MXSTATUS()                                 \
	 set_csr(NDS_MSTATUS, MSTATUS_MIE);

/* Nested IRQ exit macro : Restore CSRs */
#define NESTED_IRQ_EXIT()                               \
	 clear_csr(NDS_MSTATUS, MSTATUS_MIE);            \
	 RESTORE_CSR(NDS_MSTATUS)                        \
	 RESTORE_CSR(NDS_MEPC)                           \
	 RESTORE_MXSTATUS()
#define NDS_FENCE_IORW			__nds__fence(FENCE_IORW,FENCE_IORW)

#else

#define SAVE_MXSTATUS()
#define RESTORE_MXSTATUS()
#define NESTED_IRQ_ENTER()
#define NESTED_IRQ_EXIT()
#define NDS_FENCE_IORW

#endif


typedef enum{
	FLD_FEATURE_PREEMPT_PRIORITY_INT_EN = BIT(0),
	FLD_FEATURE_VECTOR_MODE_EN 			= BIT(1),
}feature_e;

 
/* Enable the Machine External/Timer/Sofware interrupt bit in MIE. */
/** @brief Disable interrupts globally in the system.
 * This function must be used when the system wants to restore all the interrupt
 * it could handle.
 */


void core_enable_interrupt(void);


unsigned int core_disable_interrupt(void);
void core_restore_interrupt(unsigned int en);


void tlksys_disable_interrupt(void);
void tlksys_restore_interrupt(void);

#endif

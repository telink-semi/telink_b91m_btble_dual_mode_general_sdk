/********************************************************************************************************
 * @file	tlkos_kernel.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKOS_KERNEL_H
#define TLKOS_KERNEL_H

#if (TLK_CFG_OS_ENABLE)


/******************************************************************************
 * Function: tlkos_init
 * Descript: Initialize the operating system (OS).
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface must be invoked before the operating system can be used.
 *****************************************************************************/
extern int tlkos_init(void);

/******************************************************************************
 * Function: tlkos_enter_critical
 * Descript: Enter and leave the system critical region.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_enter_critical()" is equivalent to "tlksys_enter_critical". 
 *       For details, see "tlksys_enter_critical()".
 *****************************************************************************/
void tlkos_enter_critical(void);

/******************************************************************************
 * Function: tlkos_leave_critical
 * Descript: Leave the system critical region.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_leave_critical()" is equivalent to "tlksys_leave_critical". 
 *       For details, see "tlksys_leave_critical()".
 *****************************************************************************/
void tlkos_leave_critical(void);

/******************************************************************************
 * Function: tlksys_disable_interrupt
 * Descript: Disable the system interrupt.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_disable_interrupt()" is equivalent to "tlksys_disable_interrupt". 
 *       For details, see "tlksys_disable_interrupt()".
 *****************************************************************************/
void tlkos_disable_interrupt(void);

/******************************************************************************
 * Function: tlksys_restore_interrupt
 * Descript: Restore the system interrupt.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_restore_interrupt()" is equivalent to "tlksys_restore_interrupt". 
 *       For details, see "tlksys_restore_interrupt()".
 *****************************************************************************/
void tlkos_restore_interrupt(void);

/******************************************************************************
 * Function: tlkos_delay
 * Descript: The system delays or the operating system is paused for a period 
 *           of time.
 * Params: 
 *     @timeout[IN]--The period time for pausing. Unit:us.
 * Return: None.
 * Notice:
 *     1.This interface can also be called in non-operating system situations,
 *       equivalent to "delay_us()".
 *     2.After the interface is invoked, the thread or interrupt on which it is
 *       located enters the hold phase.
 *     3.After the interface is invoked in the thread, the interrupt continues
 *       to execute.
 *     4.After the interface is called in an interrupt, all threads and 
 *       interrupts below the priority of the interrupt will not be executed, 
 *       so be careful to call in the interrupt.
 *****************************************************************************/
void tlkos_delay(uint32 timeout);

/******************************************************************************
 * Function: tlkos_delay
 * Descript: Start the operating system (OS).
 * Params: None.
 * Return: None.
 * Notice:
 *     1.Once the operating system is started, the operating system will take 
 *       over its own threads. Therefore, after the operating system starts, 
 *       the user should use the operating system's threads for task scheduling.
 *****************************************************************************/
void tlkos_start_run(void);




#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_KERNEL_H


/********************************************************************************************************
 * @file	tlkos_kernel.c
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
#if (TLK_CFG_OS_ENABLE)
#include "tlklib/os/tlkos_stdio.h"
#include "tlklib/os/tlkos_kernel.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/task.h"
#endif
#include "drivers.h"

_attribute_data_retention_sec_
uint08 gTlkOsKernelIsStart = false;

/******************************************************************************
 * Function: tlkos_enter_critical
 * Descript: Enter and leave the system critical region.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_enter_critical()" is equivalent to "tlksys_enter_critical". 
 *       For details, see "tlksys_enter_critical()".
 *****************************************************************************/
//_attribute_ram_code_sec_noinline_ 
void tlkos_enter_critical(void)
{
	return tlksys_enter_critical();
}

/******************************************************************************
 * Function: tlkos_leave_critical
 * Descript: Leave the system critical region.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_leave_critical()" is equivalent to "tlksys_leave_critical". 
 *       For details, see "tlksys_leave_critical()".
 *****************************************************************************/
//_attribute_ram_code_sec_noinline_ 
void tlkos_leave_critical(void)
{
	tlksys_leave_critical();
}

/******************************************************************************
 * Function: tlksys_disable_interrupt
 * Descript: Disable the system interrupt.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_disable_interrupt()" is equivalent to "tlksys_disable_interrupt". 
 *       For details, see "tlksys_disable_interrupt()".
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
void tlkos_disable_interrupt(void)
{
	tlksys_disable_interrupt();
}

/******************************************************************************
 * Function: tlksys_restore_interrupt
 * Descript: Restore the system interrupt.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlkos_restore_interrupt()" is equivalent to "tlksys_restore_interrupt". 
 *       For details, see "tlksys_restore_interrupt()".
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
void tlkos_restore_interrupt(void)
{
	tlksys_restore_interrupt();
}

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
void tlkos_delay(uint32 timeout)
{
#if (TLK_OS_FREERTOS_ENABLE)
	if(timeout != 0 && timeout != portMAX_DELAY){
		if(timeout < 1000000/configTICK_RATE_HZ) timeout = 1;
		else timeout = (timeout*configTICK_RATE_HZ)/1000000;
	}
	vTaskDelay(timeout);
#else
	delay_us(timeout);
#endif
}

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
void tlkos_start_run(void)
{
#if (TLK_OS_FREERTOS_ENABLE)
	gTlkOsKernelIsStart = true;
	vTaskStartScheduler();
#endif
}




#endif //#if (TLK_CFG_OS_ENABLE)


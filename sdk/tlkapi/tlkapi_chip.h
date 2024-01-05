/********************************************************************************************************
 * @file	tlkapi_chip.h
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
#ifndef TLKAPI_CHIP_H
#define TLKAPI_CHIP_H


/******************************************************************************
 * Object: TLKAPI_CHIP_STACK_CHACK_OFFSET
 * Brief : The offset position of the location of the top of the stack.
 * Notice:
 *     1.This offset is the space reserved for initialization of other modules
 *       in the system.
 *     2.The size of this value depends on the amount of stack space occupied 
 *       by other modules of the system before "tlkapi_chip_stackInit()" is 
 *       called.
 *****************************************************************************/
#define TLKAPI_CHIP_STACK_CHACK_OFFSET    1024

/******************************************************************************
 * Object: TLKAPI_CHIP_STACK_CHACK_SIGN
 * Brief : The system detects the unused mark of the stack. When two 
 *         consecutive 4-byte data in the system are marked for this purpose,
 *         this part of the stack is considered unused.
 *****************************************************************************/
#define TLKAPI_CHIP_STACK_CHACK_SIGN      0xAA5533CC



/******************************************************************************
 * Function: tlkapi_chip_stackInit
 * Descript: Initialize the stack that is not used in the chip.
 * Params: None.
 * Return: None.
 * Notice:
 *     1.The position of calling the interface should be as close as possible 
 *       to the beginning position of system initialization; otherwise, the 
 *       value initialized by other variables will be tampered with, resulting 
 *       in abnormal system operation. 
 *     2.If the unused stack in the system is already smaller than 
 *       "TLKAPI_CHIP_STACK_CHACK_OFFSET", the detection function will fail.
 *     3.This interface is only used to detect D-RAM usage.
 *     4.Because the detection function consumes MCU bandwidth, do not invoke 
 *       it unless necessary.
 *****************************************************************************/
void tlkapi_chip_stackInit(void);

/******************************************************************************
 * Function: tlkapi_chip_stackCheck
 * Descript: Detects the current stack usage.
 * Params: None.
 * Return: The usage length of the current stack.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackInit()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *****************************************************************************/
uint tlkapi_chip_stackCheck(void);

/******************************************************************************
 * Function: tlkapi_chip_stackUsed
 * Descript: Obtains the usage length of the current stack.
 * Params: None.
 * Return: The usage length of the current stack.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackCheck()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *****************************************************************************/
uint tlkapi_chip_stackUsed(void);

/******************************************************************************
 * Function: tlkapi_chip_stackDepth
 * Descript: Obtains the depth of the system stack.
 * Params: None.
 * Return: The depth of the current stack.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackInit()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *     3.For generated programs, the depth of the stack is fixed, that is, it 
 *       does not change as the system runs.
 *****************************************************************************/
uint tlkapi_chip_stackDepth(void);

/******************************************************************************
 * Function: tlkapi_chip_stackOverflow
 * Descript: Detect if the system is running out of bounds (the remaining stack
 *           of the system is 0).
 * Params: None.
 * Return: True indicates that the system runs out of bounds.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackInit()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *     3.For generated programs, the depth of the stack is fixed, that is, it 
 *       does not change as the system runs.
 *     4.If the system has crossed the line, the user needs to pay attention to
 *       the use of the system stack, otherwise it may crash at any time.
 *****************************************************************************/
bool tlkapi_chip_stackOverflow(void);


/******************************************************************************
 * Function: tlkapi_random
 * Descript: Generate a sequence of random numbers.
 * Params: 
 *     @pBuff[OUT]--Cache space for storing random sequences.
 *     @buffLen[IN]--The length of the cache space.
 * Return: None.
 *****************************************************************************/
void tlkapi_random(uint08 *pBuff, uint16 buffLen);

/******************************************************************************
 * Function: core_reboot
 * Descript: Restart MCU.
 * Params: None.
 * Return: None.
 *****************************************************************************/
extern void core_reboot(void);

/******************************************************************************
 * Function: tlksys_set_mcuMode
 * Descript: Set the work mode of the MCU.
 * Params:
 *     @mcuMode[IN]--The work mode of the MCU. Refer pm_sleep_mode_e.
 *     @wakeSrc[IN]--The wakeup source of the the MCU. Refer pm_sleep_wakeup_src_e.
 * Return: The number of actual UNICODE characters encoded.
 ********************************************************s*********************/
extern void tlksys_set_mcuMode(uint mcuMode, uint wakeSrc);

/******************************************************************************
 * Function: tlksys_enter_critical, tlksys_leave_critical
 * Descript: Enter and leave the system critical region.
 * Params: None.
 * Return: None.
 * Notice:
 *     1."tlksys_enter_critical()" and "tlksys_enter_critical()" need to be used
 *       in pairs, and "tlksys_enter_critical()" needs to be called first.
 *     2.The current critical section of the system is 1. Once you enter the 
 *       critical section, interrupts with an interrupt priority lower than or 
 *       equal to 1 will not come in.
 *****************************************************************************/
extern void tlksys_enter_critical(void);
extern void tlksys_leave_critical(void);

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
extern void tlksys_disable_interrupt(void);
extern void tlksys_restore_interrupt(void);


#endif 


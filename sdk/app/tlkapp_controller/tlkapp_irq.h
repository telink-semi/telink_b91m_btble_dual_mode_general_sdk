/********************************************************************************************************
 * @file	tlkapp_irq.h
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
#ifndef TLKAPP_IRQ_H
#define TLKAPP_IRQ_H


/******************************************************************************
 * Function: tlkapp_irq_init
 * Descript: Set the start address of interrupt ISR function table mapping to
 *           MCU hardware interrupt entry
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_irq_init(void);



#endif //TLKAPP_IRQ_H


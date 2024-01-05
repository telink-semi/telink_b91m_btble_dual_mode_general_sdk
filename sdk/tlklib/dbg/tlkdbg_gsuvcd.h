/********************************************************************************************************
 * @file	tlkdbg_gsuvcd.h
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
#ifndef TLKDBG_GSU_VCD_H
#define TLKDBG_GSU_VCD_H

#if (TLKDBG_CFG_GSU_VCD_ENABLE)


//GSU - GPIO simulate UART

#define TLKDBG_GSU_VCD_GPIO_PIN        GPIO_PA2
#define TLKDBG_GSU_VCD_BAUD_RATE       1000000

#define TLKDBG_GSU_VCD_BUFFER_SIZE       1024
#define TLKDBG_GSU_VCD_CACHE_SIZE        128


void tlkdbg_gsuvcd_init(void);
void tlkdbg_gsuvcd_reset(void);
bool tlkdbg_gsuvcd_isBusy(void);
void tlkdbg_gsuvcd_handler(void);

void tlkdbg_gsuvcd_ref(void);
void tlkdbg_gsuvcd_sync(void);
void tlkdbg_gsuvcd_tick(uint08 id);
void tlkdbg_gsuvcd_level(uint08 id, uint08 level);
void tlkdbg_gsuvcd_event(uint08 id);
void tlkdbg_gsuvcd_byte(uint08 id, uint08 value);
void tlkdbg_gsuvcd_word(uint08 id, uint16 value);


#endif //#if (TLKDBG_CFG_GSU_VCD_ENABLE)

#endif //TLKDBG_GSU_VCD_H


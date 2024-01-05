/********************************************************************************************************
 * @file	tlkdbg_hwuvcd.h
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
#ifndef TLKDBG_HWU_VCD_H
#define TLKDBG_HWU_VCD_H

#if (TLKDBG_CFG_HWU_VCD_ENABLE)


//HWU - Hardware UART

#define TLKDBG_HWU_VCD_UART_PORT          TLKDEV_DID_SERIAL0
#define TLKDBG_HWU_VCD_UART_BAUDRATE      1000000

#define TLKDBG_HWU_VCD_BUFFER_SIZE       1024
#define TLKDBG_HWU_VCD_CACHE_SIZE        128


void tlkdbg_hwuvcd_init(void);
void tlkdbg_hwuvcd_reset(void);
bool tlkdbg_hwuvcd_isBusy(void);
void tlkdbg_hwuvcd_handler(void);

void tlkdbg_hwuvcd_ref(void);
void tlkdbg_hwuvcd_sync(void);
void tlkdbg_hwuvcd_tick(uint08 id);
void tlkdbg_hwuvcd_level(uint08 id, uint08 level);
void tlkdbg_hwuvcd_event(uint08 id);
void tlkdbg_hwuvcd_byte(uint08 id, uint08 value);
void tlkdbg_hwuvcd_word(uint08 id, uint16 value);


#endif //#if (TLKDBG_CFG_HWU_VCD_ENABLE)

#endif //TLKDBG_HWU_VCD_H


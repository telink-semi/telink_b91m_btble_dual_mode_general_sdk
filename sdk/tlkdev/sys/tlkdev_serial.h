/********************************************************************************************************
 * @file	tlkdev_serial.h
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
#ifndef TLKDEV_SERIAL_H
#define TLKDEV_SERIAL_H

#if (TLK_DEV_SERIAL_ENABLE)


#if(TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1)
#define TLKDEV_SERIAL0_TX_PIN          GPIO_PD0
#define TLKDEV_SERIAL0_RX_PIN          0
#define TLKDEV_SERIAL0_RTS_PIN         0
#define TLKDEV_SERIAL0_CTS_PIN         0
#elif(TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2)
#define TLKDEV_SERIAL0_TX_PIN          GPIO_PA2
#define TLKDEV_SERIAL0_RX_PIN          0
#define TLKDEV_SERIAL0_RTS_PIN         0
#define TLKDEV_SERIAL0_CTS_PIN         0
#elif(TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
#define TLKDEV_SERIAL0_TX_PIN          GPIO_PD1
#define TLKDEV_SERIAL0_RX_PIN          0
#define TLKDEV_SERIAL0_RTS_PIN         0
#define TLKDEV_SERIAL0_CTS_PIN         0
#else
#define TLKDEV_SERIAL0_TX_PIN          0
#define TLKDEV_SERIAL0_RX_PIN          0
#define TLKDEV_SERIAL0_RTS_PIN         0
#define TLKDEV_SERIAL0_CTS_PIN         0
#endif
#define TLKDEV_SERIAL0_TX_DMA          0
#define TLKDEV_SERIAL0_RX_DMA          0


#if(TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1)
#define TLKDEV_SERIAL1_TX_PIN          GPIO_PD6
#define TLKDEV_SERIAL1_RX_PIN          GPIO_PD7
#define TLKDEV_SERIAL1_RTS_PIN         0
#define TLKDEV_SERIAL1_CTS_PIN         0
#elif(TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2)
#define TLKDEV_SERIAL1_TX_PIN          GPIO_PD6
#define TLKDEV_SERIAL1_RX_PIN          GPIO_PD7
#define TLKDEV_SERIAL1_RTS_PIN         0
#define TLKDEV_SERIAL1_CTS_PIN         0
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
#define TLKDEV_SERIAL1_TX_PIN          GPIO_PC6
#define TLKDEV_SERIAL1_RX_PIN          GPIO_PC7
#define TLKDEV_SERIAL1_RTS_PIN         0
#define TLKDEV_SERIAL1_CTS_PIN         0
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3)
#define TLKDEV_SERIAL1_TX_PIN          GPIO_PD6
#define TLKDEV_SERIAL1_RX_PIN          GPIO_PD7
#define TLKDEV_SERIAL1_RTS_PIN         0
#define TLKDEV_SERIAL1_CTS_PIN         0
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
#define TLKDEV_SERIAL1_TX_PIN          GPIO_PD3
#define TLKDEV_SERIAL1_RX_PIN          GPIO_PD2
#define TLKDEV_SERIAL1_RTS_PIN         0
#define TLKDEV_SERIAL1_CTS_PIN         0
#else
#define TLKDEV_SERIAL1_TX_PIN          0
#define TLKDEV_SERIAL1_RX_PIN          0
#define TLKDEV_SERIAL1_RTS_PIN         0
#define TLKDEV_SERIAL1_CTS_PIN         0
#endif
#define TLKDEV_SERIAL1_TX_DMA          DMA4
#define TLKDEV_SERIAL1_RX_DMA          DMA5


int tlkdev_serial_init(void);
void tlkdev_serial_deinit(void);

bool tlkdev_serial_isBusy(void);

void tlkdev_serial_enterSleep(uint mcuMode);
void tlkdev_serial_leaveSleep(uint wakeSrc);

void tlkdev_serial_handler(void);




#endif //TLK_DEV_SERIAL_ENABLE

#endif //TLKDEV_SERIAL_H


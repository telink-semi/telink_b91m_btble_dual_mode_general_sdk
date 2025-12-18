/********************************************************************************************************
 * @file	tlkdrv_iismst.h
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
#ifndef TLKDRV_IIS_MASTER_H
#define TLKDRV_IIS_MASTER_H

#if (MCU_CORE_TYPE == MCU_CORE_B92)
#if (TLKDRV_CODEC_IISMST_ENABLE)


#define TLKDRV_IISMST_CHN            I2S0

#define TLKDRV_IISMST_BCLIK_PIN      GPIO_PA3
#define TLKDRV_IISMST_DAC_CLK_PIN    GPIO_PC0
#define TLKDRV_IISMST_DAC_DAT_PIN    GPIO_PB1
#define TLKDRV_IISMST_ADC_CLK_PIN    GPIO_PA2
#define TLKDRV_IISMST_ADC_DAT_PIN    GPIO_PB5


typedef struct{
	uint08 isInit;
	uint08 isOpen;
	uint08 isMute;
	uint08 micVol;
	uint08 micIsEn;
	uint08 spkIsEn;
	uint08 channel;
	uint08 bitDepth;
	uint32 sampleRate;
}tlkdrv_iismst_t;






#endif //#if (TLKDRV_CODEC_IISMST_ENABLE)
#endif //#if (MCU_CORE_TYPE == MCU_CORE_B92)

#endif //TLKDRV_IIS_MASTER_H


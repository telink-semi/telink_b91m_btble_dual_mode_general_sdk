/********************************************************************************************************
 * @file	tlkdrv_icodec.h
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
#ifndef TLKDRV_ICODEC_B92_H
#define TLKDRV_ICODEC_B92_H

#if (MCU_CORE_TYPE == MCU_CORE_B92)

#if (TLKDRV_CODEC_ICODEC_ENABLE)


#define TLKDRV_ICODEC_POWER_PIN          GPIO_PB0

#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
#define TLKDRV_ICODEC_DMIC_DATA_PIN      GPIO_PA4
#define TLKDRV_ICODEC_DMIC_CLK0_PIN      GPIO_PA1
#define TLKDRV_ICODEC_DMIC_CLK1_PIN      GPIO_PA3
#elif(TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
#define TLKDRV_ICODEC_DMIC_DATA_PIN      GPIO_PD5
#define TLKDRV_ICODEC_DMIC_CLK0_PIN      GPIO_PD4
#define TLKDRV_ICODEC_DMIC_CLK1_PIN      GPIO_PD3
#endif



typedef struct{
	uint08 isInit;
	uint08 isOpen;
	uint08 micIsEn;
	uint08 spkIsEn;
	uint08 spkChannel;
	uint08 spkBitDepth;
	uint08 micChannel;
	uint08 micBitDepth;
	uint32 spkSampleRate;
	uint32 micSampleRate;
}tlkdrv_icodec_t;







#endif //#if (TLKDRV_CODEC_ICODEC_ENABLE)

#endif //#if (MCU_CORE_TYPE == MCU_CORE_B92)

#endif //TLKDRV_ICODEC_B92_H


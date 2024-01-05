/********************************************************************************************************
 * @file	tlkdrv_iismst_b91.h
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
#ifndef TLKDRV_IISMST_B91_H
#define TLKDRV_IISMST_B91_H

#if (MCU_CORE_TYPE == MCU_CORE_B91)
#if (TLKDRV_CODEC_IISMST_ENABLE)


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
#endif //#if (MCU_CORE_TYPE == MCU_CORE_B91)

#endif //TLKDRV_IISMST_B91_H


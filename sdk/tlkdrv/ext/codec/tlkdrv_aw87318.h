/********************************************************************************************************
 * @file	tlkdrv_aw87318.h
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
#ifndef TLKDRV_AW87318_H
#define TLKDRV_AW87318_H


#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
#define TLKDRV_AW87318_ENABLE_PIN        GPIO_PC1
#else
#define TLKDRV_AW87318_ENABLE_PIN        0
#endif


bool tlkdrv_aw87318_isOpen(void);

void tlkdrv_aw87318_init(void);

void tlkdrv_aw87318_open(void);
void tlkdrv_aw87318_close(void);



#endif //TLKDRV_AW87318_H


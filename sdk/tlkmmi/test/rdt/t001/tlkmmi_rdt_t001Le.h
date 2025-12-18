/********************************************************************************************************
 * @file	tlkmmi_rdt_t001Le.h
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
#ifndef TLKMMI_RDT_T001_LE_H
#define TLKMMI_RDT_T001_LE_H

#if (TLK_TEST_RDT_ENABLE)
#if (TLKMMI_RDT_CASE_T001_ENABLE)




void tlkmmi_rdt_t001LeStart(void);
void tlkmmi_rdt_t001LeClose(void);
bool tlkmmi_rdt_t001LeTimer(void);



#endif //#if (TLKMMI_RDT_CASE_T001_ENABLE)
#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_T001_LE_H


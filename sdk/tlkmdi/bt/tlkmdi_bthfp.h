/********************************************************************************************************
 * @file	tlkmdi_bthfp.h
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
#ifndef TLKMDI_BTHFP_H
#define TLKMDI_BTHFP_H

#if (TLK_MDI_BTHFP_ENABLE)

#define TLK_MDI_AUDAG_ENABLE         (1 && TLK_MDI_BTHFP_ENABLE)
#define TLK_MDI_AUDHF_ENABLE         (1 && TLK_MDI_BTHFP_ENABLE)


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_bthfp_init(void);

/******************************************************************************
 * Function: tlkmdi_hfphf_destroy.
 * Descript: Reset the HF control block and release resource.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_bthfp_destroy(uint16 aclHandle);

#endif //#if (TLK_MDI_BTHFP_ENABLE)

#endif //TLKMDI_BTHFP_H


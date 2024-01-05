/********************************************************************************************************
 * @file	tlkdev_mfi.h
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
#ifndef TLKDEV_MFI_H
#define TLKDEV_MFI_H

#if (TLK_DEV_MFI_ENABLE)



int tlkdev_mfi_init(void);

int tlkdev_mfi_open(void);
int tlkdev_mfi_close(void);


int tlkdev_mfi_loadCertificateData(uint08 *pBuffer, uint16 buffLen);
int tlkdev_mfi_loadChallengeData(uint08 *pData, uint16 dataLen, uint08 *pBuff, uint16 buffLen);



#endif //TLK_DEV_MFI_ENABLE

#endif //TLKDEV_MFI_H


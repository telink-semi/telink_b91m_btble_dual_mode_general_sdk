/********************************************************************************************************
 * @file	tlkmmi_file.h
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
#ifndef TLKMMI_FILE_H
#define TLKMMI_FILE_H

#if (TLKMMI_FILE_ENABLE)



#define TLKMMI_FILE_DFU_ENABLE        (1 && TLK_MDI_FILE_ENABLE)
#define TLKMMI_FILE_MP3_ENABLE        (1 && TLK_MDI_FILE_ENABLE)
#define TLKMMI_FILE_TONE_ENABLE       (1 && TLK_MDI_FILE_ENABLE)

#define TLKMMI_FILE_P2M_ENABLE        1 //Mine To Peer
#define TLKMMI_FILE_M2P_ENABLE        1 //Peer To Mine

#define TLKMMI_FILE_P2M_COUNT         1
#define TLKMMI_FILE_M2P_COUNT         0

#define TLKMMI_FILE_DBG_FLAG         ((TLK_MAJOR_DBGID_MMI_FILE << 24) | (TLK_MINOR_DBGID_MMI_FILE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_FILE_DBG_SIGN         "[MMI]"



bool tlkmmi_file_isBusy(void);



#endif //TLKMMI_FILE_ENABLE

#endif //TLKMMI_FILE_H


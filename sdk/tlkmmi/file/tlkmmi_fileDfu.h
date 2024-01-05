/********************************************************************************************************
 * @file	tlkmmi_fileDfu.h
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
#ifndef TLKMMI_FILE_DFU_H
#define TLKMMI_FILE_DFU_H

#if (TLKMMI_FILE_DFU_ENABLE)



#define TLKMMI_FILE_DFU_MAX_SIZE                 (0x200000)



#define TLKMMI_FILE_DFU_SAVE_INNER               1
#define TLKMMI_FILE_DFU_SAVE_OUTER               2
#define TLKMMI_FILE_DFU_SAVE_METHOD              TLKMMI_FILE_DFU_SAVE_INNER
#if (TLKMMI_FILE_DFU_SAVE_METHOD == TLKMMI_FILE_DFU_SAVE_INNER)
#define TLKMMI_FILE_DFU_SAVE_OFFSET              0x100000
#else
#define TLKMMI_FILE_DFU_SAVE_OFFSET              0x1000
#endif

#define TLKMMI_FILE_DFU_VERSION_CHECK_ENABLE     1

#define TLKMMI_FILE_DFU_FORCE_AUTH_ENABLE        0
#define TLKMMI_FILE_DFU_FORCE_CRYP_ENABLE        0

/******************************************************************************
 * Object: TLKMMI_FILE_DFU_SERIAL_UNIT_LENS
 * Brief : The length of a single packet during DFU transmission.
 * Notice:
 *    1."TLKMMI_FILE_DFU_SERIAL_UNIT_LENS" is closely related to 
 *        "TLKMDI_COMM_SERIAL_RBUFF_SIZE". Generally, 
 *        "TLKMDI_COMM_SERIAL_RBUFF_SIZE" is at least twice that of 
 *        "TLKMMI_FILE_DFU_SERIAL_UNIT_LENS" to maximize the performance of a 
 *        serial port upgrade.
 *    2.If TLKMMI_FILE_DFU_SERIAL_UNIT_LENS is set to 512 and the main 
 *        frequency is increased to 96 MB, the serial port upgrade performance 
 *        is optimized.
 *	  3."TLKMMI_FILE_DFU_BT_SPP_WAIT_INTV" and
 *		  "TLKMMI_FILE_DFU_BT_ATT_WAIT_INTV" only can be set to 16 or 32.
 *		  Set the value to 32 for higher transmission efficiency.
 *	  4.Follow configurations can improve the bt upgrade efficiency:
 *	   	  Set "TLKBT_HCI_C2H_FIFO_NUMB" and "TLKBT_HCI_H2C_FIFO_NUMB" to 64.
 *	   	  Increase the main frequency to 96MB.
 *	   	  Close BT Scan and Ble Adv.
 *****************************************************************************/
#define TLKMMI_FILE_DFU_SERIAL_UNIT_LENS         256//512
#define TLKMMI_FILE_DFU_SERIAL_WAIT_INTV         0//(8192/TLKMMI_FILE_DFU_SERIAL_UNIT_LENS)
#define TLKMMI_FILE_DFU_SERIAL_TRAN_INTV         10 //10*100us=1ms     
#define TLKMMI_FILE_DFU_SERIAL_SAVE_INTV         (TLKMMI_FILE_DFU_SERIAL_WAIT_INTV << 2)

#define TLKMMI_FILE_DFU_BT_SPP_UNIT_LENS         512
#define TLKMMI_FILE_DFU_BT_SPP_WAIT_INTV         32//(16384/TLKMMI_FILE_DFU_BT_SPP_UNIT_LENS)//(8192/TLKMMI_FILE_DFU_BT_SPP_UNIT_LENS)
#define TLKMMI_FILE_DFU_BT_SPP_TRAN_INTV         10 //10*100us=1ms  
#define TLKMMI_FILE_DFU_BT_SPP_SAVE_INTV         (TLKMMI_FILE_DFU_BT_SPP_WAIT_INTV << 2)

#define TLKMMI_FILE_DFU_BT_ATT_UNIT_LENS         512
#define TLKMMI_FILE_DFU_BT_ATT_WAIT_INTV         32//(8192/TLKMMI_FILE_DFU_BT_ATT_UNIT_LENS)
#define TLKMMI_FILE_DFU_BT_ATT_TRAN_INTV         10 //10*100us=1ms  
#define TLKMMI_FILE_DFU_BT_ATT_SAVE_INTV         (TLKMMI_FILE_DFU_BT_ATT_WAIT_INTV << 2)

#define TLKMMI_FILE_DFU_LE_ATT_UNIT_LENS         32
#define TLKMMI_FILE_DFU_LE_ATT_WAIT_INTV         (4096/TLKMMI_FILE_DFU_LE_ATT_UNIT_LENS)
#define TLKMMI_FILE_DFU_LE_ATT_TRAN_INTV         10 //10*100us=1ms  
#define TLKMMI_FILE_DFU_LE_ATT_SAVE_INTV         (TLKMMI_FILE_DFU_LE_ATT_WAIT_INTV << 2)



#define TLKMMI_FILE_DFU_START_FLAG               0x4B
#define TLKMMI_FILE_DFU_START_OFFSET             0x20

#define TLKMMI_FILE_DFU_BK_RESUME_ENABLE         1

extern unsigned int sTlkFlashCap;

#if (TLK_CFG_FLASH_CAP < sTlkFlashCap && (TLKMMI_FILE_DFU_SAVE_METHOD == TLKMMI_FILE_DFU_SAVE_INNER))
	#error "The inner flash is too small to store the firmware in it."
#endif


bool tlkmmi_file_dfuIsStart(void);



#endif //TLKMMI_FILE_ENABLE

#endif //TLKMMI_FILE_DFU_H


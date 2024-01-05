/********************************************************************************************************
 * @file	tlkdbg_hpuvcd.h
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
#ifndef TLKDBG_HPU_VCD_H
#define TLKDBG_HPU_VCD_H

#if (TLKDBG_CFG_HPU_VCD_ENABLE)


//HPU - Hardware Protocol UART
#define TLKDBG_HPUVCD_FLOW_CTRL_EN     0

#define TLKDBG_HPUVCD_POINTS_NUMB      1024
#define TLKDBG_HPUVCD_CACHE_POINT      64 //It has to be a multiple of 4

#define TLKDBG_HPUVCD_FCS_POINTS       128


typedef int(*TlkDbgHpuVcdSendCB)(uint08 *pData, uint16 dataLen);

typedef struct{
	uint08 isOpen;
	uint08 isFirst;
	uint16 reserved;
	uint16 sendSize;
	uint16 wOffset;
	uint16 rOffset;
	uint16 cOffset; //Current Read Offset, TLKDBG_HPUVCD_FLOW_CTRL_EN
	uint32 syncTicks;
	uint32 comfirmNumb; //The number of confirmed signals, TLKDBG_HPUVCD_FLOW_CTRL_EN
	uint32 currentNumb; //Indicates the number of sent signals, TLKDBG_HPUVCD_FLOW_CTRL_EN
	uint32 totalSignal; //Stall signal number
	uint32 stallSignal; //Total signal number
	uint32 buffer[TLKDBG_HPUVCD_POINTS_NUMB];
	TlkDbgHpuVcdSendCB sendCB;
}tlkdbg_hpuvcd_t;


typedef enum{
	TLKDBG_HPU_VCD_TYPE_EVENT  = 0x00, //None ID
	TLKDBG_HPU_VCD_TYPE_BYTE   = 0x01, //id=0~255
	TLKDBG_HPU_VCD_TYPE_WORD   = 0x02, //id=0~255
	TLKDBG_HPU_VCD_TYPE_BYTE3  = 0x03,
	TLKDBG_HPU_VCD_TYPE_LEVEL1 = 0x04,
	TLKDBG_HPU_VCD_TYPE_LEVEL0 = 0x05,
	TLKDBG_HPU_VCD_TYPE_TICK   = 0x06, //id=1~255
	TLKDBG_HPU_VCD_TYPE_RESV   = 0x07, //id=0~255	
}TLKDBG_HPU_VCD_TYPE_ENUM;


void tlkdbg_hpuvcd_init(void);
void tlkdbg_hpuvcd_reset(void);
bool tlkdbg_hpuvcd_isBusy(void);
void tlkdbg_hpuvcd_handler(void);

void tlkdbg_hpuvcd_regSendCB(TlkDbgHpuVcdSendCB cb, uint16 maxLen);

void tlkdbg_hpuvcd_sync(void);
void tlkdbg_hpuvcd_tick(uint08 id);
void tlkdbg_hpuvcd_level(uint08 id, uint08 level);
void tlkdbg_hpuvcd_event(uint08 id);
void tlkdbg_hpuvcd_byte(uint08 id, uint08 value);
void tlkdbg_hpuvcd_word(uint08 id, uint16 value);
void tlkdbg_hpuvcd_byte3(uint08 id, uint32 value);

void tlkdbg_hpuvcd_enable(bool enable);


#endif //#if (TLKDBG_CFG_HPU_VCD_ENABLE)

#endif //TLKDBG_HPU_VCD_H


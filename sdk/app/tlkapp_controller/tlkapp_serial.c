/********************************************************************************************************
 * @file	tlkapp_serial.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "types.h"
#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapp_serial.h"
#include "tlkdev/tlkdev.h"
#include "tlkstk/hci/bt_hci.h"

extern void tlkdev_serial_handler(void);

static void tlkapp_serial_input(uint08 *pData, uint16 dataLen);
static int  tlkapp_serial_send(uint08 *pData, uint16 dataLen);
static void tlkapp_serial_reset(uint08 resetSrc);

static uint08 sTlkAppHciParseChache[TLKAPP_HCI_PARSE_CHACHE_SIZE];
__attribute__((aligned(4)))
static uint08 sTlkAppSerialRecvBuffer[TLKAPP_SERIAL_RBUFF_NUMB*(TLKAPP_SERIAL_RBUFF_SIZE+4)];
__attribute__((aligned(4)))
static uint08 sTlkAppSerialSendBuffer[TLKAPP_SERIAL_SBUFF_NUMB*(TLKAPP_SERIAL_SBUFF_SIZE+4)];


int tlkapp_serial_init(void)
{
	tlkdev_mount(TLKDEV_DID_SERIAL1);
	tlkdev_ioctl(TLKDEV_DID_SERIAL1, TLKDEV_OPCODE_SERIAL_SET_BAUDRATE, TLKAPP_SERIAL_BAUDRATE, 0);
	tlkdev_ioctl(TLKDEV_DID_SERIAL1, TLKDEV_OPCODE_SERIAL_REG_RECV_CB, (uint32)tlkapp_serial_input, 0);
	tlkdev_ioctl(TLKDEV_DID_SERIAL1, TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO,
		((TLKAPP_SERIAL_RBUFF_NUMB & 0xFFFF)<<16) |((TLKAPP_SERIAL_RBUFF_SIZE+4) & 0xFFFF),
		(uint32)sTlkAppSerialRecvBuffer);
	tlkdev_ioctl(TLKDEV_DID_SERIAL1, TLKDEV_OPCODE_SERIAL_SET_TX_QFIFO,
		((TLKAPP_SERIAL_SBUFF_NUMB & 0xFFFF)<<16) | ((TLKAPP_SERIAL_SBUFF_SIZE+4) & 0xFFFF),
		(uint32)sTlkAppSerialSendBuffer);
	tlkdev_open(TLKDEV_DID_SERIAL1, 0);

	tlkbt_hci_setParseCache(sTlkAppHciParseChache, TLKAPP_HCI_PARSE_CHACHE_SIZE);
	tlkbt_hci_regSendCB(tlkapp_serial_send);
	tlkbt_hci_regResetCB(tlkapp_serial_reset);
	
	return TLK_ENONE;
}

void tlkapp_serial_handler(void)
{
	tlkdev_serial_handler();
}


static void tlkapp_serial_input(uint08 *pData, uint16 dataLen)
{
	tlkbt_hci_recvHandler(pData, dataLen);
}

static int tlkapp_serial_send(uint08 *pData, uint16 dataLen)
{
	return tlkdev_write(TLKDEV_DID_SERIAL1, 0, pData, dataLen);
}
static void tlkapp_serial_reset(uint08 resetSrc)
{
	if(resetSrc == 0x00){ //resetSrc=0x00,Controller Reset; resetSrc=0x01,HCI Reset
		tlkdev_close(TLKDEV_DID_SERIAL1);
		tlkdev_open(TLKDEV_DID_SERIAL1, 0);
	}
}


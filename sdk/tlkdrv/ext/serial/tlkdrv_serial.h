/********************************************************************************************************
 * @file	tlkdrv_serial.h
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
#ifndef TLKDRV_SERIAL_H
#define TLKDRV_SERIAL_H

#if (TLK_DEV_SERIAL_ENABLE)


#define TLKDRV_SERIAL_DBG_FLAG               0//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_SERIAL_DBG_SIGN               "[UART]"


#define TLKDRV_SERIAL_MAX_NUMB             2
#define TLKDRV_SERIAL_FLOW_CTRL_ENABLE     1

#if (MCU_CORE_TYPE == MCU_CORE_B91)
#define TLKDRV_SERIAL_B91_UART_ENABLE      1
#define TLKDRV_SERIAL_B92_UART_ENABLE      0
#elif (MCU_CORE_TYPE == MCU_CORE_B92)
#define TLKDRV_SERIAL_B91_UART_ENABLE      0
#define TLKDRV_SERIAL_B92_UART_ENABLE      1
#else
#define TLKDRV_SERIAL_B91_UART_ENABLE      0
#define TLKDRV_SERIAL_B92_UART_ENABLE      0
#endif

typedef void(*TlkDrvSerialRecvCB)(uint08 *pFrame, uint16 frmLen);


typedef enum{
	TLKDRV_SERIAL_FLAG_NONE   = 0x0000,
	TLKDRV_SERIAL_FLAG_INIT   = 0x8000,
	TLKDRV_SERIAL_FLAG_OPEN   = 0x4000,
	TLKDRV_SERIAL_FLAG_SEND   = 0x2000,
	TLKDRV_SERIAL_FLAG_RECV   = 0x1000,
	TLKDRV_SERIAL_FLAG_SLEEP  = 0x0800,
	TLKDRV_SERIAL_FLAG_TX_DMA = 0x0001,
	TLKDRV_SERIAL_FLAG_RX_DMA = 0x0002,
	TLKDRV_SERIAL_FLAG_RX_RTS = 0x0004,
	TLKDRV_SERIAL_FLAG_TX_CTS = 0x0008,
	TLKDRV_SERIAL_FLAG_TX_FIFO = 0x0010,
	TLKDRV_SERIAL_FLAG_RX_FIFO = 0x0020,
	TLKDRV_SERIAL_FLAG_TX_BUSY = 0x0040,
	TLKDRV_SERIAL_FLAG_RX_BUSY = 0x0080,
}TLKDRV_SERIAL_FLAGS_ENUM;


typedef struct{
	uint08 port;
	uint08 resv;
	uint08 txDma;
	uint08 rxDma;
	uint16 txPin;
	uint16 rxPin;
	uint16 rtsPin;
	uint16 ctsPin;
	uint32 baudRate;
}tlkdrv_serial_port_t;
typedef union{
	tlkapi_fifo_t  fifo;
	tlkapi_qfifo_t qfifo;
}tlkdrv_serial_fifo_t;

typedef struct{
	uint16 flags[TLKDRV_SERIAL_MAX_NUMB];
	uint16 sfifoM60[TLKDRV_SERIAL_MAX_NUMB];
	uint16 sfifoM85[TLKDRV_SERIAL_MAX_NUMB];
	tlkdrv_serial_port_t port[TLKDRV_SERIAL_MAX_NUMB];
	tlkdrv_serial_fifo_t sfifo[TLKDRV_SERIAL_MAX_NUMB];
	tlkdrv_serial_fifo_t rfifo[TLKDRV_SERIAL_MAX_NUMB];
	TlkDrvSerialRecvCB recvCB[TLKDRV_SERIAL_MAX_NUMB];
}tlkdrv_serial_t;



int tlkdrv_serial_init(void);

int tlkdrv_serial_mount(uint08 port, uint32 baudRate, uint16 txPin,
	uint16 rxPin, uint08 txDma, uint08 rxDma, uint16 rtsPin, uint16 ctsPin);
int tlkdrv_serial_unmount(uint08 port);

int tlkdrv_serial_portInit(uint08 port, uint32 baudRate, uint16 txPin, uint16 rxPin);

int tlkdrv_serial_open(uint08 port);
int tlkdrv_serial_close(uint08 port);

int tlkdrv_serial_read(uint08 port, uint08 *pBuff, uint16 buffLen);
int tlkdrv_serial_send(uint08 port, uint08 *pData, uint16 dataLen);

int tlkdrv_serial_setBaudrate(uint08 port, uint32 baudRate);
int tlkdrv_serial_getBaudrate(uint08 port);

int tlkdrv_serial_setTxPin(uint08 port, uint16 txPin);
int tlkdrv_serial_setRxPin(uint08 port, uint16 rxPin);

int tlkdrv_serial_setCtsPin(uint08 port, uint16 ctsPin);
int tlkdrv_serial_setRtsPin(uint08 port, uint16 rtsPin);

int tlkdrv_serial_setTxDma(uint08 port, uint08 txDma);
int tlkdrv_serial_setRxDma(uint08 port, uint08 rxDma);

int tlkdrv_serial_setTxFifo(uint08 port, uint08 *pBuffer, uint16 buffLen);
int tlkdrv_serial_setRxFifo(uint08 port, uint08 *pBuffer, uint16 buffLen);
int tlkdrv_serial_setTxQFifo(uint08 port, uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint32 buffLen);
int tlkdrv_serial_setRxQFifo(uint08 port, uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint32 buffLen);

void tlkdrv_serial_reset(uint08 port);
void tlkdrv_serial_regCB(uint08 port, TlkDrvSerialRecvCB cb);
bool tlkdrv_serial_isBusy(uint08 port);

void tlkdrv_serial_enterSleep(uint mcuMode);
void tlkdrv_serial_leaveSleep(uint wakeSrc);

void tlkdrv_serial_handler(uint08 port);

bool tlkdrv_serial_sfifoIsMore60(uint08 port,uint16 dataLen);
bool tlkdrv_serial_sfifoIsMore85(uint08 port,uint16 dataLen);

tlkdrv_serial_t *tlkdrv_serial_getNode(uint08 port);


#endif //#if (TLK_DEV_SERIAL_ENABLE)

#endif //TLKDRV_SERIAL_H


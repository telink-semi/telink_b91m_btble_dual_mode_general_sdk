/********************************************************************************************************
 * @file	tlkdrv_serial.c
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_DEV_SERIAL_ENABLE)
#include "tlkdrv_serial.h"
#if (TLKDRV_SERIAL_B91_UART_ENABLE)
#include "tlkdrv_b91uart.h"
#endif
#if (TLKDRV_SERIAL_B92_UART_ENABLE)
#include "tlkdrv_b92uart.h"
#endif
#if (TLKDRV_SERIAL_B91_UART_ENABLE)
#include "tlkdrv_b91uart.h"
#elif (TLKDRV_SERIAL_B92_UART_ENABLE)
#include "tlkdrv_b92uart.h"
#else
#error "The driver of serial being selected is not supported."
#endif
#include "drivers.h"


static void tlkdrv_serial_setTxDmaBuffer(uint08 port);
static void tlkdrv_serial_setRxDmaBuffer(uint08 port);
static int  tlkdrv_serial_sendWithDma(uint08 port, uint08 *pData, uint16 dataLen);
static int  tlkdrv_serial_sendWithoutDma(uint08 port, uint08 *pData, uint16 dataLen);

extern unsigned char uart_rx_byte_index[2];
static tlkdrv_serial_t sTlkDrvSerial = {0};



int tlkdrv_serial_init(void)
{
	tmemset(&sTlkDrvSerial, 0, sizeof(tlkdrv_serial_t));
	return TLK_ENONE;
}

int tlkdrv_serial_mount(uint08 port, uint32 baudRate, uint16 txPin,
	uint16 rxPin, uint08 txDma, uint08 rxDma, uint16 rtsPin, uint16 ctsPin)
{
	tlkdrv_serial_port_t *pPort;

	if(port >= TLKDRV_SERIAL_MAX_NUMB || baudRate < 9600 || (txPin == 0 && rxPin == 0)){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_mount: failure-ErrorParam {port[%d],baudRate[%d],txPin[0x%x],rxPin[0x%x]}",
			port, baudRate, txPin, rxPin);
		return -TLK_EPARAM;
	}
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_mount: failure-ErrorStatus {Opened}");
		return -TLK_ESTATUS;
	}
	
	if(txPin == 0 && (txDma != 0 || rtsPin != 0)){
		txDma = 0;
		rtsPin = 0;
	}
	if(rxPin == 0 && (rxDma != 0 || ctsPin != 0)){
		rxDma = 0;
		ctsPin = 0;
	}
	
	pPort = &sTlkDrvSerial.port[port];
	pPort->port = port;
	pPort->txPin = txPin;
	pPort->rxPin = rxPin;
	pPort->txDma = txDma;
	pPort->rxDma = rxDma;
	#if (TLKDRV_SERIAL_FLOW_CTRL_ENABLE)
	pPort->rtsPin = rtsPin;
	pPort->ctsPin = ctsPin;
	#endif
	pPort->baudRate = baudRate;
	sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_INIT;
	if(pPort->txPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_SEND;
	if(pPort->rxPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RECV;
	if(pPort->txDma != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_DMA;
	if(pPort->rxDma != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_DMA;
	if(pPort->rtsPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_RTS;
	if(pPort->ctsPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_CTS;
	if(pPort->txPin != 0 && pPort->txDma == 0){
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_FIFO;
	}
	tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
		"tlkdrv_serial_mount: success-port[%d],baudRate[%d],txPin[0x%x],rxPin[0x%x]"
		"txDma[%d],rxDma[%d],rtsPin[0x%x],ctsPin[0x%x],flags[0x%x]", 
		port, baudRate, txPin, rxPin, txDma, rxDma, rtsPin, ctsPin, 
		sTlkDrvSerial.flags[port]);
	
	return TLK_ENONE;
}
int tlkdrv_serial_unmount(uint08 port)
{
	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_unmount: failure-ErrorParam {port[%d]}", port);
		return -TLK_EPARAM;
	}
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_OPEN) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_unmount: failure-ErrorStatus {Closed}");
		return -TLK_ESTATUS;
	}
	
	tlkdrv_serial_close(port);
	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_INIT;

	tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_unmount: success");
	
	return TLK_ENONE;
}

int tlkdrv_serial_portInit(uint08 port, uint32 baudRate, uint16 txPin, uint16 rxPin)
{
	tlkdrv_serial_port_t *pPort;

	if(port >= TLKDRV_SERIAL_MAX_NUMB || baudRate < 9600 || (txPin == 0 && rxPin == 0)){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_mount: failure-ErrorParam {port[%d],baudRate[%d],txPin[0x%x],rxPin[0x%x]}",
			port, baudRate, txPin, rxPin);
		return -TLK_EPARAM;
	}
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_mount: failure-ErrorStatus {Opened}");
		return -TLK_ESTATUS;
	}
	
	
	pPort = &sTlkDrvSerial.port[port];
	pPort->port = port;
	pPort->txPin = txPin;
	pPort->rxPin = rxPin;
	pPort->txDma = 0;
	pPort->rxDma = 0;
	pPort->rtsPin = 0;
	pPort->ctsPin = 0;
	pPort->baudRate = baudRate;
	sTlkDrvSerial.flags[port] = TLKDRV_SERIAL_FLAG_INIT;
	if(pPort->txPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_SEND;
	if(pPort->rxPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RECV;
	tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
		"tlkdrv_serial_portInit: success-port[%d],baudRate[%d],txPin[0x%x],rxPin[0x%x],flags[0x%x]", 
		port, baudRate, txPin, rxPin, sTlkDrvSerial.flags[port]);
	
	return TLK_ENONE;
}

int tlkdrv_serial_open(uint08 port)
{
	int ret;
	uint16 flags;
	tlkdrv_serial_port_t *pPort;
		
	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_open: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	
	pPort = &sTlkDrvSerial.port[port];
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_open: failure-NoReady[NotMount]");
		return -TLK_ENOREADY;
	}
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_open: failure-RepeatOpen");
		return -TLK_EREPEAT;
	}
	if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0 && pPort->txDma != 0 && (flags & TLKDRV_SERIAL_FLAG_TX_FIFO) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_open: failure-NoReady[NotCfgTxFifo]");
		return -TLK_ENOREADY;
	}
	if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0 && (flags & TLKDRV_SERIAL_FLAG_RX_FIFO) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_open: failure-NoReady[NotCfgRxFifo]");
		return -TLK_ENOREADY;
	}
	
	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_BUSY;
	tlkdrv_serial_reset(port);

#if (TLKDRV_SERIAL_B91_UART_ENABLE)
	ret = tlkdrv_b91uart_open(port, pPort->baudRate, pPort->txPin, pPort->rxPin,
		pPort->txDma, pPort->rxDma, pPort->rtsPin, pPort->ctsPin);
#elif (TLKDRV_SERIAL_B92_UART_ENABLE)
	ret = tlkdrv_b92uart_open(port, pPort->baudRate, pPort->txPin, pPort->rxPin,
		pPort->txDma, pPort->rxDma, pPort->rtsPin, pPort->ctsPin);
#else
	ret = -TLK_ENOSUPPORT;
#endif
	if(ret == TLK_ENONE){
		tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_open: success");
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_OPEN;
		tlkdrv_serial_setRxDmaBuffer(port);
	}else{
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_open: failure-%d", ret);
	}
	return ret;
}
int tlkdrv_serial_close(uint08 port)
{
	int ret;
	uint16 flags;
	tlkdrv_serial_port_t *pPort;
		
	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_close: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}

	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_close: failure-NoReady[NotMount]");
		return -TLK_ENOREADY;
	}
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_close: failure-ErrorStatus[Closed]");
		return TLK_ENONE;
	}

	pPort = &sTlkDrvSerial.port[port];
#if (TLKDRV_SERIAL_B91_UART_ENABLE)
	ret = tlkdrv_b91uart_close(port, pPort->txDma, pPort->rxDma);
#elif (TLKDRV_SERIAL_B92_UART_ENABLE)
	ret = tlkdrv_b92uart_close(port, pPort->txDma, pPort->rxDma);
#else
	ret = -TLK_ENOSUPPORT;
#endif
	
	if(ret == TLK_ENONE){
		tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_close: success");
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_OPEN;
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_BUSY;
	}else{
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_close: failure-%d", ret);
	}
	
	return ret;
}


int tlkdrv_serial_setBaudrate(uint08 port, uint32 baudRate)
{
	uint16 flags;
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB || baudRate < 9600){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setBaudrate: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setBaudrate: failure-NoReady[NotMount]");
		return -TLK_ENOREADY;
	}
	
	sTlkDrvSerial.port[port].baudRate = baudRate;
	
	return TLK_ENONE;
}
int tlkdrv_serial_getBaudrate(uint08 port)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_getBaudrate: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_getBaudrate: failure-NoReady[NotMount]");
		return -TLK_ENOREADY;
	}
	return sTlkDrvSerial.port[port].baudRate;
}

int tlkdrv_serial_setTxPin(uint08 port, uint16 txPin)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxPin: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxPin: failure-NoReady");
		return -TLK_ENOREADY;
	}
	
	sTlkDrvSerial.port[port].txPin = txPin;
	if(txPin != 0){
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_SEND;
	}else{
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_SEND;
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_FIFO;
	}
	
	return TLK_ENONE;
}
int tlkdrv_serial_setRxPin(uint08 port, uint16 rxPin)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxPin: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxPin: failure-NoReady");
		return -TLK_ENOREADY;
	}
	
	sTlkDrvSerial.port[port].rxPin = rxPin;
	if(rxPin != 0){
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RECV;
	}else{
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RECV;
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_FIFO;
	}
	
	return TLK_ENONE;
}

int tlkdrv_serial_setCtsPin(uint08 port, uint16 ctsPin)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setCtsPin: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setCtsPin: failure-NoReady");
		return -TLK_ENOREADY;
	}
	if(sTlkDrvSerial.port[port].txPin == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setCtsPin: failure-TxPin is not set");
		return -TLK_ENOSUPPORT;
	}

	sTlkDrvSerial.port[port].ctsPin = ctsPin;
	if(ctsPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_CTS;
	else sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_CTS;
	
	return TLK_ENONE;
}
int tlkdrv_serial_setRtsPin(uint08 port, uint16 rtsPin)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setCtsPin: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setCtsPin: failure-NoReady");
		return -TLK_ENOREADY;
	}
	if(sTlkDrvSerial.port[port].rxPin == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRtsPin: failure-RxPin is not set");
		return -TLK_ENOSUPPORT;
	}

	sTlkDrvSerial.port[port].rtsPin = rtsPin;
	if(rtsPin != 0) sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_RTS;
	else sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_RTS;
	
	return TLK_ENONE;
}

int tlkdrv_serial_setTxDma(uint08 port, uint08 txDma)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxDma: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxDma: failure-NoReady");
		return -TLK_ENOREADY;
	}
	if(sTlkDrvSerial.port[port].txPin == 0 && txDma != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxDma: failure-TxPin is not set");
		return -TLK_ENOSUPPORT;
	}

	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_FIFO;
	sTlkDrvSerial.port[port].txDma = txDma;
	if(txDma == 0){
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_DMA;
	}else{
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_DMA;
	}
	
	return TLK_ENONE;
}
int tlkdrv_serial_setRxDma(uint08 port, uint08 rxDma)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxDma: failure-ErrorParam{Port[%d]}", port);
		return -TLK_EPARAM;
	}
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxDma: failure-NoReady");
		return -TLK_ENOREADY;
	}
	if(sTlkDrvSerial.port[port].rxPin == 0 && rxDma != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxDma: failure-RxPin is not set");
		return -TLK_ENOSUPPORT;
	}

	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_FIFO;
	sTlkDrvSerial.port[port].rxDma = rxDma;
	if(rxDma == 0){
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_DMA;
	}else{
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_DMA;
	}
	
	return TLK_ENONE;
}

int tlkdrv_serial_setTxFifo(uint08 port, uint08 *pBuffer, uint16 buffLen)
{
//	int ret;
	uint16 flags;
//	tlkapi_fifo_t *pFifo;
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB || pBuffer == nullptr || buffLen < 64){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxFifo: failure-ErrorParam{port[%d],buffLen[%d]}",
			port, buffLen);
		return -TLK_EPARAM;
	}

	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxFifo: failure-NoReady[NotMount]");
		return -TLK_ENOREADY;
	}
	if((flags & TLKDRV_SERIAL_FLAG_SEND) == 0 || (flags & TLKDRV_SERIAL_FLAG_TX_DMA) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxFifo: failure-NotSupp{flags[0x%x]}", flags);
		return -TLK_ENOSUPPORT;
	}
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxFifo: failure-ErrorStatus[Opened]");
		return -TLK_ESTATUS;
	}

	sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_FIFO;
	
	return TLK_ENONE;
	
//	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_FIFO;
//	pFifo = &sTlkDrvSerial.sfifo[port].fifo;
//	ret = tlkapi_fifo_init(pFifo, false, false, pBuffer, buffLen);
//	if(ret == TLK_ENONE){
//		tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setTxFifo: success");
//		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_FIFO;
//		sTlkDrvSerial.sfifoM60[port] = buffLen*3/5;
//		sTlkDrvSerial.sfifoM85[port] = buffLen*17/20;
//	}else{
//		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setTxFifo: failure-%d", ret);
//	}
//	return ret;
}
int tlkdrv_serial_setRxFifo(uint08 port, uint08 *pBuffer, uint16 buffLen)
{
	int ret;
	uint16 flags;
	tlkapi_fifo_t *pFifo;
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB || pBuffer == nullptr || buffLen < 64){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxFifo: failure-ErrorParam{port[%d],buffLen[%d]}",
			port, buffLen);
		return -TLK_EPARAM;
	}

	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxFifo: failure-NoReady[NotMount]");
		return -TLK_ENOREADY;
	}
	if(sTlkDrvSerial.port[port].rxPin == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxFifo: failure-RxPin is not set");
		return -TLK_ENOREADY;
	}
	
	if((flags & TLKDRV_SERIAL_FLAG_RECV) == 0 || (flags & TLKDRV_SERIAL_FLAG_RX_DMA) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxFifo: failure-NotSupp{flags[0x%x]}", flags);
		return -TLK_ENOSUPPORT;
	}
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxFifo: failure-ErrorStatus[Opened]");
		return -TLK_ESTATUS;
	}
		
	pFifo = &sTlkDrvSerial.rfifo[port].fifo;
	ret = tlkapi_fifo_init(pFifo, false, false, pBuffer, buffLen);
	if(ret == TLK_ENONE){
		tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setRxFifo: success");
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_FIFO;
	}else{
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_FIFO;
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setRxFifo: failure-%d", ret);
	}
	return ret;
}

int tlkdrv_serial_setTxQFifo(uint08 port, uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint32 buffLen)
{
	int ret;
	uint16 flags;
	tlkapi_qfifo_t *pFifo;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxQFifo: failure-ErrorParam{port[%d],fnumb[%d],fsize[%d]}",
			port, buffLen, fnumb, fsize);
		return -TLK_EPARAM;
	}
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxQFifo: failure-NoReady");
		return -TLK_ENOREADY;
	}
	
	if(sTlkDrvSerial.port[port].txPin == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxQFifo: failure-TxPin is not set");
		return -TLK_ENOREADY;
	}
	
	if(pBuffer == nullptr || fnumb == 0 || fsize < 8){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxQFifo: failure-ErrorParam{port[%d],fnumb[%d],fsize[%d]}",
			port, buffLen, fnumb, fsize);
		return -TLK_EPARAM;
	}
	if((((uint32)pBuffer) & 0x03) != 0 || (fsize & 0x03) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxQFifo: failure-ErrorAlign{pBuffer[0x%x],fsize[%d]}",
			pBuffer, fsize);
		return -TLK_EALIGN;
	}
	
	if((flags & TLKDRV_SERIAL_FLAG_SEND) == 0 || (flags & TLKDRV_SERIAL_FLAG_TX_DMA) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxQFifo: failure-NotSupp{flags[0x%x]}", flags);
		return -TLK_ENOSUPPORT;
	}
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setTxQFifo: failure-ErrorStatus[Opened]");
		return -TLK_ESTATUS;
	}
	
	pFifo = &sTlkDrvSerial.sfifo[port].qfifo;
	ret = tlkapi_qfifo_init(pFifo, fnumb, fsize, pBuffer, buffLen);
	if(ret == TLK_ENONE){
		tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setTxQFifo: success");
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_FIFO;
		sTlkDrvSerial.sfifoM60[port] = fnumb*3/5;
		sTlkDrvSerial.sfifoM85[port] = fnumb*17/20;
	}else{
		sTlkDrvSerial.port[port].txDma = 0;
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_FIFO;
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setTxQFifo: failure-%d", ret);
	}
	return ret;
}
int tlkdrv_serial_setRxQFifo(uint08 port, uint16 fnumb, uint16 fsize, uint08 *pBuffer, uint32 buffLen)
{
	int ret;
	uint16 flags;
	tlkapi_qfifo_t *pFifo;

	if(port >= TLKDRV_SERIAL_MAX_NUMB){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxQFifo: failure-ErrorParam{port[%d],fnumb[%d],fsize[%d]}",
			port, buffLen, fnumb, fsize);
		return -TLK_EPARAM;
	}

	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxQFifo: failure-NoReady[NotMount]");
		return -TLK_ENOREADY;
	}

	if(sTlkDrvSerial.port[port].rxPin == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxQFifo: failure-RxPin is not set");
		return -TLK_ENOREADY;
	}
	
	if(pBuffer == nullptr || fnumb == 0 || fsize < 8){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxQFifo: failure-ErrorParam{port[%d],fnumb[%d],fsize[%d]}",
			port, buffLen, fnumb, fsize);
		return -TLK_EPARAM;
	}
	if((((uint32)pBuffer) & 0x03) != 0 || (fsize & 0x03) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxQFifo: failure-ErrorAlign{pBuffer[0x%x],fsize[%d]}",
			pBuffer, fsize);
		return -TLK_EALIGN;
	}
	
	if((flags & TLKDRV_SERIAL_FLAG_RECV) == 0 || (flags & TLKDRV_SERIAL_FLAG_RX_DMA) == 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxQFifo: failure-NotSupp{flags[0x%x]}", flags);
		return -TLK_ENOSUPPORT;
	}
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
			"tlkdrv_serial_setRxQFifo: failure-ErrorStatus[Opened]");
		return -TLK_ESTATUS;
	}
	
	pFifo = &sTlkDrvSerial.rfifo[port].qfifo;
	ret = tlkapi_qfifo_init(pFifo, fnumb, fsize, pBuffer, buffLen);
	if(ret == TLK_ENONE){
		tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setRxQFifo: success");
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_FIFO;
	}else{
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_FIFO;
		tlkapi_error(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "tlkdrv_serial_setRxQFifo: failure-%d", ret);
	}
	return ret;
}

void tlkdrv_serial_reset(uint08 port)
{
	uint16 flags;
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB) return;
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0 || (flags & TLKDRV_SERIAL_FLAG_OPEN) == 0) return;
	if((flags & TLKDRV_SERIAL_FLAG_TX_BUSY) != 0 || (flags & TLKDRV_SERIAL_FLAG_RX_BUSY) != 0) return;
	if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0){
		if((flags & TLKDRV_SERIAL_FLAG_TX_DMA) == 0){
			tlkapi_fifo_clear(&sTlkDrvSerial.sfifo[port].fifo);
		}else{
			dma_chn_dis(sTlkDrvSerial.port[port].txDma);
			dma_clr_irq_mask(sTlkDrvSerial.port[port].txDma, TC_MASK|ABT_MASK|ERR_MASK);
			tlkapi_qfifo_clear(&sTlkDrvSerial.sfifo[port].qfifo);
		}
	}
	if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0){
		if((flags & TLKDRV_SERIAL_FLAG_RX_DMA) == 0){
			tlkapi_fifo_clear(&sTlkDrvSerial.rfifo[port].fifo);
		}else{
			dma_chn_dis(sTlkDrvSerial.port[1].rxDma);
			dma_clr_irq_mask(sTlkDrvSerial.port[1].rxDma, TC_MASK|ABT_MASK|ERR_MASK);
			tlkapi_qfifo_clear(&sTlkDrvSerial.rfifo[port].qfifo);
			tlkdrv_serial_setRxDmaBuffer(port);
		}
	}
}
void tlkdrv_serial_regCB(uint08 port, TlkDrvSerialRecvCB cb)
{	
	if(port >= TLKDRV_SERIAL_MAX_NUMB) return;
	sTlkDrvSerial.recvCB[port] = cb;
}
bool tlkdrv_serial_isBusy(uint08 port)
{
	uint16 flags;

	if(port >= TLKDRV_SERIAL_MAX_NUMB) return false;

	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_INIT) == 0 || (flags & TLKDRV_SERIAL_FLAG_OPEN) == 0) return false;
	if((flags & TLKDRV_SERIAL_FLAG_TX_BUSY) != 0 || (flags & TLKDRV_SERIAL_FLAG_RX_BUSY) != 0) return true;
	if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0){
		if((flags & TLKDRV_SERIAL_FLAG_TX_DMA) == 0 && !tlkapi_fifo_isEmpty(&sTlkDrvSerial.sfifo[port].fifo)){
			return true;
		}
		if((flags & TLKDRV_SERIAL_FLAG_TX_DMA) != 0 && !tlkapi_qfifo_isEmpty(&sTlkDrvSerial.sfifo[port].qfifo)){
			return true;
		}
	}
	if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0){
		if((flags & TLKDRV_SERIAL_FLAG_RX_DMA) == 0 && !tlkapi_fifo_isEmpty(&sTlkDrvSerial.rfifo[port].fifo)){
			return true;
		}
		if((flags & TLKDRV_SERIAL_FLAG_RX_DMA) != 0 && !tlkapi_qfifo_isEmpty(&sTlkDrvSerial.rfifo[port].qfifo)){
			return true;
		}
	}
	return false;
}

void tlkdrv_serial_enterSleep(uint mcuMode)
{
	uint16 flags;

	flags = sTlkDrvSerial.flags[0];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		sTlkDrvSerial.flags[0] |= TLKDRV_SERIAL_FLAG_SLEEP;
		if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0 && (flags & TLKDRV_SERIAL_FLAG_TX_DMA) != 0){
			dma_chn_dis(sTlkDrvSerial.port[0].txDma);
			dma_clr_irq_mask(sTlkDrvSerial.port[0].txDma, TC_MASK|ABT_MASK|ERR_MASK);
		}
		if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0 && (flags & TLKDRV_SERIAL_FLAG_RX_DMA) != 0){
			dma_chn_dis(sTlkDrvSerial.port[0].rxDma);
			dma_clr_irq_mask(sTlkDrvSerial.port[0].rxDma, TC_MASK|ABT_MASK|ERR_MASK);
		}
	}

	flags = sTlkDrvSerial.flags[1];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		sTlkDrvSerial.flags[1] |= TLKDRV_SERIAL_FLAG_SLEEP;
		if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0 && (flags & TLKDRV_SERIAL_FLAG_TX_DMA) != 0){
			dma_chn_dis(sTlkDrvSerial.port[1].txDma);
			dma_clr_irq_mask(sTlkDrvSerial.port[1].txDma, TC_MASK|ABT_MASK|ERR_MASK);
		}
		if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0 && (flags & TLKDRV_SERIAL_FLAG_RX_DMA) != 0){
			dma_chn_dis(sTlkDrvSerial.port[1].rxDma);
			dma_clr_irq_mask(sTlkDrvSerial.port[1].rxDma, TC_MASK|ABT_MASK|ERR_MASK);
		}
	}
	
}
void tlkdrv_serial_leaveSleep(uint wakeSrc)
{
	uint16 flags;
	
	flags = sTlkDrvSerial.flags[0];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		sTlkDrvSerial.flags[0] &= ~TLKDRV_SERIAL_FLAG_SLEEP;
		sTlkDrvSerial.flags[0] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
		sTlkDrvSerial.flags[0] &= ~TLKDRV_SERIAL_FLAG_RX_BUSY;
		tlkdrv_serial_reset(0);
		if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0 && (flags & TLKDRV_SERIAL_FLAG_TX_DMA) != 0){
			tlkdrv_serial_setTxDmaBuffer(0);
		}else{
			uart_clr_tx_index(0);
		}
		if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0 && (flags & TLKDRV_SERIAL_FLAG_RX_DMA) != 0){
			tlkdrv_serial_setRxDmaBuffer(0);
		}else{
			uart_clr_rx_index(0);
		}
	}

	flags = sTlkDrvSerial.flags[1];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) != 0){
		sTlkDrvSerial.flags[1] &= ~TLKDRV_SERIAL_FLAG_SLEEP;
		sTlkDrvSerial.flags[1] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
		sTlkDrvSerial.flags[1] &= ~TLKDRV_SERIAL_FLAG_RX_BUSY;
		tlkdrv_serial_reset(1);
		if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0 && (flags & TLKDRV_SERIAL_FLAG_TX_DMA) != 0){
			tlkdrv_serial_setTxDmaBuffer(1);
		}else{
			uart_clr_tx_index(1);
		}
		if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0 && (flags & TLKDRV_SERIAL_FLAG_RX_DMA) != 0){
			tlkdrv_serial_setRxDmaBuffer(1);
		}else{
			uart_clr_rx_index(1);
		}
	}
}
bool tlkdrv_serial_sfifoIsMore60(uint08 port, uint16 dataLen)
{
	uint16 flags;
	uint16 count;
	uint16 isize; //Item Size
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB) return true;
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) == 0 || (flags & TLKDRV_SERIAL_FLAG_SEND) == 0){
		return true;
	}
	if((flags & TLKDRV_SERIAL_FLAG_TX_DMA) == 0) return false;
	
	isize = tlkapi_qfifo_size(&sTlkDrvSerial.sfifo[port].qfifo)-4;
	if(isize == 0) return true;
	if(dataLen == 0) count = 0;
	else if(dataLen <= isize) count = 1;
	else if(dataLen+dataLen <= isize) count = 2;
	else count = (dataLen+isize-1)/isize;
	if(tlkapi_qfifo_usedNum(&sTlkDrvSerial.sfifo[port].qfifo)+count
		> sTlkDrvSerial.sfifoM60[port]){
		return true;
	}else{
		return false;
	}	
}
bool tlkdrv_serial_sfifoIsMore85(uint08 port, uint16 dataLen)
{
	uint16 flags;
	uint16 count;
	uint16 isize; //Item Size
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB) return true;
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) == 0 || (flags & TLKDRV_SERIAL_FLAG_SEND) == 0){
		return true;
	}
	if((flags & TLKDRV_SERIAL_FLAG_TX_DMA) == 0) return false;

	isize = tlkapi_qfifo_size(&sTlkDrvSerial.sfifo[port].qfifo)-4;
	if(isize == 0) return true;
	if(dataLen == 0) count = 0;
	else if(dataLen <= isize) count = 1;
	else if(dataLen+dataLen <= isize) count = 2;
	else count = (dataLen+isize-1)/isize;
	if(tlkapi_qfifo_usedNum(&sTlkDrvSerial.sfifo[port].qfifo)+count
		> sTlkDrvSerial.sfifoM85[port]){
		return true;
	}else{
		return false;
	}	
}

void tlkdrv_serial_handler(uint08 port)
{
	uint16 flags;
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB) return;
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) == 0) return;
	
	if((flags & TLKDRV_SERIAL_FLAG_SEND) != 0 && (flags & TLKDRV_SERIAL_FLAG_TX_DMA) != 0
		&& (flags & TLKDRV_SERIAL_FLAG_TX_BUSY) == 0){
		tlkdrv_serial_setTxDmaBuffer(port);
	}
	if((flags & TLKDRV_SERIAL_FLAG_RECV) != 0){
		if((flags & TLKDRV_SERIAL_FLAG_RX_DMA) == 0){
			int ret;
			uint08 data[64];
			while(true){
				ret = tlkapi_fifo_read(&sTlkDrvSerial.rfifo[port].fifo, data, 64);
				if(ret <= 0) break;
				if(sTlkDrvSerial.recvCB[port] != nullptr){
					sTlkDrvSerial.recvCB[port](data, ret);
				}
			}
		}else{
			uint08 *pData;
			uint16 dataLen;
			if((reg_dma_tc_isr & BIT(sTlkDrvSerial.port[port].rxDma)) != 0){
				reg_dma_tc_isr |= BIT(sTlkDrvSerial.port[port].rxDma);
				#if (MCU_CORE_TYPE == MCU_CORE_B91)
				uart_clr_irq_status(port, UART_RX_ERR);
				sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_BUSY;
				#endif
				#if (MCU_CORE_TYPE == MCU_CORE_B92)
				tlkapi_fatal(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN,
					"tlkdrv_serial_handler: Rx DMA fatal");
				#endif
			}
			while(!tlkapi_qfifo_isEmpty(&sTlkDrvSerial.rfifo[port].qfifo)){
				pData = tlkapi_qfifo_getData(&sTlkDrvSerial.rfifo[port].qfifo);
				dataLen = (((uint16)pData[1]) << 8) | pData[0];
				pData  += 4;
				if(sTlkDrvSerial.recvCB[port] != nullptr){
					sTlkDrvSerial.recvCB[port](pData, dataLen);
				}
				tlkapi_qfifo_dropData(&sTlkDrvSerial.rfifo[port].qfifo);
			}
			if((flags & TLKDRV_SERIAL_FLAG_RX_BUSY) != 0){
				tlkdrv_serial_setRxDmaBuffer(port);
			}
		}
	}
}


int tlkdrv_serial_read(uint08 port, uint08 *pBuff, uint16 buffLen)
{
	uint16 flags;
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB || pBuff == nullptr || buffLen == 0){
		return -TLK_EPARAM;
	}
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) == 0 || (flags & TLKDRV_SERIAL_FLAG_RECV) == 0){
		return -TLK_ENOREADY;
	}
	
	if((flags & TLKDRV_SERIAL_FLAG_RX_DMA) == 0){
		return tlkapi_fifo_read(&sTlkDrvSerial.rfifo[port].fifo, pBuff, buffLen);
	}else{
		uint08 *pData;
		uint16 dataLen = 0;
		pData = tlkapi_qfifo_getData(&sTlkDrvSerial.rfifo[port].qfifo);
		if(pData != nullptr) dataLen = ((uint16)pData[1] << 8) | pData[0];
		if(dataLen > buffLen) return -TLK_EOVERFLOW;
		if(dataLen != 0) tmemcpy(pBuff, pData, dataLen);
		if(pData != nullptr) tlkapi_qfifo_dropData(&sTlkDrvSerial.rfifo[port].qfifo);
		if((flags & TLKDRV_SERIAL_FLAG_RX_BUSY) != 0){
			tlkdrv_serial_setRxDmaBuffer(port);
		}
		return dataLen;
	}
}
int tlkdrv_serial_send(uint08 port, uint08 *pData, uint16 dataLen)
{
	int ret;
	uint16 flags;
	
	if(port >= TLKDRV_SERIAL_MAX_NUMB || pData == nullptr || dataLen == 0){
		return -TLK_EPARAM;
	}
	
	flags = sTlkDrvSerial.flags[port];
	if((flags & TLKDRV_SERIAL_FLAG_OPEN) == 0 || (flags & TLKDRV_SERIAL_FLAG_SEND) == 0){
		return -TLK_ENOREADY;
	}
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_SLEEP) != 0){
		return -TLK_EBUSY;
	}

	tlksys_enter_critical();
	if((flags & TLKDRV_SERIAL_FLAG_TX_DMA) == 0){
		ret = tlkdrv_serial_sendWithoutDma(port, pData, dataLen);
	}else{
		ret = tlkdrv_serial_sendWithDma(port, pData, dataLen);
	}
	tlksys_leave_critical();
	return ret;
}

tlkdrv_serial_t *tlkdrv_serial_getNode(uint08 port)
{
	if(port >= TLKDRV_SERIAL_MAX_NUMB || (sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_INIT) == 0){
		return nullptr;
	}
	return &sTlkDrvSerial;
}



_attribute_ram_code_sec_noinline_
static int tlkdrv_serial_sendWithDma(uint08 port, uint08 *pData, uint16 dataLen)
{
	uint16 count;
	uint16 isize; //Item Size
	uint16 tempVar;
	uint08 *pBuffer;

	isize = tlkapi_qfifo_size(&sTlkDrvSerial.sfifo[port].qfifo)-4;
	if(isize == 0) return -TLK_EFAULT;

	count = (dataLen+isize-1)/isize;
	if(count > tlkapi_qfifo_idleNum(&sTlkDrvSerial.sfifo[port].qfifo)) return -TLK_EQUOTA;
	
	while((count--) != 0 && dataLen != 0){
//		tlkapi_trace(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "send 04: %d", dataLen);
		pBuffer = tlkapi_qfifo_getBuff(&sTlkDrvSerial.sfifo[port].qfifo);
		if(pBuffer == nullptr) break;
		if(dataLen <= isize) tempVar = dataLen;
		else tempVar = isize;
		pBuffer[0] = (tempVar & 0x00FF);
		pBuffer[1] = (tempVar & 0xFF00) >> 8;
		pBuffer[2] = 0;
		pBuffer[3] = 0;
		tmemcpy(pBuffer+4, pData, tempVar);
		pData   += tempVar;
		dataLen -= tempVar;
		tlkapi_qfifo_dropBuff(&sTlkDrvSerial.sfifo[port].qfifo);
//		tlkapi_array(TLKDRV_SERIAL_DBG_FLAG, TLKDRV_SERIAL_DBG_SIGN, "snd OK:", pBuffer, tempVar+4);
	}
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_TX_BUSY) == 0){
		tlkdrv_serial_setTxDmaBuffer(port);
	}
	return TLK_ENONE;
}
static int tlkdrv_serial_sendWithoutDma(uint08 port, uint08 *pData, uint16 dataLen)
{
#if (TLKDRV_SERIAL_B91_UART_ENABLE)
	return tlkdrv_b91uart_sendWithoutDma(port, sTlkDrvSerial.port[port].rtsPin, pData, dataLen);
#elif (TLKDRV_SERIAL_B92_UART_ENABLE)
	return tlkdrv_b92uart_sendWithoutDma(port, sTlkDrvSerial.port[port].rtsPin, pData, dataLen);
#else
	return -TLK_ENOSUPPORT;
#endif
}


_attribute_ram_code_sec_noinline_
void tlkdrv_serial_irqErrDone(uint08 port)
{
	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_SEND) != 0 && 
		(sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_TX_DMA) != 0){
		tlkdrv_serial_setTxDmaBuffer(port);
	}
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_RECV) != 0 && 
		(sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_RX_DMA) != 0){
		tlkdrv_serial_setRxDmaBuffer(port);
	}
}
_attribute_ram_code_sec_noinline_
void tlkdrv_serial_irqTxDoneWithDma(uint08 port)
{
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_SEND) == 0 || 
		(sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_TX_DMA) == 0){
		return;
	}
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_TX_BUSY) != 0){
		tlkapi_qfifo_dropData(&sTlkDrvSerial.sfifo[port].qfifo);
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
	}
	tlkdrv_serial_setTxDmaBuffer(port);
}
_attribute_ram_code_sec_noinline_
void tlkdrv_serial_irqTxDoneWithoutDma(uint08 port)
{
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_SEND) == 0 || 
		(sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_TX_DMA) != 0){
		return;
	}
	sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
}
_attribute_ram_code_sec_noinline_
void tlkdrv_serial_irqRxDoneWithDma(uint08 port, bool isDmaIrq)
{
	uint recvLen;
	uint32 *pBuffer;
	
	if(!isDmaIrq){
		if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_RECV) == 0 ||
			(sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_RX_DMA) == 0){
			#if (MCU_CORE_TYPE == MCU_CORE_B91)
			uart_clr_irq_status(port, UART_CLR_RX);
			#endif
			#if (MCU_CORE_TYPE == MCU_CORE_B92)
			uart_clr_irq_status(port, UART_RXDONE_IRQ_STATUS);
			#endif
			return;
		}
	}

	recvLen = uart_get_dma_rev_data_len(port, sTlkDrvSerial.port[port].rxDma);
	#if (MCU_CORE_TYPE == MCU_CORE_B91)
	uart_clr_irq_status(port, UART_CLR_RX);
	#endif
	#if (MCU_CORE_TYPE == MCU_CORE_B92)
	if(!isDmaIrq) uart_clr_irq_status(port, UART_RXDONE_IRQ_STATUS);
	#endif

	#if (TLKDRV_SERIAL_FLOW_CTRL_ENABLE)
	if(sTlkDrvSerial.port[port].rtsPin == 0 || recvLen != 0){
	#endif
	pBuffer = (uint32*)tlkapi_qfifo_getBuff(&sTlkDrvSerial.rfifo[port].qfifo);
	if(pBuffer != nullptr){
		pBuffer[0] = recvLen;
		tlkapi_qfifo_dropBuff(&sTlkDrvSerial.rfifo[port].qfifo);
	}
	tlkdrv_serial_setRxDmaBuffer(port);
	#if (TLKDRV_SERIAL_FLOW_CTRL_ENABLE)
	}
	#endif
}
_attribute_ram_code_sec_noinline_
void tlkdrv_serial_irqRxDoneWithoutDma(uint08 port)
{
	if((sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_RECV) == 0 || 
		(sTlkDrvSerial.flags[port] & TLKDRV_SERIAL_FLAG_RX_DMA) != 0){
		return;
	}
	while(uart_get_rxfifo_num(port) > 0){
		uint08 byte = reg_uart_data_buf(port, uart_rx_byte_index[port]) ;
		uart_rx_byte_index[port] ++;
		uart_rx_byte_index[port] &= 0x03;
		tlkapi_fifo_writeByte(&sTlkDrvSerial.rfifo[port].fifo, byte);
	}
}

_attribute_ram_code_sec_noinline_
static void tlkdrv_serial_setTxDmaBuffer(uint08 port)
{
	uint08 *pData;
	uint16 dataLen = 0;
	pData = tlkapi_qfifo_getData(&sTlkDrvSerial.sfifo[port].qfifo);
	if(pData != nullptr){
		dataLen = ((uint16)pData[1] << 8) | pData[0];
	}
	if(dataLen == 0){
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_TX_BUSY;
		//This data format is error
		if(pData != nullptr) tlkapi_qfifo_dropData(&sTlkDrvSerial.sfifo[port].qfifo);
	}else{
		delay_us(10);
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_TX_BUSY;
		uart_send_dma(port, pData+4, dataLen);
	}
}
_attribute_ram_code_sec_noinline_
static void tlkdrv_serial_setRxDmaBuffer(uint08 port)
{
	uint32 *pBuffer;
	
	pBuffer = (uint32*)tlkapi_qfifo_getBuff(&sTlkDrvSerial.rfifo[port].qfifo);
	if(pBuffer == nullptr){
		sTlkDrvSerial.flags[port] |= TLKDRV_SERIAL_FLAG_RX_BUSY;
	}else{
		pBuffer[0] = 0;
		sTlkDrvSerial.flags[port] &= ~TLKDRV_SERIAL_FLAG_RX_BUSY;
		uart_receive_dma(port, (uint08*)(pBuffer+1), tlkapi_qfifo_size(&sTlkDrvSerial.rfifo[port].qfifo)-4);
	}
}


#if (TLKDRV_SERIAL_MAX_NUMB >= 1)
_attribute_ram_code_sec_
void uart0_irq_handler(void)
{
#if (TLKDRV_SERIAL_B91_UART_ENABLE)
	tlkdrv_b91uart_irqHandler(UART0);
#elif (TLKDRV_SERIAL_B92_UART_ENABLE)
	tlkdrv_b92uart_irqHandler(UART0);
#endif
}
#endif
#if (TLKDRV_SERIAL_MAX_NUMB >= 2)
_attribute_ram_code_sec_
void uart1_irq_handler(void)
{
#if (TLKDRV_SERIAL_B91_UART_ENABLE)
	tlkdrv_b91uart_irqHandler(UART1);
#elif (TLKDRV_SERIAL_B92_UART_ENABLE)
	tlkdrv_b92uart_irqHandler(UART1);
#endif
}
#endif

_attribute_ram_code_sec_
void tlkdrv_serial_dmaIrqHandler(void)
{
	#if (TLKDRV_SERIAL_B92_UART_ENABLE) && (TLKDRV_SERIAL_FLOW_CTRL_ENABLE)
	if((sTlkDrvSerial.flags[UART0] & TLKDRV_SERIAL_FLAG_OPEN) != 0 && sTlkDrvSerial.port[UART0].rxDma != 0){
		if(dma_get_tc_irq_status(BIT(sTlkDrvSerial.port[UART0].rxDma))){
			dma_clr_tc_irq_status(BIT(sTlkDrvSerial.port[UART0].rxDma));
			tlkdrv_b92uart_dmaIrqHandler(sTlkDrvSerial.port[UART0].port, sTlkDrvSerial.port[UART0].rxDma);
		}
	}
	if((sTlkDrvSerial.flags[UART1] & TLKDRV_SERIAL_FLAG_OPEN) != 0 && sTlkDrvSerial.port[UART1].rxDma != 0){
		if(dma_get_tc_irq_status(BIT(sTlkDrvSerial.port[UART1].rxDma))){
			dma_clr_tc_irq_status(BIT(sTlkDrvSerial.port[UART1].rxDma));
			tlkdrv_b92uart_dmaIrqHandler(sTlkDrvSerial.port[UART1].port, sTlkDrvSerial.port[UART1].rxDma);
		}
	}
	#endif
}



#endif //#if (TLK_DEV_SERIAL_ENABLE)


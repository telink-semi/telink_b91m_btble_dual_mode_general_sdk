/********************************************************************************************************
 * @file	tlkdev_serial.c
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
#include "tlkdrv/ext/serial/tlkdrv_serial.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdev_serial.h"
#include "drivers.h"


static int tlkdev_serial_nodeInit(uint16 did);
static int tlkdev_serial_nodeOpen(uint16 did, uint32 param);
static int tlkdev_serial_nodeClose(uint16 did);
static int tlkdev_serial_nodeState(uint16 did);
static int tlkdev_serial_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_serial_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_serial_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);

static int tlkdev_serial_recvResetDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetBaudRateDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvGetBaudRateDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetFlowCtlDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvRegRecvCBDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetRxFifoDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetRxDmaFifoDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetTxDmaFifoDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvIsMore60PerDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvIsMore80PerDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvDisableRxDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvDisableTxDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetTxPinDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetRxPinDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetTxDmaDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetRxDmaDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetCtsPinDeal(uint16 did, uint32 param0, uint32 param1);
static int tlkdev_serial_recvSetRtsPinDeal(uint16 did, uint32 param0, uint32 param1);

#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
static const tlkdev_node_t scTlkDevSerail0 = {
	TLKDEV_DID_SERIAL0,  //.devID;
	"Serail0",			 //.pName;
	tlkdev_serial_nodeInit,  //.Init
	tlkdev_serial_nodeOpen,  //.Open
	tlkdev_serial_nodeClose, //.Close
	tlkdev_serial_nodeState, //.State
	tlkdev_serial_nodeRead,  //.Read
	tlkdev_serial_nodeWrite, //.Write
	tlkdev_serial_nodeIoctl, //.Ioctl
};
#endif

#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2)\
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
static const tlkdev_node_t scTlkDevSerail1 = {
	TLKDEV_DID_SERIAL1,  //.devID;
	"Serail1",           //.pName;
	tlkdev_serial_nodeInit,  //.Init
	tlkdev_serial_nodeOpen,  //.Open
	tlkdev_serial_nodeClose, //.Close
	tlkdev_serial_nodeState, //.State
	tlkdev_serial_nodeRead,  //.Read
	tlkdev_serial_nodeWrite, //.Write
	tlkdev_serial_nodeIoctl, //.Ioctl
};
#endif


int tlkdev_serial_init(void)
{
	tlkdrv_serial_init();
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
	tlkdev_appendEctypeNode(&scTlkDevSerail0);
#endif
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2)\
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
	tlkdev_appendEctypeNode(&scTlkDevSerail1);
#endif

	return TLK_ENONE;
}
void tlkdev_serial_deinit(void)
{
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
	tlkdev_removeEctypeNode(TLKDEV_DID_SERIAL0);
#endif
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2)\
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
	tlkdev_removeEctypeNode(TLKDEV_DID_SERIAL1);
#endif
}

bool tlkdev_serial_isBusy(void)
{
	bool isBusy = false;
	if(!isBusy) isBusy = tlkdrv_serial_isBusy(UART0);
	if(!isBusy) isBusy = tlkdrv_serial_isBusy(UART1);
	return isBusy;
}

void tlkdev_serial_enterSleep(uint mcuMode)
{
	tlkdrv_serial_enterSleep(mcuMode);
}
void tlkdev_serial_leaveSleep(uint wakeSrc)
{
	tlkdrv_serial_leaveSleep(wakeSrc);
}


void tlkdev_serial_handler(void)
{
	tlkdrv_serial_handler(UART0);
	tlkdrv_serial_handler(UART1);
}



static int tlkdev_serial_nodeInit(uint16 did)
{
	int ret;
	uint08 port;
	uint16 txPin;
	uint16 rxPin;

	port = 0xFF;
	txPin = 0;
	rxPin = 0;
	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
		txPin = TLKDEV_SERIAL0_TX_PIN;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
		txPin = TLKDEV_SERIAL1_TX_PIN;
		rxPin = TLKDEV_SERIAL1_RX_PIN;
	}
	if(port == 0xFF || (txPin == 0 && rxPin == 0)){
		return -TLK_ENOSUPPORT;
	}
	ret = tlkdrv_serial_portInit(port, 115200, txPin, rxPin);
	if(ret != TLK_ENONE){
		return ret;
	}
	
	return TLK_ENONE;
}
static int tlkdev_serial_nodeOpen(uint16 did, uint32 param)
{
	uint08 port;

	port = 0xFF;
	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}
	if(port == 0xFF){
		return -TLK_ENOSUPPORT;
	}
	
	return tlkdrv_serial_open(port);
}
static int tlkdev_serial_nodeClose(uint16 did)
{
	uint08 port;

	port = 0xFF;
	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}
	if(port == 0xFF){
		return -TLK_ENOSUPPORT;
	}
	  
	return tlkdrv_serial_close(port);
}
static int tlkdev_serial_nodeState(uint16 did)
{
	uint08 port;
	tlkdrv_serial_t *pUnit;
	
	port = 0xFF;
	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}
	if(port == 0xFF){
		return TLKDEV_STATE_UNKNOWN;
	}
	
	pUnit = tlkdrv_serial_getNode(port);
	if(pUnit == nullptr) return TLKDEV_STATE_UNMOUNT;

	if((pUnit->flags[port] & TLKDRV_SERIAL_FLAG_OPEN) == 0){
		return TLKDEV_STATE_CLOSED;
	}
	if((pUnit->flags[port] & TLKDRV_SERIAL_FLAG_TX_BUSY) != 0
		|| (pUnit->flags[port] & TLKDRV_SERIAL_FLAG_RX_BUSY) != 0){
		return TLKDEV_STATE_BUSYING;
	}
	
	return TLKDEV_STATE_OPENED;
}
static int tlkdev_serial_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_read(port, pBuff, buffLen);
}
static int tlkdev_serial_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_send(port, pData, dataLen);
}

static int tlkdev_serial_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKDEV_OPCODE_ENTER_SLEEP){
		tlkdrv_serial_enterSleep(param0);
	}else if(opcode == TLKDEV_OPCODE_LEAVE_SLEEP){
		tlkdrv_serial_leaveSleep(param0);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_RESET){
		return tlkdev_serial_recvResetDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_BAUDRATE){
		return tlkdev_serial_recvSetBaudRateDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_GET_BAUDRATE){
		return tlkdev_serial_recvGetBaudRateDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_FLOW_CTL){
		return tlkdev_serial_recvSetFlowCtlDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_REG_RECV_CB){
		return tlkdev_serial_recvRegRecvCBDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_RX_FIFO){
		return tlkdev_serial_recvSetRxFifoDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO){
		return tlkdev_serial_recvSetRxDmaFifoDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_TX_QFIFO){
		return tlkdev_serial_recvSetTxDmaFifoDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_IS_MORE_60PER){
		return tlkdev_serial_recvIsMore60PerDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_IS_MORE_80PER){
		return tlkdev_serial_recvIsMore80PerDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_DISABLE_RX){
		return tlkdev_serial_recvDisableRxDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_DISABLE_TX){
		return tlkdev_serial_recvDisableTxDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_TX_PIN){
		return tlkdev_serial_recvSetTxPinDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_RX_PIN){
		return tlkdev_serial_recvSetRxPinDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_TX_DMA){
		return tlkdev_serial_recvSetTxDmaDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_RX_DMA){
		return tlkdev_serial_recvSetRxDmaDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_CTS_PIN){
		return tlkdev_serial_recvSetCtsPinDeal(did, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_SERIAL_SET_RTS_PIN){
		return tlkdev_serial_recvSetRtsPinDeal(did, param0, param1);
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}

static int tlkdev_serial_recvResetDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	tlkdrv_serial_reset(port);
	return TLK_ENONE;
}
static int tlkdev_serial_recvSetBaudRateDeal(uint16 did, uint32 param0, uint32 param1)
{
	if(did == TLKDEV_DID_SERIAL0){
		tlkdrv_serial_setBaudrate(UART0, param0);
	}else if(did == TLKDEV_DID_SERIAL1){
		tlkdrv_serial_setBaudrate(UART1, param0);
	}else{
		return -TLK_ENOSUPPORT;
	}
	return TLK_ENONE;
}
static int tlkdev_serial_recvGetBaudRateDeal(uint16 did, uint32 param0, uint32 param1)
{
	int ret = -TLK_ENOSUPPORT;
	if(param0 == 0) return -TLK_EPARAM;
	if(did == TLKDEV_DID_SERIAL0){
		ret = tlkdrv_serial_getBaudrate(UART0);
	}else if(did == TLKDEV_DID_SERIAL1){
		ret = tlkdrv_serial_getBaudrate(UART1);
	}
	if(ret < 0){
		return ret;
	}
	*((uint32*)param0) = ret;
	return TLK_ENONE;
}
static int tlkdev_serial_recvSetFlowCtlDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;
	uint16 ctsPin = 0;
	uint16 rtsPin = 0;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
		ctsPin = TLKDEV_SERIAL0_CTS_PIN;
		rtsPin = TLKDEV_SERIAL0_RTS_PIN;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
		ctsPin = TLKDEV_SERIAL1_CTS_PIN;
		rtsPin = TLKDEV_SERIAL1_RTS_PIN;
	}else{
		return -TLK_ENOSUPPORT;
	}
	
	if(((param0 & 0x01) != 0 && ctsPin == 0) || ((param0 & 0x02) != 0 && rtsPin == 0)){
		return -TLK_ENOSUPPORT;
	}
	if((param0 & 0x01) == 0) tlkdrv_serial_setCtsPin(port, 0);
	else tlkdrv_serial_setCtsPin(port, ctsPin);

	if((param0 & 0x02) == 0) tlkdrv_serial_setRtsPin(port, 0);
	else tlkdrv_serial_setRtsPin(port, rtsPin);

	return TLK_ENONE;
}
static int tlkdev_serial_recvRegRecvCBDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	tlkdrv_serial_regCB(port, (TlkDrvSerialRecvCB)param0);
	return TLK_ENONE;
}
static int tlkdev_serial_recvSetRxFifoDeal(uint16 did, uint32 param0, uint32 param1)
{
	int ret;
	uint08 port = 0xFF;
	uint16 buffLen = param0 & 0xFFFF;
	uint08 *pBuffer = (uint08*)param1;
	
	if(buffLen < 64){
		return -TLK_EPARAM;
	}
	
	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}

	ret = tlkdrv_serial_setRxDma(port, 0);
	if(ret != TLK_ENONE){
		return ret;
	}
	
	return tlkdrv_serial_setRxFifo(port, pBuffer, buffLen);
}
static int tlkdev_serial_recvSetRxDmaFifoDeal(uint16 did, uint32 param0, uint32 param1)
{
	int ret;
	uint08 port = 0xFF;
	uint08 rxDma = 0;
	uint16 fifoSize = param0 & 0xFFFF;
	uint16 fifoNumb = (param0 & 0xFFFF0000) >> 16;
	uint08 *pBuffer = (uint08*)param1;
	
	if(fifoSize == 0 || fifoNumb == 0 || pBuffer == nullptr || (fifoSize & 0x03) != 0
		|| (param1 & 0x03) != 0){
		return -TLK_EPARAM;
	}
	
	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
		rxDma = TLKDEV_SERIAL0_RX_DMA;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
		rxDma = TLKDEV_SERIAL1_RX_DMA;
	}else{
		return -TLK_ENOSUPPORT;
	}
	if(rxDma == 0){
		return -TLK_ENOSUPPORT;
	}

	ret = tlkdrv_serial_setRxDma(port, rxDma);
	if(ret != TLK_ENONE){
		return ret;
	}
	
	return tlkdrv_serial_setRxQFifo(port, fifoNumb, fifoSize, pBuffer, fifoNumb*fifoSize);
}
static int tlkdev_serial_recvSetTxDmaFifoDeal(uint16 did, uint32 param0, uint32 param1)
{
	int ret;
	uint08 port = 0xFF;
	uint08 txDma = 0;
	uint16 fifoSize = param0 & 0xFFFF;
	uint16 fifoNumb = (param0 & 0xFFFF0000) >> 16;
	uint08 *pBuffer = (uint08*)param1;
	
	if(fifoSize == 0 || fifoNumb == 0 || pBuffer == nullptr || (fifoSize & 0x03) != 0
		|| (param1 & 0x03) != 0){
		return -TLK_EPARAM;
	}
	
	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
		txDma = TLKDEV_SERIAL0_TX_DMA;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
		txDma = TLKDEV_SERIAL1_TX_DMA;
	}else{
		return -TLK_ENOSUPPORT;
	}
	if(txDma == 0){
		return -TLK_ENOSUPPORT;
	}
	ret = tlkdrv_serial_setTxDma(port, txDma);
	if(ret != TLK_ENONE){
		return ret;
	}
	return tlkdrv_serial_setTxQFifo(port, fifoNumb, fifoSize, pBuffer, fifoNumb*fifoSize);
}
static int tlkdev_serial_recvIsMore60PerDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}

	return tlkdrv_serial_sfifoIsMore60(port, param0);
}
static int tlkdev_serial_recvIsMore80PerDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	
	return tlkdrv_serial_sfifoIsMore85(port, param0);
}
static int tlkdev_serial_recvDisableRxDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setRxPin(port, 0);
}
static int tlkdev_serial_recvDisableTxDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setTxPin(port, 0);
}
static int tlkdev_serial_recvSetTxPinDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setTxPin(port, param0);
}
static int tlkdev_serial_recvSetRxPinDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setRxPin(port, param0);
}
static int tlkdev_serial_recvSetTxDmaDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setTxDma(port, param0);
}
static int tlkdev_serial_recvSetRxDmaDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setRxDma(port, param0);
}
static int tlkdev_serial_recvSetCtsPinDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setCtsPin(port, param0);
}
static int tlkdev_serial_recvSetRtsPinDeal(uint16 did, uint32 param0, uint32 param1)
{
	uint08 port = 0xFF;

	if(did == TLKDEV_DID_SERIAL0){
		port = UART0;
	}else if(did == TLKDEV_DID_SERIAL1){
		port = UART1;
	}else{
		return -TLK_ENOSUPPORT;
	}
	return tlkdrv_serial_setRtsPin(port, param0);
}



#endif //#if (TLK_DEV_SERIAL_ENABLE)


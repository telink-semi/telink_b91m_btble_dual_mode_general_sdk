/********************************************************************************************************
 * @file	tlkmdi_comm.c
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
#if (TLK_CFG_COMM_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlklib/cpc/tlkcpc.h"
#include "tlkdev/tlkdev.h"
#include "tlkmdi_comm.h"
#if (TLK_DEV_SERIAL_ENABLE)
#include "tlkdev/sys/tlkdev_serial.h"
#endif


#define TLKMDI_COMM_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_MISC << 24) | (TLK_MINOR_DBGID_MDI_COMM << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_COMM_DBG_SIGN       "[MDI]"


static void tlkmdi_comm_input(uint08 *pData, uint16 dataLen);
static int tlkmdi_comm_recvCB(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen);

#if (TLK_DEV_SERIAL_ENABLE)
__attribute__((aligned(4)))
static uint08 sTlkMdiCommRecvBuffer[TLKMDI_COMM_SERIAL_RBUFF_NUMB*(TLKMDI_COMM_SERIAL_RBUFF_SIZE+4)];
__attribute__((aligned(4)))
static uint08 sTlkMdiCommSendBuffer[TLKMDI_COMM_SERIAL_SBUFF_NUMB*(TLKMDI_COMM_SERIAL_SBUFF_SIZE+4)];
extern uint32 tlkcfg_getSerialBaudrate(void);
#endif


typedef struct{
	uint16 taskList[TLKPRT_COMM_MTYPE_MAX];
	TlkMdiCommDatCB dataList[TLKMDI_COMM_DATA_CHANNEL_MAX+1];
}tlkmdi_comm_ctrl_t;
static tlkmdi_comm_ctrl_t sTlkMdiCommCtrl;

static const tlkcpc_infs_t *scpTlkMdiCommCurInfs = nullptr;

extern const tlkcpc_infs_t gcTlkCpc1Infs;
extern const tlkcpc_infs_t gcTlkCpc2Infs;
extern const tlkcpc_infs_t gcTlkCpc3Infs;
static uint08 sTlkMdiCommRecvCache[TLKMDI_COMM_RECV_CACHE_SIZE];
static uint08 sTlkMdiCommSendCache[TLKMDI_COMM_SEND_CACHE_SIZE];



/******************************************************************************
 * Function: tlkmdi_comm_init.
 * Descript: Initial the Serial port and register event handler callback.
 * Params: 
 *     @procID--Process ID. Refer to TLKSYS_PROCID_ENUM.
 *     @type--Protocol type. Refer TLKMDI_COMM_PROTOCOL_ENUM.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_init(uint08 procID, TLKMDI_COMM_PROTOCOL_ENUM type)
{
	if(type == TLKMDI_COMM_PROTOCOL_CPC1){
		scpTlkMdiCommCurInfs = &gcTlkCpc1Infs;
	}else if(type == TLKMDI_COMM_PROTOCOL_CPC2){
		scpTlkMdiCommCurInfs = &gcTlkCpc2Infs;
	}else if(type == TLKMDI_COMM_PROTOCOL_CPC3){
		scpTlkMdiCommCurInfs = &gcTlkCpc3Infs;
	}else{
		return -TLK_ENOSUPPORT;
	}
	
	scpTlkMdiCommCurInfs->Init(procID);
	scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_REG_REG_RECEIVE_CB, (uint32)tlkmdi_comm_recvCB, 0);
	scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_SET_RECV_BUFFER, (uint32)sTlkMdiCommRecvCache, TLKMDI_COMM_RECV_CACHE_SIZE);
	scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_SET_SEND_BUFFER, (uint32)sTlkMdiCommSendCache, TLKMDI_COMM_SEND_CACHE_SIZE);
	
	tmemset(&sTlkMdiCommCtrl, 0, sizeof(tlkmdi_comm_ctrl_t));
#if (TLK_DEV_SERIAL_ENABLE)
	uint32 baudrate = tlkcfg_getSerialBaudrate();
	if(baudrate == 0xFFFFFFFF) baudrate = TLKMDI_COMM_SERIAL_BAUDRATE;
	tlkdev_mount(TLKMDI_COMM_SERIAL_DID);
	tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_BAUDRATE, baudrate, 0);
	tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_REG_RECV_CB, (uint32)tlkmdi_comm_input, 0);
	tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO,
		((TLKMDI_COMM_SERIAL_RBUFF_NUMB & 0xFFFF)<<16) |((TLKMDI_COMM_SERIAL_RBUFF_SIZE+4) & 0xFFFF),
		(uint32)sTlkMdiCommRecvBuffer);
	tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_TX_QFIFO,
		((TLKMDI_COMM_SERIAL_SBUFF_NUMB & 0xFFFF)<<16) | ((TLKMDI_COMM_SERIAL_SBUFF_SIZE+4) & 0xFFFF),
		(uint32)sTlkMdiCommSendBuffer);
	scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_REG_SENDCB, (uint32)tlkmdi_comm_send, 0);
#endif
		
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_comm_start.
 * Descript: Enable serial port communication.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_start(void)
{
#if ((TLK_DBG_LOG_ENABLE) || (TLK_DBG_VCD_ENABLE))
	if(scpTlkMdiCommCurInfs != nullptr){
		uint32 tempLen;
		uint08 buffer[16];

#if (TLK_DBG_LOG_ENABLE)
		tempLen = scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE, 0, 0);
		UINT32L_TO_ARRAY(tempLen, buffer, 0);
		tempLen = (uint32)tlkmdi_comm_sendLogDat;
		UINT32L_TO_ARRAY(tempLen, buffer, 4);
		tlksys_sendInnerMsg(TLKSYS_TASKID_DEBUG, TLKPTI_DBG_MSGID_REG_HPULOG_CB, buffer, 8);
#endif
#if (TLK_DBG_VCD_ENABLE)
		tempLen = (uint32)tlkmdi_comm_sendVcdDat;
		UINT32L_TO_ARRAY(tempLen, buffer, 4);
		tlksys_sendInnerMsg(TLKSYS_TASKID_DEBUG, TLKPTI_DBG_MSGID_REG_HPUVCD_CB, buffer, 8);
#endif
	}	
#endif

#if (TLK_DEV_SERIAL_ENABLE)
	int ret = tlkdev_open(TLKMDI_COMM_SERIAL_DID, 0);
	if(ret == TLK_ENONE && scpTlkMdiCommCurInfs != nullptr 
		&& scpTlkMdiCommCurInfs->Reset != nullptr){
		scpTlkMdiCommCurInfs->Reset();
	}
	return ret;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_reset.
 * Descript: Reset serial port communication.
 * Params: None.
 * Return: None
 *****************************************************************************/
void tlkmdi_comm_reset(void)
{
#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_RESET, 0, 0);
#endif
	if(scpTlkMdiCommCurInfs != nullptr && scpTlkMdiCommCurInfs->Reset != nullptr){
		scpTlkMdiCommCurInfs->Reset();
	}
}

/******************************************************************************
 * Function: tlkmdi_comm_handler.
 * Descript: Serial communication loop processing interface.
 * Params: None.
 * Return: None
 * Notice:
 *     1.The data in the serial communication will be cached into the fifo, 
 *       and it is necessary to call the interface repeatedly to take out the
 *       cached data.
 *****************************************************************************/
void tlkmdi_comm_handler(void)
{
#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_handler();
	if(scpTlkMdiCommCurInfs != nullptr && scpTlkMdiCommCurInfs->Process != nullptr){
		scpTlkMdiCommCurInfs->Process();
	}
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_getBufferSize.
 * Descript: Gets the size of the protocol's send buffer.
 * Params: None.
 * Return: The size of the protocol's send buffer.
 * Notice:
 *     1."tlkmdi_comm_getDataBufferSize()" is usually to get the buffer's size 
 *       of the CMD/RSP/EVT package.
 *****************************************************************************/
int tlkmdi_comm_getBufferSize(void)
{
	if(scpTlkMdiCommCurInfs == nullptr) return 0;
	return scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_GET_BUFFER_SIZE, 0, 0);
}

/******************************************************************************
 * Function: tlkmdi_comm_getDataBufferSize.
 * Descript: Gets the size of the protocol's send buffer.
 * Params: None.
 * Return: The size of the protocol's send buffer.
 * Notice:
 *     1."tlkmdi_comm_getDataBufferSize()" is usually to get the buffer's size   
 *       of the DAT package.
 *****************************************************************************/
int tlkmdi_comm_getDataBufferSize(void)
{
	if(scpTlkMdiCommCurInfs == nullptr) return 0;
	return scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE, 0, 0);
}

/******************************************************************************
 * Function: tlkmdi_comm_setSendFifo.
 * Descript: The transmit buffer of the device communication layer.
 * Params: 
 *     @pBuffer[IN]--The transmit buffer.
 *     @itemNumb[IN]--The number of items contained in the buffer.
 *     @itemSize[IN]--The length of items contained in the buffer.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice:
 *     1.If the input buffer is nullptr, the default buffer is used.
 *****************************************************************************/
int tlkmdi_comm_setSendFifo(uint08 *pBuffer, uint16 itemNumb, uint16 itemSize)
{
#if (TLK_DEV_SERIAL_ENABLE)
	uint08 state;
	
	state = tlkdev_state(TLKMDI_COMM_SERIAL_DID);
	if(state != TLKDEV_STATE_OPENED && state != TLKDEV_STATE_BUSYING && state != TLKDEV_STATE_CLOSED){
		return -TLK_ESTATUS;
	}

	if(state == TLKDEV_STATE_OPENED){
		tlkdev_close(TLKMDI_COMM_SERIAL_DID);
	}
	if(pBuffer == nullptr || itemNumb == 0 || itemSize == 0){
		tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_TX_QFIFO,
			((TLKMDI_COMM_SERIAL_SBUFF_NUMB & 0xFFFF)<<16) | ((TLKMDI_COMM_SERIAL_SBUFF_SIZE+4) & 0xFFFF),
			(uint32)sTlkMdiCommSendBuffer);
	}else{
		tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_TX_QFIFO,
			((itemNumb & 0xFFFF)<<16) | (itemSize & 0xFFFF), (uint32)pBuffer);
	}
	if(state == TLKDEV_STATE_OPENED){
		tlkdev_open(TLKMDI_COMM_SERIAL_DID, 0);
	}
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_setRecvFifo.
 * Descript: The receive buffer of the device communication layer.
 * Params: 
 *     @pBuffer[IN]--The receive buffer.
 *     @itemNumb[IN]--The number of items contained in the buffer.
 *     @itemSize[IN]--The length of items contained in the buffer.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice:
 *     1.If the input buffer is nullptr, the default buffer is used.
 *****************************************************************************/
int tlkmdi_comm_setRecvFifo(uint08 *pBuffer, uint16 itemNumb, uint16 itemSize)
{
#if (TLK_DEV_SERIAL_ENABLE)
	uint08 state;

	if(pBuffer == nullptr || itemNumb == 0 || itemSize == 0){
		return -TLK_EPARAM;
	}
	
	state = tlkdev_state(TLKMDI_COMM_SERIAL_DID);
	if(state != TLKDEV_STATE_OPENED && state != TLKDEV_STATE_BUSYING && state != TLKDEV_STATE_CLOSED){
		return -TLK_ESTATUS;
	}

	if(state == TLKDEV_STATE_OPENED){
		tlkdev_close(TLKMDI_COMM_SERIAL_DID);
	}
	if(pBuffer == nullptr || itemNumb == 0 || itemSize == 0){
		tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO,
			((TLKMDI_COMM_SERIAL_RBUFF_NUMB & 0xFFFF)<<16) |(TLKMDI_COMM_SERIAL_RBUFF_SIZE+4 & 0xFFFF),
			(uint32)sTlkMdiCommRecvBuffer);
	}else{
		tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO,
			((itemNumb & 0xFFFF)<<16) | (itemSize & 0xFFFF), (uint32)pBuffer);
	}
	if(state == TLKDEV_STATE_OPENED){
		tlkdev_open(TLKMDI_COMM_SERIAL_DID, 0);
	}
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_getBaudrate.
 * Descript: Obtain the baud rate of the current serial port communication.
 * Params: None.
 * Return: The baud rate for the current serial. Negative values mean failure.
 *****************************************************************************/
int tlkmdi_comm_getBaudrate(void)
{
#if (TLK_DEV_SERIAL_ENABLE)
	int ret;
	uint32 baudRate = 0;
	
	ret = tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_GET_BAUDRATE, (uint32)&baudRate, 0);
	if(ret < 0) return ret;
	
	return baudRate;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_setBaudrate.
 * Descript: Set the baud rate for the current serial port communication.
 * Params:
 *     @baudrate[IN]--The baud rate for the current serial.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_setBaudrate(uint32 baudrate)
{
#if (TLK_DEV_SERIAL_ENABLE)
	int ret;
	ret = tlkdev_state(TLKMDI_COMM_SERIAL_DID);;
	if(ret == TLKDEV_STATE_OPENED || ret == TLKDEV_STATE_BUSYING){
		ret = tlkdev_close(TLKMDI_COMM_SERIAL_DID);
		if(!ret) ret = tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_BAUDRATE, baudrate, 0);;
		if(!ret) ret = tlkdev_open(TLKMDI_COMM_SERIAL_DID, 0);
	}else{
		ret = tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_BAUDRATE, baudrate, 0);;
	}
	return ret;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_regResetRequestCB.
 * Descript: Registration protocol reset callback.
 * Params:
 *     @resetCB[IN]--The callback after reset.
 * Return: TLK_ENONE is success,other value is failure.
 * Notice: 
 *     1.The interface only be available under TLKMDI_COMM_PROTOCOL_CPC2 enable.
 *****************************************************************************/
int tlkmdi_comm_regResetRequestCB(TlkMdiCommResetCB resetCB)
{
	if(scpTlkMdiCommCurInfs != nullptr && scpTlkMdiCommCurInfs->Handler != nullptr){
		return scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_REG_REG_RESET_REQUEST_CB, (uint32)resetCB, 0);
	}else{
		return -TLK_ENOSUPPORT;
	}
}

/******************************************************************************
 * Function: tlkmdi_comm_regResetCompleteCB.
 * Descript: Registration protocol reset callback.
 * Params:
 *     @resetCB[IN]--The callback after reset.
 * Return: TLK_ENONE is success,other value is failure.
 * Notice: 
 *     1.The interface only be available under TLKMDI_COMM_PROTOCOL_CPC2 enable.
 *****************************************************************************/
int tlkmdi_comm_regResetCompleteCB(TlkMdiCommResetCB resetCB)
{
	if(scpTlkMdiCommCurInfs != nullptr && scpTlkMdiCommCurInfs->Handler != nullptr){
		return scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_REG_REG_RESET_COMPLETE_CB, (uint32)resetCB, 0);
	}else{
		return -TLK_ENOSUPPORT;
	}
}

/******************************************************************************
 * Function: tlkmdi_comm_sfifoIsMore60.
 * Descript: Detects whether the transmission fifo usage exceeds 60%.
 * Params:
 *     @dataLen[IN]--Length to be used by the user.
 * Return: TRUE indicates that the current fifo usage exceeds 60%.
 *****************************************************************************/
bool tlkmdi_comm_sfifoIsMore60(uint16 dataLen)
{
#if (TLK_DEV_SERIAL_ENABLE)
	int ret = tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_IS_MORE_60PER, dataLen, 0);
	if(ret < 0) return true;
	return false;
#else
	return true;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_sfifoIsMore80.
 * Descript: Detects whether the transmission fifo usage exceeds 80%.
 * Params:
 *     @dataLen[IN]--Length to be used by the user.
 * Return: TRUE indicates that the current fifo usage exceeds 80%.
 *****************************************************************************/
bool tlkmdi_comm_sfifoIsMore80(uint16 dataLen)
{
#if (TLK_DEV_SERIAL_ENABLE)
	int ret = tlkdev_ioctl(TLKMDI_COMM_SERIAL_DID, TLKDEV_OPCODE_SERIAL_IS_MORE_80PER, dataLen, 0);
	if(ret < 0) return true;
	return false;
#else
	return true;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_getValidDatID.
 * Descript: From the device number table in the system, look for an available
 *           device number.
 * Params:
 *     @pDatID[OUT]--Available device number.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_getValidDatID(uint08 *pDatID)
{
	uint08 index;
	for(index=1; index<=TLKMDI_COMM_DATA_CHANNEL_MAX; index++){
		if(sTlkMdiCommCtrl.dataList[index] == 0) break;
	}
	if(index == TLKMDI_COMM_DATA_CHANNEL_MAX+1) return -TLK_EQUOTA;
	if(pDatID != nullptr) *pDatID = index;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_comm_regCmdCB.
 * Descript: Registers a task for the message type in the system to distribute
 *     the received message to the specified task.
 * Params:
 *     @mtype[IN]--The Type of message.
 *     @taskID[IN]--The ID of the task.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_regCmdCB(uint08 mtype, uint16 taskID)
{
	if(mtype == 0 || mtype >= TLKPRT_COMM_MTYPE_MAX) return -TLK_EPARAM;
	sTlkMdiCommCtrl.taskList[mtype] = taskID;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_comm_getTaskID.
 * Descript: Get the task number based on the protocol type.
 * Params:
 *     @mtype[IN]--The Type of message.
 *     @pTaskID[OUT]--The ID of the task.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice: 
 *     1.This interface corresponds to "tlkmdi_comm_regCmdCB()".
 *****************************************************************************/
int tlkmdi_comm_getTaskID(uint08 mtype, uint16 *pTaskID)
{
	if(mtype == 0 || mtype >= TLKPRT_COMM_MTYPE_MAX) return -TLK_EPARAM;
	if(sTlkMdiCommCtrl.taskList[mtype] == 0) return -TLK_ENOREADY;
	if(pTaskID != nullptr) *pTaskID = sTlkMdiCommCtrl.taskList[mtype];
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_comm_regDatCB.
 * Descript: Register the data callback.
 * Params:
 *        @datID[IN]--The data channel Id.
 *        @datCB[IN]--The data callback.
 *        @isForce[IN]--is force to process.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_comm_regDatCB(uint08 datID, TlkMdiCommDatCB datCB, bool isForce)
{
	if(datID > TLKMDI_COMM_DATA_CHANNEL_MAX) return -TLK_EQUOTA;
	if(!isForce && datCB != nullptr && sTlkMdiCommCtrl.dataList[datID] != nullptr){
		return -TLK_EREPEAT;
	}
	sTlkMdiCommCtrl.dataList[datID] = datCB;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_comm_send.
 * Descript: Send data to peripheral devices.
 * Params: 
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of data to be sent.
 * Return: The length to be sent. Negative values mean failure.
 * Notice:
 *     1.The data sent by this interface will not be packaged. Therefore, use 
 *       caution when invoking the data.
 *****************************************************************************/
int tlkmdi_comm_send(uint08 *pData, uint16 dataLen)
{
#if (TLK_DEV_SERIAL_ENABLE)
	return tlkdev_write(TLKMDI_COMM_SERIAL_DID, 0, pData, dataLen);
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkmdi_comm_sendCmd.
 * Descript: Send a command packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @cmdID[IN]--The command ID. Range:0~4095.
 *     @pData[IN]--The data to be sent. Range:0~1024.
 *     @dataLen[IN]--The length of the data to be sent.
 * Return: Negative values mean failure, other values mean success.
 *****************************************************************************/
int tlkmdi_comm_sendCmd(uint08 mType, uint16 cmdID, uint08 *pData, uint08 dataLen)
{
	uint32 param;

	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param = ((uint32)(cmdID & 0x0FFF)<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_CMD;
	return scpTlkMdiCommCurInfs->SendProc(param, 0, pData, dataLen);
}
int tlkmdi_comm_sendCmdNoFlow(uint08 mType, uint16 cmdID, uint08 *pData, uint08 dataLen)
{
	uint32 param;

	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param = ((uint32)(cmdID & 0x0FFF)<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_CMD
		| 0x80000000;
	return scpTlkMdiCommCurInfs->SendProc(param, 0, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendRsp.
 * Descript: Send a response packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @cmdID[IN]--The command ID. Range:0~4095.
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~1024.
 *     @status[IN]--Command processing status. Refer to TLKPRT_COMM_RSP_STATUE_ENUM.
 *     @reason[IN]--Failure cause code. Refer to "tlk_error.h".
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1.The reason field is only valid if the status is failure.
 *****************************************************************************/
int tlkmdi_comm_sendRsp(uint08 mType, uint16 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint32 param0;
	uint32 param1;
	
	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param0 = ((uint32)(cmdID & 0x0FFF)<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_RSP;
	param1 = ((uint16)reason<<8)|status;
	return scpTlkMdiCommCurInfs->SendProc(param0, param1, pData, dataLen);
}
int tlkmdi_comm_sendRspNoFlow(uint08 mType, uint16 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint32 param0;
	uint32 param1;
	
	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param0 = ((uint32)(cmdID & 0x0FFF)<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_RSP
		| 0x80000000;
	param1 = ((uint16)reason<<8)|status;
	return scpTlkMdiCommCurInfs->SendProc(param0, param1, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendEvt.
 * Descript: Send a event packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @evtID[IN]--The event ID. Range:0~4095.
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~1024.
 * Return: Negative values mean failure, other values mean success.
 *****************************************************************************/
int tlkmdi_comm_sendEvt(uint08 mType, uint16 evtID, uint08 *pData, uint08 dataLen)
{
	uint32 param;

	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param = ((uint32)(evtID & 0x0FFF)<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_EVT;
	return scpTlkMdiCommCurInfs->SendProc(param, 0, pData, dataLen);
}
int tlkmdi_comm_sendEvtNoFlow(uint08 mType, uint16 evtID, uint08 *pData, uint08 dataLen)
{
	uint32 param;

	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param = ((uint32)(evtID & 0x0FFF)<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_EVT
		| 0x80000000;
	return scpTlkMdiCommCurInfs->SendProc(param, 0, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendDat.
 * Descript: Send a data packet to the peripheral.
 * Params:
 *     @datID[IN]--The number of data port.
 *     @numb[IN]--The serial number of data. Range(20bit, 0~0xFFFFF)
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~2000.
 * Return: Negative values mean failure, other values mean success.
 *****************************************************************************/
int tlkmdi_comm_sendDat(uint08 datID, uint32 numb, uint08 *pData, uint16 dataLen)
{
	uint32 param0;
	uint32 param1;

	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	if(pData == nullptr || dataLen == 0) return -TLK_EPARAM;

	param0 = ((uint32)(datID & 0xFF)<<8)|TLKPRT_COMM_PTYPE_DAT;
	param1 = numb;
	return scpTlkMdiCommCurInfs->SendProc(param0, param1, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendDat.
 * Descript: Send a log packet to the peripheral.
 * Params:
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~2000.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1."dataLen" is not more than "scpTlkMdiCommCurInfs->Handler
 *       (TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE, 0, 0) - 16".
 *****************************************************************************/
int tlkmdi_comm_sendLogDat(uint08 *pData, uint16 dataLen)
{
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	if(tlkmdi_comm_sfifoIsMore60(dataLen)) return -TLK_EBUSY;
#else
	if(tlkmdi_comm_sfifoIsMore80(dataLen)) return -TLK_EBUSY;
#endif
	return tlkmdi_comm_sendDat(TLKPRT_COMM_SYS_DAT_PORT, TLKPRT_COMM_SYS_DAT_LOG, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendVcdDat.
 * Descript: Send a VCD packet to the peripheral.
 * Params:
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~2000.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1."dataLen" is not more than "scpTlkMdiCommCurInfs->Handler
 *       (TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE, 0, 0) - 16".
 *****************************************************************************/
int tlkmdi_comm_sendVcdDat(uint08 *pData, uint16 dataLen)
{
	if(tlkmdi_comm_sfifoIsMore80(dataLen)) return -TLK_EBUSY;
	return tlkmdi_comm_sendDat(TLKPRT_COMM_SYS_DAT_PORT, TLKPRT_COMM_SYS_DAT_VCD, pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_sendVcdDat.
 * Descript: Send a Firmware data to the peripheral.
 * Params:
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~2000.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1."dataLen" is not more than "scpTlkMdiCommCurInfs->Handler
 *       (TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE, 0, 0) - 16".
 *****************************************************************************/
int tlkmdi_comm_sendFwDat(uint08 *pData, uint16 dataLen)
{
	if(tlkmdi_comm_sfifoIsMore80(dataLen)) return -TLK_EBUSY;
	return tlkmdi_comm_sendDat(TLKPRT_COMM_SYS_DAT_PORT, TLKPRT_COMM_SYS_DAT_FW, pData, dataLen);
}


/******************************************************************************
 * Function: tlkmdi_comm_input.
 * Descript: The interface for processing received external data.
 * Params:
 *     @pData[IN]--Received external data.
 *     @dataLen[IN]--Length received.
 * Return: None.
 *****************************************************************************/
static void tlkmdi_comm_input(uint08 *pData, uint16 dataLen)
{
	if(scpTlkMdiCommCurInfs == nullptr || scpTlkMdiCommCurInfs->RecvProc == nullptr){
		return;
	}
	scpTlkMdiCommCurInfs->RecvProc(pData, dataLen);
}

/******************************************************************************
 * Function: tlkmdi_comm_recvCB.
 * Descript: This interface is used to process received packets that conform to
 *           predefined protocol formats.
 * Params:
 *     @param0[IN]--
 *     @param1[IN]--
 *     @pData[IN]--
 *     @dataLen[IN]--
 * Return: None.
 *****************************************************************************/
static int tlkmdi_comm_recvCB(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen)
{
	uint08 ptype;

	ptype = param0 & 0xFF;
	if(ptype == TLKPRT_COMM_PTYPE_CMD){
		uint08 mtype = (param0 & 0xFF00) >> 8;
		uint16 msgID = (param0 & 0xFFFF0000) >> 16;
		if(mtype >= TLKPRT_COMM_MTYPE_MAX){
			tlkapi_error(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv Error MsgType: mtype-%d", mtype);
			return TLK_ENONE;
		}

		tlkapi_trace(TLKMDI_COMM_DBG_FLAG, TLKMDI_COMM_DBG_SIGN, "Recv MsgType: mtype-%d, taskID[%d]",
			mtype, sTlkMdiCommCtrl.taskList[mtype]);
		if(mtype != 0 && mtype < TLKPRT_COMM_MTYPE_MAX && sTlkMdiCommCtrl.taskList[mtype] != 0){
#if (TLK_CFG_SYS_ENABLE)
			tlksys_sendOuterMsg(sTlkMdiCommCtrl.taskList[mtype], mtype, msgID, pData, dataLen);
#endif
		}
	}else if(ptype == TLKPRT_COMM_PTYPE_DAT){
		uint08 dataID = (param0 & 0xFFFF00) >> 8;
		uint32 number = (param1 & 0x0FFFFF);
		if(dataID <= TLKMDI_COMM_DATA_CHANNEL_MAX && sTlkMdiCommCtrl.dataList[dataID] != nullptr){
			sTlkMdiCommCtrl.dataList[dataID](dataID, number, pData, dataLen);
		}
	}
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkmdi_comm_enableAuthen.
 * Descript: Enable authentication.
 * Params:
 *     @isEnable[IN]--The callback after reset.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice: 
 *     1.After authentication is enabled, the sent messages enter the waiting
 *       state. Non-authentication messages are forwarded only after 
 *       authentication is complete.
 *     2.After authentication is enabled, the upper-layer needs to register the
 *       "TLKPRT_COMM_MTYPE_AUTHEN" message and notify the communication layer
 *       through tlkmdi_comm_setAuthenStatus() after the upper-layer completes
 *       the authentication.
 *****************************************************************************/
int tlkmdi_comm_enableAuthen(bool isEnable)
{
	if(scpTlkMdiCommCurInfs != nullptr && scpTlkMdiCommCurInfs->Handler != nullptr){
		return scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_ENABLE_AUTHEN, isEnable, 0);
	}else{
		return -TLK_ENOSUPPORT;
	}
}

/******************************************************************************
 * Function: tlkmdi_comm_setAuthenStatus.
 * Descript: Setting authentication status.
 * Params:
 *     @status[IN]--Authen's status. Refer TLKMDI_COMM_AUTH_STATUS_ENUM.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_setAuthenStatus(uint08 status)
{
	if(scpTlkMdiCommCurInfs != nullptr && scpTlkMdiCommCurInfs->Handler != nullptr){
		return scpTlkMdiCommCurInfs->Handler(TLKCPC_OPCODE_SET_AUTH_STATUE, status, 0);
	}else{
		return -TLK_ENOSUPPORT;
	}
}



#endif //#if (TLK_CFG_COMM_ENABLE)


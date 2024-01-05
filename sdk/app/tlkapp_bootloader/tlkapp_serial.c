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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapp_config.h"
#include "tlkapp.h"
#if (TLKAPP_DFU1_ENABLE)
#include "tlksys/prt/tlkpto_comm.h"
#include "tlklib/cpc/tlkcpc.h"
#include "tlkdev/tlkdev.h"
#include "tlkapp_serial.h"
#if (TLK_DEV_SERIAL_ENABLE)
#include "tlkdev/sys/tlkdev_serial.h"
#endif


extern uint32 tlkcfg_getSerialBaudrate(void);
#if (TLK_DEV_SERIAL_ENABLE)
extern void tlkdev_serial_handler(void);
#endif

static void tlkapp_serial_input(uint08 *pData, uint16 dataLen);
static int  tlkapp_serial_recvCB(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen);

extern const tlkcpc_infs_t gcTlkCpc3Infs;
#if (TLK_DEV_SERIAL_ENABLE)
__attribute__((aligned(4)))
static uint08 sAppSerialRecvBuffer[TLKAPP_SERIAL_RBUFF_NUMB*TLKAPP_SERIAL_RBUFF_SIZE];
__attribute__((aligned(4)))
static uint08 sAppSerialSendBuffer[TLKAPP_SERIAL_SBUFF_NUMB*TLKAPP_SERIAL_SBUFF_SIZE];
#endif
static const tlkcpc_infs_t *scpAppSerialCurInfs;
static TlkAppSerialRecvCmdCB sTlkAppSerialRecvCmdCB;
static TlkAppSerialRecvDatCB sTlkAppSerialRecvDatCB;
static uint08 sAppSerialRecvCache[TLKAPP_SERIAL_RECV_CACHE_SIZE];
static uint08 sAppSerialSendCache[TLKAPP_SERIAL_SEND_CACHE_SIZE];



/******************************************************************************
 * Function: tlkapp_serial_init.
 * Descript: Initial the Serial port and register event handler callback.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 * Others: None.
 *****************************************************************************/
int tlkapp_serial_init(void)
{
	sTlkAppSerialRecvCmdCB = nullptr;
	sTlkAppSerialRecvDatCB = nullptr;
	
	scpAppSerialCurInfs = &gcTlkCpc3Infs;
	scpAppSerialCurInfs->Init(0);
	scpAppSerialCurInfs->Handler(TLKCPC_OPCODE_REG_REG_RECEIVE_CB, (uint32)tlkapp_serial_recvCB, 0);
	scpAppSerialCurInfs->Handler(TLKCPC_OPCODE_SET_RECV_BUFFER, (uint32)sAppSerialRecvCache, TLKAPP_SERIAL_RECV_CACHE_SIZE);
	scpAppSerialCurInfs->Handler(TLKCPC_OPCODE_SET_SEND_BUFFER, (uint32)sAppSerialSendCache, TLKAPP_SERIAL_SEND_CACHE_SIZE);
	
#if (TLK_DEV_SERIAL_ENABLE)
	uint32 baudrate = tlkcfg_getSerialBaudrate();
	if(baudrate == 0xFFFFFFFF) baudrate = TLKAPP_SERIAL_BAUDRATE;
	tlkdev_mount(TLKAPP_SERIAL_DID);
	tlkdev_ioctl(TLKAPP_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_BAUDRATE, baudrate, 0);
	tlkdev_ioctl(TLKAPP_SERIAL_DID, TLKDEV_OPCODE_SERIAL_REG_RECV_CB, (uint32)tlkapp_serial_input, 0);
	tlkdev_ioctl(TLKAPP_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO,
		((TLKAPP_SERIAL_RBUFF_NUMB & 0xFFFF)<<16) |(TLKAPP_SERIAL_RBUFF_SIZE & 0xFFFF),
		(uint32)sAppSerialRecvBuffer);
	tlkdev_ioctl(TLKAPP_SERIAL_DID, TLKDEV_OPCODE_SERIAL_SET_TX_QFIFO,
		((TLKAPP_SERIAL_SBUFF_NUMB & 0xFFFF)<<16) | (TLKAPP_SERIAL_SBUFF_SIZE & 0xFFFF),
		(uint32)sAppSerialSendBuffer);
	scpAppSerialCurInfs->Handler(TLKCPC_OPCODE_REG_SENDCB, (uint32)tlkapp_serial_send, 0);
#endif	
	
	return TLK_ENONE;
}

void tlkapp_serial_open(void)
{
#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_open(TLKAPP_SERIAL_DID, 0);
	if(scpAppSerialCurInfs != nullptr && scpAppSerialCurInfs->Reset != nullptr){
		scpAppSerialCurInfs->Reset();
	}
#endif
}

void tlkapp_serial_close(void)
{
#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_close(TLKAPP_SERIAL_DID);
#endif
}

void tlkapp_serial_reset(void)
{
#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_ioctl(TLKAPP_SERIAL_DID, TLKDEV_OPCODE_SERIAL_RESET, 0, 0);
#endif
}

/******************************************************************************
 * Function: tlkapp_serial_handler.
 * Descript: Serial communication loop processing interface.
 * Params: None.
 * Return: None
 * Notice:
 *     1.The data in the serial communication will be cached into the fifo, 
 *       and it is necessary to call the interface repeatedly to take out the
 *       cached data.
 *****************************************************************************/
void tlkapp_serial_handler(void)
{
#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_handler();
#endif
	if(scpAppSerialCurInfs != nullptr && scpAppSerialCurInfs->Process != nullptr){
		scpAppSerialCurInfs->Process();
	}
}

void tlkapp_serial_regCmdCB(TlkAppSerialRecvCmdCB cmdCB)
{
	sTlkAppSerialRecvCmdCB = cmdCB;
}
void tlkapp_serial_regDatCB(TlkAppSerialRecvDatCB datCB)
{
	sTlkAppSerialRecvDatCB = datCB;
}

/******************************************************************************
 * Function: tlkapp_serial_send.
 * Descript: Send data to peripheral devices.
 * Params: 
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of data to be sent.
 * Return: The length to be sent. Negative values mean failure.
 * Notice:
 *     1.The data sent by this interface will not be packaged. Therefore, use 
 *       caution when invoking the data.
 *****************************************************************************/
int tlkapp_serial_send(uint08 *pData, uint16 dataLen)
{
#if (TLK_DEV_SERIAL_ENABLE)
	return tlkdev_write(TLKAPP_SERIAL_DID, 0, pData, dataLen);
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkapp_serial_sendCmd.
 * Descript: Send a command packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @cmdID[IN]--The command ID.
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent.
 * Return: Negative values mean failure, other values mean success.
 *****************************************************************************/
int tlkapp_serial_sendCmd(uint08 mType, uint08 cmdID, uint08 *pData, uint08 dataLen)
{
	uint32 param;

	if(scpAppSerialCurInfs == nullptr || scpAppSerialCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param = ((uint32)cmdID<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_CMD|0x80000000;
	return scpAppSerialCurInfs->SendProc(param, 0, pData, dataLen);
}

/******************************************************************************
 * Function: tlkapp_serial_sendRsp.
 * Descript: Send a response packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @cmdID[IN]--The command ID.
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent.
 *     @status[IN]--Command processing status. Refer to TLKPRT_COMM_RSP_STATUE_ENUM.
 *     @reason[IN]--Failure cause code. Refer to "tlk_error.h".
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1.The reason field is only valid if the status is failure.
 *****************************************************************************/
int tlkapp_serial_sendRsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen)
{
	uint32 param0;
	uint32 param1;
	
	if(scpAppSerialCurInfs == nullptr || scpAppSerialCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param0 = ((uint32)cmdID<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_RSP|0x80000000;
	param1 = ((uint16)reason<<8)|status;
	return scpAppSerialCurInfs->SendProc(param0, param1, pData, dataLen);
}

/******************************************************************************
 * Function: tlkapp_serial_sendEvt.
 * Descript: Send a event packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @evtID[IN]--The event ID.
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent.
 * Return: Negative values mean failure, other values mean success.
 *****************************************************************************/
int tlkapp_serial_sendEvt(uint08 mType, uint08 evtID, uint08 *pData, uint08 dataLen)
{
	uint32 param;

	if(scpAppSerialCurInfs == nullptr || scpAppSerialCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	param = ((uint32)evtID<<16)|((uint32)mType<<8)|TLKPRT_COMM_PTYPE_EVT|0x80000000;
	return scpAppSerialCurInfs->SendProc(param, 0, pData, dataLen);
}

/******************************************************************************
 * Function: tlkapp_serial_sendDat.
 * Descript: Send a data packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @evtID[IN]--The number of data port.
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent.
 * Return: Negative values mean failure, other values mean success.
 *****************************************************************************/
int tlkapp_serial_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen)
{
	uint32 param0;
	uint32 param1;

	if(scpAppSerialCurInfs == nullptr || scpAppSerialCurInfs->SendProc == nullptr){
		return -TLK_ENOREADY;
	}
	if(pData == nullptr || dataLen == 0 || dataLen > 256) return -TLK_EPARAM;

	param0 = ((uint32)datID<<8)|TLKPRT_COMM_PTYPE_DAT;
	param1 = numb;
	return scpAppSerialCurInfs->SendProc(param0, param1, pData, dataLen);
}


/******************************************************************************
 * Function: tlkapp_serial_input.
 * Descript: The interface for processing received external data.
 * Params:
 *     @pData[IN]--Received external data.
 *     @dataLen[IN]--Length received.
 * Return: None.
 *****************************************************************************/
static void tlkapp_serial_input(uint08 *pData, uint16 dataLen)
{
	if(scpAppSerialCurInfs == nullptr || scpAppSerialCurInfs->RecvProc == nullptr){
		return;
	}
	scpAppSerialCurInfs->RecvProc(pData, dataLen);
}

/******************************************************************************
 * Function: tlkapp_serial_recvCB.
 * Descript: This interface is used to process received packets that conform to
 *           predefined protocol formats.
 * Params:
 *     @param0[IN]--
 *     @param1[IN]--
 *     @pData[IN]--
 *     @dataLen[IN]--
 * Return: None.
 *****************************************************************************/
static int tlkapp_serial_recvCB(uint32 param0, uint32 param1, uint08 *pData, uint16 dataLen)
{
	uint08 ptype;

	ptype = param0 & 0xFF;
	if(ptype == TLKPRT_COMM_PTYPE_CMD){
		uint08 mtype = (param0 & 0xFF00) >> 8;
		uint08 msgID = (param0 & 0xFF0000) >> 16;
		if(sTlkAppSerialRecvCmdCB != nullptr){
			sTlkAppSerialRecvCmdCB(mtype, msgID, pData, dataLen);
		}
	}else if(ptype == TLKPRT_COMM_PTYPE_DAT){
		uint08 dataID = (param0 & 0xFF00) >> 8;
		uint32 number = (param1 & 0xFFFFF);
		if(sTlkAppSerialRecvDatCB != nullptr){
			sTlkAppSerialRecvDatCB(dataID, number, pData, dataLen);
		}
	}
	return TLK_ENONE;
}





#endif //#if (TLKAPP_DFU1_ENABLE)


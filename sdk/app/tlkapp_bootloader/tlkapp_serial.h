/********************************************************************************************************
 * @file	tlkapp_serial.h
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
#ifndef TLKAPP_SERIAL_H
#define TLKAPP_SERIAL_H

#if (TLKAPP_DFU1_ENABLE)


#define TLKAPP_SERIAL_DID             TLKDEV_DID_SERIAL1

#define TLKAPP_SERIAL_RBUFF_NUMB      16
#define TLKAPP_SERIAL_RBUFF_SIZE      2200//8192 //2200//1200//512
#define TLKAPP_SERIAL_SBUFF_NUMB      16
#define TLKAPP_SERIAL_SBUFF_SIZE      256

#define TLKAPP_SERIAL_RECV_DATA_SIZE      512//2048//1024//1024 //<TLKAPP_SERIAL_RBUFF_SIZE-16
#define TLKAPP_SERIAL_RECV_CACHE_SIZE     (TLKAPP_SERIAL_RECV_DATA_SIZE << 1)
#define TLKAPP_SERIAL_SEND_CACHE_SIZE     128

#define TLKAPP_SERIAL_BAUDRATE        1500000//921600//115200


typedef void(*TlkAppSerialRecvCmdCB)(uint08 mtype, uint08 cmdID, uint08 *pData, uint16 dataLen);
typedef void(*TlkAppSerialRecvDatCB)(uint08 dataID, uint32 numb, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: tlkapp_serial_init.
 * Descript: Initial the Serial port and register event handler callback.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 * Others: None.
 *****************************************************************************/
int tlkapp_serial_init(void);

void tlkapp_serial_open(void);
void tlkapp_serial_close(void);
void tlkapp_serial_reset(void);

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
void tlkapp_serial_handler(void);

void tlkapp_serial_regCmdCB(TlkAppSerialRecvCmdCB cmdCB);
void tlkapp_serial_regDatCB(TlkAppSerialRecvDatCB datCB);


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
int tlkapp_serial_send(uint08 *pData, uint16 dataLen);

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
int tlkapp_serial_sendCmd(uint08 mType, uint08 cmdID, uint08 *pData, uint08 dataLen);

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
int tlkapp_serial_sendRsp(uint08 mType, uint08 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);

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
int tlkapp_serial_sendEvt(uint08 mType, uint08 evtID, uint08 *pData, uint08 dataLen);

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
int tlkapp_serial_sendDat(uint08 datID, uint16 numb, uint08 *pData, uint16 dataLen);


#endif //#if (TLKAPP_DFU1_ENABLE)

#endif /* TLKAPP_SERIAL_H */


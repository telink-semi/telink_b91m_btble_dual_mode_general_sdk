/********************************************************************************************************
 * @file	tlkmdi_comm.h
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
#ifndef TLKMDI_COMM_H
#define TLKMDI_COMM_H

#if (TLK_CFG_COMM_ENABLE)


/******************************************************************************
 * Object: TLKMDI_COMM_SERIAL_RBUFF_SIZE
 * Brief : Serial port RxDMA cache space after receiving data.
 * Notice:
 *     1."TLKMMI_FILE_DFU_SERIAL_UNIT_LENS" is closely related to 
 *       "TLKMDI_COMM_SERIAL_RBUFF_SIZE". Generally, 
 *       "TLKMDI_COMM_SERIAL_RBUFF_SIZE" is at least twice that of 
 *       "TLKMMI_FILE_DFU_SERIAL_UNIT_LENS" to maximize the performance of a 
 *       serial port upgrade. Note this option if serial port upgrade is enabled.
 *****************************************************************************/
#define TLKMDI_COMM_SERIAL_DID             TLKDEV_DID_SERIAL1
#define TLKMDI_COMM_SERIAL_BAUDRATE        1500000//921600//115200
#define TLKMDI_COMM_SERIAL_RBUFF_NUMB      8
#define TLKMDI_COMM_SERIAL_RBUFF_SIZE      540//280 //The configuration should be larger than the maximum frame of data in the system
#define TLKMDI_COMM_SERIAL_SBUFF_NUMB      8
#define TLKMDI_COMM_SERIAL_SBUFF_SIZE      128

/******************************************************************************
 * Object: TLKMDI_COMM_DATA_CHANNEL_MAX
 * Brief : Configure the number of data transmission channels allowed for 
 *         serial communication.
 *****************************************************************************/
#define TLKMDI_COMM_DATA_CHANNEL_MAX       4

/******************************************************************************
 * Object: TLKMDI_COMM_RECV_FRAME_MAXLEN, TLKMDI_COMM_SEND_FRAME_MAXLEN
 * Brief : Configure the size of the received and sent packet space for serial
 *         communication.
 * Notice:
 *     1.This option needs to be larger than the maximum frame size that the 
 *       user actually transfers.
 *****************************************************************************/
#define TLKMDI_COMM_RECV_FRAME_MAXLEN      256 //The configuration should be greater than or equal to the length of the largest frame of data in the system
#define TLKMDI_COMM_SEND_FRAME_MAXLEN      128
#define TLKMDI_COMM_RECV_CACHE_SIZE        (16+TLKMDI_COMM_RECV_FRAME_MAXLEN+32)
#define TLKMDI_COMM_SEND_CACHE_SIZE        (16+TLKMDI_COMM_SEND_FRAME_MAXLEN+16)




typedef int(*TlkMdiCommResetCB)(void);
typedef void(*TlkMdiCommDatCB)(uint08 datID, uint32 number, uint08 *pData, uint16 dataLen); //number:Range[0,0xFFFFF]


/******************************************************************************
 * Object: TLKMDI_COMM_PROTOCOL_CPC1
 * Notice:
 *     1.Frame Head Sign: 7F 7F
 *     2.Frame Tail Sign: 80 80
 *     3.Escap: No
 *     4.Flow Ctrl: Yes (CMD/RSP/EVT)
 ******************************************************************************
 * Object: TLKMDI_COMM_PROTOCOL_CPC2
 * Notice:
 *     1.Frame Head Sign: AC
 *     2.Frame Tail Sign: CA
 *     3.Escap: Yes (<DC>-><DC 01>; <AC>-><DC 02>; <CA>-><DC 03>)
 *     4.Flow Ctrl: Yes (CMD/RSP/EVT)
 ******************************************************************************
 * Object: TLKMDI_COMM_PROTOCOL_CPC3
 * Notice:
 *     1.Frame Head Sign: AC
 *     2.Frame Tail Sign: CA
 *     3.Escap: Yes (<DC>-><DC 01>; <AC>-><DC 02>; <CA>-><DC 03>)
 *     4.Flow Ctrl: No
 *****************************************************************************/
typedef enum{
	TLKMDI_COMM_PROTOCOL_NONE = 0,
	TLKMDI_COMM_PROTOCOL_CPC1,
	TLKMDI_COMM_PROTOCOL_CPC2,
	TLKMDI_COMM_PROTOCOL_CPC3,
}TLKMDI_COMM_PROTOCOL_ENUM;

/******************************************************************************
 * Object: TLKMDI_COMM_AUTH_STATUS_ENUM
 * Notice:
 *     1.TLKMDI_COMM_AUTH_STATUS_ENUM is bound to TLKCPC_AUTH_STATUS_ENUM and
 *       needs to be modified accordingly
 *****************************************************************************/
typedef enum{
    TLKMDI_COMM_AUTH_STATUS_NONE    = 0,
    TLKMDI_COMM_AUTH_STATUS_SUCCESS = 1,
    TLKMDI_COMM_AUTH_STATUS_FAILURE = 2,
    TLKMDI_COMM_AUTH_STATUS_RUNNING = 3,
}TLKMDI_COMM_AUTH_STATUS_ENUM;


/******************************************************************************
 * Function: tlkmdi_comm_init.
 * Descript: Initial the Serial port and register event handler callback.
 * Params: 
 *     @procID--Process ID. Refer to TLKSYS_PROCID_ENUM.
 *     @type--Protocol type. Refer TLKMDI_COMM_PROTOCOL_ENUM.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_init(uint08 procID, TLKMDI_COMM_PROTOCOL_ENUM type);

/******************************************************************************
 * Function: tlkmdi_comm_start.
 * Descript: Enable serial port communication.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_start(void);

/******************************************************************************
 * Function: tlkmdi_comm_reset.
 * Descript: Reset serial port communication.
 * Params: None.
 * Return: None
 *****************************************************************************/
void tlkmdi_comm_reset(void);

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
void tlkmdi_comm_handler(void);

/******************************************************************************
 * Function: tlkmdi_comm_getBufferSize.
 * Descript: Gets the size of the protocol's send buffer.
 * Params: None.
 * Return: The size of the protocol's send buffer.
 * Notice:
 *     1."tlkmdi_comm_getDataBufferSize()" is usually to get the buffer's size 
 *       of the CMD/RSP/EVT package.
 *****************************************************************************/
int tlkmdi_comm_getBufferSize(void);

/******************************************************************************
 * Function: tlkmdi_comm_getDataBufferSize.
 * Descript: Gets the size of the protocol's send buffer.
 * Params: None.
 * Return: The size of the protocol's send buffer.
 * Notice:
 *     1."tlkmdi_comm_getDataBufferSize()" is usually to get the buffer's size   
 *       of the DAT package.
 *****************************************************************************/
int tlkmdi_comm_getDataBufferSize(void);

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
int tlkmdi_comm_setSendFifo(uint08 *pBuffer, uint16 itemNumb, uint16 itemSize);

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
int tlkmdi_comm_setRecvFifo(uint08 *pBuffer, uint16 itemNumb, uint16 itemSize);

/******************************************************************************
 * Function: tlkmdi_comm_getBaudrate.
 * Descript: Obtain the baud rate of the current serial port communication.
 * Params: None.
 * Return: The baud rate for the current serial. Negative values mean failure.
 *****************************************************************************/
int tlkmdi_comm_getBaudrate(void);

/******************************************************************************
 * Function: tlkmdi_comm_setBaudrate.
 * Descript: Set the baud rate for the current serial port communication.
 * Params:
 *     @baudrate[IN]--The baud rate for the current serial.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_setBaudrate(uint32 baudrate);

/******************************************************************************
 * Function: tlkmdi_comm_sfifoIsMore60.
 * Descript: Detects whether the transmission fifo usage exceeds 60%.
 * Params:
 *     @dataLen[IN]--Length to be used by the user.
 * Return: TRUE indicates that the current fifo usage exceeds 60%.
 *****************************************************************************/
bool tlkmdi_comm_sfifoIsMore60(uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sfifoIsMore80.
 * Descript: Detects whether the transmission fifo usage exceeds 80%.
 * Params:
 *     @dataLen[IN]--Length to be used by the user.
 * Return: TRUE indicates that the current fifo usage exceeds 80%.
 *****************************************************************************/
bool tlkmdi_comm_sfifoIsMore80(uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_getValidDatID.
 * Descript: From the device number table in the system, look for an available
 *           device number.
 * Params:
 *     @pDatID[OUT]--Available device number.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_getValidDatID(uint08 *pDatID);

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
int tlkmdi_comm_getTaskID(uint08 mtype, uint16 *pTaskID);

/******************************************************************************
 * Function: tlkmdi_comm_regCmdCB.
 * Descript: Registers a task for the message type in the system to distribute
 *     the received message to the specified task.
 * Params:
 *     @mtype[IN]--The Type of message.
 *     @taskID[IN]--The ID of the task.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_regCmdCB(uint08 mtype, uint16 taskID);

/******************************************************************************
 * Function: tlkmdi_comm_regDatCB.
 * Descript: Register the data callback.
 * Params:
 *     @datID[IN]--The data channel Id.
 *     @datCB[IN]--The data callback.
 *     @isForce[IN]--is force to process.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_regDatCB(uint08 datID, TlkMdiCommDatCB datCB, bool isForce);

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
int tlkmdi_comm_send(uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendCmd, tlkmdi_comm_sendCmdNoFlow.
 * Descript: Send a command packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @cmdID[IN]--The command ID. Range:0~4095
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~1024.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1.If the selected protocol supports flow control, "tlkmdi_comm_sendCmd"
 *       sends messages with flow control, while "tlkmdi_comm_sendCmdNoFlow"
 *       does not send messages with flow control.
 *****************************************************************************/
int tlkmdi_comm_sendCmd(uint08 mType, uint16 cmdID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendCmdNoFlow(uint08 mType, uint16 cmdID, uint08 *pData, uint08 dataLen);

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
 *     2.If the selected protocol supports flow control, "tlkmdi_comm_sendRsp"
 *       sends messages with flow control, while "tlkmdi_comm_sendRspNoFlow"
 *       does not send messages with flow control.
 *****************************************************************************/
int tlkmdi_comm_sendRsp(uint08 mType, uint16 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendRspNoFlow(uint08 mType, uint16 cmdID, uint08 status, uint08 reason, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendEvt.
 * Descript: Send a event packet to the peripheral.
 * Params:
 *     @mType[IN]--The message type.
 *     @evtID[IN]--The event ID. Range:0~1024.
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~1024.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1.If the selected protocol supports flow control, "tlkmdi_comm_sendEvt"
 *       sends messages with flow control, while "tlkmdi_comm_sendEvtNoFlow"
 *       does not send messages with flow control.
 *****************************************************************************/
int tlkmdi_comm_sendEvt(uint08 mType, uint16 evtID, uint08 *pData, uint08 dataLen);
int tlkmdi_comm_sendEvtNoFlow(uint08 mType, uint16 evtID, uint08 *pData, uint08 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendDat.
 * Descript: Send a data packet to the peripheral.
 * Params:
 *     @datID[IN]--The number of data port.
 *     @numb[IN]--The serial number of data. Range(20bit, 0~0xFFFFF)
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~2000.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1."tlkmdi_comm_sendDat()" sends messages without flow control.
 *****************************************************************************/
int tlkmdi_comm_sendDat(uint08 datID, uint32 numb, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendDat.
 * Descript: Send a log data to the peripheral.
 * Params:
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~2000.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1."dataLen" is not more than "scpTlkMdiCommCurInfs->Handler
 *       (TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE, 0, 0) - 16".
 *****************************************************************************/
int tlkmdi_comm_sendLogDat(uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_sendVcdDat.
 * Descript: Send a VCD data to the peripheral.
 * Params:
 *     @pData[IN]--The data to be sent.
 *     @dataLen[IN]--The length of the data to be sent. Range:0~2000.
 * Return: Negative values mean failure, other values mean success.
 * Notice:
 *     1."dataLen" is not more than "scpTlkMdiCommCurInfs->Handler
 *       (TLKCPC_OPCODE_GET_DAT_BUFFER_SIZE, 0, 0) - 16".
 *****************************************************************************/
int tlkmdi_comm_sendVcdDat(uint08 *pData, uint16 dataLen);

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
int tlkmdi_comm_sendFwDat(uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_comm_regResetRequestCB.
 * Descript: Registration protocol reset callback.
 * Params:
 *     @resetCB[IN]--The callback after reset.
 * Return: TLK_ENONE is success,other value is failure.
 * Notice: 
 *     1.The interface only be available under TLKMDI_COMM_PROTOCOL_CPC2 enable.
 *****************************************************************************/
int tlkmdi_comm_regResetRequestCB(TlkMdiCommResetCB resetCB);

/******************************************************************************
 * Function: tlkmdi_comm_regResetCompleteCB.
 * Descript: Registration protocol reset callback.
 * Params:
 *     @resetCB[IN]--The callback after reset.
 * Return: TLK_ENONE is success,other value is failure.
 * Notice: 
 *     1.The interface only be available under TLKMDI_COMM_PROTOCOL_CPC2 enable.
 *****************************************************************************/
int tlkmdi_comm_regResetCompleteCB(TlkMdiCommResetCB resetCB);

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
int tlkmdi_comm_enableAuthen(bool isEnable);

/******************************************************************************
 * Function: tlkmdi_comm_setAuthenStatus.
 * Descript: Setting authentication status.
 * Params:
 *     @status[IN]--Authen's status. Refer TLKMDI_COMM_AUTH_STATUS_ENUM.
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkmdi_comm_setAuthenStatus(uint08 status);


#endif //#if (TLK_CFG_COMM_ENABLE)

#endif //TLKMDI_COMM_H


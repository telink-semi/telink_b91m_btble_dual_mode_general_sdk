/********************************************************************************************************
 * @file	tlkdev.h
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
#ifndef TLKDEV_H
#define TLKDEV_H


#if (TLK_CFG_DEV_ENABLE)

#define TLKDEV_DBG_FLAG         ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_DBG_SIGN         "[CDEV]"

#define TLKDEV_SYS_DBG_FLAG     ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV_SYS << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_SYS_DBG_SIGN     "[SDEV]"

#define TLKDEV_EXT_DBG_FLAG     ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_EXT_DBG_SIGN     "[EDEV]"

#define TLKDEV_USR_DBG_FLAG     ((TLK_MAJOR_DBGID_DEV << 24) | (TLK_MINOR_DBGID_DEV_USR << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDEV_USR_DBG_SIGN     "[UDEV]"


typedef enum{
	TLKDEV_DID_NONE = 0x0000,
	TLKDEV_DID_KEY1 = 0x0001,
	TLKDEV_DID_KEY2 = 0x0002,
	TLKDEV_DID_LED1 = 0x0010,
	TLKDEV_DID_LED2 = 0x0011,
	TLKDEV_DID_LED3 = 0x0012,
	TLKDEV_DID_LED4 = 0x0013,
	TLKDEV_DID_BATTERY  = 0x0020,
	TLKDEV_DID_SDCARD0  = 0x0025,
	TLKDEV_DID_SERIAL0  = 0x0030,
	TLKDEV_DID_SERIAL1  = 0x0031,
	TLKDEV_DID_KEYBOARD = 0x0040,
	TLKDEV_DID_CODEC    = 0x0050,
	TLKDEV_DID_TOUCH    = 0x0051,
	TLKDEV_DID_USB      = 0x0052,
}TLKDEV_DID_ENUM;
typedef enum{
	TLKDEV_OPCODE_NONE = 0x0000,
	//Universal opcode
	TLKDEV_OPCODE_WAKE_TIME   = 0xF001,
	TLKDEV_OPCODE_ENTER_SLEEP = 0xF002,
	TLKDEV_OPCODE_LEAVE_SLEEP = 0xF003,
	//Proprietary opcode
	//TLKDEV_DID_SERIAL0 and TLKDEV_DID_SERIAL1
	TLKDEV_OPCODE_SERIAL_RESET         = 0x0001,
	TLKDEV_OPCODE_SERIAL_SET_BAUDRATE  = 0x0010,
	TLKDEV_OPCODE_SERIAL_GET_BAUDRATE  = 0x0011,
	TLKDEV_OPCODE_SERIAL_SET_FLOW_CTL  = 0x0015,
	TLKDEV_OPCODE_SERIAL_REG_RECV_CB   = 0x0016,
	TLKDEV_OPCODE_SERIAL_SET_RX_FIFO   = 0x0020, //For NoDMA mode
	TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO  = 0x0021, //For DMA mode
	TLKDEV_OPCODE_SERIAL_SET_TX_QFIFO  = 0x0022, //For DMA mode
	TLKDEV_OPCODE_SERIAL_IS_MORE_60PER = 0x0030,
	TLKDEV_OPCODE_SERIAL_IS_MORE_80PER = 0x0031,
	TLKDEV_OPCODE_SERIAL_DISABLE_RX    = 0x0040,
	TLKDEV_OPCODE_SERIAL_DISABLE_TX    = 0x0041,
	TLKDEV_OPCODE_SERIAL_SET_TX_PIN    = 0x0050,
	TLKDEV_OPCODE_SERIAL_SET_RX_PIN    = 0x0051,
	TLKDEV_OPCODE_SERIAL_SET_TX_DMA    = 0x0052,
	TLKDEV_OPCODE_SERIAL_SET_RX_DMA    = 0x0053,
	TLKDEV_OPCODE_SERIAL_SET_CTS_PIN   = 0x0054,
	TLKDEV_OPCODE_SERIAL_SET_RTS_PIN   = 0x0055,
	//TLKDEV_DID_STORE
	TLKDEV_OPCODE_STORE_ERASE           = 0x0001,
	TLKDEV_OPCODE_STORE_FORMAT          = 0x0002,
	TLKDEV_OPCODE_STORE_GET_PAGE_SIZE   = 0x0010,
	TLKDEV_OPCODE_STORE_GET_PAGE_NUMB   = 0x0011,
	TLKDEV_OPCODE_STORE_GET_SECTOR_SIZE = 0x0012,
	TLKDEV_OPCODE_STORE_GET_SECTOR_NUMB = 0x0013,
	TLKDEV_OPCODE_STORE_GET_BLOCK_SIZE  = 0x0014,
	TLKDEV_OPCODE_STORE_GET_BLOCK_NUMB  = 0x0015,
	//TLKDEV_DID_CODEC
	TLKDEV_OPCODE_CODEC_SET_SPK_MUTE    = 0x0001,
	TLKDEV_OPCODE_CODEC_SET_MIC_MUTE    = 0x0002,
	TLKDEV_OPCODE_CODEC_GET_SPK_VOLUME  = 0x0003,
	TLKDEV_OPCODE_CODEC_SET_SPK_VOLUME  = 0x0004,
	TLKDEV_OPCODE_CODEC_GET_MIC_VOLUME  = 0x0005,
	TLKDEV_OPCODE_CODEC_SET_MIC_VOLUME  = 0x0006,	
	TLKDEV_OPCODE_CODEC_GET_SPK_OFFSET  = 0x0010,
	TLKDEV_OPCODE_CODEC_SET_SPK_OFFSET  = 0x0011,
	TLKDEV_OPCODE_CODEC_GET_MIC_OFFSET  = 0x0012,
	TLKDEV_OPCODE_CODEC_SET_MIC_OFFSET  = 0x0013,
	TLKDEV_OPCODE_CODEC_GET_MIC_BUFFLEN = 0x0020,
	TLKDEV_OPCODE_CODEC_SET_MIC_BUFFLEN = 0x0021,
	TLKDEV_OPCODE_CODEC_GET_SPK_BUFFLEN = 0x0022,
	TLKDEV_OPCODE_CODEC_SET_SPK_BUFFLEN = 0x0023,
	TLKDEV_OPCODE_CODEC_GET_MIC_DATALEN = 0x0024,
	TLKDEV_OPCODE_CODEC_GET_SPK_DATALEN = 0x0025,
	TLKDEV_OPCODE_CODEC_GET_SPK_IDLELEN = 0x0026,
	TLKDEV_OPCODE_CODEC_MUTE_SPK_BUFF   = 0x0030,
	TLKDEV_OPCODE_CODEC_ZERO_SPK_BUFF   = 0x0031,
	TLKDEV_OPCODE_CODEC_GET_CHANNEL     = 0x0050, //param0:[uint08]chnCnt-1/2/3, 0x01-left channel, 0x02-right channel, 0x03-stereo
	TLKDEV_OPCODE_CODEC_SET_CHANNEL     = 0x0051, //param0:[uint08]chnCnt-1/2/3, 0x01-left channel, 0x02-right channel, 0x03-stereo
	TLKDEV_OPCODE_CODEC_GET_BIT_DEPTH   = 0x0052, //param0:[uint08]bitDepth-16/20/24
	TLKDEV_OPCODE_CODEC_SET_BIT_DEPTH   = 0x0053, //param0:[uint08]bitDepth-16/20/24
	TLKDEV_OPCODE_CODEC_GET_SAMPLE_RATE = 0x0054,
	TLKDEV_OPCODE_CODEC_SET_SAMPLE_RATE = 0x0055, //param0:[uint32]sampleRate-8k/16k/32k/44.1k/48k
	TLKDEV_OPCODE_CODEC_OPEN_MIC_BIAS   = 0x0060,
	TLKDEV_OPCODE_CODEC_CLOSE_MIC_BIAS  = 0x0061,
	//TLKDEV_DID_USB
	TLKDEV_OPCODE_USB_CHANGE_MODE = 0x0001,
}TLKDEV_OPCODE_ENUM;


/******************************************************************************
 * Function: tlkdev_init
 * Descript: Initialize all devices used in the system.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice:
 *     1.The device that performs this operation first backs up its own device 
 *       in the system device copy.
 *     2.Only devices in the system device copy can be mounted by calling 
 *       "tlkdev_mount()".
 *****************************************************************************/
int tlkdev_init(void);

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Deinitialize all devices used in the system.
 * Params: None.
 * Return: None.
 *****************************************************************************/
void tlkdev_deinit(void);

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Mount the device to the device list.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_mount(uint16 did);

/******************************************************************************
 * Function: tlkdev_unmount
 * Descript: Unmount the device from the device list.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_unmount(uint16 did);

/******************************************************************************
 * Function: tlkdev_open
 * Descript: Open the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_open(uint16 did, uint32 param);

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Close the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_close(uint16 did);

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Get the state of the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: The state of the mounted device. Refer TLKDEV_STATE_ENUM.
 *****************************************************************************/
int tlkdev_state(uint16 did);

/******************************************************************************
 * Function: tlkdev_read
 * Descript: Read the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 *     @param[IN]--Parameters used to control read operations. 
 *     @pBuff[OUT]--The storage area where data is read.
 *     @buffLen[IN]--The length of the buffer.
 * Return: The length of the read, and a negative value means failure.
 *****************************************************************************/
int tlkdev_read(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);

/******************************************************************************
 * Function: tlkdev_write
 * Descript: Write the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 *     @param[IN]--Parameters used to control write operations.
 *     @pData[IN]--Data to be written. 
 *     @dataLen[IN]--The length of the data.
 * Return: Write length. Negative values mean failure, others mean success.
 *****************************************************************************/
int tlkdev_write(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);

/******************************************************************************
 * Function: tlkdev_ioctl
 * Descript: Perform input and output operations on mounted devices.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 *     @opcode[IN]--IO operation code. Refer TLKDEV_OPCODE_ENUM.
 *     @param0[IN/OUT]--The first input or output parameter.
 *     @param1[IN/OUT]--The second input or output parameter.
 * Return: Negative values mean failure, others mean success.
 *****************************************************************************/
int tlkdev_ioctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);


#endif //#if (TLK_CFG_DEV_ENABLE)

#endif //TLKDEV_H


/********************************************************************************************************
 * @file	tlkapi_fifo.h
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
#ifndef TLKAPI_FIFO_H
#define TLKAPI_FIFO_H


/******************************************************************************
 * Note: fifo is a control algorithm for read and write access to a section of
 *       memory. Unlike qfifo, the smallest unit of access for fifo is byte.
 *****************************************************************************/

/******************************************************************************
 * Object: TLKAPI_FIFO_MIN_SIZE
 * Brief : Defines the minimum size of the space for the fifo accessing.
 *****************************************************************************/
#define TLKAPI_FIFO_MIN_SIZE      64

#define TLKAPI_FIFO_SET_COVER(pFifo, cover)   ((pFifo)->isCover = (cover))
#define TLKAPI_FIFO_SET_PARTY(pFifo, party)   ((pFifo)->isParty = (party))

/******************************************************************************
 * Object: tlkapi_fifo_t
 * Brief : The info of the control block of the fifo.
 * Params:
 *     @isCover--Whether overwrite is allowed.
 *     @isParty--Whether partial writing is allowed.
 *     @more60L--The 60 percent threshold of fifo usage exceeds.
 *     @more85L--The 85 percent threshold of fifo usage exceeds.
 *     @woffset--The offset of the item to which the user wrote.
 *     @roffset--The offset of the item to which the user read.
 *     @buffLen--The length of the "pBuffer".
 *     @pBuffer--The buffer for caching user data.
 * Notice:
 *     1.When "isCover" is set to 1, it means that the user's new data can 
 *       overwrite the old data if the fifo will be full after wrote.
 *     2.When "isParty" is set to 1, it means that the user is allowed to write
 *       some data if the fifo will be full after wrote.
 *     3.When isCover is set to 1, "isParty" will automatically becomes invalid.
 *     4."isCover" and "isParty" will be applied to "tlkapi_fifo_write()".
 *****************************************************************************/
typedef struct{
	uint08 isCover;
	uint08 isParty;
	uint16 more60L;
	uint16 more85L;
	uint16 woffset;
	uint16 roffset;
	uint16 buffLen;
	uint08 *pBuffer;
}tlkapi_fifo_t;


/******************************************************************************
 * Function: tlkapi_fifo_init
 * Descript: Initialize the infomation of the fifo.
 * Params:
 *     @pFifo[IN/OUT]--The control block of the fifo.
 *     @isCover[IN]--Whether overwrite is allowed.
 *     @isParty--Whether partial writing is allowed.
 *     @buffLen[IN]--The length of the "pBuffer".
 *     @pBuffer[IN]--The buffer for caching user data.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice: 
 *     1.When "isCover" is set to 1, it means that the user's new data can 
 *       overwrite the old data if the fifo will be full after wrote.
 *     2.When "isParty" is set to 1, it means that the user is allowed to write
 *       some data if the fifo will be full after wrote.
 *     3.When isCover is set to 1, "isParty" will automatically becomes invalid.
 *     4."isCover" and "isParty" will be applied to "tlkapi_fifo_write()".
 *****************************************************************************/
int tlkapi_fifo_init(tlkapi_fifo_t *pFifo, uint08 isCover, uint08 isPartly,
	uint08 *pBuffer, uint16 buffLen);

/******************************************************************************
 * Function: tlkapi_fifo_deinit
 * Descript: Uninitialize the infomation of the fifo.
 * Params:
 *     @fifo[IN]--The control block of the fifo.
 * Return: None.
 *****************************************************************************/
void tlkapi_fifo_deinit(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_reset
 * Descript: Reset the fifo control block.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 * Return: None.
 * Notice: 
 *     1.After the reset, all parameters of fifo will be forcibly cleared to 
 *       zero. Please call with caution.
 *     2.If you just want to clear the data stored internally, you can call 
 *       "tlkapi_fifo_clear()".
 *****************************************************************************/
void tlkapi_fifo_reset(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_clear
 * Descript: Clear the data saved in fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 * Return: None.
 * Notice: 
 *     1.After calling this interface, "pFifo->woffset"/"pFifo->roffset" will 
 *       be cleared to zero.
 *****************************************************************************/
void tlkapi_fifo_clear(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_isEmpty
 * Descript: Detects whether the data in the fifo is null.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 * Return: True means no data in fifo, other means have data in fifo.
 *****************************************************************************/
bool tlkapi_fifo_isEmpty(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_idleLen
 * Descript: Gets the length unused in the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 * Return: The length unused in the fifo.
 *****************************************************************************/
uint16 tlkapi_fifo_idleLen(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_dataLen
 * Descript: Gets the length used in the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 * Return: The length used in the fifo.
 *****************************************************************************/
uint16 tlkapi_fifo_dataLen(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_buffLen
 * Descript: Get The buffer length which specify by the FIFO.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 * Return: The length of the "pFifo->pBuffer".
 *****************************************************************************/
uint16 tlkapi_fifo_buffLen(tlkapi_fifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_fifo_isMore85
 * Descript: Detect whether the current fifo usage exceeds 60 percent.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @dataLen[IN]--The length of data will be insert.
 * Return: True means the fifo usage exceeds 60 percent.
 *****************************************************************************/
bool tlkapi_fifo_isMore60(tlkapi_fifo_t *pFifo, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_fifo_isMore85
 * Descript: Detect whether the current fifo usage exceeds 85 percent.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @dataLen[IN]--The length of data will be insert.
 * Return: True means the fifo usage exceeds 85 percent.
 *****************************************************************************/
bool tlkapi_fifo_isMore85(tlkapi_fifo_t *pFifo, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_fifo_read
 * Descript: Read data from the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @pBuff[OUT]--The buffer for caching data to be read.
 *     @readLen[IN]--The maximum length of the data to be read.
 * Return: The length of the data to be read. A negative value means the read
 *     failed.
 * Notice:
 *     1.This interface is similar to "tlkapi_fifo_readCommon(chgOffset=true)".
 *****************************************************************************/
int tlkapi_fifo_read(tlkapi_fifo_t *pFifo, uint08 *pBuff, uint16 readLen);

/******************************************************************************
 * Function: tlkapi_fifo_write
 * Descript: Write data into the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @pData[IN]--The data to be wrote.
 *     @dataLen[IN]--The length of the data to be wrote.
 * Return: The length of the data to be wrote. A negative value means the write
 *     failed.
 * Notice:
 *     1.If "dataLen" exceeds "pFifo->buffLen", the write fails.
 *     2.If the remaining space in the fifo is less than dataLen and "isCover"
 *       is set to 1, the new data will overwrite the part of the FIFO that was
 *       written first.
 *     3.If the remaining fifo space is less than dataLen and "isParty" is set
 *       to 1, data with the remaining space size is written and the remaining
 *       data is discarded
 *     4.This interface is similar to "tlkapi_fifo_writeCommon(chgOffset=true)".
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
int tlkapi_fifo_write(tlkapi_fifo_t *pFifo, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_fifo_readByte
 * Descript: Read one byte of data from the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @pByte[OUT]--A buffer that saves read data.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkapi_fifo_readByte(tlkapi_fifo_t *pFifo, uint08 *pByte);

/******************************************************************************
 * Function: tlkapi_fifo_readByte
 * Descript: Write one byte of data into the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @obyte[IN]--The data to be wrote.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
int tlkapi_fifo_writeByte(tlkapi_fifo_t *pFifo, uint08 obyte);

/******************************************************************************
 * Function: tlkapi_fifo_readCommon
 * Descript: Read data from the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @pBuff[OUT]--The buffer for caching data to be read.
 *     @readLen[IN]--The maximum length of the data to be read.
 *     @chgOffset[IN]--Whether to change the value of offset, if false, the
 *         read data will still be stored in the fifo.
 * Return: The length of the data to be read. A negative value means the read
 *     failed.
 * Notice: 
 *     1.If you want to remove this data from the fifo after calling the 
 *       interface successfully, you can call "tlkapi_fifo_chgReadPos()" with 
 *       the length of the read data.
 *****************************************************************************/
int tlkapi_fifo_readCommon(tlkapi_fifo_t *pFifo, uint08 *pBuff, 
	uint16 readLen, bool chgOffset);

/******************************************************************************
 * Function: tlkapi_fifo_write
 * Descript: Write data into FIFO.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @pData[IN]--The data to be wrote.
 *     @dataLen[IN]--The length of the data to be wrote.
 *     @chgOffset[IN]--Whether to change the value of offset, if false, the
 *         wrote data will not be stored in the fifo.
 * Return: The length of the data to be wrote. A negative value means the write
 *     failed.
 * Notice: 
 *     1.The interface is placed in I-RAM and can be accessed by high-priority
 *       interrupts.
 *     2.If you want to save this data from the fifo after calling the 
 *       interface successfully, you can call "tlkapi_fifo_chgWritePos()" with 
 *       the length of the wrote data.
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
int tlkapi_fifo_writeCommon(tlkapi_fifo_t *pFifo, uint08 *pData, 
	uint16 dataLen, bool chgOffset);

/******************************************************************************
 * Function: tlkapi_fifo_readByte
 * Descript: Adjust the relative position of the read pointer in the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @readLen[IN]--The length to be offset.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1."readLen" should not exceed "pFifo->buffLen".
 *****************************************************************************/
int tlkapi_fifo_chgReadPos(tlkapi_fifo_t *pFifo, uint16 readLen);

/******************************************************************************
 * Function: tlkapi_fifo_readByte
 * Descript: Adjust the relative position of the write pointer in the fifo.
 * Params:
 *     @pFifo[IN]--The control block of the fifo.
 *     @writeLen[IN]--The length to be offset.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1."writeLen" should not exceed "pFifo->buffLen".
 *****************************************************************************/
int tlkapi_fifo_chgWritePos(tlkapi_fifo_t *pFifo, uint16 writeLen);



#endif //TLKAPI_FIFO_H


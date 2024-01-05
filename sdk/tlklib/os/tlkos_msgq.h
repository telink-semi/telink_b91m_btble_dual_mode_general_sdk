/********************************************************************************************************
 * @file	tlkos_msgq.h
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
#ifndef TLKOS_MSGQ_H
#define TLKOS_MSGQ_H

#if (TLK_CFG_OS_ENABLE)



typedef struct{
#if (TLK_OS_FREERTOS_ENABLE)
	uint size;
	ulong handle;
#else
	ulong pool;
	ulong msgq;
#endif
}tlkos_msgq_t;


/******************************************************************************
 * Function: tlkos_msgq_create
 * Descript: Create a message queue.
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 *     @numb[IN]--Number of items in the queue.
 *     @size[IN]--The size of a single item.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
int tlkos_msgq_create(tlkos_msgq_t *pMsgq, uint numb, uint size);

/******************************************************************************
 * Function: tlkos_msgq_destroy
 * Descript: Destroy message queue.
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_msgq_destroy(tlkos_msgq_t *pMsgq);

/******************************************************************************
 * Function: tlkos_msgq_send
 * Descript: Sends a chunk of data to a message queue.
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--The Length of data to be sent.
 *     @timeout[IN]--The maximum amount of time the task should block
 *      waiting for space to become available on the queue, should it already
 *      be full. The call will return immediately if this is set to 0 and the
 *      queue is full. Unit:us.
 * Return: Returns the length of the sent data. A negative value means failure.
 * Others: None.
*******************************************************************************/
int tlkos_msgq_send(tlkos_msgq_t *pMsgq, uint08 *pData, uint16 dataLen, uint32 timeout);
int tlkos_msgq_extSend(tlkos_msgq_t *pMsgq, uint08 *pData0, uint16 data0Len, 
	uint08 *pData1, uint16 data1Len, uint08 *pData2, uint16 data2Len, uint32 timeout);
int tlkos_msgq_sendFromIrq(tlkos_msgq_t *pMsgq, uint08 *pData, uint16 dataLen);
int tlkos_msgq_extSendFromIrq(tlkos_msgq_t *pMsgq, uint08 *pData0, uint16 data0Len, 
	uint08 *pData1, uint16 data1Len, uint08 *pData2, uint16 data2Len);

/******************************************************************************
 * Function: tlkos_msgq_recv
 * Descript: Read data from the message queue.
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 *     @pBuff[OUT]--Data to be sent.
 *     @buffLen[IN]--The Length of data to be sent.
 *     @timeout[IN]--The maximum amount of time the task should block
 *      waiting for space to become available on the queue, should it already
 *      be full. The call will return immediately if this is set to 0 and the
 *      queue is full. Unit:us.
 * Return: Returns the length of the read data. A negative value means failure.
 * Others: When the item is read, it will be discarded regardless of whether
 *      the data has been read.
*******************************************************************************/
int tlkos_msgq_wait(tlkos_msgq_t *pMsgq, uint08 *pBuff, uint16 buffLen, uint32 timeout);

/******************************************************************************
 * Function: tlkos_msgq_clear
 * Descript: Reset a queue back to its original empty state. 
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 * Return: None.
*******************************************************************************/
void tlkos_msgq_clear(tlkos_msgq_t *pMsgq);



#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_MSGQ_H


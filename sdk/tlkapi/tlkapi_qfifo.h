/********************************************************************************************************
 * @file	tlkapi_qfifo.h
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
#ifndef TLKAPI_QFIFO_H
#define TLKAPI_QFIFO_H


/******************************************************************************
 * Note: qfifo(queue fifo) is an algorithm used to store the information of 
 *       multiple entries, and the data length of each entry does not exceed a
 *       certain value. qfifo uses a FIFO mechanism to control access to items.
 *       The smallest unit of access for fifo is item in qfifo.
 *****************************************************************************/

/******************************************************************************
 * Object: TLKAPI_QFIFO_NEED_SIZE
 * Brief : Calculate the size of the space required for qfifo.
 * Params:
 *     @numb--The number of the items in queue fifo.
 *     @size--The length of each item in queue fifo.
 *****************************************************************************/
#define TLKAPI_QFIFO_NEED_SIZE(numb, size)     ((size)*(numb))

#define tlkapi_qfifo_size(pFifo)               ((pFifo)->size)
#define tlkapi_qfifo_numb(pFifo)               ((pFifo)->numb)
#define tlkapi_qfifo_isFull(pFifo)             ((pFifo)->full || (pFifo)->numb == 0)
#define tlkapi_qfifo_isEmpty(pFifo)            (!(pFifo)->full && (pFifo)->wptr == (pFifo)->rptr)

#define tlkapi_qfifo_getFull(pFifo)            ((pFifo)->full)
#define tlkapi_qfifo_getTemp(pFifo)            ((pFifo)->temp)
#define tlkapi_qfifo_getResv(pFifo)            ((pFifo)->resv)
#define tlkapi_qfifo_getRptr(pFifo)            ((pFifo)->rptr)
#define tlkapi_qfifo_getWptr(pFifo)            ((pFifo)->wptr)
#define tlkapi_qfifo_setFull(pFifo,value)      ((pFifo)->full = (value))
#define tlkapi_qfifo_setTemp(pFifo,value)      ((pFifo)->temp = (value))
#define tlkapi_qfifo_setResv(pFifo,value)      ((pFifo)->resv = (value))
#define tlkapi_qfifo_setRptr(pFifo,value)      ((pFifo)->rptr = (value))
#define tlkapi_qfifo_setWptr(pFifo,value)      ((pFifo)->wptr = (value))

/******************************************************************************
 * Object: tlkapi_qfifo_getBase
 * Brief : Get the first address of the buffer in qfifo.
 * Params:
 *     @pFifo--The control block of the queue fifo.
 *****************************************************************************/
#define tlkapi_qfifo_getBase(pFifo)            ((pFifo)->pBuff)


/******************************************************************************
 * Object: tlkapi_qfifo_t
 * Brief : The info of the control block of the queue fifo.
 * Params:
 *     @full--Whether all FIFOs in the queue are in use.
 *     @temp--Reserved, not used.
 *     @resv--Reserved, not used.
 *     @numb--The number of the items in queue fifo.
 *     @size--The length of each item in queue fifo.
 *     @wptr--The offset of the item to which the user wrote.
 *     @rptr--The offset of the item to which the user read.
 *     @pBuff--The buffer for caching user data.
 *****************************************************************************/
typedef struct{
	uint08 full;
	uint08 temp;
	uint16 resv;
	uint16 numb;
	uint16 size;
	uint16 wptr;
	uint16 rptr;
	uint08 *pBuff;
}tlkapi_qfifo_t;


/******************************************************************************
 * Function: tlkapi_qfifo_init
 * Descript: Initializes the parameters of the qfifo control block.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 *     @numb[IN]--The number of the items in queue fifo.
 *     @size[IN]--The length of each item in queue fifo.
 *     @pBuffer[IN]--buffer for caching user data.
 *     @buffLen[IN]--The length of the "pBuffer".
 * Return: Operating results. TLK_ENONE means success, others means failure.
 *****************************************************************************/
int tlkapi_qfifo_init(tlkapi_qfifo_t *pFifo, uint16 numb, uint16 size,
	uint08 *pBuffer, uint32 buffLen);

/******************************************************************************
 * Function: tlkapi_qfifo_reset
 * Descript: Reset the qfifo control block.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: None.
 * Notice:
 *     1.After the reset, all parameters of qfifo will be forcibly cleared to 
 *       zero. Please call with caution.
 *     2.If you just want to clear the data stored internally, you can call 
 *       "tlkapi_qfifo_clear()".
 *****************************************************************************/
void tlkapi_qfifo_reset(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_clear
 * Descript: Clear the data saved in qfifo.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: None.
 * Notice:
 *     1.After calling this interface, "pFifo->wptr"/"pFifo->rptr"/"pFifo->full"
 *       will be cleared to zero.
 *****************************************************************************/
void tlkapi_qfifo_clear(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_usedNum
 * Descript: Get the number of the items in the usage list.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: The number of the items in the usage list.
 *****************************************************************************/
uint16 tlkapi_qfifo_usedNum(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_idleNum
 * Descript: Get the number of the items in the free list.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: The number of the items in the free list.
 *****************************************************************************/
uint16 tlkapi_qfifo_idleNum(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_getBuff
 * Descript: Gets the first item from the free list in qfifo.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: The first address of the item, and nullptr means failure.
 * Notice:
 *     1.This interface, unlike "tlkapi_qfifo_takeBuff()", does not actively  
 *       remove the item from the qfifo free list. If the user drops the first
 *       free item, please call tlkapi_qfifo_dropBuff().
 *     2."tlkapi_qfifo_getBuff()" corresponds to "tlkapi_qfifo_getData()",
 *       with one fetching the free item and the other fetching the used item.
 *       In general, the item after "tlkapi_qfifo_dropBuff()" can be retrieved
 *       by "tlkapi_qfifo_getData()"; the item after "tlkapi_qfifo_dropData()"
 *       is returned to the free list of qfifo, and can be obtained by 
 *       "tlkapi_qfifo_getBuff()".
 *     3.The interface is placed in I-RAM and can be accessed by high-priority
 *       interrupts.
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
uint08 *tlkapi_qfifo_getBuff(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_dropBuff
 * Descript: Removes the first item from the free list.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: None.
 * Notice:
 *     1.After calling this interface, the first item in the free list will be 
 *       placed in the last position in the used list.
 *     2.This interface is usually used with "tlkapi_qfifo_getBuff()". The user
 *       stores data through "tlkapi_qfifo_getBuff()" and then calls the 
 *       interface to automatically place the item in the last place in the 
 *       usage queue. Then you can then call "tlkapi_qfifo_getData()" to
 *       retrieve the data.
 *     3.The interface is placed in I-RAM and can be accessed by high-priority
 *       interrupts.
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
void tlkapi_qfifo_dropBuff(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_getData
 * Descript: Gets the first item from the usage list in qfifo.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: The first address of the item, and nullptr means failure.
 * Notice:
 *     1.This interface, unlike "tlkapi_qfifo_takeData()", does not actively  
 *       remove the item from the qfifo usage list. If the user drops the first
 *       usage item, please call tlkapi_qfifo_dropData().
 *     2.The interface is placed in I-RAM and can be accessed by high-priority
 *       interrupts.
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
uint08 *tlkapi_qfifo_getData(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_dropData
 * Descript: Removes the first item from the usage list.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: None.
 * Notice:
 *     1.After calling this interface, the first item in the usage list will be 
 *       placed in the last position in the free list.
 *     2.This interface is usually used with "tlkapi_qfifo_getData()". The user
 *       access data through "tlkapi_qfifo_getData()" and then calls the 
 *       interface to automatically place the item in the last place in the 
 *       free list. Then you can then call "tlkapi_qfifo_getBuff()" to use the
 *       item continuely.
 *     3.The interface is placed in I-RAM and can be accessed by high-priority
 *       interrupts.
 *****************************************************************************/
_attribute_ram_code_sec_noinline_
void tlkapi_qfifo_dropData(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_takeBuff
 * Descript: Gets the first item from the free list in qfifo.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: The first address of the item, and nullptr means failure.
 * Notice:
 *     1.This interface, unlike "tlkapi_qfifo_getBuff()", will actively remove
 *       the item from the qfifo free list. This is equivalent to calling 
 *       "tlkapi_qfifo_dropBuff()" after calling "tlkapi_qfifo_getBuff()".
 *****************************************************************************/
uint08 *tlkapi_qfifo_takeBuff(tlkapi_qfifo_t *pFifo);

/******************************************************************************
 * Function: tlkapi_qfifo_getData
 * Descript: Gets the first item from the usage list in qfifo.
 * Params:
 *     @pFifo[IN]--The control block of the queue fifo.
 * Return: The first address of the item, and nullptr means failure.
 * Notice:
 *     1.This interface, unlike "tlkapi_qfifo_getData()", will actively remove
 *       the item from the qfifo free list. This is equivalent to calling 
 *       "tlkapi_qfifo_dropData()" after calling "tlkapi_qfifo_getData()".
 *****************************************************************************/
uint08 *tlkapi_qfifo_takeData(tlkapi_qfifo_t *pFifo);





#endif //TLKAPI_QFIFO_H


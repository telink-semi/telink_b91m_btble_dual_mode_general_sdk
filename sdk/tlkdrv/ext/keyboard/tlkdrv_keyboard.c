/********************************************************************************************************
 * @file	tlkdrv_keyboard.c
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
#if (TLKDEV_KEYBOARD_ENABLE)
#include "tlkdrv_keyboard.h"
#include "drivers.h"





static uint32 tlkdrv_keyboard_readStatus(uint08 *gpio);
static bool tlkdrv_keyboard_scanKeyMap(bool readKey, uint08 *gpio);
static uint32 tlkdrv_keyboard_scanRowMatrix(int drvInd, uint08 *gpio);
static void tlkdrv_keyboard_saveKeyCode(uint32 *pMatrix);
static bool tlkdrv_keyboard_debounceAndFilter(uint32 curMatrix[], uint32 filt_en);
#if (TLKDRV_KEYBOARD_REMOVE_GHOST_KEY_EN)
static void tlkdrv_keyboard_removeGhostKey(uint32 *pressed_matrix);
#endif



_attribute_data_retention_  uint32 sTlkDrvKeyboardStatus;


#if (TLKDRV_KEYBOARD_LONG_PRESS_OPTIMIZE)
static uint08 sTlkDrvKbMatrixSameAsLastCnt = 0;  //record key matrix no change cnt
#endif

static uint08 sTlkDrvKbMatrixWptr = 0;
static uint08 sTlkDrvKbMatrixRptr = 0;
static uint08 sTlkDrvKbReleaseCnt = 0;
static uint32 sTlkDrvKbLastRecord = 0;

static uint32 sTlkDrvKbMatrixRecord[4][TLKDRV_KEYBOARD_COLPIN_MAX_NUMB];
static uint32 sTlkDrvKbPreMatrix[TLKDRV_KEYBOARD_COLPIN_MAX_NUMB] = {0};
static uint32 sTlkDrvKbLastMatrix[TLKDRV_KEYBOARD_COLPIN_MAX_NUMB] = {0};
static tlkdrv_keyboard_t sTlkDrvKeyboardCtrl;



int tlkdrv_keyboard_init(void)
{
	tmemset(&sTlkDrvKeyboardCtrl, 0, sizeof(tlkdrv_keyboard_t));
	
	return TLK_ENONE;
}

int tlkdrv_keyboard_open(const uint08 *pKeysMap, const uint16 *pColPins,
	const uint16 *pRowPins, uint08 colNumb, uint08 rowNumb, uint08 colWrite,
	uint08 rowValid, uint08 colPullup, uint08 rowPullup)
{
	uint08 index;

	if(pColPins == nullptr || pRowPins == nullptr || colNumb == 0 || rowNumb == 0){
		tlkapi_error(TLKDRV_KB_DBG_FLAG, TLKDRV_KB_DBG_SIGN,
			"tlkdrv_keyboard_open: error param - colNumb[%d], rowNumb[%d]", colNumb, rowNumb);
		return -TLK_EPARAM;
	}
	if(colNumb > TLKDRV_KEYBOARD_COLPIN_MAX_NUMB || rowNumb > TLKDRV_KEYBOARD_ROLPIN_MAX_NUMB){
		tlkapi_error(TLKDRV_KB_DBG_FLAG, TLKDRV_KB_DBG_SIGN,
			"tlkdrv_keyboard_open: colNumb[%d-%d], rowNumb[%d-%d]",
			colNumb, TLKDRV_KEYBOARD_COLPIN_MAX_NUMB, rowNumb, TLKDRV_KEYBOARD_ROLPIN_MAX_NUMB);
		return -TLK_EOVERFLOW;
	}
	
	if(colWrite != 0) colWrite = 1;
	if(rowValid != 0) rowValid = 1;

	sTlkDrvKbMatrixRptr = 0;
	sTlkDrvKbMatrixWptr = 0;
	tmemset(sTlkDrvKbPreMatrix, 0, sizeof(sTlkDrvKbPreMatrix));
	tmemset(sTlkDrvKbLastMatrix, 0, sizeof(sTlkDrvKbLastMatrix));
	tmemset(sTlkDrvKbMatrixRecord, 0, sizeof(sTlkDrvKbMatrixRecord));

	sTlkDrvKeyboardCtrl.colNumb = colNumb;
	sTlkDrvKeyboardCtrl.rowNumb = rowNumb;
	sTlkDrvKeyboardCtrl.colWrite = colWrite;
	sTlkDrvKeyboardCtrl.rowValid = rowValid;
	sTlkDrvKeyboardCtrl.colPullup = colPullup;
	sTlkDrvKeyboardCtrl.rowPullup = rowPullup;
	sTlkDrvKeyboardCtrl.pColPins = pColPins;
	sTlkDrvKeyboardCtrl.pRowPins = pRowPins;
	sTlkDrvKeyboardCtrl.pKeysMap = pKeysMap;
	for(index=0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		gpio_function_en(sTlkDrvKeyboardCtrl.pColPins[index]);
		gpio_set_output_en(sTlkDrvKeyboardCtrl.pColPins[index], 0);
		gpio_set_input_en(sTlkDrvKeyboardCtrl.pColPins[index], 1);
		gpio_setup_up_down_resistor(sTlkDrvKeyboardCtrl.pColPins[index], sTlkDrvKeyboardCtrl.colPullup);
	}
	for(index=0; index<sTlkDrvKeyboardCtrl.rowNumb; index++){
		gpio_function_en(sTlkDrvKeyboardCtrl.pRowPins[index]);
		gpio_set_output_en(sTlkDrvKeyboardCtrl.pRowPins[index], 0);
		gpio_set_input_en(sTlkDrvKeyboardCtrl.pRowPins[index], 1);
		gpio_setup_up_down_resistor(sTlkDrvKeyboardCtrl.pRowPins[index], sTlkDrvKeyboardCtrl.rowPullup);
	}
	sTlkDrvKeyboardCtrl.isOpen = true;
	
	return TLK_ENONE;
}
int tlkdrv_keyboard_close(void)
{
	tmemset(&sTlkDrvKeyboardCtrl, 0, sizeof(tlkdrv_keyboard_t));
	sTlkDrvKeyboardCtrl.isOpen = false;
	return TLK_ENONE;
}



void tlkdrv_keyboard_getKey(uint08 *pBuffer, uint08 buffLen, uint08 *pKeyNum)
{
	if(!sTlkDrvKeyboardCtrl.isOpen) return;
	if(pBuffer == nullptr || buffLen == 0) return;
	if(buffLen >= sTlkDrvKeyboardCtrl.keyNumb){
		buffLen = sTlkDrvKeyboardCtrl.keyNumb;
	}
	if(buffLen != 0) tmemcpy(pBuffer, sTlkDrvKeyboardCtrl.keycode, buffLen);
	if(pKeyNum != nullptr) *pKeyNum = buffLen;
}

bool tlkdrv_keyboard_scanKey(bool readKey)
{
	uint08 gpio[12];

	if(!sTlkDrvKeyboardCtrl.isOpen) return false;
	sTlkDrvKeyboardStatus = tlkdrv_keyboard_readStatus(gpio);
	if(sTlkDrvKeyboardStatus){
		return tlkdrv_keyboard_scanKeyMap(readKey, gpio);
	}else{
		return 0;
	}
}

static uint32 tlkdrv_keyboard_readStatus(uint08 *gpio)
{
	uint08 index;
	uint32 record;
	
	record = 0;
	for(index = 0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		gpio_write(sTlkDrvKeyboardCtrl.pColPins[index], sTlkDrvKeyboardCtrl.colWrite);
		gpio_set_output_en(sTlkDrvKeyboardCtrl.pColPins[index], 1);
	}
	sleep_us(20);
	gpio_read_all(gpio);
	
	for(index = 0; index<sTlkDrvKeyboardCtrl.rowNumb; index++){
		if(sTlkDrvKeyboardCtrl.rowValid != !gpio_read_cache(sTlkDrvKeyboardCtrl.pRowPins[index], gpio)){
			record |= (1 << index);
			sTlkDrvKbReleaseCnt = 6;
			sTlkDrvKbLastRecord = record;
		}
	}
	if(sTlkDrvKbReleaseCnt != 0){
		record = sTlkDrvKbLastRecord;
		sTlkDrvKbReleaseCnt--;
	}
	for(index = 0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		gpio_write(sTlkDrvKeyboardCtrl.pColPins[index], 0);
		gpio_set_output_en(sTlkDrvKeyboardCtrl.pColPins[index], 0);
	}
	return record;
}
 
static bool tlkdrv_keyboard_scanKeyMap(bool readKey, uint08 *gpio)
{
	bool isChange;
	uint08 index;
	uint32 matrix;
	uint32 *pMatrix;
	uint32 rowMatrix[TLKDRV_KEYBOARD_COLPIN_MAX_NUMB] = {0};
	
	sTlkDrvKeyboardCtrl.keyNumb = 0;
			
	tlkdrv_keyboard_scanRowMatrix(0, gpio);
	for(index=0; index<=sTlkDrvKeyboardCtrl.colNumb; index++){
		matrix = tlkdrv_keyboard_scanRowMatrix(index < sTlkDrvKeyboardCtrl.colNumb ? index : 0, gpio);
		if(index != 0){
			rowMatrix[index - 1] = matrix;
		}
	}
	
#if(TLKDRV_KEYBOARD_REMOVE_GHOST_KEY_EN)
	tlkdrv_keyboard_removeGhostKey(rowMatrix);
#endif
	
	isChange = tlkdrv_keyboard_debounceAndFilter(rowMatrix, 1);	
	if(isChange){
		/////////// push to matrix buffer /////////////////////////
		pMatrix = sTlkDrvKbMatrixRecord[sTlkDrvKbMatrixWptr & 3];
		for(index=0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
			*pMatrix++ = rowMatrix[index];
		}
		sTlkDrvKbMatrixWptr = (sTlkDrvKbMatrixWptr + 1) & 7;
		if(((sTlkDrvKbMatrixWptr - sTlkDrvKbMatrixRptr) & 7) > 4){  //overwrite older data
			sTlkDrvKbMatrixRptr = (sTlkDrvKbMatrixWptr - 4) & 7;
		}
	}
	 
	////////// read out //////////
	if(sTlkDrvKbMatrixWptr == sTlkDrvKbMatrixRptr || !readKey){
		return false;
	}
	pMatrix = sTlkDrvKbMatrixRecord[sTlkDrvKbMatrixRptr & 3];
	sTlkDrvKbMatrixRptr = (sTlkDrvKbMatrixRptr + 1) & 7;

	///////////////////////////////////////////////////////////////////
	tlkdrv_keyboard_saveKeyCode(pMatrix);
 	
	return true;
}

static void tlkdrv_keyboard_saveKeyCode(uint32 *pMatrix)
{
	uint08 indexI;
	uint08 indexJ;
	uint32 matrix;
	
	for(indexI=0; indexI<sTlkDrvKeyboardCtrl.colNumb; indexI++){
		matrix = pMatrix[indexI];
		if(matrix == 0) continue;
		
		for(indexJ=0; indexJ<sTlkDrvKeyboardCtrl.rowNumb; indexJ++){
			if((matrix & 0x01) != 0){
				sTlkDrvKeyboardCtrl.keycode[sTlkDrvKeyboardCtrl.keyNumb++] = 
					sTlkDrvKeyboardCtrl.pKeysMap[indexJ*sTlkDrvKeyboardCtrl.colNumb+indexI];
				if(sTlkDrvKeyboardCtrl.keyNumb >= TLKDRV_KEYBOARD_KEY_MAX_NUMB) break;
			}
			matrix = matrix >> 1;
			if(matrix == 0) break;
		}
		if(sTlkDrvKeyboardCtrl.keyNumb >= TLKDRV_KEYBOARD_KEY_MAX_NUMB) break;
	}
}


static uint32 tlkdrv_keyboard_scanRowMatrix(int drvInd, uint08 *gpio)
{
	uint08 index;
	uint32 sr = irq_disable();
	uint32 matrix = 0;
	uint32 drvPin = sTlkDrvKeyboardCtrl.pColPins[drvInd];
	
	gpio_write(drvPin, sTlkDrvKeyboardCtrl.colWrite);
	gpio_set_output_en(drvPin, 1);
	
	for(index=0; index<sTlkDrvKeyboardCtrl.rowNumb; index++){
		if(sTlkDrvKeyboardStatus & BIT(index)){
			int key = !gpio_read_cache (sTlkDrvKeyboardCtrl.pRowPins[index], gpio);
			if(sTlkDrvKeyboardCtrl.rowValid != key) {
				matrix |= (1 << index);
			}
		}
	}
	//sleep_us(20);
	gpio_read_all(gpio);
 
	//sleep_us(20);
	gpio_write(drvPin, 0);
	gpio_set_output_en(drvPin, 0);
		
	irq_restore(sr);
	return matrix;
}

static bool tlkdrv_keyboard_debounceAndFilter(uint32 curMatrix[], uint32 filt_en)
{
	bool isChg;
	uint08 index;
#if (TLKDRV_KEYBOARD_LONG_PRESS_OPTIMIZE)
	uint08 matrix_differ = 0;
#endif
	uint32 mtrx_tmp;

	isChg = false;
	for(index = 0; index<sTlkDrvKeyboardCtrl.colNumb; index++){
		mtrx_tmp = curMatrix[index];
		if(filt_en){
			curMatrix[index] = ( ~sTlkDrvKbLastMatrix[index] & (sTlkDrvKbPreMatrix[index] & mtrx_tmp) )
				| ( sTlkDrvKbLastMatrix[index] & (sTlkDrvKbPreMatrix[index] | mtrx_tmp) );
		}
		if(curMatrix[index] != sTlkDrvKbLastMatrix[index]){
			isChg = true;
		}
#if (TLKDRV_KEYBOARD_LONG_PRESS_OPTIMIZE)
		if(curMatrix[index]^sTlkDrvKbPreMatrix[index]){  //when same, XOR value is 0
			matrix_differ = 1;
		}
#endif
		sTlkDrvKbPreMatrix[index] = mtrx_tmp;
		sTlkDrvKbLastMatrix[index] = curMatrix[index];
	}
 
#if (TLKDRV_KEYBOARD_LONG_PRESS_OPTIMIZE)
	if(matrix_differ){
		sTlkDrvKbMatrixSameAsLastCnt = 0;
	}else{
		sTlkDrvKbMatrixSameAsLastCnt ++;
	}
#endif
 
	return isChg;
}


#if(TLKDRV_KEYBOARD_REMOVE_GHOST_KEY_EN)
#define BIT_IS_POW2(x)     (!((x) & ((x)-1)))
static void tlkdrv_keyboard_removeGhostKey(uint32 *pRowMatrix)
{
	uint08 indexI;
	uint08 indexJ;
	uint32 mix_final = 0;
	for(indexI=0; indexI< sTlkDrvKeyboardCtrl.colNumb; indexI++){
		for(indexJ=(indexI+1); indexJ<sTlkDrvKeyboardCtrl.colNumb; ++indexJ){
			uint32 mix = (pRowMatrix[indexI] & pRowMatrix[indexJ]);
			//four or three key at "#" is pressed at the same time, should remove ghost key
			if( mix && (!BIT_IS_POW2(mix) || (pRowMatrix[indexI] ^ pRowMatrix[indexJ])) ){
				// remove ghost keys
				mix_final |= mix;
			}
		}
		pRowMatrix[indexI] &= ~mix_final;
	}
}
#endif



#endif //#if (TLKDEV_KEYBOARD_ENABLE)


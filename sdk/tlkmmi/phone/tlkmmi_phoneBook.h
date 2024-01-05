/********************************************************************************************************
 * @file	tlkmmi_phoneBook.h
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
#ifndef TLKMMI_PHONE_BOOK_H
#define TLKMMI_PHONE_BOOK_H

#if (TLKMMI_PHONE_ENABLE)


#define TLKMMI_PHONE_NAME_MAX_LEN         32

#define TLKMMI_PHONE_BOOK_HEADLEN        16


#define TLKMMI_PHONE_BOOK_FLAG_INIT            0xFF
#define TLKMMI_PHONE_BOOK_FLAG_WRITE           0xFE
#define TLKMMI_PHONE_BOOK_FLAG_COMPLETE        0xF0

#define TLKMMI_PHONE_BOOK_SIGN                 0x50424150
#define TLKMMI_PHONE_BOOK_ITEM_LENGTH          (BTP_PBAP_CALL_NAME_LENGTH*2+BTP_PBAP_CELL_NUMB_LENGTH*BTP_PBAP_CELL_NUMB_COUNT)
#define TLKMMI_PHONE_BOOK_MAX_ITEM_NUMB        ((TLK_CFG_FLASH_PBAP_LIST_LENS-32)/TLKMMI_PHONE_BOOK_ITEM_LENGTH)



typedef struct{
	uint32 pbapSign;
	uint08 btAddr[6];
	uint08 itemLens;
	uint08 callFlag;
	uint32 callNumb;
}__attribute__ ((__packed__)) tlkmmi_phoneBookHead_t;

typedef struct{
	uint08 isBusy;
	uint08 isReady;
	uint08 posi;
	uint08 type;
	uint08 sort;
	uint16 offset;
	uint16 number;
	uint16 aclHandle;
	uint08 btAddr[6];
	uint32 bookCount;
	tlkapi_timer_t timer;
}tlkmmi_phoneBookCtrl_t;


/******************************************************************************
 * Function: tlkmmi_phone_bookInit
 * Descript: Trigger the phone book to initial, register the pbap ready and 
 *           phone book content callback and reset the control block, and 
 *           read the Bt address. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_bookInit(void);

/******************************************************************************
 * Function: tlkmmi_phone_bookInfoValid
 * Descript: Verify the pbap info is valid. 
 * Params:@pBtAddr[IN]--The Bt address.
 * Return: Return true is valid, other value is invalid.
 * Others: None.
*******************************************************************************/
bool tlkmmi_phone_bookInfoValid(uint08 *pBtAddr);

/******************************************************************************
 * Function: tlkmmi_phone_bookCleanInfo
 * Descript: Use to erase all the pb info. 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_phone_bookCleanInfo(void);

/******************************************************************************
 * Function: tlkmmi_phone_startSyncBook
 * Descript: Use to start to sync the pb info. 
 * Params:
 *        @isForce[IN]--is force to sync the pb info.
 *        @pBtAddr[IN]--The device's Bt address
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_startSyncBook(uint16 aclHandle, uint08 *pBtAddr, bool isForce);

/******************************************************************************
 * Function: tlkmmi_phone_closeSyncBook
 * Descript: Use to stop sync pb info. 
 *        @isForce[IN]--is force to sync the pb info.
 *        @pBtAddr[IN]--The device's Bt address
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_closeSyncBook(uint16 aclHandle);


/******************************************************************************
 * Function: tlkmmi_phone_bookGetName
 * Descript: Use to Get the name. 
 * Params:
 *        @pNumber[IN]--The cell number
 *        @numLen[IN]--The length.
 *        @pName[IN]--The name.
 *        @nameLen[IN]--The name length.
 *        @pGetLen[IN]--The get length.
 * Return: Return TLK_ENONE is success/other value is false.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_bookGetName(uint08 *pNumber, uint08 numbLen, uint08 *pName, uint08 nameLen, uint08 *pGetLen);

int tlkmmi_phone_bookSetParam(uint08 posi, uint08 type, uint08 sort, uint16 offset, uint16 number);



#endif //#if (TLKMMI_PHONE_ENABLE)

#endif //TLKMMI_PHONE_BOOK_H


/********************************************************************************************************
 * @file	tlkmmi_phoneBook.c
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
#if (TLKMMI_PHONE_ENABLE)
#include "tlkmmi/phone/tlkmmi_phone.h"
#include "tlkmmi/phone/tlkmmi_phoneCtrl.h"
#include "tlkmmi/phone/tlkmmi_phoneBook.h"

#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"


#if (TLKBTP_CFG_PBAPCLT_ENABLE)
void tlkmmi_phone_printBook(btp_pbap_info_t *pInfo);

static void tlkmmi_phone_saveBookOver(bool isSucc);
static void tlkmmi_phone_savePhoneBook(btp_pbap_info_t *pInfo);
static void tlkmmi_phone_bookReportCB(uint16 aclHandle, uint08 endFlag, bool isSucc, btp_pbap_info_t *pInfo, uint08 itemCount);

extern unsigned int tlkcfg_getFlashAddr(unsigned int offset);
#endif

static tlkmmi_phoneBookCtrl_t sTlkMmmiPhoneBookCtrl;


/******************************************************************************
 * Function: tlkmmi_phone_bookInit
 * Descript: Trigger the phone book to initial, register the pbap ready and 
 *           phone book content callback and reset the control block, and 
 *           read the Bt address. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_bookInit(void)
{
	#if (TLKBTP_CFG_PBAPCLT_ENABLE)
	tlkmmi_phoneBookHead_t head;
	#endif
	
	tmemset(&sTlkMmmiPhoneBookCtrl, 0, sizeof(tlkmmi_phoneBookCtrl_t));
	#if (TLKBTP_CFG_PBAPCLT_ENABLE)
	unsigned int pbapAddr;
	pbapAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PBAP_LIST_ADDR);
	if(pbapAddr == 0) return -TLK_EFAIL;
	
	btp_pbapclt_regBookCB(tlkmmi_phone_bookReportCB);
	
	tlkapi_flash_read(pbapAddr, (uint08*)&head, 16);
	if(head.pbapSign != 0xFFFFFFFF && (head.itemLens != TLKMMI_PHONE_BOOK_ITEM_LENGTH
		|| head.callFlag != TLKMMI_PHONE_BOOK_FLAG_COMPLETE || head.callNumb == 0xFFFFFFFF)){
		tlkmmi_phone_bookCleanInfo();
	}else{
		if(head.callNumb > TLKMMI_PHONE_BOOK_MAX_ITEM_NUMB){
			head.callNumb = TLKMMI_PHONE_BOOK_MAX_ITEM_NUMB;
		}
		sTlkMmmiPhoneBookCtrl.isReady = true;
		sTlkMmmiPhoneBookCtrl.bookCount = head.callNumb;
		tmemcpy(sTlkMmmiPhoneBookCtrl.btAddr, head.btAddr, 6);
	}
	#endif

	sTlkMmmiPhoneBookCtrl.posi = BTP_PBAP_BOOK_POSI_PHONE;
	sTlkMmmiPhoneBookCtrl.type = BTP_PBAP_BOOK_TYPE_PHONE_BOOK;
	sTlkMmmiPhoneBookCtrl.sort = BTP_PBAP_BOOK_SORT_INDEX;
	sTlkMmmiPhoneBookCtrl.offset = 0x0000;
	sTlkMmmiPhoneBookCtrl.number = 0xFFFF;
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_phone_bookInfoValid
 * Descript: Verify the pbap info is valid. 
 * Params:@pBtAddr[IN]--The Bt address.
 * Return: Return true is valid, other value is invalid.
 * Others: None.
*******************************************************************************/
bool tlkmmi_phone_bookInfoValid(uint08 *pBtAddr)
{
	if(!sTlkMmmiPhoneBookCtrl.isReady || pBtAddr == nullptr) return false;
	if(tmemcmp(sTlkMmmiPhoneBookCtrl.btAddr, pBtAddr, 6) == 0) return true;
	else return false;
}

/******************************************************************************
 * Function: tlkmmi_phone_bookCleanInfo
 * Descript: Use to erase all the pb info. 
 * Params:
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_phone_bookCleanInfo(void)
{
#if (TLKBTP_CFG_PBAPCLT_ENABLE)
	unsigned int pbapAddr;
	pbapAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PBAP_LIST_ADDR);
	if(pbapAddr == 0) return;

	uint32 addr;
	if(sTlkMmmiPhoneBookCtrl.isBusy) return;
	sTlkMmmiPhoneBookCtrl.isReady = false;
	sTlkMmmiPhoneBookCtrl.bookCount = 0;
	for(addr=0; addr<TLK_CFG_FLASH_PBAP_LIST_LENS; addr+=0x1000){
		tlkapi_flash_eraseSector(pbapAddr+addr);
	}
#endif
}

/******************************************************************************
 * Function: tlkmmi_phone_startSyncBook
 * Descript: Use to start to sync the pb info. 
 * Params:
 *        @isForce[IN]--is force to sync the pb info.
 *        @pBtAddr[IN]--The device's Bt address
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_startSyncBook(uint16 aclHandle, uint08 *pBtAddr, bool isForce)
{
	#if (TLKBTP_CFG_PBAPCLT_ENABLE)
	int ret;
	unsigned int pbapAddr;

	pbapAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PBAP_LIST_ADDR);
	if(pbapAddr == 0) {
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_startSyncBook: failure - addr");
		return -TLK_EFAIL;
	}
	
	if(aclHandle == 0 || pBtAddr == nullptr){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_startSyncBook: failure - param");
		return -TLK_EPARAM;
	}
	if(sTlkMmmiPhoneBookCtrl.isBusy){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_startSyncBook: failure - busy");
		return -TLK_EBUSY;
	}
	if(sTlkMmmiPhoneBookCtrl.isReady){
		if(!isForce && tmemcmp(sTlkMmmiPhoneBookCtrl.btAddr, pBtAddr, 6) == 0){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_startSyncBook: failure - exist");
			return TLK_ENONE;
		}
		tlkmmi_phone_bookCleanInfo();
	}
	
	sTlkMmmiPhoneBookCtrl.bookCount = 0;
	sTlkMmmiPhoneBookCtrl.aclHandle = aclHandle;
	tmemcpy(sTlkMmmiPhoneBookCtrl.btAddr, pBtAddr, 6);
	
	ret = btp_pbapclt_startSyncBook(aclHandle, sTlkMmmiPhoneBookCtrl.posi, sTlkMmmiPhoneBookCtrl.type,
		sTlkMmmiPhoneBookCtrl.sort, sTlkMmmiPhoneBookCtrl.offset, sTlkMmmiPhoneBookCtrl.number);
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_startSyncBook: failure - reject");
	}else{
		tlkmmi_phoneBookHead_t head;
		
		sTlkMmmiPhoneBookCtrl.isBusy = true;

		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_startSyncBook: success");
		tmemset(&head, 0, 16);
		head.pbapSign = TLKMMI_PHONE_BOOK_SIGN;
		tmemcpy(head.btAddr, pBtAddr, 6);;
		head.itemLens = TLKMMI_PHONE_BOOK_ITEM_LENGTH;
		head.callFlag = TLKMMI_PHONE_BOOK_FLAG_WRITE;
		head.callNumb = 0xFFFFFFFF;
		tlkapi_flash_write(pbapAddr, (uint08*)&head, 16);
	}
	return ret;
	#else
	return -TLK_ENOSUPPORT;
	#endif
}

/******************************************************************************
 * Function: tlkmmi_phone_closeSyncBook
 * Descript: Use to stop sync pb info. 
 *        @isForce[IN]--is force to sync the pb info.
 *        @pBtAddr[IN]--The device's Bt address
 * Return: Return TLK_ENONE is success/other is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_phone_closeSyncBook(uint16 aclHandle)
{
	#if (TLKBTP_CFG_PBAPCLT_ENABLE)
	int ret;
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_closeSyncBook ");
	if(!sTlkMmmiPhoneBookCtrl.isBusy) return TLK_ENONE;
	if(aclHandle == 0 || sTlkMmmiPhoneBookCtrl.aclHandle != aclHandle) return -TLK_EPARAM;
	sTlkMmmiPhoneBookCtrl.isBusy = false;
	sTlkMmmiPhoneBookCtrl.aclHandle = 0;
	ret = btp_pbapclt_closeSyncBook(aclHandle);
	if(!sTlkMmmiPhoneBookCtrl.isReady){
		tlkmmi_phone_bookCleanInfo();
	}
	return ret;
	#else
	return -TLK_ENOSUPPORT;
	#endif
}


int tlkmmi_phone_bookSetParam(uint08 posi, uint08 type, uint08 sort, uint16 offset, uint16 number)
{
	if(sTlkMmmiPhoneBookCtrl.isBusy){
		tlkapi_error(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookSetParam: failure - busy");
		return -TLK_EBUSY;
	}
	if(number == 0) number = 0xFFFF;

	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookSetParam: {posi-%d,type-%d,sort-%d,count-%d}", 
		posi, type, sort, number);
	
	sTlkMmmiPhoneBookCtrl.posi = posi;
	sTlkMmmiPhoneBookCtrl.type = type;
	sTlkMmmiPhoneBookCtrl.sort = sort;
	sTlkMmmiPhoneBookCtrl.offset = offset;
	sTlkMmmiPhoneBookCtrl.number = number;
	
	return TLK_ENONE;
}

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
int tlkmmi_phone_bookGetName(uint08 *pNumber, uint08 numbLen, uint08 *pName, uint08 nameLen, uint08 *pGetLen)
{
#if (TLKBTP_CFG_PBAPCLT_ENABLE)
	int ret;
	uint32 saddr;
	uint16 iIndex;
	uint08 jIndex;
	uint08 offset;
	uint08 buffer[TLKMMI_PHONE_BOOK_ITEM_LENGTH] = {0};
	unsigned int pbapAddr;

	pbapAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PBAP_LIST_ADDR);
	if(pbapAddr == 0) return -TLK_EFAIL;
	

//	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookGetName: %d, %d, %d, %d", 
//		sTlkMmmiPhoneBookCtrl.isReady, numbLen, sTlkMmmiPhoneBookCtrl.bookCount, nameLen);
	if(!sTlkMmmiPhoneBookCtrl.isReady) return -TLK_ENOREADY;
	if(pNumber == nullptr || numbLen == 0 || numbLen > BTP_PBAP_CELL_NUMB_LENGTH
		|| pName == nullptr || nameLen == 0){
		return -TLK_EPARAM;
	}
	saddr = pbapAddr+TLKMMI_PHONE_BOOK_HEADLEN;
	for(iIndex=0; iIndex<sTlkMmmiPhoneBookCtrl.bookCount; iIndex++){
		tlkapi_flash_read(saddr, buffer, TLKMMI_PHONE_BOOK_ITEM_LENGTH);
		offset = BTP_PBAP_CALL_NAME_LENGTH*2;
//		tlkapi_array(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookGetName: phone - ", &buffer[offset], 11);
		for(jIndex=0; jIndex<BTP_PBAP_CELL_NUMB_COUNT; jIndex++){
			if(tmemcmp(&buffer[offset], pNumber, numbLen) == 0) break;
			offset += BTP_PBAP_CELL_NUMB_LENGTH;
		}
		if(jIndex != BTP_PBAP_CELL_NUMB_COUNT) break;
		saddr += TLKMMI_PHONE_BOOK_ITEM_LENGTH;
	}
	if(iIndex == sTlkMmmiPhoneBookCtrl.bookCount) return -TLK_EFAIL;
	
	ret = tlkapi_wcharStrlen(buffer, BTP_PBAP_CALL_NAME_LENGTH*2);
	if(nameLen > ret) nameLen = ret;
	tmemcpy(pName, buffer, nameLen);
	if(pGetLen != nullptr) *pGetLen = nameLen;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif

}


#if (TLKBTP_CFG_PBAPCLT_ENABLE)
static void tlkmmi_phone_bookReportCB(uint16 aclHandle, uint08 endFlag, bool isSucc, btp_pbap_info_t *pInfo, uint08 itemCount)
{
	uint08 index;

	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB ");
	if(!sTlkMmmiPhoneBookCtrl.isBusy || aclHandle != sTlkMmmiPhoneBookCtrl.aclHandle){
		btp_pbapclt_closeSyncBook(aclHandle);
		return;
	}
	
	if(endFlag){
		if(isSucc){ 
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB[END][SUCC]: bookCount-%d", sTlkMmmiPhoneBookCtrl.bookCount);
		}else{ 
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB[END][FAIL]: endFlag-%d", endFlag);
		}
	}else{
		if(isSucc){ tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB[SUCC]: itemCount-%d", itemCount);}
		else{ tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB[FAIL]: ");}
	}
	if(!isSucc){
		tlkmmi_phone_saveBookOver(false);
		tlkmmi_phone_closeSyncBook(aclHandle);
		return;
	}
	if(!sTlkMmmiPhoneBookCtrl.isBusy) return;
	if(sTlkMmmiPhoneBookCtrl.bookCount >= TLKMMI_PHONE_BOOK_MAX_ITEM_NUMB){
		itemCount = 0;
	}else if(sTlkMmmiPhoneBookCtrl.bookCount+itemCount > TLKMMI_PHONE_BOOK_MAX_ITEM_NUMB){
		itemCount = TLKMMI_PHONE_BOOK_MAX_ITEM_NUMB-sTlkMmmiPhoneBookCtrl.bookCount;
	}
	
	tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB 01: itemCount-%d, bookCount-%d", 
		itemCount, sTlkMmmiPhoneBookCtrl.bookCount);
	for(index=0; index<itemCount; index++){
//		my_dump_str_data(TLKMMI_PHONE_BOOK_DBG_ENABLE, "tlkmmi_phone_bookReportCB 02", 0, 0);
		if(pInfo->callType == BTP_PBAP_BOOK_TYPE_PHONE_BOOK){
			tlkmmi_phone_savePhoneBook(&pInfo[index]);
			if(0) tlkmmi_phone_printBook(&pInfo[index]);
			sTlkMmmiPhoneBookCtrl.bookCount ++;
		}
	}
	if(endFlag || sTlkMmmiPhoneBookCtrl.bookCount >= TLKMMI_PHONE_BOOK_MAX_ITEM_NUMB){
		if(endFlag){
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB[COUNT]: normal stop- bookCount-%d", sTlkMmmiPhoneBookCtrl.bookCount);
		}else{
			tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "tlkmmi_phone_bookReportCB[COUNT]: overflow stop- bookCount-%d", sTlkMmmiPhoneBookCtrl.bookCount);
		}
		tlkmmi_phone_saveBookOver(isSucc);
		tlkmmi_phone_closeSyncBook(aclHandle);
	}
}
static void tlkmmi_phone_savePhoneBook(btp_pbap_info_t *pInfo)
{
	uint08 index;
	uint32 saddr;
	uint16 offset;
	uint08 buffer[TLKMMI_PHONE_BOOK_ITEM_LENGTH];
	unsigned int pbapAddr;

	pbapAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PBAP_LIST_ADDR);
	if(pbapAddr == 0) return;
	

	saddr = pbapAddr+TLKMMI_PHONE_BOOK_HEADLEN
		+sTlkMmmiPhoneBookCtrl.bookCount*TLKMMI_PHONE_BOOK_ITEM_LENGTH;
		
	offset = 0;
	tmemcpy(buffer+offset, pInfo->callName, BTP_PBAP_CALL_NAME_LENGTH*2);
	offset += BTP_PBAP_CALL_NAME_LENGTH*2;
	for(index=0; index<BTP_PBAP_CELL_NUMB_COUNT; index++){
		tmemcpy(buffer+offset, pInfo->cellNumb[index], BTP_PBAP_CELL_NUMB_LENGTH);
		offset += BTP_PBAP_CELL_NUMB_LENGTH;
	}
	tlkapi_flash_write(saddr, buffer, TLKMMI_PHONE_BOOK_ITEM_LENGTH);
}
static void tlkmmi_phone_saveBookOver(bool isSucc)
{
	tlkmmi_phoneBookHead_t head;
	unsigned int pbapAddr;
	pbapAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_PBAP_LIST_ADDR);

	if(!sTlkMmmiPhoneBookCtrl.isBusy || pbapAddr == 0) return;

	sTlkMmmiPhoneBookCtrl.isBusy = false;
	sTlkMmmiPhoneBookCtrl.aclHandle = 0;
	if(!isSucc){
		sTlkMmmiPhoneBookCtrl.isReady = false;
		tlkmmi_phone_bookCleanInfo();
		tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "PbapSaveOver: erase sectors");
	}else{
		sTlkMmmiPhoneBookCtrl.isReady = true;
		tlkapi_flash_read(pbapAddr, (uint08*)&head, 16);
		head.pbapSign = TLKMMI_PHONE_BOOK_SIGN;
		head.callFlag = TLKMMI_PHONE_BOOK_FLAG_COMPLETE;
		head.callNumb = sTlkMmmiPhoneBookCtrl.bookCount;
		tlkapi_flash_write(pbapAddr, (uint08*)&head, 16);
	}
}


void tlkmmi_phone_printBook(btp_pbap_info_t *pInfo)
{
    tlkapi_trace(TLKMMI_PHONE_DBG_FLAG, TLKMMI_PHONE_DBG_SIGN, "   book: name[%d][%4x-%4x-%4x-%4x]; numb[%d][%c%c%c%c%c%c%c%c%c%c%c]",
		pInfo->nameLen,
		pInfo->callName[0],
		pInfo->callName[1],
		pInfo->callName[2],
		pInfo->callName[3],
		pInfo->callCnt,
		pInfo->cellNumb[0][0],
		pInfo->cellNumb[0][1],
		pInfo->cellNumb[0][2],
		pInfo->cellNumb[0][3],
		pInfo->cellNumb[0][4],
		pInfo->cellNumb[0][5],
		pInfo->cellNumb[0][6],
		pInfo->cellNumb[0][7],
		pInfo->cellNumb[0][8],
		pInfo->cellNumb[0][9],
		pInfo->cellNumb[0][10]
	  );
}


#endif


#endif //#if (TLKMMI_PHONE_ENABLE)


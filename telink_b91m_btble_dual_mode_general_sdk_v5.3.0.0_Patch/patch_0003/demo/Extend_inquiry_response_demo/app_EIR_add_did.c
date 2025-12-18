/******************************************************************************
 * Function: tlkmmi_btmgr_ctrlInit
 * Descript: Initial the Bt manager ctrl block, and read local name and 
 *           Bt address. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_ctrlInit(void)
{
	uint16 index;
	uint08 bttemp[TLK_CFG_FLASH_BT_NAME_LENS + 3+50];
	unsigned int nameAddr, addrAddr;

	nameAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_NAME_ADDR);
	addrAddr = tlkcfg_getFlashAddr(TLK_CFG_FLASH_BT_ADDR_ADDR);

	if(nameAddr == 0 || addrAddr == 0){
		return -TLK_EFAIL;
	}

	tmemset(&gTlkMmiBtmgrCtrl, 0, sizeof(tlkmmi_btmgr_ctrl_t));
	
	// Read Local Name
	tlkapi_flash_read(nameAddr, gTlkMmiBtmgrCtrl.btname, TLK_CFG_FLASH_BT_NAME_LENS-1);
	for(index=0; index<TLK_CFG_FLASH_BT_NAME_LENS-1; index++){
		if(gTlkMmiBtmgrCtrl.btname[index] == 0xFF || gTlkMmiBtmgrCtrl.btname[index] == 0x00) break;
	}
	if(index == 0){
		index = strlen(TLKMMI_BTMGR_NAME_DEF);
		tmemcpy(gTlkMmiBtmgrCtrl.btname, TLKMMI_BTMGR_NAME_DEF, index);
	}
	gTlkMmiBtmgrCtrl.btname[index] = 0x00;
	// Read Local Address
	
	
	tlkapi_flash_read(addrAddr, bttemp, 6+2+4+1);
	if(bttemp[0] == 0xFF && bttemp[1] == 0xFF){
	    uint08 randValue[6];
	    tlkapi_random(randValue, 6);
	    bttemp[0] = randValue[5];
	    bttemp[1] = randValue[4];
	    bttemp[2] = randValue[3];
	    bttemp[3] = randValue[2];
	    bttemp[4] = randValue[1];
	    bttemp[5] = randValue[0];
	    tlkapi_flash_write(addrAddr, bttemp, 6);
	}
	tmemcpy(gTlkMmiBtmgrCtrl.btaddr, bttemp, 6);

	bth_hci_sendSetBtAddrCmd(gTlkMmiBtmgrCtrl.btaddr);
	bth_hci_sendWriteLocalNameCmd(gTlkMmiBtmgrCtrl.btname);
	bth_hci_exeCmdNow();
	bth_hci_exeEvtNow();

	#if (TLK_MDI_BTIAP_ENABLE)
	tlkmdi_btiap_setAddr(gTlkMmiBtmgrCtrl.btaddr);
	tlkmdi_btiap_setName(gTlkMmiBtmgrCtrl.btname, strlen((char*)gTlkMmiBtmgrCtrl.btname));
	#endif

	uint08 nameLen = 0;
	uint08 offset = 0;
	nameLen = strlen((char *)(gTlkMmiBtmgrCtrl.btname));
	tmemset(bttemp, 0, TLK_CFG_FLASH_BT_NAME_LENS + 3+50);
	bttemp[0] = nameLen + 1; //data length:
	offset++;
	bttemp[1] = 0x08;        //data type: local name
	offset++;
	tmemcpy(&bttemp[2], gTlkMmiBtmgrCtrl.btname, nameLen);

	offset += nameLen;


	bttemp[offset++] = 7;// DID length

	bttemp[offset++] = 0x16;// DID type

	bttemp[offset++] = 0x0A; //Device ID Service  0x180A
	bttemp[offset++] = 0x18;

	bttemp[offset++] = 0x12; //Vid 0x3412
	bttemp[offset++] = 0x34;

	bttemp[offset++] = 0x11;// Pid 0x2211;
	bttemp[offset++] = 0x22;

	bth_hci_sendWriteExtendedInquiryRspCmd(0, bttemp, offset);
	
	return TLK_ENONE;
}

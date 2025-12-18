/********************************************************************************************************
 * @file	tlkdev_mfi.c
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
#if (TLK_DEV_MFI_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlkdrv/ext/iphone/tlkdrv_mfi.h"
#include "tlkdev/ext/tlkdev_mfi.h"



#define IPA2_CONTROL_SESSION_VERSION                  1



int tlkdev_mfi_init(void)
{
	int ret;

	ret = tlkdrv_mfi_init();
		
	return ret;
}

int tlkdev_mfi_open(void)
{
	return tlkdrv_mfi_open();
}
int tlkdev_mfi_close(void)
{
	return tlkdrv_mfi_close();
}

//<Accessory Interface Specification R36.pdf> P665

int tlkdev_mfi_loadCertificateData(uint08 *pBuffer, uint16 buffLen)
{
	uint16 index;
    uint08 bytes[2];
    uint08 protocolVersionMajor;
    uint16 accessoryCertificateDataLength,numCertificateBytes;

	if(!tlkdrv_mfi_isOpen()){
		tlkapi_error(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "loadCertificateData: Device is not open!");
		return -TLK_ENOREADY;
	}

    tlkdrv_mfi_read(kHAPMFiHWAuthRegister_ErrorCode, bytes, 1);
    tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "ErrorCode=%d", bytes[0]);

    tlkdrv_mfi_read(kHAPMFiHWAuthRegister_DeviceVersion, &protocolVersionMajor, 1);
    tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "device_version=%d", protocolVersionMajor);
    
    tlkdrv_mfi_read(kHAPMFiHWAuthRegister_AuthenticationRevision, &protocolVersionMajor, 1);
    tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "authen_version=%d", protocolVersionMajor);
    
    tlkdrv_mfi_read(kHAPMFiHWAuthRegister_AuthenticationProtocolMajorVersion, &protocolVersionMajor, 1);
    tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "major_version=%d", protocolVersionMajor);

    tlkdrv_mfi_read(kHAPMFiHWAuthRegister_AccessoryCertificateDataLength, bytes, sizeof(bytes));
    tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "data len=%x %x", bytes[0], bytes[1]);
    
    accessoryCertificateDataLength = ((uint16)bytes[0] << 0x08U) | bytes[1];
    if((protocolVersionMajor == 3 && (accessoryCertificateDataLength < 607 || accessoryCertificateDataLength > 609))
		|| (protocolVersionMajor == 2 && accessoryCertificateDataLength > 1280)) 
    {
         tlkapi_error(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "tlkdev_mfi_loadCertificateData fail");
         return -TLK_EFAIL;
    }
       
    numCertificateBytes = 0;
    for(index = 0; accessoryCertificateDataLength != 0; index++) 
    {
        uint16 numBytes = tlkapi_min(accessoryCertificateDataLength, (uint16)128);
		if(numCertificateBytes+numBytes > buffLen){
			tlkapi_fatal(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "loadCertificateData: overflow ! [%d-%d]",
				numCertificateBytes, numBytes);
			break;
		}
        tlkdrv_mfi_read(kHAPMFiHWAuthRegister_AccessoryCertificateData1+index, &((uint08*)pBuffer)[numCertificateBytes], numBytes);
        accessoryCertificateDataLength -= numBytes;
        numCertificateBytes += numBytes;
    }
    tlkapi_array(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "Certificate data=", pBuffer, numCertificateBytes); 
//    tlkapi_array(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "Certificate data2=", &pBuffer[256],256); 
//    tlkapi_array(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "Certificate data3=", &pBuffer[512], 96); 
    tlkdrv_mfi_read(kHAPMFiHWAuthRegister_ErrorCode, bytes, 1);
    tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "ErrorCode=%d", bytes[0]);

	return TLK_ENONE;
}


int tlkdev_mfi_loadChallengeData(uint08 *pData, uint16 dataLen, uint08 *pBuff, uint16 buffLen)
{	
	uint16 nc;
	uint08 length;

	if(!tlkdrv_mfi_isOpen()){
		tlkapi_error(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "loadCertificateData: Device is not open!");
		return -TLK_ENOREADY;
	}

	tlkapi_array(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "tlkdev_mfi_loadChallengeData=", pData, dataLen);  
	tlkdrv_mfi_write(kHAPMFiHWAuthRegister_ChallengeData, pData, dataLen);
	
	nc = 1;
	tlkdrv_mfi_write(kHAPMFiHWAuthRegister_AuthenticationControlAndStatus, (uint08*)&nc, 1);

	tlkdrv_mfi_read(kHAPMFiHWAuthRegister_AuthenticationControlAndStatus, (uint08*)&nc, 1); 
	tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "kHAPMFiHWAuthRegister_AuthenticationControlAndStatus=%d", nc);  

	tlkdrv_mfi_read(kHAPMFiHWAuthRegister_ChallengeResponseDataLength, (uint08*)&nc, 2);
	tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "kHAPMFiHWAuthRegister_ChallengeResponseDataLength=%d", nc);

	length = nc>>8;
	if(length > buffLen) length = buffLen;
	
	tlkdrv_mfi_read(kHAPMFiHWAuthRegister_ChallengeResponseData, pBuff, length);
	tlkapi_trace(TLKDEV_EXT_DBG_FLAG,TLKDEV_EXT_DBG_SIGN,"length = %d",length);
	tlkapi_array(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, "ChallengeResponseData=", pBuff, length);
	
	tlkdrv_mfi_read(kHAPMFiHWAuthRegister_ErrorCode, (uint08*)&nc, 1);
    tlkapi_trace(TLKDEV_EXT_DBG_FLAG, TLKDEV_EXT_DBG_SIGN, ">>> ErrorCode=%d", nc);
	
	return TLK_ENONE;
}





#endif //#if (TLK_DEV_MFI_ENABLE)


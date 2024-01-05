/********************************************************************************************************
 * @file	tlkats_auth.h
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
#ifndef TLKMMI_ATS_AUTH_H
#define TLKMMI_ATS_AUTH_H

#if (TLK_TEST_ATS_ENABLE)


#define TLKMMI_ATS_AUT_ENABLE   0

#if (TLKMMI_ATS_AUT_ENABLE)
#define TLKMMI_ATS_DEVICE_INFO     "AUT-0001"
#else
#define TLKMMI_ATS_DEVICE_INFO     "DUT-0001"
#endif
#define TLKMMI_ATS_DEVICE_VERSION  0x01000001
#define TLKMMI_ATS_DEVICE_VENDOR   0x00000001
#define TLKMMI_ATS_CLIENT_VENDOR   0x00000001

#define TLKMMI_ATS_AUTHEN_INFO     "telink-ats"
#define TLKMMI_ATS_AUTHEN_SCHEME   0x00000001



typedef enum{
    TLKMMI_ATS_AUTH_FLAG_NONE           = 0x00,
	TLKMMI_ATS_AUTH_FLAG_AUTHEN_REQUEST = 0x01,
    TLKMMI_ATS_AUTH_FLAG_AUTHEN_CONFIRM  = 0x02,
    TLKMMI_ATS_AUTH_FLAG_MASK           = 0xFF,
}TLKMMI_ATS_AUTH_FLAG_ENUM;
typedef enum{
	TLKMMI_ATS_FIELD_ID_SERVER_NAME = 0x01,
	TLKMMI_ATS_FIELD_ID_CLIENT_NAME = 0x02,
	TLKMMI_ATS_FIELD_ID_DEVICE_NAME = 0x03,
}TLKMMI_ATS_FIELD_ID_EENUM;


typedef struct{
	uint08 reserved;
	uint08 mineRole;
	uint08 mineChID;
	uint08 waitFlags; //TLKMMI_ATS_AUTH_FLAG_ENUM
	uint08 mineRandom[32];
	uint08 peerRandom[32];
	uint08 mineConfirm[32];
	uint08 peerConfirm[32];
}tlkmmi_ats_auth_t;


int tlkmmi_ats_authInit(void);

int tlkmmi_ats_authReset(void);

void tlkmmi_ats_recvHandler(uint16 msgID, uint08 *pData, uint16 dataLen);



#endif //#if (TLK_TEST_ATS_ENABLE)

#endif //TLKMMI_ATS_AUTH_H


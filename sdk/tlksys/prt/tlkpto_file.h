/********************************************************************************************************
 * @file	tlkpto_file.h
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
#ifndef TLKPTO_FILE_H
#define TLKPTO_FILE_H


typedef enum{
	TLKPRT_FILE_TYPE_DFU  = 0x81,
	TLKPRT_FILE_TYPE_MP3  = 0x91,
	TLKPRT_FILE_TYPE_TONE = 0xA1,
}TLKPRT_FILE_TYPE_ENUM;
typedef enum{
	TLKPRT_FILE_STATUS_SUCCESS       = 0x00,
	TLKPRT_FILE_STATUS_FAILURE       = 0x81,
	TLKPRT_FILE_STATUS_ERR_PARAM     = 0x82,
	TLKPRT_FILE_STATUS_NULL_NAME     = 0x83,
	TLKPRT_FILE_STATUS_ERR_DIGEST    = 0x84,
	TLKPRT_FILE_STATUS_TRAN_STOP     = 0x85,
	TLKPRT_FILE_STATUS_TRAN_FAULT    = 0x86,
	TLKPRT_FILE_STATUS_TRAN_TIMEOUT  = 0x87,
	TLKPRT_FILE_STATUS_ERR_FORMAT    = 0x88,
	TLKPRT_FILE_STATUS_ERR_LENGTH    = 0x89,
	TLKPRT_FILE_STATUS_ERR_PORT      = 0x8A,
	TLKPRT_FILE_STATUS_REPEAT        = 0x8B,
	TLKPRT_FILE_STATUS_NO_QUOTA      = 0x8C,
	TLKPRT_FILE_STATUS_NO_READY      = 0x8D,
	TLKPRT_FILE_STATUS_ERR_VERSION   = 0x8E,
	TLKPRT_FILE_STATUS_AUTH_STALL    = 0x90,
	TLKPRT_FILE_STATUS_AUTH_FAILURE  = 0x91,
	TLKPRT_FILE_STATUS_DEC_FAILURE   = 0x92,
	TLKPRT_FILE_STATUS_NOSUPP_CRYPT  = 0x96,
	TLKPRT_FILE_STATUS_NOSUPP_COMPR  = 0x97,
	TLKPRT_FILE_STATUS_NOSUPP_DIGEST = 0x98,
}TLKPRT_FILE_STATUS_ENUM;


#define TLKPRT_FILE_VERSION             0x0102


typedef enum{
	TLKPRT_FILE_COMPR_FEATURE_LZ4    = 0x00000001,
	TLKPRT_FILE_COMPR_FEATURE_LZ4_HC = 0x00000002,

	TLKPRT_FILE_COMPR_FEATURE_MASK   = 0
		| TLKPRT_FILE_COMPR_FEATURE_LZ4
		| TLKPRT_FILE_COMPR_FEATURE_LZ4_HC,
}TLKPRT_FILE_COMPR_FEATURE_ENUM;

typedef enum{
	TLKPRT_FILE_DIGEST_FEATURE_CRC32 = 0x00000001,
	TLKPRT_FILE_DIGEST_FEATURE_MD5   = 0x00000002,

	TLKPRT_FILE_DIGEST_FEATURE_MASK   = 0
		| TLKPRT_FILE_DIGEST_FEATURE_CRC32,
}TLKPRT_FILE_DIGEST_FEATURE_ENUM;


typedef enum{
	TLKPRT_FILE_CSIGN_NONE = 0x00,
	TLKPRT_FILE_CSIGN_AUTH = 0x81,
	TLKPRT_FILE_CSIGN_CRYP = 0x82,
}TLKPRT_FILE_CSIGN_ENUM;


#endif //TLKPTO_FILE_H


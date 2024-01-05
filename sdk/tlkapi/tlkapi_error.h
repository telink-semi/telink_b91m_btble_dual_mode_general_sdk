/********************************************************************************************************
 * @file	tlkapi_error.h
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
#ifndef TLKAPI_ERROR_H
#define TLKAPI_ERROR_H



#define TLK_ENONE       0x00 //"none error"
#define TLK_EFAIL       0x01 //"fail operate or others"
#define TLK_EBUSY       0x02 //"resource busy or locked"
#define TLK_EOPEN       0x03 //"open error"
#define TLK_EREAD       0x04 //"read error"
#define TLK_ESEEK       0x05 //"seek error"
#define TLK_ECTRL       0x06 //"ctrl error"
#define TLK_EWRITE      0x07 //"write error"
#define TLK_ECLOSE      0x08 //"close error"
#define TLK_EPARAM      0x09 //"param error"
#define TLK_EALIGN      0x0A //"align error"
#define TLK_EFAULT      0x0B //"fault error"
#define TLK_EHANDLE     0x0D //"invalid handle"
#define TLK_EFORMAT     0x0E //"format error"
#define TLK_EREPEAT     0x0F //"repeat operate"
#define TLK_ESTATUS     0x10 //"status error"
#define TLK_ETIMEOUT    0x11 //"operate timed out"
#define TLK_EINVALID    0x12 //"invalid argument or command"
#define TLK_EILLEGAL    0x13 //"illegal operate"
#define TLK_EOPERATE    0x14 //"operate error"
#define TLK_EOVERFLOW   0x15 //"buffer overflow"
#define TLK_EUNKNOWN    0x1F //"unknown error"
#define TLK_EINIT       0x21 //"Init Failure"
#define TLK_EDENY       0x25 //"access denied"
#define TLK_EROLE       0x26 //"Role Error"
#define TLK_ETYPE       0x27 //"file type or other type error"
#define TLK_EFULL       0x28 //"queue or other is full, cannot add new element"
#define TLK_EAGAIN      0x29 //"try again"
#define TLK_EEMPTY      0x2A //"empty"
#define TLK_ECHECK      0x2B //"checksum error"
#define TLK_EQUOTA      0x2C //"The quota is insufficient and more space needs to be allocated."
#define TLK_ELENGTH     0x2D //"length error or too long"
#define TLK_EVERSION    0x2E //"version not support"
#define TLK_EUNREACH    0x2F //"destination Unreachable"
#define TLK_ENODEV      0x41 //"no such device"
#define TLK_ENOMEM      0x42 //"not enough memory"
#define TLK_ENOITEM     0x43 //"not enough item"
#define TLK_ENODATA     0x44 //"no data"
#define TLK_ENOPERM     0x45 //"operation not permitted"
#define TLK_ENONAME     0x46 //"unknown node or service"
#define TLK_ENOREADY    0x47 //"not ready"
#define TLK_ENOSPACE    0x48 //"no space left on device"
#define TLK_ENOOBJECT   0x4A //"object is not exist"
#define TLK_ENOSUPPORT  0x4B //"not support"
#define TLK_EEXIST      0x62 //"file already exists"
#define TLK_EALREADY    0x66 //"handler already in progress"
#define TLK_EENCODE     0x67 //"encode error"
#define TLK_EDECODE     0x68 //"decode error"
#define TLK_EENCRYPT    0x69 //"encode error"
#define TLK_EDECRYPT    0x6A //"decode error"





#endif //TLKAPI_ERROR_H


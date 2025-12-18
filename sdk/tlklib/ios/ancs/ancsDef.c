/********************************************************************************************************
 * @file	ancsDef.c
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
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////Finding the others bundle id, link to : https://offcornerdev.com/bundleid.html////////////
//////////Finding the others bundle id, link to : https://offcornerdev.com/bundleid.html////////////
//////////Finding the others bundle id, link to : https://offcornerdev.com/bundleid.html////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ancsDef.h"

///////////////////////////////////////App Bundle Id///////////////////////////////////
/*******APP ID字符串,用于识别是哪个APP发过来的消息,更多APP ID可以在上面注释的网站中查找*******/
const u8 appId_call[] 			=
	{'c','o','m','.','a','p','p','l','e','.','m','o','b','i','l','e','p','h','o','n','e'};
const u8 appId_weChat[] 		=
	{'c','o','m','.','t','e','n','c','e','n','t','.','x','i','n'};
const u8 appId_SMS[]			=
	{'c','o','m','.','a','p','p','l','e','.','M','o','b','i','l','e','S','M','S'};
const u8 appId_qq[]				=
	{'c','o','m','.','t','e','n','c','e','n','t','.','m','q','q'};
const u8 appId_twitter[]		=
	{'c','o','m','.','a','t','e','b','i','t','s','.','T','w','e','e','t','i','e','2'};
const u8 appId_whatsapp[]		=
	{'n','e','t','.','w','h','a','t','s','a','p','p','.','W','h','a','t','s','A','p','p'};
const u8 appId_instagram[]		=
	{'c','o','m','.','b','u','r','b','n','.','i','n','s','t','a','g','r','a','m'};
const u8 appId_skype[]			=
	{'c','o','m','.','s','k','y','p','e','.','s','k','y','p','e'};
const u8 appId_facebook[]		=
	{'c','o','m','.','f','a','c','e','b','o','o','k','.','F','a','c','e','b','o','o','k'};
const u8 appId_line[]			=
	{'j','p','.','n','a','v','e','r','.','l','i','n','e'};

const u8 appId_ipad_qq[] = 
	{'c','o','m','.','t','e','n','c','e','n','t','.','m','i', 'p', 'a', 'd','q','q'};

	
const u8 appId_null[]			=///this array was the last one for ensure where is the end of appBundleId[]
	{'N','U','L','L'};
/**appBundleId里元素的位置,必须与ancsDef.h中APP ID枚举的排序一一对应,方便程序定位当前消息由哪个APP ID发来的**/
const u8 * appBundleId[] = {appId_call, appId_SMS, appId_weChat, appId_qq, appId_facebook, appId_twitter, appId_line, appId_whatsapp, appId_instagram, appId_ipad_qq, appId_null};

///////////////////////////////////Phone Call State////////////////////////////////////////
const u8 utf8_activeCall[] 		= {0x41, 0x63, 0x74, 0x69, 0x76, 0x65, 0x20, 0x43, 0x61, 0x6C, 0x6C};
const u8 utf8_incomingCall[] 	= {0x49, 0x6E, 0x63, 0x6F, 0x6D, 0x69, 0x6E, 0x67, 0x20, 0x43, 0x61, 0x6C, 0x6C};


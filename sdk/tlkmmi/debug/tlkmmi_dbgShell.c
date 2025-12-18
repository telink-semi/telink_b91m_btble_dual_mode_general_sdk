/********************************************************************************************************
 * @file	tlkmmi_dbgShell.c
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
#if (TLKMMI_DEBUG_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkdev/tlkdev.h"
#include "tlkmmi_dbg.h"
#include "tlkmmi_dbgShell.h"

#include "tlklib/usb/tlkusb_stdio.h"
#include "tlklib/usb/udb/tlkusb_udb.h"


#if (TLK_USB_UDB_ENABLE)
static void tlkmmi_debug_shell_input(uint08 *pData, uint16 dataLen);
#endif

int tlkmmi_debug_shellInit(void)
{
	#if (TLK_USB_UDB_ENABLE)
	tlkusb_udb_regDbgCB(tlkmmi_debug_shell_input);
	#endif

	return TLK_ENONE;
}


#if (TLK_USB_UDB_ENABLE)
static void tlkmmi_debug_shell_input(uint08 *pData, uint16 dataLen)
{
	tlkapi_array(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkmmi_dbgShell_input-usbHandler: receive", pData, dataLen);

	if(pData[0] != 0x11) return;

	switch(pData[1])
	{
//		case 0x01:
//			tlkdev_lcd_init();
//			break;
//		case 0x02:
//			tlkdev_lcd_open();
//			break;
//		case 0x03:
//			tlkdev_touch_init();
//			break;
//		case 0x05:
//			tlkdev_touch_open();
//			break;
//		case 0x06:
//			tlkdev_lcd_open();
//			tlkdev_touch_open();
//			break;

		case 0x10:
			break;
		case 0x11:
			break;

		case 0xf2:
			tlkdev_ioctl(TLKDEV_DID_SDCARD0, TLKDEV_OPCODE_STORE_FORMAT, 0, 0);;
			tlkapi_trace(TLKMMI_DBG_FLAG, TLKMMI_DBG_SIGN, "tlkdev_store_format");
			break;
	    case 0xfe:
	        break;

		case 0xFF:
		{

		}
			break;

	    default:
	        break;
	}

}
#endif

#endif //#if (TLKMMI_DEBUG_ENABLE)

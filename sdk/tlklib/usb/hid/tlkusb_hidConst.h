/********************************************************************************************************
 * @file	tlkusb_hidConst.h
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
#ifndef TLKUSB_HID_CONST_H
#define TLKUSB_HID_CONST_H

#if (TLK_USB_HID_ENABLE)


const uint08 cTlkUsbHidKeyboardDesc[] = {
    TLKUSB_HID_RPT_USAGE_PAGE			(8, 0x01),//Generic Desktop
	TLKUSB_HID_RPT_USAGE				(8, 0x06),//keyboard
	TLKUSB_HID_RPT_COLLECTION			(8, 0x01),//COLLECTION (Application)
		//Input report descriptor
		TLKUSB_HID_RPT_USAGE_PAGE		(8, 0x07),//usage page(Keyboard)
		TLKUSB_HID_RPT_USAGE_MINIMUM	(8, 0xE0),//local, min:0xe0
		TLKUSB_HID_RPT_USAGE_MAXIMUM	(8, 0xE7),//local, max 0xe7
		TLKUSB_HID_RPT_LOGICAL_MINIMUM	(8, 0x00),//global,min:0x00
		TLKUSB_HID_RPT_LOGICAL_MAXIMUM	(8, 0x01),//global,max:0x01
		TLKUSB_HID_RPT_REPORT_COUNT		(8, 0x08),//report count 8
		TLKUSB_HID_RPT_REPORT_SIZE		(8, 0x01),
		TLKUSB_HID_RPT_INPUT			(8, TLKUSB_HID_IOF_DATA | TLKUSB_HID_IOF_VARIABLE | TLKUSB_HID_IOF_ABSOLUTE),
		TLKUSB_HID_RPT_INPUT			(8, TLKUSB_HID_IOF_CONSTANT | TLKUSB_HID_IOF_VARIABLE),
		TLKUSB_HID_RPT_REPORT_COUNT		(8, 0x05),//report count 5
		//Output report descriptor,LED
		TLKUSB_HID_RPT_USAGE_PAGE		(8, 0x08),//usage page(LED)
		TLKUSB_HID_RPT_USAGE_MINIMUM	(8, 0x01),//local min:0x01
		TLKUSB_HID_RPT_USAGE_MAXIMUM	(8, 0x05),//local max:0x05
		TLKUSB_HID_RPT_OUTPUT			(8, TLKUSB_HID_IOF_DATA | TLKUSB_HID_IOF_VARIABLE | TLKUSB_HID_IOF_ABSOLUTE | TLKUSB_HID_IOF_NON_VOLATILE),
		TLKUSB_HID_RPT_REPORT_COUNT		(8, 0x01),//report count 1
		TLKUSB_HID_RPT_REPORT_SIZE		(8, 0x03),
		TLKUSB_HID_RPT_OUTPUT			(8, TLKUSB_HID_IOF_CONSTANT),
		TLKUSB_HID_RPT_REPORT_COUNT		(8, 0x06),//report count 6
		TLKUSB_HID_RPT_REPORT_SIZE		(8, 0x08),
		TLKUSB_HID_RPT_LOGICAL_MINIMUM	(8, 0x00),//global min:0x00
		0x26, 0xa4,	0x00,
		TLKUSB_HID_RPT_USAGE_PAGE		(8, 0x07),//usage page(keyboard)
		TLKUSB_HID_RPT_USAGE_MINIMUM	(8, 0x00),//local min:0x00
		0x2a, 0xa4, 0x00,
		TLKUSB_HID_RPT_INPUT			(8, TLKUSB_HID_IOF_DATA | TLKUSB_HID_IOF_ARRAY | TLKUSB_HID_IOF_ABSOLUTE),
	TLKUSB_HID_RPT_END_COLLECTION(0)//end collection
};

const uint08 cTlkUsbHidMouseDesc[] = {
	TLKUSB_HID_RPT_USAGE_PAGE(8, 0x01), //Generic Desktop
    TLKUSB_HID_RPT_USAGE(8, 0x02), //Mouse)
    TLKUSB_HID_RPT_COLLECTION(8, 0x01), //COLLECTION (Application)
	    TLKUSB_HID_RPT_REPORT_ID(8, TLKUSB_HID_REPORT_ID_MOUSE), //report ID
		TLKUSB_HID_RPT_USAGE(8, 0x01), //Pointer
	    TLKUSB_HID_RPT_COLLECTION(8, 0x00), //Physical
		    TLKUSB_HID_RPT_USAGE_PAGE(8, 0x09), //USAGE_PAGE (Button)
			// 1 is mouse left button,2 is mouse right button,3 is central button
		    TLKUSB_HID_RPT_USAGE_MINIMUM(8, 0x01),
		    TLKUSB_HID_RPT_USAGE_MAXIMUM(8, 0x05),
		    TLKUSB_HID_RPT_LOGICAL_MINIMUM(8, 0x00),
		    TLKUSB_HID_RPT_LOGICAL_MAXIMUM(8, 0x01),
			TLKUSB_HID_RPT_REPORT_SIZE(8, 0x01),
		    TLKUSB_HID_RPT_REPORT_COUNT(8, 0x05),
		    TLKUSB_HID_RPT_INPUT(8, TLKUSB_HID_IOF_DATA | TLKUSB_HID_IOF_VARIABLE | TLKUSB_HID_IOF_ABSOLUTE),
			TLKUSB_HID_RPT_REPORT_SIZE(8, 0x03), /* debug note: 5->3*/
		    TLKUSB_HID_RPT_REPORT_COUNT(8, 0x01),
		    TLKUSB_HID_RPT_INPUT(8, TLKUSB_HID_IOF_CONSTANT),
			
			//Generic Desktop Page(0x01)
		    TLKUSB_HID_RPT_USAGE_PAGE(8, 0x01),  //Generic Desktop 
		    TLKUSB_HID_RPT_USAGE(8, 0x30), //USAGE (X)
		    TLKUSB_HID_RPT_USAGE(8, 0x31), //USAGE (Y)
		    TLKUSB_HID_RPT_LOGICAL_MINIMUM(8, 0x81), //LOGICAL_MINIMUM (-127)
		    TLKUSB_HID_RPT_LOGICAL_MAXIMUM(8, 0x7f), //LOGICAL_MAXIMUM (127)
		    TLKUSB_HID_RPT_REPORT_SIZE(8, 0x10), //16bit
		    TLKUSB_HID_RPT_REPORT_COUNT(8, 0x02),
		    TLKUSB_HID_RPT_INPUT(8, TLKUSB_HID_IOF_DATA | TLKUSB_HID_IOF_VARIABLE | TLKUSB_HID_IOF_RELATIVE),
		    
		    TLKUSB_HID_RPT_USAGE(8, 0x38),  // Usage Wheel
		    TLKUSB_HID_RPT_LOGICAL_MINIMUM(8, 0x81),	 //LOGICAL_MINIMUM (-127)
		    TLKUSB_HID_RPT_LOGICAL_MAXIMUM(8, 0x7f),	 //LOGICAL_MAXIMUM (127)
		    TLKUSB_HID_RPT_REPORT_SIZE(8, 0x08),		 //REPORT_SIZE (8)
		    TLKUSB_HID_RPT_REPORT_COUNT(8, 0x01),		//REPORT_COUNT (1)
		    TLKUSB_HID_RPT_INPUT(8, TLKUSB_HID_IOF_DATA | TLKUSB_HID_IOF_VARIABLE | TLKUSB_HID_IOF_RELATIVE),
	    TLKUSB_HID_RPT_END_COLLECTION(0),
    TLKUSB_HID_RPT_END_COLLECTION(0),
        
    TLKUSB_HID_RPT_USAGE_PAGE(8, 0x0c),             //global, usage page (follow 1 bytes) consumer page
    TLKUSB_HID_RPT_USAGE(8, 0x01),                  //local,  usage ID 01  Consumer Control
    TLKUSB_HID_RPT_COLLECTION(8, 0x01),             //main, collection
	    TLKUSB_HID_RPT_REPORT_ID(8, TLKUSB_HID_REPORT_ID_KB_MEDIA),  //Report ID
	    TLKUSB_HID_RPT_REPORT_SIZE(8, 0x10),            //global, report size 16 bits
	    TLKUSB_HID_RPT_REPORT_COUNT(8, 0x02),           //global, report count 2
	    TLKUSB_HID_RPT_LOGICAL_MINIMUM(8, 0x01),        //global, min  0x01
	    TLKUSB_HID_RPT_LOGICAL_MAXIMUM(16, 0x029c),     //global, max  0x029c
	    TLKUSB_HID_RPT_USAGE_MINIMUM(8, 0x01),          //local, min   0x01
	    TLKUSB_HID_RPT_USAGE_MAXIMUM(16, 0x028c),       //local, max    0x28c
	    TLKUSB_HID_RPT_INPUT(8, TLKUSB_HID_IOF_ABSOLUTE), //main,  input data variable, absolute
    TLKUSB_HID_RPT_END_COLLECTION(0),               //main, end collection

	TLKUSB_HID_RPT_USAGE_PAGE(8, 0x01),             //global,  USAGE_PAGE 1 (Generic Desktop)
	TLKUSB_HID_RPT_USAGE(8, 0x80),                  //local, usage ID 0x80 system control
	TLKUSB_HID_RPT_COLLECTION(8, 0x01),             //main collection
	    TLKUSB_HID_RPT_REPORT_ID(8, TLKUSB_HID_REPORT_ID_KB_SYS),    //Report ID
		TLKUSB_HID_RPT_REPORT_SIZE(8, 0x02),            //global, report size 2
		TLKUSB_HID_RPT_REPORT_COUNT(8, 0x01),           //report count  1
		TLKUSB_HID_RPT_LOGICAL_MINIMUM(8, 0x01),        //global min 1
	    TLKUSB_HID_RPT_LOGICAL_MAXIMUM(8, 0x03),        //global, max 3
		TLKUSB_HID_RPT_USAGE(8, 0x81),					//local usage ID 0x81 system power down
	    TLKUSB_HID_RPT_USAGE(8, 0x82),                  //local usage ID 0x82 system sleep
	    TLKUSB_HID_RPT_USAGE(8, 0x83),                  //local usage ID 0x83 system wakeup
	    TLKUSB_HID_RPT_INPUT(8, TLKUSB_HID_IOF_ABSOLUTE|TLKUSB_HID_IOF_NO_PREFERRED_STATE|TLKUSB_HID_IOF_NULLSTATE),
	    TLKUSB_HID_RPT_REPORT_SIZE(8, 0x06),            //global report size 6
	    TLKUSB_HID_RPT_INPUT(8, TLKUSB_HID_IOF_CONSTANT|TLKUSB_HID_IOF_VARIABLE),
    TLKUSB_HID_RPT_END_COLLECTION(0),               //end of collection

	//need Jensen's help: report ID 5
    TLKUSB_HID_RPT_USAGE_PAGE(8, 0x01),     //global,  USAGE_PAGE 1 (Generic Desktop)
	TLKUSB_HID_RPT_USAGE(8, 0x00),     //usage undefined
	TLKUSB_HID_RPT_COLLECTION(8,0x01),     //main collection
		TLKUSB_HID_RPT_REPORT_ID(8, TLKUSB_HID_REPORT_ID_SOMATIC),     //global report ID 0x5
		TLKUSB_HID_RPT_USAGE_PAGE(16, 0xFF00),  //global usage page
		TLKUSB_HID_RPT_USAGE(8, 0x01),     //local,  usage ID 01  Consumer Control
		TLKUSB_HID_RPT_LOGICAL_MINIMUM(8, 0x81),     //global min 81
		TLKUSB_HID_RPT_LOGICAL_MINIMUM(8, 0x7f),     //global, max 7f
		TLKUSB_HID_RPT_REPORT_SIZE(8, 0x08),     //global, report size 8
		TLKUSB_HID_RPT_REPORT_COUNT(8, 0x07),     //report count  7
		0xb1, 0x02,     //feature (data, var, abs)
	TLKUSB_HID_RPT_END_COLLECTION(0),         //main, end collection
};







#endif //#if (TLK_USB_HID_ENABLE)

#endif //TLKUSB_HID_CONST_H


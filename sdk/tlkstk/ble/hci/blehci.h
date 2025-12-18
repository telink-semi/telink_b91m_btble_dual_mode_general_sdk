/********************************************************************************************************
 * @file	blehci.h
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
#pragma  once


#include "tlkstk/ble/ble_common.h"

typedef int (*blc_hci_rx_handler_t) (void);
typedef int (*blc_hci_tx_handler_t) (void);
typedef int (*blc_hci_handler_t) (unsigned char *p, int n);
typedef int (*blc_hci_app_handler_t) (unsigned char *p);





#define			HCI_MAX_ACL_DATA_LEN              			27

#define 		HCI_MAX_DATA_BUFFERS_SALVE              	8
#define 		HCI_MAX_DATA_BUFFERS_MASTER              	8





/**
 *  @brief  Definition for HCI ACL Data packets Packet_Boundary_Flag
 */
typedef enum{
	HCI_FIRST_NAF_PACKET          =		0x00,	//LE Host to Controller
	HCI_CONTINUING_PACKET         =		0x01,	//LE Host to Controller / Controller to Host
	HCI_FIRST_AF_PACKET           =    	0x02,	//LE 					  Controller to Host
} acl_pb_flag_t;




/**
 *  @brief  Definition for HCI ISO Data packets PB_Flag
 */
typedef enum{
	HCI_ISO_SDU_FIRST_FRAG		=	0x00,	//The ISO_Data_Load field contains the first fragment of a fragmented SDU
	HCI_ISO_SDU_CONTINUE_FRAG	=	0x01,	//The ISO_Data_Load field contains a continuation fragment of an SDU
	HCI_ISO_SDU_COMPLETE		=	0x02,	//The ISO_Data_Load field contains a complete SDU
	HCI_ISO_SDU_LAST_FRAG		=	0x03,	//The ISO_Data_Load field contains the last fragment of an SDU.
} iso_pb_flag_t;



/**
 *  @brief  Definition for HCI ISO Data packets Packet_Status_Flag
 */
typedef enum{
	HCI_ISO_VALID_DATA				=	0x00, //Valid data. The complete ISO_SDU was received correctly
	HCI_ISO_POSSIBLE_VALID_DATA		=	0x01, //Possibly invalid data
	HCI_ISO_LOST_DATA				=	0x02, //Part(s) of the ISO_SDU were not received correctly. This is reported as "lost data"
} iso_ps_flag_t;








// hci event
extern u32		hci_eventMask;
extern u32		hci_le_eventMask;
extern u32		hci_le_eventMask_2;



// Controller data handler
typedef int (*hci_data_handler_t) (u16 conn, u8 * p);
extern hci_data_handler_t		blc_hci_data_handler;









/******************************* Stack Interface Begin, user can not use!!! ********************************************/
//int blc_acl_from_btusb ();


int blc_hci_rx_from_usb (void);


int blc_hci_handler (u8 *p, int n);


int blc_hci_proc (void);

/******************************* Stack Interface End *******************************************************************/







/******************************* User Interface  Begin *****************************************************************/
ble_sts_t	blc_hci_setEventMask_cmd(u32 evtMask);      //eventMask: BT/EDR
ble_sts_t	blc_hci_le_setEventMask_cmd(u32 evtMask);   //eventMask: LE event  0~31
ble_sts_t 	blc_hci_le_setEventMask_2_cmd(u32 evtMask_2);   //eventMask: LE event 32~63

void    	blc_hci_registerControllerDataHandler (hci_data_handler_t  handler);



void 		blc_register_hci_handler (void *prx, void *ptx);

int 		blc_hci_sendACLData2Host (u16 handle, u8 *p);

int         blc_hci_send_data (u32 h, u8 *para, u16 n);

int 		blc_hci_iso_send_data (u32 h, u8 *iso_load, int data_load_len);

ble_sts_t   blc_ll_initHciAclDataFifo(u8 *pAclbuf, int fifo_size, int fifo_number);
/******************************* User Interface  End  ******************************************************************/

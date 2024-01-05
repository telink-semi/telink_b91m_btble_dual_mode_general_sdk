/********************************************************************************************************
 * @file	btc_hci.h
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
/**
 ****************************************************************************************
 *
 * @file co_hci.h
 *
 * @brief This file contains the HCI Bluetooth defines, enumerations and structures
 *        definitions for use by all modules in RW stack.
 *
 * Copyright (C) OpenC 2009-2015
 *
 ****************************************************************************************
 */

#ifndef BTC_HCI_H_
#define BTC_HCI_H_

/**
 ****************************************************************************************
 * @addtogroup COMMON Common SW Block
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>       // standard boolean definitions
#include <stddef.h>        // standard definitions
#include <stdint.h>        // standard integer definitions


/*
 * DEFINES
 ****************************************************************************************
 */


/******************************************************************************************/
/* -------------------------   H4TL DEFINITIONS Part IV.A    -----------------------------*/
/******************************************************************************************/

///HCI Transport Header length - change if different transport
#define HCI_TRANSPORT_HDR_LEN                       0x01

///UART header: command message type
#define HCI_CMD_MSG_TYPE                            0x01

///UART header: ACL data message type
#define HCI_ACL_MSG_TYPE                            0x02

///UART header: Synchronous data message type
#define HCI_SYNC_MSG_TYPE                           0x03

///UART header: event message type
#define HCI_EVT_MSG_TYPE                            0x04

/******************************************************************************************/
/* -------------------------   HCI DEFINITIONS Part II.E     -----------------------------*/
/******************************************************************************************/

///HCI Command Opcode byte length
#define HCI_CMD_OPCODE_LEN         (0x02)

///HCI Event code byte length
#define HCI_EVT_CODE_LEN           (0x01)

///HCI Command/Event parameter length field byte length
#define HCI_CMDEVT_PARLEN_LEN      (0x01)

///HCI Command header length
#define HCI_CMD_HDR_LEN            (HCI_CMD_OPCODE_LEN + HCI_CMDEVT_PARLEN_LEN)

///HCI Event header length
#define HCI_EVT_HDR_LEN            (HCI_EVT_CODE_LEN + HCI_CMDEVT_PARLEN_LEN)

/// HCI ACL header: handle and flags decoding
#define HCI_ACL_HDR_HDL_FLAGS_POS  (0)
#define HCI_ACL_HDR_HDL_FLAGS_LEN  (2)
#define HCI_ACL_HDR_HDL_POS        (0)
#define HCI_ACL_HDR_HDL_MASK       (0x0FFF)
#define HCI_ACL_HDR_PB_FLAG_POS    (12)
#define HCI_ACL_HDR_PB_FLAG_MASK   (0x3000)
#define HCI_ACL_HDR_BC_FLAG_POS    (14)
#define HCI_ACL_HDR_BC_FLAG_MASK   (0xC000)
#define HCI_ACL_HDR_DATA_FLAG_POS  (12)
#define HCI_ACL_HDR_DATA_FLAG_MASK (0xF000)

/// HCI ACL header: data length field length
#define HCI_ACL_HDR_DATA_LEN_POS   (HCI_ACL_HDR_HDL_FLAGS_LEN)
#define HCI_ACL_HDR_DATA_LEN_LEN   (2)

///HCI ACL data packet header length
#define HCI_ACL_HDR_LEN            (HCI_ACL_HDR_HDL_FLAGS_LEN + HCI_ACL_HDR_DATA_LEN_LEN)

///HCI sync data packet header length

/// HCI Synchronous header: handle and flags decoding
#define HCI_SYNC_HDR_HDL_FLAGS_POS  (0)
#define HCI_SYNC_HDR_HDL_FLAGS_LEN  (2)
#define HCI_SYNC_HDR_HDL_POS        (0)
#define HCI_SYNC_HDR_HDL_MASK       (0x0FFF)
#define HCI_SYNC_HDR_PSF_FLAG_POS   (12)
#define HCI_SYNC_HDR_PSF_FLAG_MASK  (0x3000)
#define HCI_SYNC_HDR_RES_FLAG_POS   (14)
#define HCI_SYNC_HDR_RES_FLAG_MASK  (0xC000)
#define HCI_SYNC_HDR_DATA_FLAG_POS  (12)
#define HCI_SYNC_HDR_DATA_FLAG_MASK (0xF000)

/// HCI Synchronous header: data length field length
#define HCI_SYNC_HDR_DATA_LEN_POS   (HCI_SYNC_HDR_HDL_FLAGS_LEN)
#define HCI_SYNC_HDR_DATA_LEN_LEN   (1)
#define HCI_SYNC_MAX_DATA_SIZE      (255)

///HCI sync data packet header length
#define HCI_SYNC_HDR_LEN           (HCI_SYNC_HDR_HDL_FLAGS_LEN + HCI_SYNC_HDR_DATA_LEN_LEN)


///HCI Command Complete Event minimum parameter length: 1(nb_pk)+2(opcode)
#define HCI_CCEVT_HDR_PARLEN       (0x03)
#define HCI_CCEVT_LETSTEND_RETPAR_LEN    0x03
///HCI Command Complete Event header length:1(code)+1(len)+1(pk)+2(opcode)
#define HCI_CCEVT_HDR_LEN          (HCI_EVT_HDR_LEN + HCI_CCEVT_HDR_PARLEN)

///HCI Basic Command Complete Event packet length
#define HCI_CCEVT_BASIC_LEN        (HCI_CCEVT_HDR_LEN + 1)

///HCI Command Status Event parameter length - constant
#define HCI_CSEVT_PARLEN           (0x04)

///HCI Command Status Event length:1(code)+1(len)+1(st)+1(pk)+2(opcode)
#define HCI_CSEVT_LEN              (HCI_EVT_HDR_LEN + HCI_CSEVT_PARLEN)

///HCI Reset Command parameter length
#define HCI_RESET_CMD_PARLEN       0

/// Default return parameter length for HCI Command Complete Event
#define HCI_CCEVT_BASIC_RETPAR_LEN 1

/// Max HCI commands param size
#define HCI_MAX_CMD_PARAM_SIZE    255

/// Macro to extract OCF from OPCODE
#define HCI_OP2OCF(opcode)        ((opcode) & 0x03FF)

/// Macro to extract OGF from OPCODE
#define HCI_OP2OGF(opcode)        ((opcode) >> 10 & 0x003F)

/// Macro to create OPCODE from OGF and OCF
#define HCI_OPCODE(ocf, ogf)      (((ogf) << 10) | ocf)

/**************************************************************************************
 **************                       HCI COMMANDS                     ****************
 **************************************************************************************/


///Commands Opcodes: OGF(6b) | OCF(10b)
/* Some Abbreviation used in names:
 *  - LK   = Link Key
 *  - RD   = Read
 *  - WR   = Write
 *  - REM  = Remote
 *  - STG  = Settings
 *  - CON  = Connection
 *  - CHG  = Change
 *  - DFT  = Default
 *  - PER  = Periodic
 */

#define HCI_NO_OPERATION_CMD_OPCODE                 0x0000

//Link Control Commands
#define HCI_INQ_CMD_OPCODE                          0x0401
#define HCI_INQ_CANCEL_CMD_OPCODE                   0x0402
#define HCI_PER_INQ_MODE_CMD_OPCODE                 0x0403
#define HCI_EXIT_PER_INQ_MODE_CMD_OPCODE            0x0404
#define HCI_CREATE_CON_CMD_OPCODE                   0x0405
#define HCI_DISCONNECT_CMD_OPCODE                   0x0406
#define HCI_ADD_SYNC_CON_CMD_OPCODE                 0x0407
#define HCI_CREATE_CON_CANCEL_CMD_OPCODE            0x0408
#define HCI_ACCEPT_CON_REQ_CMD_OPCODE               0x0409
#define HCI_REJECT_CON_REQ_CMD_OPCODE               0x040A
#define HCI_LK_REQ_REPLY_CMD_OPCODE                 0x040B
#define HCI_LK_REQ_NEG_REPLY_CMD_OPCODE             0x040C
#define HCI_PIN_CODE_REQ_REPLY_CMD_OPCODE           0x040D
#define HCI_PIN_CODE_REQ_NEG_REPLY_CMD_OPCODE       0x040E
#define HCI_CHG_CON_PKT_TYPE_CMD_OPCODE             0x040F
#define HCI_AUTH_REQ_CMD_OPCODE                     0x0411
#define HCI_SET_CON_ENC_CMD_OPCODE                  0x0413
#define HCI_CHG_CON_LK_CMD_OPCODE                   0x0415
#define HCI_MASTER_LK_CMD_OPCODE                    0x0417
#define HCI_REM_NAME_REQ_CMD_OPCODE                 0x0419
#define HCI_REM_NAME_REQ_CANCEL_CMD_OPCODE          0x041A
#define HCI_RD_REM_SUPP_FEATS_CMD_OPCODE            0x041B
#define HCI_RD_REM_EXT_FEATS_CMD_OPCODE             0x041C
#define HCI_RD_REM_VER_INFO_CMD_OPCODE              0x041D
#define HCI_RD_CLK_OFF_CMD_OPCODE                   0x041F
#define HCI_RD_LMP_HDL_CMD_OPCODE                   0x0420
#define HCI_SETUP_SYNC_CON_CMD_OPCODE               0x0428
#define HCI_ACCEPT_SYNC_CON_REQ_CMD_OPCODE          0x0429
#define HCI_REJECT_SYNC_CON_REQ_CMD_OPCODE          0x042A
#define HCI_IO_CAP_REQ_REPLY_CMD_OPCODE             0x042B
#define HCI_USER_CFM_REQ_REPLY_CMD_OPCODE           0x042C
#define HCI_USER_CFM_REQ_NEG_REPLY_CMD_OPCODE       0x042D
#define HCI_USER_PASSKEY_REQ_REPLY_CMD_OPCODE       0x042E
#define HCI_USER_PASSKEY_REQ_NEG_REPLY_CMD_OPCODE   0x042F
#define HCI_REM_OOB_DATA_REQ_REPLY_CMD_OPCODE       0x0430
#define HCI_REM_OOB_DATA_REQ_NEG_REPLY_CMD_OPCODE   0x0433
#define HCI_IO_CAP_REQ_NEG_REPLY_CMD_OPCODE         0x0434
#define HCI_ENH_SETUP_SYNC_CON_CMD_OPCODE           0x043D
#define HCI_ENH_ACCEPT_SYNC_CON_CMD_OPCODE          0x043E

//Link Policy Commands
#define HCI_HOLD_MODE_CMD_OPCODE                    0x0801
#define HCI_SNIFF_MODE_CMD_OPCODE                   0x0803
#define HCI_EXIT_SNIFF_MODE_CMD_OPCODE              0x0804
#define HCI_PARK_STATE_CMD_OPCODE                   0x0805
#define HCI_EXIT_PARK_STATE_CMD_OPCODE              0x0806
#define HCI_QOS_SETUP_CMD_OPCODE                    0x0807
#define HCI_ROLE_DISCOVERY_CMD_OPCODE               0x0809
#define HCI_SWITCH_ROLE_CMD_OPCODE                  0x080B
#define HCI_RD_LINK_POL_STG_CMD_OPCODE              0x080C
#define HCI_WR_LINK_POL_STG_CMD_OPCODE              0x080D
#define HCI_RD_DFT_LINK_POL_STG_CMD_OPCODE          0x080E
#define HCI_WR_DFT_LINK_POL_STG_CMD_OPCODE          0x080F
#define HCI_FLOW_SPEC_CMD_OPCODE                    0x0810
#define HCI_SNIFF_SUB_CMD_OPCODE                    0x0811

//Controller and Baseband Commands
#define HCI_SET_EVT_MASK_CMD_OPCODE                 0x0C01
#define HCI_RESET_CMD_OPCODE                        0x0C03
#define HCI_SET_EVT_FILTER_CMD_OPCODE               0x0C05
#define HCI_FLUSH_CMD_OPCODE                        0x0C08
#define HCI_RD_PIN_TYPE_CMD_OPCODE                  0x0C09
#define HCI_WR_PIN_TYPE_CMD_OPCODE                  0x0C0A
#define HCI_CREATE_NEW_UNIT_KEY_CMD_OPCODE          0x0C0B
#define HCI_RD_STORED_LK_CMD_OPCODE                 0x0C0D
#define HCI_WR_STORED_LK_CMD_OPCODE                 0x0C11
#define HCI_DEL_STORED_LK_CMD_OPCODE                0x0C12
#define HCI_WR_LOCAL_NAME_CMD_OPCODE                0x0C13
#define HCI_RD_LOCAL_NAME_CMD_OPCODE                0x0C14
#define HCI_RD_CON_ACCEPT_TO_CMD_OPCODE             0x0C15
#define HCI_WR_CON_ACCEPT_TO_CMD_OPCODE             0x0C16
#define HCI_RD_PAGE_TO_CMD_OPCODE                   0x0C17
#define HCI_WR_PAGE_TO_CMD_OPCODE                   0x0C18
#define HCI_RD_SCAN_EN_CMD_OPCODE                   0x0C19
#define HCI_WR_SCAN_EN_CMD_OPCODE                   0x0C1A
#define HCI_RD_PAGE_SCAN_ACT_CMD_OPCODE             0x0C1B
#define HCI_WR_PAGE_SCAN_ACT_CMD_OPCODE             0x0C1C
#define HCI_RD_INQ_SCAN_ACT_CMD_OPCODE              0x0C1D
#define HCI_WR_INQ_SCAN_ACT_CMD_OPCODE              0x0C1E
#define HCI_RD_AUTH_EN_CMD_OPCODE                   0x0C1F
#define HCI_WR_AUTH_EN_CMD_OPCODE                   0x0C20
#define HCI_RD_CLASS_OF_DEV_CMD_OPCODE              0x0C23
#define HCI_WR_CLASS_OF_DEV_CMD_OPCODE              0x0C24
#define HCI_RD_VOICE_STG_CMD_OPCODE                 0x0C25
#define HCI_WR_VOICE_STG_CMD_OPCODE                 0x0C26
#define HCI_RD_AUTO_FLUSH_TO_CMD_OPCODE             0x0C27
#define HCI_WR_AUTO_FLUSH_TO_CMD_OPCODE             0x0C28
#define HCI_RD_NB_BDCST_RETX_CMD_OPCODE             0x0C29
#define HCI_WR_NB_BDCST_RETX_CMD_OPCODE             0x0C2A
#define HCI_RD_HOLD_MODE_ACTIVITY_CMD_OPCODE        0x0C2B
#define HCI_WR_HOLD_MODE_ACTIVITY_CMD_OPCODE        0x0C2C
#define HCI_RD_TX_PWR_LVL_CMD_OPCODE                0x0C2D
#define HCI_RD_SYNC_FLOW_CNTL_EN_CMD_OPCODE         0x0C2E
#define HCI_WR_SYNC_FLOW_CNTL_EN_CMD_OPCODE         0x0C2F
#define HCI_SET_CTRL_TO_HOST_FLOW_CTRL_CMD_OPCODE   0x0C31
#define HCI_HOST_BUF_SIZE_CMD_OPCODE                0x0C33
#define HCI_HOST_NB_CMP_PKTS_CMD_OPCODE             0x0C35
#define HCI_RD_LINK_SUPV_TO_CMD_OPCODE              0x0C36
#define HCI_WR_LINK_SUPV_TO_CMD_OPCODE              0x0C37
#define HCI_RD_NB_SUPP_IAC_CMD_OPCODE               0x0C38
#define HCI_RD_CURR_IAC_LAP_CMD_OPCODE              0x0C39
#define HCI_WR_CURR_IAC_LAP_CMD_OPCODE              0x0C3A
#define HCI_SET_AFH_HOST_CH_CLASS_CMD_OPCODE        0x0C3F
#define HCI_RD_INQ_SCAN_TYPE_CMD_OPCODE             0x0C42
#define HCI_WR_INQ_SCAN_TYPE_CMD_OPCODE             0x0C43
#define HCI_RD_INQ_MODE_CMD_OPCODE                  0x0C44
#define HCI_WR_INQ_MODE_CMD_OPCODE                  0x0C45
#define HCI_RD_PAGE_SCAN_TYPE_CMD_OPCODE            0x0C46
#define HCI_WR_PAGE_SCAN_TYPE_CMD_OPCODE            0x0C47
#define HCI_RD_AFH_CH_ASSESS_MODE_CMD_OPCODE        0x0C48
#define HCI_WR_AFH_CH_ASSESS_MODE_CMD_OPCODE        0x0C49
#define HCI_RD_EXT_INQ_RSP_CMD_OPCODE               0x0C51
#define HCI_WR_EXT_INQ_RSP_CMD_OPCODE               0x0C52
#define HCI_REFRESH_ENC_KEY_CMD_OPCODE              0x0C53
#define HCI_RD_SP_MODE_CMD_OPCODE                   0x0C55
#define HCI_WR_SP_MODE_CMD_OPCODE                   0x0C56
#define HCI_RD_LOC_OOB_DATA_CMD_OPCODE              0x0C57
#define HCI_RD_INQ_RSP_TX_PWR_LVL_CMD_OPCODE        0x0C58
#define HCI_WR_INQ_TX_PWR_LVL_CMD_OPCODE            0x0C59
#define HCI_RD_DFT_ERR_DATA_REP_CMD_OPCODE          0x0C5A
#define HCI_WR_DFT_ERR_DATA_REP_CMD_OPCODE          0x0C5B
#define HCI_ENH_FLUSH_CMD_OPCODE                    0x0C5F
#define HCI_SEND_KEYPRESS_NOTIF_CMD_OPCODE          0x0C60
#define HCI_SET_EVT_MASK_PAGE_2_CMD_OPCODE          0x0C63
#define HCI_RD_FLOW_CNTL_MODE_CMD_OPCODE            0x0C66
#define HCI_WR_FLOW_CNTL_MODE_CMD_OPCODE            0x0C67
#define HCI_RD_ENH_TX_PWR_LVL_CMD_OPCODE            0x0C68
#define HCI_RD_LE_HOST_SUPP_CMD_OPCODE              0x0C6C
#define HCI_WR_LE_HOST_SUPP_CMD_OPCODE              0x0C6D
#define HCI_RD_SECURE_CONN_HOST_SUPP_CMD_OPCODE     0x0C79
#define HCI_WR_SECURE_CONN_HOST_SUPP_CMD_OPCODE     0x0C7A
#define HCI_RD_AUTH_PAYL_TO_CMD_OPCODE              0x0C7B
#define HCI_WR_AUTH_PAYL_TO_CMD_OPCODE              0x0C7C
#define HCI_RD_EXT_PAGE_TO_CMD_OPCODE               0x0C7E
#define HCI_WR_EXT_PAGE_TO_CMD_OPCODE               0x0C7F
#define HCI_RD_EXT_INQ_LEN_CMD_OPCODE               0x0C80
#define HCI_WR_EXT_INQ_LEN_CMD_OPCODE               0x0C81

//Info Params
#define HCI_RD_LOCAL_VER_INFO_CMD_OPCODE            0x1001
#define HCI_RD_LOCAL_SUPP_CMDS_CMD_OPCODE           0x1002
#define HCI_RD_LOCAL_SUPP_FEATS_CMD_OPCODE          0x1003
#define HCI_RD_LOCAL_EXT_FEATS_CMD_OPCODE           0x1004
#define HCI_RD_BUFF_SIZE_CMD_OPCODE                 0x1005
#define HCI_RD_BD_ADDR_CMD_OPCODE                   0x1009
#define HCI_RD_LOCAL_SUPP_CODECS_CMD_OPCODE         0x100B

//Status Params
#define HCI_RD_FAIL_CONTACT_CNT_CMD_OPCODE          0x1401
#define HCI_RST_FAIL_CONTACT_CNT_CMD_OPCODE         0x1402
#define HCI_RD_LINK_QUAL_CMD_OPCODE                 0x1403
#define HCI_RD_RSSI_CMD_OPCODE                      0x1405
#define HCI_RD_AFH_CH_MAP_CMD_OPCODE                0x1406
#define HCI_RD_CLK_CMD_OPCODE                       0x1407
#define HCI_RD_ENC_KEY_SIZE_CMD_OPCODE              0x1408

//Testing Commands
#define HCI_RD_LOOPBACK_MODE_CMD_OPCODE             0x1801
#define HCI_WR_LOOPBACK_MODE_CMD_OPCODE             0x1802
#define HCI_EN_DUT_MODE_CMD_OPCODE                  0x1803
#define HCI_WR_SP_DBG_MODE_CMD_OPCODE               0x1804

/// LE Commands Opcodes
#define HCI_LE_SET_EVT_MASK_CMD_OPCODE              0x2001
#define HCI_LE_RD_BUFF_SIZE_CMD_OPCODE              0x2002
#define HCI_LE_RD_LOCAL_SUPP_FEATS_CMD_OPCODE       0x2003
#define HCI_LE_SET_RAND_ADDR_CMD_OPCODE             0x2005
#define HCI_LE_SET_ADV_PARAM_CMD_OPCODE             0x2006
#define HCI_LE_RD_ADV_CHNL_TX_PW_CMD_OPCODE         0x2007
#define HCI_LE_SET_ADV_DATA_CMD_OPCODE              0x2008
#define HCI_LE_SET_SCAN_RSP_DATA_CMD_OPCODE         0x2009
#define HCI_LE_SET_ADV_EN_CMD_OPCODE                0x200A
#define HCI_LE_SET_SCAN_PARAM_CMD_OPCODE            0x200B
#define HCI_LE_SET_SCAN_EN_CMD_OPCODE               0x200C
#define HCI_LE_CREATE_CON_CMD_OPCODE                0x200D
#define HCI_LE_CREATE_CON_CANCEL_CMD_OPCODE         0x200E
#define HCI_LE_RD_WLST_SIZE_CMD_OPCODE              0x200F
#define HCI_LE_CLEAR_WLST_CMD_OPCODE                0x2010
#define HCI_LE_ADD_DEV_TO_WLST_CMD_OPCODE           0x2011
#define HCI_LE_RMV_DEV_FROM_WLST_CMD_OPCODE         0x2012
#define HCI_LE_CON_UPDATE_CMD_OPCODE                0x2013
#define HCI_LE_SET_HOST_CH_CLASS_CMD_OPCODE         0x2014
#define HCI_LE_RD_CHNL_MAP_CMD_OPCODE               0x2015
#define HCI_LE_RD_REM_USED_FEATS_CMD_OPCODE         0x2016
#define HCI_LE_ENC_CMD_OPCODE                       0x2017
#define HCI_LE_RAND_CMD_OPCODE                      0x2018
#define HCI_LE_START_ENC_CMD_OPCODE                 0x2019
#define HCI_LE_LTK_REQ_REPLY_CMD_OPCODE             0x201A
#define HCI_LE_LTK_REQ_NEG_REPLY_CMD_OPCODE         0x201B
#define HCI_LE_RD_SUPP_STATES_CMD_OPCODE            0x201C
#define HCI_LE_RX_TEST_CMD_OPCODE                   0x201D
#define HCI_LE_TX_TEST_CMD_OPCODE                   0x201E
#define HCI_LE_TEST_END_CMD_OPCODE                  0x201F
#define HCI_LE_REM_CON_PARAM_REQ_REPLY_CMD_OPCODE   0x2020
#define HCI_LE_REM_CON_PARAM_REQ_NEG_REPLY_CMD_OPCODE 0x2021

///Debug commands - OGF = 0x3F (spec)
#define HCI_TDB_SET_BT_BD_ADDR_CMD_OPCODE             0xFC01
#define HCI_SET_ESCO_MUTE_CMD_OPCODE                  0xFC02
#define HCI_ADJUST_ACL_LINK_PRIO_OPCODE				  0xFC03


/**************************************************************************************
 **************                        HCI EVENTS                      ****************
 **************************************************************************************/

///Event Codes
#define HCI_INQ_CMP_EVT_CODE                       0x01
#define HCI_INQ_RES_EVT_CODE                       0x02
#define HCI_CON_CMP_EVT_CODE                       0x03
#define HCI_CON_REQ_EVT_CODE                       0x04
#define HCI_DISC_CMP_EVT_CODE                      0x05
#define HCI_AUTH_CMP_EVT_CODE                      0x06
#define HCI_REM_NAME_REQ_CMP_EVT_CODE              0x07
#define HCI_ENC_CHG_EVT_CODE                       0x08
#define HCI_CHG_CON_LK_CMP_EVT_CODE                0x09
#define HCI_MASTER_LK_CMP_EVT_CODE                 0x0A
#define HCI_RD_REM_SUPP_FEATS_CMP_EVT_CODE         0x0B
#define HCI_RD_REM_VER_INFO_CMP_EVT_CODE           0x0C
#define HCI_QOS_SETUP_CMP_EVT_CODE                 0x0D
#define HCI_CMD_CMP_EVT_CODE                       0x0E
#define HCI_CMD_STATUS_EVT_CODE                    0x0F
#define HCI_HW_ERR_EVT_CODE                        0x10
#define HCI_FLUSH_OCCURRED_EVT_CODE                0x11
#define HCI_ROLE_CHG_EVT_CODE                      0x12
#define HCI_NB_CMP_PKTS_EVT_CODE                   0x13
#define HCI_MODE_CHG_EVT_CODE                      0x14
#define HCI_RETURN_LINK_KEYS_EVT_CODE              0x15
#define HCI_PIN_CODE_REQ_EVT_CODE                  0x16
#define HCI_LK_REQ_EVT_CODE                        0x17
#define HCI_LK_NOTIF_EVT_CODE                      0x18
#define HCI_DATA_BUF_OVFLW_EVT_CODE                0x1A
#define HCI_MAX_SLOT_CHG_EVT_CODE                  0x1B
#define HCI_RD_CLK_OFF_CMP_EVT_CODE                0x1C
#define HCI_CON_PKT_TYPE_CHG_EVT_CODE              0x1D
#define HCI_QOS_VIOL_EVT_CODE                      0x1E
#define HCI_PAGE_SCAN_REPEAT_MODE_CHG_EVT_CODE     0x20
#define HCI_FLOW_SPEC_CMP_EVT_CODE                 0x21
#define HCI_INQ_RES_WITH_RSSI_EVT_CODE             0x22
#define HCI_RD_REM_EXT_FEATS_CMP_EVT_CODE          0x23
#define HCI_SYNC_CON_CMP_EVT_CODE                  0x2C
#define HCI_SYNC_CON_CHG_EVT_CODE                  0x2D
#define HCI_SNIFF_SUB_EVT_CODE                     0x2E
#define HCI_EXT_INQ_RES_EVT_CODE                   0x2F
#define HCI_ENC_KEY_REFRESH_CMP_EVT_CODE           0x30
#define HCI_IO_CAP_REQ_EVT_CODE                    0x31
#define HCI_IO_CAP_RSP_EVT_CODE                    0x32
#define HCI_USER_CFM_REQ_EVT_CODE                  0x33
#define HCI_USER_PASSKEY_REQ_EVT_CODE              0x34
#define HCI_REM_OOB_DATA_REQ_EVT_CODE              0x35
#define HCI_SP_CMP_EVT_CODE                        0x36
#define HCI_LINK_SUPV_TO_CHG_EVT_CODE              0x38
#define HCI_ENH_FLUSH_CMP_EVT_CODE                 0x39
#define HCI_USER_PASSKEY_NOTIF_EVT_CODE            0x3B
#define HCI_KEYPRESS_NOTIF_EVT_CODE                0x3C
#define HCI_REM_HOST_SUPP_FEATS_NOTIF_EVT_CODE     0x3D
#define HCI_LE_META_EVT_CODE                       0x3E
#define HCI_MAX_EVT_MSK_PAGE_1_CODE                0x40
#define HCI_AUTH_PAYL_TO_EXP_EVT_CODE              0x57
#define HCI_MAX_EVT_MSK_PAGE_2_CODE                0x58
#define HCI_DBG_EVT_CODE                           0xFF
///Debug event subcodes - same as command OCF
#define HCI_DBG_TRACE_WARNING_EVT_CODE             0x01
#define HCI_DBG_EMUL_FINISHED_EVT_CODE             0x2D
/// LE Events Subcodes
#define HCI_LE_CON_CMP_EVT_SUBCODE                  0x01
#define HCI_LE_ADV_REPORT_EVT_SUBCODE               0x02
#define HCI_LE_CON_UPDATE_CMP_EVT_SUBCODE           0x03
#define HCI_LE_RD_REM_USED_FEATS_CMP_EVT_SUBCODE    0x04
#define HCI_LE_LTK_REQUEST_EVT_SUBCODE              0x05
#define HCI_LE_REM_CON_PARAM_REQ_EVT_SUBCODE        0x06

/// Event mask page enum
enum hci_evt_mask_page
{
    /// page 0
    HCI_PAGE_0,
    /// page 1
    HCI_PAGE_1,
    /// page 2
    HCI_PAGE_2,
    /// Default
    HCI_PAGE_DFT
};


#endif // BTC_HCI_H_

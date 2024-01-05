/********************************************************************************************************
 * @file	btc_hci_evt.h
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
#ifndef BTC_HCI_EVT_H
#define BTC_HCI_EVT_H



bool btc_hci_evtMaskIsSet(uint32_t h);
void btc_hci_setEventMaskBit(uint8_t evt_code, bool bit_v);
void btc_hci_setEventMask(uint8_t *event_mask, uint8_t page, uint8_t *pStatus);
int  btc_hci_sendEvent(uint8_t evtID, uint8_t *pHead,
						   uint16_t headLen, uint8_t *pBody, uint16_t bodyLen);
int btc_hci_sendHardwareErrorEvt(uint8_t error_code);
int btc_hci_sendEnhancedFlushCompleteEvt(uint16_t con_hdl);
int btc_hci_sendInquiryCompleteEvt(uint8_t status);
int btc_hci_sendReadClockOffsetCompleteEvt(uint8_t status, uint16_t con_hdl,
        									   uint8_t *p,     uint8_t len);
int btc_hci_sendReadRemoteVersionInfoCompleteEvt(uint8_t status, uint16_t con_hdl,
        										     uint8_t *p, uint8_t len);
int btc_hci_sendConnectCompleteEvt(uint8_t status, uint16_t con_hdl, uint8_t *bd_addr,
									   uint8_t link_type, uint8_t encryption_enabled);
int btc_hci_sendConnectPacketTypeChangedEvt(uint8_t status, uint16_t con_hdl,uint16_t Packet_Type );
int btc_hci_sendNumOfCompleteEvt(uint8_t num_handle, uint16_t con_hdl,uint16_t num_complete_pkt);
int btc_hci_sendFlushOccurredEvt(uint16_t con_hdl);
int btc_hci_sendCommandCompleteEvt(uint8_t num_hci_cmd, uint8_t* op_code,
									   uint8_t *ret_para,   uint8_t ret_para_len);
int btc_hci_sendQosSetupCmopleteEvt(uint8_t status, uint16_t con_hdl,
										uint8_t srv_type, uint32_t tkn_rate, uint32_t pk_bw, uint32_t latency, uint32_t dly_var);
int btc_hci_sendAclEstablishEvt(uint8_t status, uint16_t con_hdl, uint8_t *bd_addr, uint8_t role, uint8_t *remote_cod);
int btc_hci_sendMaxSlotsChangeEvt(uint16_t con_hdl,uint8_t max_slots);
int btc_hci_sendConnectRequestEvt(uint8_t *bd_addr,uint8_t*class_of_device,uint8_t link_type);
int btc_hci_sendCommandStatusEvt(uint8_t status, uint8_t num_hci_cmd, uint8_t* op_code);
int btc_hci_sendLinkSupervisonTimeoutChangeEvt(uint16_t con_hdl, uint16_t ls_to);
int btc_hci_sendReadRemoteExtFeaturesCompleteEvt(uint8_t status, uint16_t con_hdl,
													 uint8_t *p,     uint8_t len);
int btc_hci_sendReadRemoteFeaturesCompleteEvt(uint8_t status, uint16_t con_hdl,
        										  uint8_t *p,     uint8_t len)  ;
int btc_hci_sendReadHostSuppFeaturesNotifyEvt(uint8_t status, uint8_t *bd_addr, uint8_t *ext_feature);
int btc_hci_sendRoleChangeEvt(uint8_t status, uint8_t *bd_addr, uint8_t new_role);
int btc_hci_sendSyncConnChangedEvt(uint8_t status, 				  uint16_t con_hdl,           uint8_t transmission_interval,
									   uint8_t retransmission_window, uint8_t rx_packet_length,	uint8_t tx_packet_length,
									   uint8_t sync_linkid);

int btc_hci_sendSyncConnCompleteEvt(uint8_t status,          uint16_t con_hdl,	            uint8_t *bd_addr,
										uint8_t link_type,       uint8_t transmission_interval, uint8_t retransmission_window,
		                                uint16_t rx_packet_length, uint16_t tx_packet_length,       uint8_t air_mode);
int btc_hci_sendLinkkeyRequestEvt(uint8_t *bd_addr);
int btc_hci_sendLinkKeyNotifyEvt(uint8_t *bd_addr, uint8_t *key, uint8_t keytype);
int btc_hci_sendAuthenCompleteEvt(uint8_t status, uint16_t con_hdl);
int btc_hci_sendDisconnCompleteEvt(uint8_t status, uint16_t con_hdl, uint8_t reason);
int btc_hci_sendKeyRefreshCompleteEvt(uint8_t status, uint16_t con_hdl);
int btc_hci_sendEncrypChangeEvt(uint8_t status, uint16_t con_hdl, uint8_t encrypt_enabled);
int btc_hci_sendNameReqCompleteEvt(uint8_t status, uint8_t* bd_addr, uint8_t* name);
int btc_hci_sendPinCodeRequestEvt(uint8_t* bd_addr);
int btc_hci_sendChangeConnectLinkKeyCompleteEvt(uint8_t status, uint16_t con_hdl);
int btc_hci_sendIoCapabilityReqEvt(uint8_t *bd_addr);
int btc_hci_sendIoCapabilityRspEvt(uint8_t * bd_addr, uint8_t io_cap,
									   uint8_t oob_data_present, uint8_t authen_req);
int btc_hci_sendUserConfirmReqEvt(uint8_t * bd_addr, uint8_t * user_cfm);
int btc_hci_sendUserPasskeyNotifyEvt(uint8_t * bd_addr, uint32_t passkey);
int btc_hci_sendSimplePairingCompleteEvt(uint8_t status, uint8_t *bd_addr);
int btc_hci_sendUserPasskeyRequestEvt(uint8_t *bd_addr);
int btc_hci_sendRemoteOobDataRequestEvt(uint8_t *bd_addr);
int btc_hci_sendModeChangeEvt(uint8_t status, uint16_t con_hdl,
								  uint8_t cur_mode, uint16_t interval);





#endif // BTC_HCI_EVT_H


/********************************************************************************************************
 * @file	blehci_event.h
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
#ifndef BLEHCI_EVENT_H_
#define BLEHCI_EVENT_H_




int		hci_le_periodicAdvSyncEstablished_evt (u8 status, u16 syncHandle,u8 advSID, u8 advAddrType, u8 advAddress[6], u8 advPHY,
										       u16 perdAdvItvl, u8 advClkAccuracy);
//int		hci_le_periodicAdvSyncEstablished_evt (u8 status, u16 syncHandle, extadv_id_t *pId, u8 advPHY, u16 perdAdvItvl, u8 advClkAccuracy);

int		hci_le_periodicAdvReport_evt (u8 subEventCode, u16 syncHandle, u8 txPower, u8 RSSI, u8 cteType,u8 dataStatus, u8 dataLength,
								      u8* data);
int		hci_le_periodicAdvSyncLost_evt (u16 syncHandle);
int		hci_le_cisEstablished_evt(u8 status, u16 cisHandle, u8 cigSyncDly[3], u8 cisSyncDly[3], u8 transLaty_m2s[3], u8 transLaty_s2m[3], u8 phy_m2s,
		                         u8 phy_s2m, u8 nse, u8 bn_m2s, u8 bn_s2m, u8 ft_m2s, u8 ft_s2m, u16 maxPDU_m2s, u16 maxPDU_s2m, u16 isoIntvl );
int		hci_le_cisReq_evt(u16 aclHandle, u16 cisHandle, u8 cigId, u8 cisId);
int		hci_le_createBigComplete_evt(u8 status, u8 bigHandle, u8 bigSyncDly[3], u8 transLatyBig[3], u8 phy, u8 nse,
								     u8 bn, u8 pto, u8 irc, u16 maxPDU, u16 isoIntvl, u8 numBis, u16* bisHandles);
int		hci_le_terminateBigComplete_evt(u8 bigHandle, u8 reason);
int		hci_le_bigSyncEstablished_evt(u8 status, u8 bigHandle, u8 transLatyBig[3], u8 nse, u8 bn, u8 pto, u8 irc,
		                              u16 maxPDU, u16 isoIntvl,  u8 numBis, u16* bisHandles);
int		hci_le_bigSyncLost_evt(u8 bigHandle, u8 reason);
int		hci_le_BigInfoAdvReport_evt(u16 syncHandle, u8 numBis, u8 nse, u16 IsoItvl, u8 bn, u8 pto, u8 irc,
		                             u16 maxPdu, u8 sduItvl[3], u16 maxSdu, u8 phy, u8 framing, u8 enc);
int		hci_disconnectionComplete_evt(u8 status, u16 connHandle, u8 reason);
int		hci_cmdComplete_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 paraLen, u8 *para, u8 *result);
void	hci_cmdStatus_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 status, u8 *result);
int		hci_le_connectionComplete_evt(u8 status, u16 connHandle, u8 role, u8 peerAddrType, u8 *peerAddr,
                                      u16 connInterval, u16 slaveLatency, u16 supervisionTimeout, u8 masterClkAccuracy);
int		hci_le_enhancedConnectionComplete_evt(u8 status, u16 connHandle, u8 role, u8 peerAddrType, u8 *peerAddr, u8 *loaclRpa, u8 *peerRpa,
                                              u16 connInterval, u16 connLatency, u16 supervisionTimeout, u8 masterClkAccuracy);
int		hci_le_connectionUpdateComplete_evt(u8 status, u16 connHandle, u16 connInterval,
        									u16 connLatency, u16 supervisionTimeout);
int		hci_le_readRemoteFeaturesComplete_evt(u8 status, u16 connHandle, u8 * feature);
int		hci_le_channel_selection_algorithm_evt(u16 connhandle, u8 channel_selection_alg);
int		hci_le_phyUpdateComplete_evt(u16 connhandle,u8 status, u8 new_phy);
int		hci_le_data_len_update_evt(u16 connhandle,u16 effTxOctets, u16 effRxOctets, u16 maxtxtime, u16 maxrxtime);
int		hci_le_longTermKeyRequest_evt(u16 connHandle, u8* random, u16 ediv, u8* result);
int		hci_le_readLocalP256KeyComplete_evt(u8* localP256Key, u8 status);
int		hci_le_generateDHKeyComplete_evt(u8* DHkey, u8 status);
int		hci_le_encryptChange_evt(u16 connhandle,  u8 encrypt_en);
int		hci_le_encryptKeyRefresh_evt(u16 connhandle);
int		hci_remoteNateReqComplete_evt (u8* bd_addr);
int     hci_readRemoteVerInfoCmpEvt(u8 status, u16 connHandle, u8 version, u16 manuName, u16 supVersion);
int 	hci_sendCmdComplete_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 paraLen, u8 *para);
#endif /* HCI_EVENT_H_ */






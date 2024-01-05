extern int btp_browsing_sendGeneralReject(uint16 aclHandle, uint08 errCode);
extern int btp_browsing_setChangePathCmd(uint16 aclHandle, uint16 uidCounter, uint08 direct, uint08 *pUID);
extern int btp_browsing_setBrowsedPlayerCmd(uint16 aclHandle, uint16 playerID);
extern int btp_browsing_setBrowsedPlayerRsp(uint16 aclHandle, uint08 status);
extern int btp_browsing_sendGetFolderItemsCmd(uint16 aclHandle, uint08 scope, 
	uint32 startItem, uint32 endItem, uint08 attCount, uint32 *pAttList);
extern int btp_browsing_sendGetItemAttrCmd(uint16 aclHandle, uint08 scope, 
	uint08 *pUids, uint08 uidCounter, uint08 attCount, uint32 *pAttList);
extern int btp_browsing_sendGetTotalNumbOfItemsCmd(uint16 aclHandle, uint08 scope);
extern int btp_browsing_sendGetTotalNumbOfItemsRsp(uint16 aclHandle, uint08 status,
	uint16 uidCounter, uint32 number);

extern void btp_browsing_regItemsReportCB(BtpBrowsingItemsReportCB cb);
extern void btp_browsing_regInsCompleteCB(BtpBrowsingInsCompleteCB cb);


1. 获取当前的播放器列表
发送：
	btp_browsing_sendGetFolderItemsCmd(, BTP_BROWSING_SCOPE_MEDIA_PLAYER_LIST, 0, 100, 0xFF, nullptr);
返回：
	BtpBrowsingItemsReportCB: btp_browsing_playerItem_t, 返回playerID, IsPlayable
	BtpBrowsingInsCompleteCB：返回指令完成；
2. 设置当前播放器，参数使用第1步中的playerID
发送：
	btp_browsing_setBrowsedPlayerCmd(,playerID)
返回：
	BtpBrowsingInsCompleteCB：返回播放器playerID的Item个数，以及播放器名字
3. 获取正在播放的播放器的音乐信息
发送：
	btp_browsing_sendGetItemAttrCmd：
		scope=BTP_BROWSING_SCOPE_NOW_PLAYGING，
		pUids=FF FF FF FF FF FF FF FF,   （该字段为：BTP_AVRCP_EVTID_TRACK_CHANGED事件的回调中返回的TrackID）
		uidCounter=0,
		pAttList=BTP_BROWSING_ATTID_TITLE+BTP_BROWSING_ATTID_ARTIST_NAME+BTP_BROWSING_ATTID_ALBUM_NAME+BTP_BROWSING_ATTID_TRACK_NUMBER+BTP_BROWSING_ATTID_GENRE+BTP_BROWSING_ATTID_PLAYING_TIME
返回：
	BtpBrowsingItemsReportCB：btp_browsing_attrItem_t （如果有歌曲正在播放，则会将歌曲信息列举返回）
	BtpBrowsingInsCompleteCB：返回指令完成状态
4. 获取播放器的虚拟文件系统
发送：
	btp_browsing_sendGetFolderItemsCmd
		scope=BTP_BROWSING_SCOPE_MEDIA_PLAYER_VIRTUAL_FILESYSTEM，
		startItem=0，
		endItem=1,
		attCount=7，和pAttList相关
		pAttList=BTP_BROWSING_ATTID_TITLE+BTP_BROWSING_ATTID_ARTIST_NAME+BTP_BROWSING_ATTID_ALBUM_NAME+BTP_BROWSING_ATTID_TRACK_NUMBER+BTP_BROWSING_ATTID_TOTAL_TRACKS+BTP_BROWSING_ATTID_GENRE+BTP_BROWSING_ATTID_PLAYING_TIME
返回：
	BtpBrowsingItemsReportCB：btp_browsing_folderItem_t
	BtpBrowsingInsCompleteCB：返回指令完成状态






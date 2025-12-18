## V5.3.0.0_Patch_0002(PR)

### Dependency Updates

- N/A

### Features
* BT
   - Added the function of UART to transmit PCM data.
     - Added macro "TLKMDI_SCO_PCM_FROM_UART" which is disabled by default in tlkmdi/aud/tlkmdi_audsco.c. User can enable this macro when they need this function. 
	 
* BLE
   - Added one BLE ACL Master connection function. 
     - "MAX_BLE_LINK" changed from 3 to 4 in tlkstk/tlkstk_config.h.
   - Added new feature: Host support connection-oriented channels(COC).
     - Add COC related data structure and APIs in tlkstk/ble/host/l2cap/l2cap_signal.h.
	 - Add COC related callback events in tlkstk/ble/host/gap/gap_event.h.
	 - Add COC related function return value in tlkstk/ble/ble_common.h.
   - Added customized timing alignment function between Sending Advertising packets and reporting scanned Advertising packets.
     - Add telink controller event "CONTR_EVT_LE_LEGACY_ADV_TX_EACH_CHANNEL" in btble.h to set timestamp in BLE Advertising packet. 
	 - Add API "blc_ll_advReport_setRxPacketTickEnable" in tlkstk/ble/controller/ll.h to enable adding timestamp in scanned Advertising packets report.
	 

### Bug Fixes
* BT
   - Fixed BT pincode function error which could lead to pairing failure when using pincode pairing mode.

* BLE
   - Fixed BLE create connection device address transmitted from PC tool.
   

### KNOWN ISSUES

- N/A

### BREAKING CHANGES

- N/A

### Dependency Updates

- N/A

### Features
* BT
   - 新增UART传输PCM数据的功能。
     - 新增宏“TLKMDI_SCO_PCM_FROM_UART”，默认在tlkmdi/aud/tlkmdi_audscoc中关闭。用户需要这个功能时启用该宏。
	 
* BLE
   - 新增1路BLE ACL Master connection功能。
	 - 在tlkstk/tlkstk_config.h中“MAX_BLE_LINK”从3修改为4。
   - 新增connection-oriented channels(COC)功能。
     - 在tlkstk/ble/host/l2cap/l2cap_signal.h中增加COC相关的数据结构和接口。
	 - 在tlkstk/ble/host/gap/gap_event.h中增加COC相关的回调事件。
	 - 在tlkstk/ble/ble_common.h中增加COC相关的函数返回值。
   - 新增自定义发送广播报文和上报扫描广播报文的时间对齐功能。
     - 在btble.h中增加telink控制器事件“CONTR_EVT_LE_LEGACY_ADV_TX_EACH_CHANNEL”，用于设置BLE广播报文的时间戳。
	 - 在tlkstk/ble/controller/ll.h中增加API "blc_ll_advReport_setRxPacketTickEnable"，用于在扫描广播报文上报中添加时间戳。
	 
	 
### Bug Fixes
* BT
   - 修复pincode功能异常，使用该配对模式配对时导致配对失败的问题。

* BLE
   - 修复通过上位机创建连接时设备地址传输异常的问题。


### KNOWN ISSUES

- N/A


### BREAKING CHANGES

- N/A






## V5.3.0.0_Patch_0001(PR)

### Dependency Updates

- N/A

### Features

- N/A

### Bug Fixs

- Fixed the issue of program running abnormally when the Firmware binary storage address exceeds 0xFC000 in Flash.
  - Peer BT device pairing parameters storage addresses are modified from constant value to address which matches to Flash capacity in tlkstk/bt/bth_device.c
  - BLE SMP pairing parameters storage address is modified from constant value to address which matches to Flash capacity in tlkstk/ble/ble.c.
  - BLE SMP pairing parameters storage address is modified from constant value to address which matches to Flash capacity in tlkmmi/test/rdt/le/tlkmmi_rdt_leCore.c.
  - Audio volume storage address is modified from constant value to address which matches to Flash capacity in tlkmmi/audio/tlkmmi_audioInfo.c.
  - BLE Simple SDP peer Attribute Handles storage address is modified from constant value to address which matches to Flash capacity in tlkmdi/le/tlk_le_simple_sdp.c.

### KNOWN ISSUES

- N/A

### BREAKING CHANGES

- N/A


### Dependency Updates

- N/A

### Features

- N/A

### Bug Fixs

- 修复Firmware binary文件存储位置超出Flash地址0xFC000时引发程序运行异常问题。
  - tlkstk/bt/bth_device.c中对端BT设备配对信息存储地址由固定值修改为根据Flash capacity自动匹配。
  - tlkstk/ble/ble.c中BLE SMP配对参数存储地址由固定值修改为根据Flash capacity自动匹配。
  - tlkmmi/test/rdt/le/tlkmmi_rdt_leCore.c中BLE SMP配对参数存储地址由固定值修改为根据Flash capacity自动匹配。
  - tlkmmi/audio/tlkmmi_audioInfo.c中 Audio volume存储地址由固定值修改为根据Flash capacity自动匹配。
  - tlkmdi/le/tlk_le_simple_sdp.c中BLE Simple SDP Attibute Handles存储地址由固定值修改为根据Flash capacity自动匹配。
  
### KNOWN ISSUES

- N/A

### BREAKING CHANGES

- N/A
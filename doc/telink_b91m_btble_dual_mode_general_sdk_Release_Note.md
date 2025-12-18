## V5.3.0.0_Patch_0003 (PR)

### Dependency Updates

- N/A

### Features

* Bluetooth
  - Added AAC decoding support controlled by the "TLK_ALG_AAC_ENABLE" macro, which is disabled by default. Users can enable this macro when this functionality is required.
   - Added AAC decoding algorithm interfaces in tlkalg/audio/aac/tlkalg_aac.c and tlkalg_aac.h. Added AAC decoding algorithm library tlklib/lib/libB91m_aac_dec.a.
   - The size of TLK_DEV_SPK_BUFF_SIZE in the file tlkdev/sys/tlkdev_codec.h must be adjusted. If the AAC decoding function is enabled,.
   - Added inclusion of the AAC decoding library libB91m_aac_dec.a in the core/B91/B91.cproject and core/B92/B92.cproject files.
   - Added tlkmdi/aud/tlkmdi_audsnk_aac.c, which is only active when TLK_ALG_AAC_ENABLE is enabled. Modified tlkmdi/aud/tlkmdi_audsnk.c, which is only active when TLK_ALG_AAC_ENABLE is disabled.
  - Implemented flow control management for the RFCOMM layer, refer to the demo [spp_flow_control_demo] for usage implementation.
    - Added data structures and interfaces related to flow control management in the btp_rfcomm.h header file.
    - Added flow control management interfaces in the btp_spp.h header file.
  - Added custom device information in Extended Inquiry Response (EIR) packets, refer to the demo [Extend_inquiry_response_demo] for usage implementation.
    - Updated the implementation of the bth_hci_sendWriteExtendedInquiryRspCmd interface in tlkstk/bt/bth/bth_hcicmd.c.
    - Adjusted the length limit of the TLKBT_HCI_H2C_CMD_FIFO_SIZE macro in the tlkstk/hci/bt_hci.h header file.

* Bluetooth Low Energy (BLE)
  - Added support for BLE Long Range Coded PHY:
    - Implemented support for BLE Long Range Coded PHY (S2/S8 coding modes) within the BLE stack.
    - This feature is disabled by default.
    - To enable, users must invoke the blc_ll_init2MPhyCodedPhy_feature API within the ble_core_init function in src/tlkstk/ble/ble.c.
	 

### Bug Fixs

* General
  - Fixed known compatibility issues.

* Bluetooth
  - Addressed a connection issue arising from query conflicts when SDP operates as both a server and a client concurrently.
  - Fix the problem of abnormal packet loss that occurs when the device is connected as a slave to the master device with certain special MAC addresses..
  - Fixed the known compatibility issues.
  - Resolved an issue of unintended Stimer interrupt occupancy due to conflicts between BLE advertising and BT eSCO link operations.

* Bluetooth Low Energy (BLE)
  - Fixed a reconnection failure in ACL mode by modifying the processing logic of the app_le_adv_report_event_handle function in src/tlkmdi/le/tlkmdi_leAcl.c.
   

### KNOWN ISSUES

- N/A

### BREAKING CHANGES

- N/A

### Dependency Updates

- N/A

### Features
* BT
   - 新增A2dp SNK场景下的AAC音频解码功能，使用“TLK_ALG_AAC_ENABLE”管控，默认关闭，用户需要这个功能时启用该宏。
      - tlkalg/audio/aac/tlkalg_aac.c、tlkalg_aac.h中新增AAC 解码算法相关接口，新增tlklib/lib/libB91m_aac_dec.a AAC解码的算法库。
      - tlkdev/sys/tlkdev_codec.h 中打开AAC解码后需要修改“TLK_DEV_SPK_BUFF_SIZE”的资源大小。
      - core/B91/B91.cproject、core/B92/B92.cproject 中添加include AAC的解码库 libB91m_aac_dec.a。
      - 新增tlkmdi/aud/tlkmdi_audsnk_aac.c，仅在“TLK_ALG_AAC_ENABLE”使能时生效，修改tlkmdi/aud/tlkmdi_audsnk.c，仅在“TLK_ALG_AAC_ENABLE”失能时生效。
   - 新增RFCOMM层的流控管理。
      - btp_rfcomm.h增加流控管理相关的数据结构和接口。
      - btp_spp.h增加流控管理相关的接口，使用demo参考 spp_flow_control_demo。
   - 新增在Extend inquiry response中添加自定义的device information。
      - tlkstk/bt/bth/bth_hcicmd.c中修改bth_hci_sendWriteExtendedInquiryRspCmd接口的逻辑。
      - 修改tlkstk/hci/bt_hci.h中 “TLKBT_HCI_H2C_CMD_FIFO_SIZE”的长度限制。
         
* BLE
   - BLE Long Range Coded PHY 支持
      - BLE Stack 新增对 BLE Long Range Coded PHY (S2/S8 编码模式) 的支持。
      - 此功能默认关闭。
      - 用户需在 src/tlkstk/ble/ble.c 文件中的 ble_core_init 函数中调用 blc_ll_init2MPhyCodedPhy_feature API 来启用此特性。
         
         
### Bug Fixs
* BT
   - 修复SDP 同时作为server 和 client时的查询冲突导致连接异常问题。
   - 修复设备作为slave 连接某些特殊MAC地址的主设备时出现的异常丢包的问题。
   - 修复已知的兼容性问题。
   - 修复BLE adv与BT esco冲突导致的Stimer中断异常占用问题。

* BLE
   - 修复ACL 模式回连失败问题
      - 修改文件 src/tlkmdi/le/tlkmdi_leAcl.c 中的 app_le_adv_report_event_handle 函数处理逻辑。


### KNOWN ISSUES

- N/A


### BREAKING CHANGES

- N/A






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
     - Added COC related data structure and APIs in tlkstk/ble/host/l2cap/l2cap_signal.h.
	 - Added COC related callback events in tlkstk/ble/host/gap/gap_event.h.
	 - Added COC related function return value in tlkstk/ble/ble_common.h.
   - Added customized timing alignment function between sending Advertising packets and reporting scanned Advertising packets.
     - Added telink controller event "CONTR_EVT_LE_LEGACY_ADV_TX_EACH_CHANNEL" in btble.h to set timestamp in BLE Advertising packet. 
	 - Added API "blc_ll_advReport_setRxPacketTickEnable" in tlkstk/ble/controller/ll.h to enable adding timestamp in scanned Advertising packets report.
	 

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

## V5.3.0.0(PR)

### Dependency Updates

- N/A

### Features

- Support BT security connection mode
- Support IIS's master and slave mode for B91 and B92 project
- Support Chinese/English version of the PC's tool
- Supports downloading through the serial port on the bootloader (Rate 72KB/s)
- Supports coexistence of multiple serial port protocols (Currently, CPC1, CPC2, and CPC3 are available. The default protocol is CPC3, and the older version is CPC1)

### Bug Fixs

- Fixed an issue where SCO is created/destroyed frequently, and SCO silence occurs with probability.
- Fixed an issue where frequent entry and exit sniff would cause low probability disconnection.
- Fixed an issue of BT connection exception on some mobile phones.
- Fixed BT connection anomalies caused by "RoleSwitch" processing defects.
- Fixed an issue where the negotiated volume was different from the actual volume when the headset was connected.
- Fixed an issue where some connected earphones could play music but failed to play again after pausing.

### KNOWN ISSUES

- N/A

### BREAKING CHANGES

- Merge the projects of B91 and B92 and align B91 with the toolchains of B92
- Adjustment of part of the B91 interface to align with the B92 interface
- Split system tasks to separate out device task and debug task
- Refactor device's driver code, and support getting available device list and running device list
- BT Host and Profile use dynamic memory mechanism, which can be tailored by the user as needed
- The location for saving global configuration information is bound to the flash model and automatically adjusts with the flash model
- Adjust the synchronization protocol between the LOG and VCD control switches and the PC's tool to increase the synchronization rate

### CodeSize

#### B91

- Flash: 751KB with log and 549KB without log
- RAM: I-RAM is 112KB and D-RAM is 95KB (with log);I -RAM is 104KB and D-RAM is 90KB (without log)

#### B92

- Flash:761KB with log and 556KB without log
- RAM: I-RAM is 114KB and D-RAM is 95KB (with log);I -RAM is 106KB and D-RAM is 91KB (without log)

### Dependency Updates

- N/A

### Features

- 支持BT安全连接模式
- 支持B91和B92系列IIS主从模式
- 支持中文/英文版上位机工具
- 支持在bootloader中通过串口进行下载（速率72KB）
- 支持多协议共存（目前有CPC1/CPC2/CPC3，缺省为CPC3，老版本为CPC1）

### Bug Fixs

- 修复频繁创建/销毁SCO，概率性出现SCO无声的问题
- 修复频繁进出sniff模式，低概率出现断连的问题
- 修复部分手机BT连接异常的兼容性问题
- 修复RoleSwitch处理缺陷造成的BT连接异常的问题
- 修复连接耳机时，协商音量与实际音量不一致的问题
- 修复连接部分耳机播放音乐，暂停后再次播放失败的问题

### KNOWN ISSUES

- N/A

### BREAKING CHANGES

- 合并B91和B92的SDK，并将B91与B92的toolchains保持一致
- 调整部分B91接口，与B92接口保持一致
- 将系统任务拆分，独立出设备管理和调试管理任务
- 重构设备驱动代码，支持获取可用设备列表和运行设备列表
- BT Host和Profile采用动态内存机制，用户可根据需要进行裁剪
- 全局配置信息保存位置与flash型号进行绑定，并随flash型号自动调整
- 调整LOG和VCD控制开关与上位机之间的同步协议，提升同步速率

### CodeSize

#### B91

- Flash: 带log版本为751KB，不带log版本为549KB
- RAM:   带log版本，I-RAM为112KB，D-RAM为95KB；不带log版本，I-RAM为104KB，D-RAM为90KB

#### B92

- Flash: 带log版本为761KB，不带log版本为556KB
- RAM:   带log版本，I-RAM为114KB，D-RAM为95KB；不带log版本，I-RAM为106KB，D-RAM为91KB
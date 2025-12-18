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
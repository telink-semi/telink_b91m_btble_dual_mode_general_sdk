测试角色分为DUT(待测设备)和AUT(复测设备)

t001. ble连接 断开，对bt的影响
t002. bt连接、断开，开启sco，sco无语音数据
t003. bt spp发送数据，查看是否有丢包
t004. 校验role switch的影响
t005. 校验Profile连接超时

T002
功能：
	检测BT-SCO是否有无声的问题，以及反复切换SCO，是否会引发ACL断开
角色：
	1. DUT - 负责与AUT建立连接
	2. AUT1 - 连接DUT，建立sco
	3. AUT2 - 连接DUT
注意：
	1. DUT只能与AUT1和AUT2建立连接；
	2. AUT1负责创建和断开与DUT的SCO连接；
	3. AUT2负责与DUT建立ACL连接；
	4. AUT1定时监测SCO是否有无声的问题；
	5. DUT定时监测SCO是否有无声的问题；

T003
功能：
	检测Controller是否有丢包
角色：
	1. DUT - 负责与AUT1和AUT2建立连接
	2. AUT1 - 连接DUT，建立spp，并发送数据
	3. AUT2 - 频繁连接与断开DUT，辅助DUT测试
注意：
	1. DUT只能与AUT1和AUT2建立连接；
	2. DUT可以向AUT1和AUT2发送SPP数据，可选择发送间隔与发送数据长度；
	3. AUT1可以向DUT发送数据，数据长度与间隔可配置；
	4. AUT2可选择是否向DUT发送数据，以及发送数据时长与间隔、长度。
	5. AUT1做Master, AUT2做Slave;
	6. TLKMMI_RDT_T003_BTNAME_DUT/AUT/AUT2和TLKMMI_RDT_T003_BTADDR_DUT/AUT/AUT2是固定的；

T004
功能：
	校验role switch的影响
角色：
	1. DUT - 处于InquiryScan和PageScan模式，负责与AUT1和AUT2建立连接
	2. AUT1 - 连接DUT，建立SPP，发送spp数据，同时控制switch反复切换
注意：
	1. DUT只能与AUT1建立连接；
	2. DUT可以向AUT1发送SPP数据，可选择发送间隔与发送数据长度；
	3. AUT1可以向DUT发送数据，数据长度与间隔可配置。

T005
功能：
	校验Profile连接超时
角色：
	1. DUT - 发起Page连接手机；
注意：
	1. DUT与手机之间反复连接与断连，验证Profile测试失败的问题



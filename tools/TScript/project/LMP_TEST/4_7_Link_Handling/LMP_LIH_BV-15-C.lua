
require("printf")
require("device")
require("DefaultSetting-LinkHandling")
require("Preamble-IUT-Slave")
require("definition")
-- IUT  						 Lower Tester
-- caracara<kite dongle> 	<->  caracara<kite dongle> 
-- caracara<uart>			<->  caracara<uart>
-- caracara<kite dongle>	<->  csr<usb>       


Testcase = "LMP/LIH/BV-15-C"
iut_mode = "uart"
lt_mode  = "usb"
OPTIONAL = ALT1
print("--------------------<<   "..Testcase.."   >>--------------------")

-- Open Device
iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w = Open_Device()
if(iut_handle_r==nil or iut_handle_w==nil or lt_handle_r==nil or lt_handle_w==nil) then
	-- Close Device
	Close_Device(lt_handle)
	return
end
-- Default Settings
role = "slave"
Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role)
-- Preamble-IUT-Master
Allow_Role_Switch = 0  -- allow role switch
lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch)
-- Test Begin
print("============ Test Begin:"..Testcase.."  =============")

local Test_Status="Pass"
-- <= LT:HCI_QoS_Setup
Service_Type 	= 1            	-- best effort
Token_Rate 		= 0xffffffff   	-- TODO
Peak_Bandwidth 	= 0xffffffff	-- TODO
Latency 		= 0xffffffff	-- TODO
Delay_Variation = 0xffffffff	-- TODO
HCI_QoS_Setup(lt_handle_w,lt_mode,lt_Connection_Handle,Service_Type,Token_Rate,Peak_Bandwidth,Latency,Delay_Variation)

-- => LT:HCI Command Complete event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End	
end

if(OPTIONAL==ALT1) then
	-- => IUT:HCI_QoS_Setup_Complete
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_QoS_Setup_Complete",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
	    goto Test_Case_End
	end
elseif(OPTIONAL==ALT2) then
	-- => IUT:HCI_Flow_Specification_Complete
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Flow_Specification_Complete",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
	    goto Test_Case_End
	end
end

-- => LT:HCI_QoS_Setup_Complete
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_QoS_Setup_Complete",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End	
end

-- <= LT:HCI_Write_Link_Policy_Settings
Link_Policy_Settings = SNIFF_MODE_ENABLE

HCI_Write_Link_Policy_Settings(lt_handle_w,lt_mode,lt_Connection_Handle,Link_Policy_Settings)
-- => LT:HCI Command Complete event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end
-- <= IUT:HCI_Write_Link_Policy_Settings
Link_Policy_Settings = SNIFF_MODE_ENABLE

HCI_Write_Link_Policy_Settings(iut_handle_w,iut_mode,iut_Connection_Handle,Link_Policy_Settings)
-- => IUT:HCI Command Complete event
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end

-- <= IUT:HCI_Sniff_Mode
Sniff_Max_Interval = 18
Sniff_Min_Interval = 18
Sniff_Attempt = 4
Sniff_Timeout = 2
HCI_Sniff_Mode(iut_handle_w,iut_mode,iut_Connection_Handle,Sniff_Max_Interval,Sniff_Min_Interval,Sniff_Attempt,Sniff_Timeout)
-- => IUT:HCI_Command_Status
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end
-- => LT:HCI_Mode_Change
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Mode_Change",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end

-- => IUT:HCI_Mode_Change
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Mode_Change",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End	
end

--[[
-- <= IUT:HCI_Remote_Name_Request
for i=1,6 do
	BD_ADDR[i] = IUT_BD_Addr[i];
end
Page_Scan_Repetition_Mode = 1
Clock_Offset = 0
HCI_Remote_Name_Request(lt_handle_w,lt_mode,BD_ADDR,Page_Scan_Repetition_Mode,Clock_Offset)
-- => IUT:HCI_Command_Status
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
if(rst_tbl.Status==0x12) then -- a parameter is outside of the specified range
	Test_Status="Pass"
end
]]

::Test_Case_End::
print("============ Test Result:"..Test_Status.."  =============")
-- Close Device
Close_Device(lt_handle)
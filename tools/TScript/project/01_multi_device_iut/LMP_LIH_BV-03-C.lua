
require("printf")
require("device")
require("DefaultSetting-LinkHandling")
require("Preamble-IUT-Slave")
require("Preamble-IUT-Master")
require("definition")
-- IUT  						 Lower Tester
-- caracara<kite dongle> 	<->  caracara<kite dongle> 
-- caracara<uart>			<->  caracara<uart>
-- caracara<kite dongle>	<->  csr<usb>       


Testcase = "LMP/LIH/BV-03-C"
iut_mode = "dongle"
lt_mode  = "usb"
role     = "master"
OPTIONAL = ALT1
Timeout  = 10
-- Test Begin
print("============ Test Begin:"..Testcase.."  =============")

local Test_Status="Pass"
-- Open Device
iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w = Open_Device(iut_mode,lt_mode)

if(lt_mode~="unused")then
	if(lt_handle_r==nil or lt_handle_w==nil) then
		Test_Status= "Fail"
		goto Test_Case_End
	end
end

if(iut_mode~="unused")then
	if(iut_handle_r==nil or iut_handle_w==nil) then
		Test_Status= "Fail"
		goto Test_Case_End
	end
end


-- Default Settings
flag = Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role,Timeout)

if(flag==false) then
	Test_Status= "Fail"
	goto Test_Case_End
end

if(role=="master") then
	-- Preamble-IUT-Master
	Allow_Role_Switch = 0  -- allow role switch
	lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Master(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

	if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
		Test_Status="Fail"
		goto Test_Case_End
	end
end


if(role=="slave") then
	-- Preamble-IUT-Master
	Allow_Role_Switch = 0  -- allow role switch
	lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

	if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
		Test_Status="Fail"
		goto Test_Case_End
	end
end

-- <= IUT:HCI_Write_Link_Policy_Settings
Link_Policy_Settings = ROLE_SWITCH_ENABLE

HCI_Write_Link_Policy_Settings(iut_handle_w,iut_mode,iut_Connection_Handle,Link_Policy_Settings)
-- => IUT:HCI Command Complete event
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end

-- <= LT:HCI_Write_Link_Policy_Settings
Link_Policy_Settings = ROLE_SWITCH_ENABLE

HCI_Write_Link_Policy_Settings(lt_handle_w,lt_mode,lt_Connection_Handle,Link_Policy_Settings)
-- => LT:HCI Command Complete event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end

LMP_Features = array.new(8);

LMP_Features[1] = 0x5f
LMP_Features[2] = 0xff
LMP_Features[3] = 0xcd
LMP_Features[4] = 0xf2
LMP_Features[5] = 0xc3
LMP_Features[6] = 0xa5
LMP_Features[7] = 0x7b
LMP_Features[8] = 0x83

-- modify role switch feature
HCI_AB_Write_Feature(iut_handle_w,iut_mode,LMP_Features)
flag,rst_tbl = HCI_Wait_ABEvent_Done(iut_handle_r,iut_mode,"HCI_Abnormal_Write_Feature_Complete",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end

-- <= LT:HCI_Switch_Role
for i=1,6 do
	BD_ADDR[i] = IUT_BD_Addr[i]
end

Role = 0 -- 0:slave->master,1:master->slave
HCI_Switch_Role(lt_handle_w,lt_mode,BD_ADDR,Role)
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end

flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Role_Change",Timeout)
if(flag~=true) then
	Test_Status = "Fail"
	goto Test_Case_End
end

if(rst_tbl.Status~=0x1A) then
	Test_Status = "Fail"
	goto Test_Case_End
end

::Test_Case_End::
print("============ Test Result:"..Test_Status.." =============")
-- Close Device
Close_Device(iut_mode,lt_mode)
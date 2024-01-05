
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


Testcase = "LMP/LIH/BV-122-C"
iut_mode = "dongle"
lt_mode  = "unused"
OPTIONAL = ALT1
role     = "master"
Timeout  = 10
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


if(role == "slave") then
-- Preamble-IUT-Slave
Allow_Role_Switch = 0  -- allow role switch
lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)
elseif(role == "master") then
-- Preamble-IUT-Master
Allow_Role_Switch = 0
lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Master(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)
end

if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
	Test_Status="Fail"
	goto Test_Case_End
end
-- Test Begin



::Test_Case_End::
print("============ Test Result:"..Test_Status.."  =============")
-- Close Device
Close_Device(iut_mode,lt_mode)
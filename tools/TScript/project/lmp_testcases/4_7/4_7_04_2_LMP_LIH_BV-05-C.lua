
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


Testcase = "LMP/LIH/BV-05-C"
iut_mode = "uart"
lt_mode  = "usb"
Timeout  = 5
role     = "slave"
local Test_Status="Pass"
::Test_Case_Init::
print("--------------------<<   "..Testcase.."   >>--------------------")

-- Open Device
iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w = Open_Device()
if(iut_handle_r==nil or iut_handle_w==nil or lt_handle_r==nil or lt_handle_w==nil) then
	-- Close Device
	Close_Device(lt_handle)
	return
end
-- Default Settings

Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role)

-- Preamble_IUT_Slave/Preamble_IUT_Master
Allow_Role_Switch = 0  -- disallow role switch
if(role == "slave") then
-- Preamble-IUT-Slave
lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch)
elseif(role == "master") then
-- Preamble-IUT-Master
lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Master(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch)
end


if(lt_Connection_Handle==0 or iut_Connection_Handle==0) then
    Test_Status="Fail"
    goto Test_Case_End
end

-- Test Begin
::Test_Case_Begin::
print("============ Test Begin:"..Testcase.."  =============")



--<= IUT:HCI_Disconnect
Reason = 0x13
HCI_Disconnect (iut_handle_w,iut_mode,iut_Connection_Handle,Reason)
-- => IUT:HCI Command Status event
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end

tl_progress(25);
-- => LT:HCI Disconnection Complete event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Disconnection_Complete",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end
tl_progress(50);
-- => IUT:HCI Disconnection Complete event
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Disconnection_Complete",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end

-- Wait for 3 second
print("wait 3 seccond...");
delay_sec(1)
print("wait 2 seccond...");
delay_sec(1)
print("wait 1 seccond...");
delay_sec(1)
tl_progress(75);
-- <= LT:HCI_Read_Remote_Supported_Features
print("wait time out(about 5 s)...");
HCI_Read_Remote_Supported_Features(lt_handle_w,lt_mode,lt_Connection_Handle)
-- => LT:HCI_Read_Remote_Supported_Features_Complete
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Read_Remote_Supported_Features_Complete",Timeout)
if(flag==true) then
	Test_Status="Fail"
	goto Test_Case_End
end

::Test_Case_End::
print("============ Test Result:"..Test_Status.."  =============")
if(Test_Status=="Pass") then
   tl_progress(100);
else
   tl_error(1);
end

-- Close Device
Close_Device(lt_handle)



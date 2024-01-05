
require("printf")
require("device")
require("DefaultSetting-LinkHandling")
require("Preamble-IUT-Slave")
require("definition")
-- IUT  						 Lower Tester
-- caracara<kite dongle> 	<->  caracara<kite dongle> 
-- caracara<uart>			<->  caracara<uart>
-- caracara<kite dongle>	<->  csr<usb>       


Testcase = "LMP/LIH/BV-117-C"
iut_mode = "uart"
lt_mode  = "usb"
OPTIONAL = ALT1
print("--------------------<<   "..Testcase.."   >>--------------------")
local Test_Status="Pass"
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

if(lt_Connection_Handle==0 or iut_Connection_Handle==0) then
    Test_Status="Fail"
    goto Test_Case_End
end

tl_progress(25);


-- <= LT:HCI_Read_Remote_Supported_Features
HCI_Read_Remote_Supported_Features(lt_handle_w,lt_mode,lt_Connection_Handle)
HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Read_Remote_Supported_Features_Complete",Timeout)

tl_progress(75);

if(bit32.band(rst_tbl.LMP_Features[1],0x80)~=0 ) then
	print("IUT Support Sniff Mode")
else
	print("IUT NOT Support Sniff Mode")
	Test_Status = "Fail"
end

if(bit32.band(rst_tbl.LMP_Features[6],0x02)~=0 ) then
	print("IUT Support Sniff Subrating Mode")
else
	print("IUT NOT Support Sniff Subrating Mode")
	Test_Status = "Fail"
end

::Test_Case_End::

print("============ Test Result:"..Test_Status.."  =============")
-- Close Device

if(Test_Status=="Pass") then
   tl_progress(100);
else
   tl_error(1);
end


Close_Device(lt_handle)

require("printf")
require("device")
require("DefaultSetting-LinkHandling")
require("Preamble-IUT-Master")
require("definition")
-- IUT  						 Lower Tester
-- caracara<kite dongle> 	<->  caracara<kite dongle> 
-- caracara<uart>			<->  caracara<uart>
-- caracara<kite dongle>	<->  csr<usb>       


Testcase = "LMP/LIH/BV-82-C"
iut_mode = "uart"
lt_mode  = "usb"
Timeout  = 5

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
role = "master"
Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role)
-- Test Begin
::Test_Case_Begin::
print("============ Test Begin:"..Testcase.."  =============")

local Test_Status="Pass"

-- <= LT:HCI Reset
HCI_Reset(lt_handle_w,lt_mode)
-- => LT:HCI Command Complete event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end
-- <= LT:HCI Write Scan Enable(Scan_Enable=3,Enable Inquiry scan and Page scan)
Scan_Enable=3
HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
-- => LT:HCI Command Complete event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end
tl_progress(25);

if(OPTIONAL==1)then
-- <= IUT:HCI Inquiry
	LAP= 0x9e8b33
	Inquiry_Length = 10  -- 10*1.28s=12.8s
	Timeout = 13		 -- >Inquiry_Length
	Num_Responses  = 1   
	HCI_Inquiry(iut_handle_w,iut_mode,LAP,Inquiry_Length,Num_Responses)
-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

-- => IUT:HCI Inquiry Result event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Inquiry_Result",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
-- => IUT:HCI Inquiry Complete event
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Inquiry_Complete",Timeout)
    if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
end
-- <= IUT:HCI Create Connection
BD_ADDR = array.new(6);
for i=1,6 do
	BD_ADDR[i] = LT_BD_Addr[i]
end
Packet_Type = 0x0008  				-- DM1
Page_Scan_Repetition_Mode = 0x01  	-- R1
Clock_Offset = 0x0000				-- 0
Allow_Role_Switch = 0				-- not allow
HCI_Create_Connection(iut_handle_w,iut_mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
Clock_Offset,Allow_Role_Switch)
-- => IUT:HCI Command Status event
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
tl_progress(50);

if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end
-- => LT:HCI_Connection_Request
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Request",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end
tl_progress(75);
-- <= LT:HCI_Accept_Connection_Request
Reason = 0x1f
HCI_Reject_Connection_Request (lt_handle_w,lt_mode,rst_tbl.BD_ADDR,Reason)
-- => LT:HCI Command Status event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end

-- => IUT:HCI Connection complete event
flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Connection_Complete",Timeout)
if(flag~=true) then
	Test_Status="Fail"
	goto Test_Case_End
end

-- => LT:HCI Connection complete event
flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Complete",Timeout)
if(rst_tbl.Status == 0x1f) then
	Test_Status ="Pass"
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
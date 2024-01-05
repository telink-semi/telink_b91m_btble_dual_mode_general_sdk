
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")
require("project.bt_controller_certification.lib.device")
function  HCI_HFC_BV_01_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "HCI/HFC/BV-01-C"
		-- iut_mode = "dongle"
	-- lt_mode  = "usb"
	-- role     = "slave"
	OPTIONAL = ALT1
	-- Timeout  = 10
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
	-- flag = Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role,Timeout)

	-- <= IUT:HCI_Set_Event_Mask
	Event_Mask = array.new(8);

	Event_Mask[1] = 0xFB
	Event_Mask[2] = 0xFF
	Event_Mask[3] = 0xFF
	Event_Mask[4] = 0xFF
	Event_Mask[5] = 0xFF
	Event_Mask[6] = 0x1F
	Event_Mask[7] = 0x00
	Event_Mask[8] = 0x00

	HCI_Set_Event_Mask(iut_handle_w,iut_mode,Event_Mask)
	-- => IUT:HCI_Command_Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
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
		return nil,nil
	end
	-- <= IUT:HCI Create Connection

	Packet_Type = 0xcc18  				-- DM1
	Page_Scan_Repetition_Mode = 0x00  	-- R0
	Clock_Offset = 0x0000				-- 0
    Allow_Role_Switch =0;
	HCI_Create_Connection(iut_handle_w,iut_mode,LT_BD_Addr,Packet_Type,Page_Scan_Repetition_Mode,
	Clock_Offset,Allow_Role_Switch)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(rst_tbl.Status~=0) then
		return nil,nil
	end



	HCI_Accept_Connection_Request(lt_handle_w,lt_mode,IUT_BD_Addr,0);
	-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		return nil,nil
	end
	

-- => LT:HCI Connection complete event
	lt_flag,lt_rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Complete",Timeout)
	if(lt_flag~=true) then
		return nil,nil
	elseif(lt_rst_tbl~=nil) then
		lt_Connection_Handle = bit32.band(lt_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(lt_rst_tbl.Connection_Handle[2],8),0xff00)	
		log_printf(HCI_PROCESS_STATUS_DISPLAY,"lt_Connection_Handle",lt_rst_tbl.Connection_Handle,2)
	end	

	log_printf(HCI_PROCESS_STATUS_DISPLAY,"HCI Connection Setup",0,0)
    -- --------------- --------------------------ACL connection established.---------------------------------------------
	

	
	::Test_Case_End::
	print("============ Test Result:"..Test_Status.." ============")
	-- Close Device
	Close_Device(iut_mode,lt_mode)
	return Test_Status
end


Total_times = 1
fail=0
success=0



for i=1,Total_times do

	flag = HCI_HFC_BV_01_C("dongle","usb","master",10)
	if(flag=="Pass") then
		success=success+1
	else
		fail=fail+1
	end
	print("")
	print("")
	print("#######################################")
	print("######                                                                        ######")
	print("######    Test"..i.." times, success:"..success.."  fail:"..fail.."  Total:"..Total_times.."        ######")
	print("######                                                                        ######")
	print("#######################################")
	print("")
	print("")
	delay_sec(1)
end
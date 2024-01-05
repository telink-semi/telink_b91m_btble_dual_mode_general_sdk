
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")
require("project.bt_controller_certification.lib.device")
function  HCI_CCO_BI_14_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "HCI/CCO/BI-14-C"
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

-- <= LT:HCI Write Scan Enable(Scan_Enable=3,Enable Inquiry scan and Page scan)
-- 	Scan_Enable=3
-- 	HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
-- -- => LT:HCI Command Complete event
-- 	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
-- 	if(flag~=true) then
-- 		return nil,nil
-- 	end
	-- <= IUT:HCI Create Connection

	Packet_Type = 0x0c00  				-- DM1
	Page_Scan_Repetition_Mode = 0x00  	-- R0
	Clock_Offset = 0x0000				-- 0
    Allow_Role_Switch =0;
	HCI_Create_Connection(lt_handle_w,lt_mode,LT_BD_Addr,Packet_Type,Page_Scan_Repetition_Mode,
	Clock_Offset,Allow_Role_Switch)
	-- => lT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(rst_tbl.Status~=0) then
		return nil,nil
	end




    -- --------------- --------------------------ACL connection no need established.---------------------------------------------
	

	
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

	flag = HCI_CCO_BI_14_C("dongle","usb","master",10)
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
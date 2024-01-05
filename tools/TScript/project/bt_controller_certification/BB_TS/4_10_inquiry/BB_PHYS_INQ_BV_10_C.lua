
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")

function  BB_PHYS_INQ_BV_10_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "BB/PHYS/INQ/BV-10-C"
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



	-- -- Default Settings
	-- flag = Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role,Timeout)

	-- if(flag==false) then
	-- 	Test_Status= "Fail"
	-- 	goto Test_Case_End
	-- end

	-- if(role=="master") then
	-- 	-- Preamble-IUT-Master
	-- 	Allow_Role_Switch = 1  -- allow role switch
	-- 	lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Master(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

	-- 	if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
	-- 		Test_Status="Fail"
	-- 		goto Test_Case_End
	-- 	end
	-- end


	-- if(role=="slave") then
	-- 	-- Preamble-IUT-Master
	-- 	Allow_Role_Switch = 1  -- allow role switch
	-- 	lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

	-- 	if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
	-- 		Test_Status="Fail"
	-- 		goto Test_Case_End
	-- 	end
	-- end
    -- --------------- --------------------------ACL connection established.---------------------------------------------
	print("============ iut tester Write_Inquiry_Scan_Activity:=============")

	Inquiry_Scan_Interval = 4096
	Inquiry_Scan_Window = 18
	HCI_Write_Inquiry_Scan_Activity(iut_handle_w,iut_mode,Inquiry_Scan_Interval,Inquiry_Scan_Window)
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end

	-- <= LT:HCI Write Scan Enable(Scan_Enable=3,Enable Inquiry scan and Page scan)
	Scan_Enable=1
	HCI_Write_Scan_Enable(iut_handle_w,iut_mode,Scan_Enable)
-- => iut:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end
	

for i=1,100 do


	print("============ lt tester as master ,enable inquiry:=============")
-- <= lt:HCI Inquiry
	LAP= 0x9e8b33
	Inquiry_Length = 0x10  -- 10*1.28s=12.8s
	Num_Responses  = 1
	HCI_Inquiry(lt_handle_w,lt_mode,LAP,Inquiry_Length,Num_Responses)
-- => lt:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		return nil,nil
	end
-- -- => lt:HCI Inquiry Result event
-- 	flag,rst_tbl = HCI_Wait_Inquiry_Result(lt_handle_r,lt_mode,Timeout)
-- 	if(flag~=true) then
-- 		return nil,nil
-- 	end	
-- -- => lt:HCI Inquiry Complete event
-- 	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Inquiry_Complete",Timeout)
-- 	if(flag~=true) then
-- 		return nil,nil
-- 	end

end


	::Test_Case_End::
	print("============ Test Result:"..Test_Status.." =============")
	-- Close Device
	Close_Device(iut_mode,lt_mode)
	return Test_Status
end


Total_times = 1
fail=0
success=0



for i=1,Total_times do

	flag = BB_PHYS_INQ_BV_10_C("dongle","usb","slave",70)
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

require("project.bt_controller_certification.4_6_Information_Request.DefaultSetting-Information_Requests")

--[[
	this function serves to check the testing scenarios that iut as slave and lt as master,
	master Initiate ROLE SWITCH REQ and slave receive this request， if succeeded, master 

]]


function LMP_INF_BV_19_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "LMP_INF_BV-19-C"
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

	
	simple_paring = 1
	-- Simple paring Settings
	flag = Default_Setting(iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,Timeout)

	if(flag==false) then
		Test_Status= "Fail"
		goto Test_Case_End
	end
     

     -- Lower Tester's LMP extended feature bit is not set
    print("============ Lower Tester's LMP extended feature bit is not set =============")


-- <= IUT:HCI_Set_Event_Mask
	Event_Mask = array.new(8);

	Event_Mask[1] = 0x40
	Event_Mask[2] = 0x00
	Event_Mask[3] = 0x00
	Event_Mask[4] = 0x00
	Event_Mask[5] = 0x00
	Event_Mask[6] = 0x00
	Event_Mask[7] = 0x00
	Event_Mask[8] = 0x10

	-- HCI_Set_Event_Mask(iut_handle_w,iut_mode,Event_Mask)
	-- -- => IUT:HCI_Command_Complete event
	-- flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	-- if(flag~=true) then
	-- 	return false
	-- end
	
	
    Page_Scan_Repetition_Mode = 0;
    Clock_Offset = 0;
	-- <= IUT:HCI_Read_Remote_Supported_Features
	HCI_Remote_Name_Request(iut_handle_w,iut_mode,LT_BD_Addr,Page_Scan_Repetition_Mode,Clock_Offset)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end


	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Remote_Name_Request_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end


	::Test_Case_End::
	print("============ Test Result:"..Test_Status.." =============")
	-- Close Device
	Close_Device(iut_mode,lt_mode)
	return Test_Status
end

-- lt device Initiate a role switch request

Total_times = 1
fail=0
success=0



for i=1,Total_times do

	flag = LMP_INF_BV_19_C("dongle","usb","master",10)

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


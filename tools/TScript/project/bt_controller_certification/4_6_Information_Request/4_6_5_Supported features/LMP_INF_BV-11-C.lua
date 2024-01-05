
require("project.bt_controller_certification.4_6_Information_Request.DefaultSetting-Information_Requests")

--[[
	this function serves to check the testing scenarios that iut as slave and lt as master,
	master Initiate ROLE SWITCH REQ and slave receive this request， if succeeded, master 

]]


function LMP_INF_BV_11_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "LMP_INF_BV-11-C"
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
	flag = Default_Setting(iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,Timeout)

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
    -- --------------- --------------------------ACL connection established.---------------------------------------------
	-- LMP_Features = array.new(8);

	-- for i=1,8 do
	-- 	LMP_Features[i] = 0x00  --para
	-- end
	-- -- <= IUT:HCI_AB_Write_Feature
	-- HCI_AB_Write_Feature(iut_handle_w,iut_mode,LMP_Features)

	
	-- -- => IUT:HCI Command Status abevent
	-- flag,rst_tbl = HCI_Wait_ABEvent_Done(iut_handle_r,iut_mode,"HCI_Abnormal_Write_Feature_Complete",Timeout)
	-- if(flag~=true) then
	-- 	Test_Status="Fail"
	-- 	goto Test_Case_End
	-- end

	-- <= IUT:HCI_Read_Remote_Supported_Features
	HCI_Read_Remote_Supported_Features(iut_handle_w,iut_mode,iut_Connection_Handle)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Read_Remote_Supported_Features_Complete",Timeout)
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

	flag = LMP_INF_BV_11_C("dongle","usb","slave",11)

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
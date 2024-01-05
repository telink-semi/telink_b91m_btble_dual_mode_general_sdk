
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")

function  LMP_LIH_BV_81_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "LMP/LIH/BV-81-C"
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
	flag = Default_Setting(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,Timeout)

	if(flag==false) then
		Test_Status= "Fail"
		goto Test_Case_End
	end

	if(role=="master") then
		-- Preamble-IUT-Master
		Allow_Role_Switch = 1  -- allow role switch
		lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Master(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

		if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
			Test_Status="Fail"
			goto Test_Case_End
		end
	end


	if(role=="slave") then
		-- Preamble-IUT-Master
		Allow_Role_Switch = 1  -- allow role switch
		lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

		if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
			Test_Status="Fail"
			goto Test_Case_End
		end
	end
    -- --------------- --------------------------ACL connection established.---------------------------------------------
	print("============ iut QoS start  =============")
	
	Service_Type 	= 1            	-- best effort
	Token_Rate 		= 0xffffffff   	-- TODO
	Peak_Bandwidth 	= 0xffffffff	-- TODO
	Latency 		= 0xffffffff	-- TODO
	Delay_Variation = 0xffffffff	-- TODO
	HCI_QoS_Setup(iut_handle_w,iut_mode,iut_Connection_Handle,Service_Type,Token_Rate,Peak_Bandwidth,Latency,Delay_Variation)

	print("============ lt feature start  =============")
	-- <= LT:HCI_Read_Remote_Supported_Features
	HCI_Read_Remote_Supported_Features(lt_handle_w,lt_mode,lt_Connection_Handle)


	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
	-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Read_Remote_Supported_Features_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_QoS_Setup_Complete",Timeout)
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


Total_times = 1
fail=0
success=0



for i=1,Total_times do

	flag = LMP_LIH_BV_81_C("dongle","usb","slave",10)
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
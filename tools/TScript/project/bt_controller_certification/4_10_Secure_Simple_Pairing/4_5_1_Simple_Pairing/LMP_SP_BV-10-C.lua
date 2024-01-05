
require("project.bt_controller_certification.4_10_Secure_Simple_Pairing.DefaultSetting-Simple_Pairing")

--[[
	this function serves to check the testing scenarios that iut as slave and lt as master,
	master Initiate ROLE SWITCH REQ and slave receive this request， if succeeded, master 

]]
   

function LMP_SP_BV_10_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "LMP/SP/BV-10-C"
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
	initial_authen = 1 
	
	flag = Default_Setting(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,initial_authen,Timeout)

	if(flag==false) then
		Test_Status= "Fail"
		goto Test_Case_End
	end

	-- <= IUT:HCI_Write_Simple_Pairing_Mode
	Simple_Pairing_Mode = 1;
	HCI_Write_Simple_Pairing_Mode(iut_handle_w,iut_mode,Simple_Pairing_Mode)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- <= LT:HCI_Write_Simple_Pairing_Mode
	Simple_Pairing_Mode = 1;
	HCI_Write_Simple_Pairing_Mode(lt_handle_w,lt_mode,Simple_Pairing_Mode)
	-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
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

	-- <= IUT:HCI_Authentication_Requested
	HCI_Authentication_Requested(iut_handle_w,iut_mode,iut_Connection_Handle)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
	
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Link_Key_Request",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- <= IUT:HCI_Link_Key_Request_Negative_Reply
	HCI_Link_Key_Request_Negative_Reply(iut_handle_w,iut_mode,rst_tbl.BD_ADDR)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => IUT:HCI PIN Code Request event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_IO_Capability_Request",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- <= IUT:HCI_IO_Capability_Request_Reply
	IO_Capability = 0x03
	OOB_Data_Present = 0x00
	Authentication_Requirement = 0x04 
	HCI_IO_Capability_Request_Reply(iut_handle_w, iut_mode, rst_tbl.BD_ADDR,IO_Capability,OOB_Data_Present,Authentication_Requirement)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => LT:HCI_IO_Capability_Request event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_IO_Capability_Request",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- <= IUT:HCI_IO_Capability_Request_Reply
	IO_Capability = 0x01
	OOB_Data_Present = 0x00
	Authentication_Requirement = 0x04 
	HCI_IO_Capability_Request_Reply(lt_handle_w, lt_mode, rst_tbl.BD_ADDR,IO_Capability,OOB_Data_Present,Authentication_Requirement)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => LT:HCI PIN Code Request event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_IO_Capability_Response",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => LT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_User_Confirmation_Request",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- <= LT:HCI_User_Confirmation_Request_Reply
	HCI_User_Confirmation_Request_Reply(iut_handle_w, iut_mode, rst_tbl.BD_ADDR)

	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => LT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_User_Confirmation_Request",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- <= LT:HCI_User_Confirmation_Request_Reply
	HCI_User_Confirmation_Request_Negative_Reply(lt_handle_w, lt_mode, rst_tbl.BD_ADDR)

	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Simple_Pairing_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Simple_Pairing_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => IUT:HCI Authentication Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Authentication_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	--<= LT:HCI_Disconnect
	Reason = 0x13
	HCI_Disconnect (lt_handle_w,lt_mode,lt_Connection_Handle,Reason)
	-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
	-- => IUT:HCI Disconnection Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Disconnection_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => IUT:HCI Disconnection Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Disconnection_Complete",Timeout)
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

	flag = LMP_SP_BV_10_C("dongle","usb","master",10)

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


for i=1,Total_times do

	flag = LMP_SP_BV_10_C("dongle","usb","slave",10)

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


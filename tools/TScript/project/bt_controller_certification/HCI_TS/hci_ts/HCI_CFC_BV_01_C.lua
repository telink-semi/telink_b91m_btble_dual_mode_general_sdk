
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")

function  HCI_CFC_BV_01_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "HCI/CFC/BV-01-C"
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
    print("============ Read Buffer Size  =============")
	HCI_Read_Buffer_size(iut_handle_w,iut_mode)
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end


	-- Default Settings
	flag = Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role,Timeout)

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
	print("============ ACL data start send 1:=============")
	Pb_Flag = 0x10
	data = array.new(8);
	for i=1,8 do
		data[i]=0x11
	end
	HCI_Acl_data_send(iut_handle_w,iut_mode,iut_Connection_Handle,Pb_Flag,8,data)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Num_Of_Commplete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

    print("============ ACL data start send 2:=============")
	data = array.new(8);
	for i=1,8 do
		data[i]=0x22
	end
	HCI_Acl_data_send(iut_handle_w,iut_mode,iut_Connection_Handle,Pb_Flag,8,data)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Num_Of_Commplete",Timeout)
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

	flag = HCI_CFC_BV_01_C("dongle","usb","slave",10)
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
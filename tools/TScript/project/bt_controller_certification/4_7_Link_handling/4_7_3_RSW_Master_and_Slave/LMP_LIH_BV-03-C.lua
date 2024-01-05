require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")


function  LMP_LIH_BV_03_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "LMP/LIH/BV-03-C"
	-- iut_mode = "dongle"
	-- lt_mode  = "usb"
	-- role     = "master"
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

	-- <= IUT:HCI_Write_Link_Policy_Settings
	Link_Policy_Settings = ROLE_SWITCH_ENABLE

	HCI_Write_Link_Policy_Settings(iut_handle_w,iut_mode,iut_Connection_Handle,Link_Policy_Settings)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
		goto Test_Case_End
	end

	-- <= LT:HCI_Write_Link_Policy_Settings
	Link_Policy_Settings = ROLE_SWITCH_ENABLE

	HCI_Write_Link_Policy_Settings(lt_handle_w,lt_mode,lt_Connection_Handle,Link_Policy_Settings)
	-- => LT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
		goto Test_Case_End
	end

	HCI_Read_Local_Support_Features(iut_handle_w,iut_mode)

	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
		goto Test_Case_End
	end


	LMP_Features = array.new(8);

	LMP_Features[1] = bit32.band(rst_tbl.Return_Parameters[2],0xdf)
	LMP_Features[2] = rst_tbl.Return_Parameters[3]
	LMP_Features[3] = rst_tbl.Return_Parameters[4]
	LMP_Features[4] = rst_tbl.Return_Parameters[5]
	LMP_Features[5] = rst_tbl.Return_Parameters[6]
	LMP_Features[6] = rst_tbl.Return_Parameters[7]
	LMP_Features[7] = rst_tbl.Return_Parameters[8]
	LMP_Features[8] = rst_tbl.Return_Parameters[9]

	-- modify role switch feature
	HCI_AB_Write_Feature(iut_handle_w,iut_mode,LMP_Features)
	flag,rst_tbl = HCI_Wait_ABEvent_Done(iut_handle_r,iut_mode,"HCI_Abnormal_Write_Feature_Complete",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
		goto Test_Case_End
	end

	-- <= LT:HCI_Switch_Role
	for i=1,6 do
		BD_ADDR[i] = IUT_BD_Addr[i]
	end

	Role = 0 -- 0:slave->master,1:master->slave
	HCI_Switch_Role(lt_handle_w,lt_mode,BD_ADDR,Role)
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
		goto Test_Case_End
	end

	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Role_Change",Timeout)
	if(flag~=true) then
		Test_Status = "Fail"
		goto Test_Case_End
	end

	if(rst_tbl.Status~=0x1A) then
		Test_Status = "Fail"
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

LMP_LIH_BV_03_C("dongle","usb","master",10)
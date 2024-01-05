require("project.bt_controller_certification.4_4_Authentication.config_app")

function Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role,initial_authen,Timeout)
	print("============ Default Setting: LinkHandling  =============")

	-- <= IUT:HCI_Reset
	-- HCI_Reset(iut_handle_w,iut_mode)
	-- => IUT:HCI Command Complete event
	-- flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	-- if(flag~=true) then
	-- 	return false
	-- end

	if(IXIT==1) then
		-- <= IUT:HCI_Write_PIN_Type
		PIN_Type = 0 -- 0-> variable PIN, 1-> fixed PIN
		HCI_Write_PIN_Type(iut_handle_w,iut_mode,PIN_Type)
		-- => IUT:HCI Command Complete event(Slave)
		flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
			return false
		end
	end

	if(role == "slave") then
		Scan_Enable = 3 -- enable inquiry scan and page scan
	else
		Scan_Enable = 0
	end	
	
	-- <= IUT:HCI_Write_Scan_Enable(Slave)
	HCI_Write_Scan_Enable(iut_handle_w,iut_mode,Scan_Enable)
	-- => IUT:HCI Command Complete event(Slave)
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	
	if(role == "master" and initial_authen == 1) then
		-- <= IUT:HCI_Write_Authentication_Enable
		Authentication_Enable = 1 -- Authentication_Enabled
	else
		Authentication_Enable = 0 -- Authentication_Disabled
	end

	HCI_Write_Authentication_Enable(iut_handle_w,iut_mode,Authentication_Enable)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end

	if(IXIT==1) then
		
		for i=1,6 do
			BD_ADDR[i] = LT_BD_Addr[i] 
		end
		
		Delete_All = 0  -- 0-delete specified BD_ADDR's link key, 1-delete all link key

		HCI_Delete_Stored_Link_Key(iut_handle_w,iut_mode,BD_ADDR,Delete_All)

		-- => IUT:HCI Command Complete event
		flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
			return false
		end
	end

	-- <= IUT:HCI_Write_Local_Name
	Local_Name = "IUT0"
	HCI_Write_Local_Name(iut_handle_w,iut_mode,Local_Name)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end

	-- <= IUT:HCI_Set_Event_Mask
	Event_Mask = array.new(8);

	Event_Mask[1] = 0x1D
	Event_Mask[2] = 0xBF
	Event_Mask[3] = 0xF8
	Event_Mask[4] = 0x07
	Event_Mask[5] = 0xBF
	Event_Mask[6] = 0xFB
	Event_Mask[7] = 0x9F
	Event_Mask[8] = 0xFF

	HCI_Set_Event_Mask(iut_handle_w,iut_mode,Event_Mask)
	-- => IUT:HCI_Command_Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	return true
end
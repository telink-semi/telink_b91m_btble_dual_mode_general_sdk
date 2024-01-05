require("project.bt_controller_certification.4_6_Information_Request.config_app")

function Default_Setting(iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,Timeout)

			-- <= IUT:HCI_Reset
	HCI_Reset(iut_handle_w,iut_mode)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	
	-- <= IUT:HCI_Reset
	HCI_Reset(lt_handle_w,lt_mode)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end



	if(role == "slave") then
		Scan_Enable = 3 -- enable inquiry scan and page scan
			-- <= IUT:HCI_Write_Scan_Enable(Slave)
		print("============ iut_mode: Scan_Enable  =============")

		HCI_Write_Scan_Enable(iut_handle_w,iut_mode,Scan_Enable)
		-- => IUT:HCI Command Complete event(Slave)
		flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
		return false
		end
	else
		print("============ lt_handle_w: Scan_Enable  =============")

		Scan_Enable = 3
		-- <= IUT:HCI_Write_Scan_Enable(Slave)
		HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
		-- => IUT:HCI Command Complete event(Slave)
		flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
		return false
		end
	end	
	
	-- <= IUT:HCI_Write_Authentication_Enable
	Authentication_Enable = 0 -- Authentication_Disabled
	HCI_Write_Authentication_Enable(iut_handle_w,iut_mode,Authentication_Enable)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end

	-- <= IUT:HCI_Write_Local_Name
	Local_Name = "IUT0"
	HCI_Write_Local_Name(iut_handle_w,iut_mode,Local_Name)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	if(simple_paring == 1) then
	print("============ Simple paring Setting: Information_Request  =============")
	else
	print("============ Default Setting: Information_Request  =============")

	-- <= IUT:HCI_Set_Event_Mask
	Event_Mask = array.new(8);

	Event_Mask[1] = 0xFF
	Event_Mask[2] = 0x9F
	Event_Mask[3] = 0xFB
	Event_Mask[4] = 0xBF
	Event_Mask[5] = 0x07
	Event_Mask[6] = 0xF8
	Event_Mask[7] = 0xBF
	Event_Mask[8] = 0x1D

	HCI_Set_Event_Mask(iut_handle_w,iut_mode,Event_Mask)

	-- => IUT:HCI_Command_Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	end
	return true
end

function Simple_paring_Setting(iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,Timeout)

	print("============ Simple paring Setting: Information_Request  =============")

	-- <=IUT:HCI_Reset
	HCI_Reset(iut_handle_w,iut_mode)
	-- => IUT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	-- <= LT:HCI Reset
		HCI_Reset(lt_handle_w,lt_mode)
	-- => LT:HCI Command Complete event
		flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
			return nil,nil
		end
	
	
	if(role == "slave") then
		Scan_Enable = 3 -- enable inquiry scan and page scan
			-- <= IUT:HCI_Write_Scan_Enable(Slave)
		print("============ iut_mode: Scan_Enable  =============")

		HCI_Write_Scan_Enable(iut_handle_w,iut_mode,Scan_Enable)
		-- => IUT:HCI Command Complete event(Slave)
		flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
		return false
		end
	else
		print("============ lt_handle_w: Scan_Enable  =============")

		Scan_Enable = 3
		-- <= IUT:HCI_Write_Scan_Enable(Slave)
		HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
		-- => IUT:HCI Command Complete event(Slave)
		flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
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

	
end
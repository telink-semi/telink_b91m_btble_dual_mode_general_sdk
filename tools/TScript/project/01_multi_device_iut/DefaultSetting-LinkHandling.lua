require("printf")
require("hci_func")
require("time")

function Default_Setting(iut_handle_r, iut_handle_w,iut_mode,role,Timeout)
	print("============ Default Setting: LinkHandling  =============")

	-- <= IUT:HCI_Reset
	-- HCI_Reset(iut_handle_w,iut_mode)
	-- => IUT:HCI Command Complete event
	-- HCI_Wait_Command_Complete(iut_handle_r,iut_mode)


	if(role == "slave") then
		Scan_Enable = 3 -- enable inquiry scan and page scan
		-- <= IUT:HCI_Write_Scan_Enable(Slave)
		HCI_Write_Scan_Enable(iut_handle_w,iut_mode,Scan_Enable)
		-- => IUT:HCI Command Complete event(Slave)
		flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
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
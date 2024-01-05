require("device")

function Preamble_IUT_Slave(iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

	print("============ Preamble: IUT AS Slave  =============")
	if(lt_mode=="usb") then
	-- <= LT:HCI Reset
		HCI_Reset(lt_handle_w,lt_mode)
	-- => LT:HCI Command Complete event
		flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
			return nil,nil
		end
	end
-- <= LT:HCI Inquiry
	LAP= 0x9e8b33
	Inquiry_Length = 10  -- Inquiry_Length: 10*1.28s=12.8s
	Num_Responses  = 1   
	HCI_Inquiry(lt_handle_w,lt_mode,LAP,Inquiry_Length,Num_Responses)
-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		return nil,nil
	end
-- => LT:HCI Inquiry Result event
	flag,rst_tbl = HCI_Wait_Inquiry_Result(lt_handle_r,lt_mode,Timeout)
	if(flag~=true) then
		return nil,nil
	end	
-- => LT:HCI Inquiry Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Inquiry_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end
-- <= LT:HCI Create Connection
	BD_ADDR = array.new(6);
	for i=1,6 do
		BD_ADDR[i] = IUT_BD_Addr[i]
	end
	Packet_Type = 0xCC18  				-- DM1
	Page_Scan_Repetition_Mode = 0x02  	-- R1
	Clock_Offset = 0x0000				-- 0
	-- Allow_Role_Switch = 0x00 			-- local device will be master,not allow role switch

	HCI_Create_Connection(lt_handle_w,lt_mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
Clock_Offset,Allow_Role_Switch)
-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		return nil,nil
	end
-- => IUT:HCI_Connection_Request
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Connection_Request",Timeout)
	if(flag~=true) then
		return nil,nil
	end	
	-- <= IUT:HCI_Write_Scan_Enable(Slave)
	if(IXIT==1) then
		Scan_Enable = 0 -- disable inquiry scan and page scan		
		HCI_Write_Scan_Enable(iut_handle_w,iut_mode,Scan_Enable)
	-- => IUT:HCI Command Complete event(Slave)
		HCI_Wait_Command_Complete(iut_handle_r,iut_mode)		
	end
-- <= IUT:HCI_Accept_Connection_Request
	if(flag==true and iut_mode~="unused") then
		if(Allow_Role_Switch==1)then
			role=0  -- 0->device to be connected will be master ,LM will perform role switch
		else
	    	role=1  -- 1->device to be connected is still slave ,Do not allow role switch
	    end
		HCI_Accept_Connection_Request(iut_handle_w,iut_mode,rst_tbl.BD_ADDR,role);
		-- => IUT:HCI Command Status event	
		flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
		if(flag~=true) then
			return nil,nil
		end		
	end


-- => LT:HCI Connection complete event
	lt_flag,lt_rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Complete",Timeout)
	if(lt_flag~=true) then
		return nil,nil
	elseif(lt_rst_tbl~=nil) then	
		lt_Connection_Handle = bit32.band(lt_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(lt_rst_tbl.Connection_Handle[2],8),0xff)
	end
-- => IUT:HCI Connection complete event
	iut_flag,iut_rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Connection_Complete",Timeout)
	if(iut_flag~=true) then
		return nil,nil
	elseif(iut_rst_tbl~=nil) then
		iut_Connection_Handle = bit32.band(iut_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(iut_rst_tbl.Connection_Handle[2],8),0xff)
	end

	log_printf(HCI_PROCESS_STATUS_DISPLAY,"HCI Connection Setup",0,0)

-- => IUT:Wait for other event	
	HCI_Wait_Other_Event(iut_handle_r,iut_mode)
-- => LT:Wait for other event
	HCI_Wait_Other_Event(lt_handle_r,lt_mode)
--$$ The following flow will be only used for the Authentication and Encryption test cases $$--

-- => IUT:HCI Link Key Request event

-- <= IUT:HCI_Link_Key_Request_Negative_Reply

-- => IUT:HCI Command Complete event

-- => IUT:HCI PIN Code Request event

-- <= IUT:HCI_PIN_Code_Request_Reply

-- => IUT:HCI Command Complete event

	return lt_Connection_Handle,iut_Connection_Handle
end
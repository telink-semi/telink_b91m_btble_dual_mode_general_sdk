require("device")

function Preamble_IUT_Slave(iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch)

	print("============ Preamble: IUT AS Slave  =============")
	lt_Connection_Handle = 0;
	iut_Connection_Handle = 0;
-- <= LT:HCI Reset
	HCI_Reset(lt_handle_w,lt_mode)
-- => LT:HCI Command Complete event
	HCI_Wait_Command_Complete(lt_handle_r,lt_mode)

-- <= LT:HCI Inquiry
	LAP= 0x9e8b33
	Inquiry_Length = 10  -- 10*1.28s=12.8s
	Timeout = 13		 -- >Inquiry_Length
	Num_Responses  = 1   
	HCI_Inquiry(lt_handle_w,lt_mode,LAP,Inquiry_Length,Num_Responses)
-- => LT:HCI Command Status event
	HCI_Wait_Command_Status(lt_handle_r,lt_mode)
-- => LT:HCI Inquiry Result event
	HCI_Wait_Inquiry_Result(lt_handle_w,lt_mode,Timeout)
-- => LT:HCI Inquiry Complete event
	HCI_Wait_Inquiry_Complete(lt_handle_r,lt_mode)

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
	HCI_Wait_Command_Status(lt_handle_r,lt_mode)
-- => IUT:HCI_Connection_Request
	flag,bd_addr = HCI_Wait_Connect_Request(iut_handle_r,iut_mode)

	-- <= IUT:HCI_Write_Scan_Enable(Slave)
	if(IXIT==1) then
		Scan_Enable = 0 -- disable inquiry scan and page scan		
		HCI_Write_Scan_Enable(iut_handle_w,iut_mode,Scan_Enable)
	-- => IUT:HCI Command Complete event(Slave)
		HCI_Wait_Command_Complete(iut_handle_r,iut_mode)		
	end
-- <= IUT:HCI_Accept_Connection_Request
	if(flag==true) then
    role=1  -- 1->device to be connected is still slave ,Do not allow role switch
            -- 0->device to be connected will be master ,LM will perform role switch
	HCI_Accept_Connection_Request(iut_handle_w,iut_mode,bd_addr,role);
	-- => IUT:HCI Command Status event
	HCI_Wait_Command_Status(iut_handle_r,iut_mode)	
	end


-- => LT:HCI Connection complete event
	lt_flag,lt_rst_tbl = HCI_Wait_Connect_Complete(lt_handle_r,lt_mode)

-- => IUT:HCI Connection complete event
	iut_flag,iut_rst_tbl = HCI_Wait_Connect_Complete(iut_handle_r,iut_mode)
	
		
	if(lt_flag==true and iut_flag==true) then
	    lt_Connection_Handle = bit32.band(lt_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(lt_rst_tbl.Connection_Handle[2],8),0xff)
		iut_Connection_Handle = bit32.band(iut_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(iut_rst_tbl.Connection_Handle[2],8),0xff)
		log_printf(HCI_PROCESS_STATUS_DISPLAY,"HCI Connection Setup",0,0)
	end
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
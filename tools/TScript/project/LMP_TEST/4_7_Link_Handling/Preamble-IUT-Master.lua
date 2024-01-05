require("device")

function Preamble_IUT_Master(iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch)

	print("============ Preamble: IUT AS Master  =============")

-- <= LT:HCI Reset
	HCI_Reset(lt_handle_w,lt_mode)
-- => LT:HCI Command Complete event
	HCI_Wait_Command_Complete(lt_handle_r,lt_mode)
-- <= LT:HCI Write Scan Enable(Scan_Enable=3,Enable Inquiry scan and Page scan)
	Scan_Enable=3
	HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
-- => LT:HCI Command Complete event
	HCI_Wait_Command_Complete(lt_handle_r,lt_mode)
-- <= IUT:HCI Inquiry
	LAP= 0x9e8b33
	Inquiry_Length = 10  -- 10*1.28s=12.8s
	Timeout = 13		 -- >Inquiry_Length
	Num_Responses  = 1   
	HCI_Inquiry(iut_handle_w,iut_mode,LAP,Inquiry_Length,Num_Responses)
-- => IUT:HCI Command Status event
	HCI_Wait_Command_Status(iut_handle_r,iut_mode)
-- => IUT:HCI Inquiry Result event
	HCI_Wait_Inquiry_Result(iut_handle_r,iut_mode,Timeout)
-- => IUT:HCI Inquiry Complete event
	HCI_Wait_Inquiry_Complete(iut_handle_r,iut_mode)
-- <= IUT:HCI Create Connection
	BD_ADDR = array.new(6);
	for i=1,6 do
		BD_ADDR[i] = LT_BD_Addr[i]
	end
	Packet_Type = 0x0008  				-- DM1
	Page_Scan_Repetition_Mode = 0x01  	-- R1
	Clock_Offset = 0x0000				-- 0

	HCI_Create_Connection(iut_handle_w,iut_mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
Clock_Offset,Allow_Role_Switch)
-- => IUT:HCI Command Status event
	HCI_Wait_Command_Status(iut_handle_r,iut_mode)
-- => LT:HCI_Connection_Request
	flag,bd_addr = HCI_Wait_Connect_Request(lt_handle_r,lt_mode)
-- <= LT:HCI_Accept_Connection_Request
	if(flag==true) then
    role=1  -- 1->device to be connected is still slave ,Do not allow role switch
            -- 0->device to be connected will be master ,LM will perform role switch
	HCI_Accept_Connection_Request(lt_handle_w,lt_mode,bd_addr,role);
	-- => LT:HCI Command Status event
	HCI_Wait_Command_Status(lt_handle_r,lt_mode)
	end
-- => IUT:HCI Connection complete event
	iut_flag,iut_rst_tbl = HCI_Wait_Connect_Complete(iut_handle_r,iut_mode)
	iut_Connection_Handle = bit32.band(iut_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(iut_rst_tbl.Connection_Handle[2],8),0xff)
-- => LT:HCI Connection complete event
	lt_flag,lt_rst_tbl = HCI_Wait_Connect_Complete(lt_handle_r,lt_mode)
	lt_Connection_Handle = bit32.band(lt_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(lt_rst_tbl.Connection_Handle[2],8),0xff)

	if(lt_flag==true and iut_flag==true) then
		log_printf(HCI_PROCESS_STATUS_DISPLAY,"HCI Connection Setup",0,0)
	end
-- => LT:Wait for other event
	HCI_Wait_Other_Event(lt_handle_r,lt_mode)
-- => IUT:Wait for other event	
	HCI_Wait_Other_Event(iut_handle_r,iut_mode)
	
--$$ The following flow will be only used for the Authentication and Encryption test cases $$--

-- => IUT:HCI Link Key Request event

-- <= IUT:HCI_Link_Key_Request_Negative_Reply

-- => IUT:HCI Command Complete event

-- => IUT:HCI PIN Code Request event

-- <= IUT:HCI_PIN_Code_Request_Reply

-- => IUT:HCI Command Complete event
	return lt_Connection_Handle,iut_Connection_Handle
end




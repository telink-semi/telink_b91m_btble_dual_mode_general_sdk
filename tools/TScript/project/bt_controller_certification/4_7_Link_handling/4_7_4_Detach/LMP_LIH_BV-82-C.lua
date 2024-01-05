require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")

function LMP_LIH_BV_82_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "LMP/LIH/BV-82-C"

	OPTIONAL = ALT1

	-- Test Begin
	::Test_Case_Begin::
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

	-- <= LT:HCI Write Scan Enable(Scan_Enable=3,Enable Inquiry scan and Page scan)
	Scan_Enable=3
	HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
	-- => LT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	if(OPTIONAL==ALT1)then
	-- <= IUT:HCI Inquiry
		LAP= 0x9e8b33
		Inquiry_Length = 10  -- 10*1.28s=12.8s
		Timeout = 13		 -- >Inquiry_Length
		Num_Responses  = 1   
		HCI_Inquiry(iut_handle_w,iut_mode,LAP,Inquiry_Length,Num_Responses)
	-- => IUT:HCI Command Status event
		flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
		if(flag~=true) then
			Test_Status="Fail"
			goto Test_Case_End
		end

	-- => IUT:HCI Inquiry Result event
		flag,rst_tbl = HCI_Wait_Inquiry_Result(iut_handle_r,iut_mode,Timeout)
		if(flag~=true) then
			Test_Status="Fail"
			goto Test_Case_End
		end
	-- => IUT:HCI Inquiry Complete event
	    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Inquiry_Complete",Timeout)
	    if(flag~=true) then
			Test_Status="Fail"
			goto Test_Case_End
		end
	end

	-- <= IUT:HCI Create Connection
	BD_ADDR = array.new(6);
	for i=1,6 do
		BD_ADDR[i] = LT_BD_Addr[i]
	end
	Packet_Type = 0x0008  				-- DM1
	Page_Scan_Repetition_Mode = 0x01  	-- R1
	Clock_Offset = 0x0000				-- 0
	Allow_Role_Switch = 0				-- not allow
	HCI_Create_Connection(iut_handle_w,iut_mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
	Clock_Offset,Allow_Role_Switch)
	-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
	-- => LT:HCI_Connection_Request
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Request",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- <= LT:HCI_Reject_Connection_Request
	reason=0x0d  
	        
	HCI_Reject_Connection_Request(lt_handle_w,lt_mode,rst_tbl.BD_ADDR,reason);
	-- => LT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- => IUT:HCI Connection complete event
	iut_flag,iut_rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Connection_Complete",Timeout)

	if(iut_flag~=true) then
		return nil,nil
	elseif(iut_rst_tbl~=nil) then
		iut_Connection_Handle = bit32.band(iut_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(iut_rst_tbl.Connection_Handle[2],8),0xff00)
		log_printf(HCI_PROCESS_STATUS_DISPLAY,"iut_Connection_Handle",iut_rst_tbl.Connection_Handle,2)
	end

	-- => LT:HCI Connection complete event
	lt_flag,lt_rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Complete",Timeout)

	if(lt_flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	elseif(lt_rst_tbl~=nil) then	
		lt_Connection_Handle = bit32.band(lt_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(lt_rst_tbl.Connection_Handle[2],8),0xff00)
		log_printf(HCI_PROCESS_STATUS_DISPLAY,"lt_Connection_Handle",lt_rst_tbl.Connection_Handle,2)
	end

	-- => IUT:HCI Disconnection Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Disconnection_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- Wait for 3 second

	delay_sec(3)

	-- <= LT:HCI_Read_Remote_Supported_Features
	HCI_Read_Remote_Supported_Features(lt_handle_w,lt_mode,lt_Connection_Handle)
	-- => LT:HCI_Read_Remote_Supported_Features_Complete
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",1)
	if(flag~=true or rst_tbl.Status~=2) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	::Test_Case_End::
	print("============ Test Result:"..Test_Status.."  =============")
	-- Close Device
	Close_Device(iut_mode,lt_mode)

	return Test_Status
end


LMP_LIH_BV_82_C("dongle","usb","master",10)
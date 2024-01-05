
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")
require("project.bt_controller_certification.lib.device")
function  BB_PHYS_PAG_BV_18_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "BB/PHYS/PAG/BV-01-C"
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



	print("============ Preamble: IUT AS Master  =============")
	if(lt_mode=="usb") then
	-- <= LT:HCI Reset
		HCI_Reset(lt_handle_w,lt_mode)
	-- => LT:HCI Command Complete event
		flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
		if(flag~=true) then
			return nil,nil
		end
	end
-- <= LT:HCI Write Scan Enable(Scan_Enable=3,Enable Inquiry scan and Page scan)
	Scan_Enable=3
	HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
-- => LT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end

-- -- <= IUT:HCI Inquiry
-- 	LAP= 0x9e8b33
-- 	Inquiry_Length = 10  -- 10*1.28s=12.8s
-- 	Num_Responses  = 1  
-- 	HCI_Inquiry(iut_handle_w,iut_mode,LAP,Inquiry_Length,Num_Responses)
-- -- => IUT:HCI Command Status event
-- 	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
-- 	if(flag~=true) then
-- 		return nil,nil
-- 	end
-- -- => IUT:HCI Inquiry Result event
-- 	flag,rst_tbl = HCI_Wait_Inquiry_Result(iut_handle_r,iut_mode,Timeout)
-- 	if(flag~=true) then
-- 		return nil,nil
-- 	end	
-- -- => IUT:HCI Inquiry Complete event
-- 	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Inquiry_Complete",Timeout)
-- 	if(flag~=true) then
-- 		return nil,nil
-- 	end

-- <= IUT:HCI Create Connection
    page_timeout= 0x2800
	HCI_Write_Page_Timeout(iut_handle_w,iut_mode,page_timeout)
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end
    
    ex_page_timeout =0
	HCI_Write_Extended_Page_Timeout(iut_handle_w,iut_mode,ex_page_timeout)

	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end
	BD_ADDR = array.new(6);
	for i=1,6 do
		BD_ADDR[i] = LT_BD_Addr[i]
	end

	Packet_Type = 0xcc18  				-- DM1
	Page_Scan_Repetition_Mode = 0x00  	-- R0
	Clock_Offset = 0x0000				-- 0
    Allow_Role_Switch = 0
	HCI_Create_Connection(iut_handle_w,iut_mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
Clock_Offset,Allow_Role_Switch)
-- => IUT:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(rst_tbl.Status~=0) then
		return nil,nil
	end
-- => LT:HCI_Connection_Request
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Request",Timeout)
	if(flag~=true) then
		return nil,nil
	end	
-- <= LT:HCI_Accept_Connection_Request
	if(flag==true and lt_mode~="unused") then
		if(Allow_Role_Switch==1)then
			role=0  -- 0->device to be connected will be master ,LM will perform role switch
		else
	    	role=1  -- 1->device to be connected is still slave ,Do not allow role switch
	    end
		HCI_Accept_Connection_Request(lt_handle_w,lt_mode,rst_tbl.BD_ADDR,role);
		-- => LT:HCI Command Status event
		flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
		if(flag~=true) then
			return nil,nil
		end
	end

-- -- => IUT:HCI Connection complete event
-- 	iut_flag,iut_rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Connection_Complete",Timeout)
-- 	if(iut_flag~=true) then
-- 		return nil,nil
-- 	elseif(iut_rst_tbl~=nil) then
-- 		iut_Connection_Handle = bit32.band(iut_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(iut_rst_tbl.Connection_Handle[2],8),0xff00)
-- 		log_printf(HCI_PROCESS_STATUS_DISPLAY,"iut_Connection_Handle",iut_rst_tbl.Connection_Handle,2)
-- 	end	

-- -- => LT:HCI Connection complete event
-- 	lt_flag,lt_rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Complete",Timeout)
-- 	if(lt_flag~=true) then
-- 		return nil,nil
-- 	elseif(lt_rst_tbl~=nil) then
-- 		lt_Connection_Handle = bit32.band(lt_rst_tbl.Connection_Handle[1],0xff) + bit32.band(bit32.lshift(lt_rst_tbl.Connection_Handle[2],8),0xff00)	
-- 		log_printf(HCI_PROCESS_STATUS_DISPLAY,"lt_Connection_Handle",lt_rst_tbl.Connection_Handle,2)
-- 	end	

-- 	log_printf(HCI_PROCESS_STATUS_DISPLAY,"HCI Connection Setup",0,0)

	-- <= IUT:HCI_Write_Scan_Enable(Slave)
	HCI_Write_Scan_Enable(lt_handle_w,lt_mode,0)
	-- => IUT:HCI Command Complete event(Slave)
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end

-- => LT:Wait for other event
	HCI_Wait_Other_Event(lt_handle_r,lt_mode)
-- => IUT:Wait for other event	
	HCI_Wait_Other_Event(iut_handle_r,iut_mode)


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

	flag = BB_PHYS_PAG_BV_18_C("dongle","usb","master",10)
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
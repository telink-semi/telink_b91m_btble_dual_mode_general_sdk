
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")

function  BB_PHYS_INQ_BV_16_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "BB/PHYS/INQ/BV-16-C"
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



	print("============ lt tester enable inquiry scan:=============")

	Inquiry_Scan_Interval = 4096
	Inquiry_Scan_Window = 18
	HCI_Write_Inquiry_Scan_Activity(lt_handle_w,lt_mode,Inquiry_Scan_Interval,Inquiry_Scan_Window)
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end

	-- <= LT:HCI Write Scan Enable(Scan_Enable=3,Enable Inquiry scan and Page scan)
	-- 	<= IUT:HCI_Write_Local_Name
	-- Local_Name = "ABCDE"
	Local_Name = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	HCI_Write_Local_Name(lt_handle_w,lt_mode,Local_Name)
	-- => lT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	HCI_Read_Local_Name(lt_handle_w,lt_mode)

	-- => lT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	Scan_Enable=1
	HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
-- => lt:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end
	



	print("============ iut tester as master ,enable inquiry:=============")
	print("============ set event mask:=============")
-- <= IUT:HCI_Set_Event_Mask
	Event_Mask = array.new(8);

	Event_Mask[1] = 0xFF
	Event_Mask[2] = 0xFF
	Event_Mask[3] = 0xFF
	Event_Mask[4] = 0xFF
	Event_Mask[5] = 0xFF
	Event_Mask[6] = 0x5f
	Event_Mask[7] = 0x00
	Event_Mask[8] = 0x00

	HCI_Set_Event_Mask(iut_handle_w,iut_mode,Event_Mask)
	-- => IUT:HCI_Command_Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	print("============ set inquiry mode :0x02 =============")

	HCI_Write_Inquiry_Mode(iut_handle_w,iut_mode,0x02)

	-- => IUT:HCI_Command_Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end
	print("============ read local feature =============")

	HCI_Read_Local_Support_Features(iut_handle_w,iut_mode)

	-- => IUT:HCI_Command_Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return false
	end


	for i=1,5 do

-- <= iut:HCI Inquiry
	LAP= 0x9e8b33
	Inquiry_Length = 0x10  -- 10*1.28s=12.8s
	Num_Responses  = 1
	HCI_Inquiry(iut_handle_w,iut_mode,LAP,Inquiry_Length,Num_Responses)
-- => iut:HCI Command Status event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		return nil,nil
	end
	print("============ HCI_Wait_Inquiry_Resuiut:=============")
-- => iut:HCI Inquiry Resuiut event
	flag,rst_tbl = HCI_Wait_Inquiry_Result(iut_handle_r,iut_mode,Timeout)
	if(flag~=true) then
		return nil,nil
	end	
-- => iut:HCI Inquiry Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Inquiry_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end
	delay_sec(1)
	Scan_Enable=1
	HCI_Write_Scan_Enable(lt_handle_w,lt_mode,Scan_Enable)
-- => lt:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end

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

	flag = BB_PHYS_INQ_BV_16_C("dongle","usb","slave",10)
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
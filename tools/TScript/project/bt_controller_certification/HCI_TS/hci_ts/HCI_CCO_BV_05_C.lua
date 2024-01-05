
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")

function  HCI_CCO_BV_05_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "HCI/CCO/BV-05-C"
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
    print("============ HCI_LE_Set_Event_MasK  =============")
		-- <= IUT:HCI_Set_Event_Mask
	Event_Mask = array.new(8);

	Event_Mask[1] = 0xFF
	Event_Mask[2] = 0xFF
	Event_Mask[3] = 0xFF
	Event_Mask[4] = 0xFF
	Event_Mask[5] = 0xFF
	Event_Mask[6] = 0xFF
	Event_Mask[7] = 0x00
	Event_Mask[8] = 0x00

	HCI_LE_Set_Event_Mask(iut_handle_w,iut_mode,Event_Mask)
	-- => IUT:HCI_Command_Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
	
	::Test_Case_End::
	print("============ Test Result:"..Test_Status.." ============")
	-- Close Device
	Close_Device(iut_mode,lt_mode)
	return Test_Status
end


Total_times = 1
fail=0
success=0



for i=1,Total_times do

	flag = HCI_CCO_BV_05_C("dongle","unused","slave",10)
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
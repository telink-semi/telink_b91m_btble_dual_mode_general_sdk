
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")

function  LMP_LIH_BV_126_C(iut_mode,lt_mode,role,Timeout)

	Testcase = "LMP/LIH/BV-126-C"
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


	-- Default Settings
	flag = Default_Setting(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,Timeout)

	if(flag==false) then
		Test_Status= "Fail"
		goto Test_Case_End
	end

	if(role=="master") then
		-- Preamble-IUT-Master
		Allow_Role_Switch = 1  -- allow role switch
		lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Master(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

		if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
			Test_Status="Fail"
			goto Test_Case_End
		end
	end


	if(role=="slave") then
		-- Preamble-IUT-Master
		Allow_Role_Switch = 1  -- allow role switch
		lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

		if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
			Test_Status="Fail"
			goto Test_Case_End
		end
	end
    -- --------------- --------------------------ACL connection established.---------------------------------------------
	
	-- --------------- --------------------------STP1: stop polling IUT--------------------------------------------------
	print("============ STP1: stop polling IUT =============")

    print("============ please power off/on master <LT> =============")



    delay_sec(30)
    
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Disconnection_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end
    
	-- --------------- --------------------------STP2: Establish new connection----------------------------------------------
	print("============ STP2: Establish new connection =============")
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

	if(flag==false) then
		Test_Status= "Fail"
		goto Test_Case_End
	end
	-- Preamble-IUT-Slave
	Allow_Role_Switch = 0  -- allow role switch
	lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

	if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
		Test_Status="Fail"
		goto Test_Case_End
	end
	-- --------------- --------------------------SET EVENT MASK----------------------------------------------
	EVE_CODE = 0X38
	B_V = 1;
        HCI_AB_EVENT_MASK(iut_handle_w,iut_mode,EVE_CODE,B_V)
    	-- => IUT:HCI Command Status abevent
	flag,rst_tbl = HCI_Wait_ABEvent_Done(iut_handle_r,iut_mode,"HCI_Abnormal_EVENT_MASK_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
	end

	-- --------------- --------------------------STP3: set linksupvision timeout----------------------------------------------
	print("============STP3: set linksupvision timeout ==============")
	LSTO = 0X3E80
	-- <= LT:HCI_Write_Link_Supervision_Timeout
	HCI_Write_Link_Supervision_Timeout(lt_handle_w,lt_mode,lt_Connection_Handle,LSTO)
	-- => LT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Complete",Timeout)
	if(flag~=true) then
		return nil,nil
	end


	-- => LT:HCI Command Complete event
	flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Link_Supervision_Timeout_Changed",Timeout)
	if(flag~=true) then
		return nil,nil
	end
	-- --------------- --------------------------STP4: stop polling IUT--------------------------------------------------
	print("============ STP4: stop polling IUT ======================")


    print("============ please power off master  <LT> =============")

    delay_sec(15)
    
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Disconnection_Complete",Timeout)
	if(flag~=true) then
		Test_Status="Fail"
		goto Test_Case_End
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

	flag = LMP_LIH_BV_126_C("dongle","usb","slave",10)
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
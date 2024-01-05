require("printf")

function Open_Device(iut_mode,lt_mode)
	print("============ Initiate Device =============")
	local iut_ret=0
	local lt_ret=0
	if(iut_mode=="dongle") then
		-- Caracara as IUT
		-- kite usb dongle as the intermediary of HCI interface 
		-- This Tool as Upper Tester of IUT
		iut_usbid    = 0x01209218
		iut_handle_r = tl_usb_init_pid2(iut_usbid);
		iut_handle_w = tl_usb_init_pid2(iut_usbid); 
		tl_usb_bulk_monitor_start(iut_handle_r)
		tl_usb_bulk_bufmode_set(1)
		if(iut_handle_r==0 or iut_handle_w==0) then
			log_printf(1," Not Find dongle!",0,0)
			tl_usb_bulk_monitor_end()
			iut_ret = 1
		else
			log_printf(1,"Open iut device: dongle",0,0)
		end
		--[[
			----------    Function Map   -----------
			init:	tl_usb_init_pid(0x19218);
			start: 	tl_usb_bulk_monitor_start(iut_handle_r)
			read:  	r_tbl,r_len = tl_usb_bulk_read()
			write: 	len = tl_usb_bulk_out(iut_handle_w,array,array_len)
			end:	tl_usb_bulk_monitor_end()
		]]
	elseif(iut_mode=="uart") then
	-- Caracara as IUT
	-- Directly use UART as HCI interface 
	-- This Tool as Upper Tester of IUT	
		device_list,device_num=tl_rs232_list()
		if(device_num==0) then
			log_printf(1," Not Find UART!",0,0)
			iut_ret = 1
		else
    -- baudrate_tbl: 6="9600",8="19200",12="115200",15="921600"
		log_printf(1,"Open iut device:"..device_list[1],0,0)
		tl_rs232_open(device_list[1],15) -- baudrate : 921600
		iut_handle_r = 1
		iut_handle_w = 1
		--[[
			----------    Function Map   -----------
			find:	tl_rs232_list();
			read:  	r_tbl,r_len = tl_rs232_recv()
			write: 	len = tl_rs232_send(array,array_len)
			end:	tl_rs232_close()
		]]	
		end
	elseif(iut_mode=="unused") then
		iut_ret=2	
	end
 
	if(lt_mode=="usb") then
		-- csr dongle as Lower Tester ---
		lt_handle = tl_btusb_init();
		lt_handle_r = lt_handle
		lt_handle_w = lt_handle
		--[[
			----------    Function Map   -----------
			init:	tl_btusb_init();
			read:  	r_tbl,r_len = tl_btusb_cmd_read(lt_handle)
			write: 	len = tl_btusb_cmd_write(lt_handle,array,array_len)
			end:	tl_btusb_close(lt_handle)
		]] 
		if(lt_handle==0) then
			tl_btusb_close(lt_handle)
			log_printf(1,"NOT Find CSR",0,0)
			lt_ret = 1
		else
			log_printf(1,"Open lt device:CSR",0,0)
		end
	elseif(lt_mode=="dongle") then
		-- Caracara as LT
		-- kite usb dongle as the intermediary of HCI interface 
		-- This Tool as Upper Tester of IUT
		lt_usbid    = 0x01309218
		lt_handle_r = tl_usb_init_pid2(lt_usbid);
		lt_handle_w = tl_usb_init_pid2(lt_usbid); 
		tl_usb_bulk_monitor_start(lt_handle_r)
		tl_usb_bulk_bufmode_set(1)
		if(lt_handle_r==0 or lt_handle_w==0) then
			log_printf(1," Not Find dongle!",0,0)
			tl_usb_bulk_monitor_end()
			lt_ret = 1
		else
			log_printf(1,"Open lt device: dongle",0,0)
		end
		--[[
			----------    Function Map   -----------
			init:	tl_usb_init_pid(0x19218);
			start: 	tl_usb_bulk_monitor_start(iut_handle_r)
			read:  	r_tbl,r_len = tl_usb_bulk_read()
			write: 	len = tl_usb_bulk_out(iut_handle_w,array,array_len)
			end:	tl_usb_bulk_monitor_end()
		]]
	elseif(lt_mode=="uart") then
	-- Caracara as IUT
	-- Directly use UART as HCI interface 
	-- This Tool as Upper Tester of IUT	
		device_list,device_num=tl_rs232_list()
		if(device_num==0) then
			log_printf(1," Not Find UART!",0,0)
			lt_ret = 1
		else
    -- baudrate_tbl: 6="9600",8="19200",12="115200",15="921600"
		log_printf(1,"Open lt device:"..device_list[1],0,0)
		tl_rs232_open(device_list[1],15) -- baudrate : 921600
		--[[
			----------    Function Map   -----------
			find:	tl_rs232_list();
			read:  	r_tbl,r_len = tl_rs232_recv()
			write: 	len = tl_rs232_send(array,array_len)
			end:	tl_rs232_close()
		]]	
		end
	elseif(lt_mode=="unused") then
		lt_ret=2
	end

	if(lt_ret==1 or iut_ret==1) then
		log_printf(1,"Open device Failed!",0,0)
		return nil,nil,nil,nil
	elseif(lt_ret==2 and iut_ret==2) then
		log_printf(1,"Do NOT use any device!",0,0)
		return nil,nil,nil,nil
	elseif((lt_ret==2 and iut_ret==0) or ((lt_ret==0 and iut_ret==2))) then
		log_printf(1,"Single-point Testcase, Open device Succeed!",0,0)
	else
		log_printf(1,"Double-point Testcase, Open device Succeed!",0,0)
	end

	return iut_handle_r, iut_handle_w, lt_handle_r, lt_handle_w
end

function Close_Device(iut_mode,lt_mode)
	print("============ Close Device =============")
	if(lt_mode=="usb") then
		tl_btusb_close(lt_handle)
	end

	if(lt_mode=="dongle" or iut_mode=="dongle")then
		tl_usb_bulk_monitor_end()
	end

	if(iut_mode=="uart") then
		tl_rs232_close()
	end
end

-- Bluetooth address of Lower Tester 
LT_BD_Addr = {0x13,0x71, 0xda, 0x7d, 0x1a, 0x00}

-- Bluetooth address of IUT 
IUT_BD_Addr= {0x2f, 0xcd, 0xc3, 0x67, 0xb5, 0x00}

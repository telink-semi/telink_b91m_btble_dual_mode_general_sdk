require("printf")

function Open_Device()
	print("============ Initiate Device =============")
	-- Caracara as IUT
	-- kite usb dongle as the intermediary of HCI interface 
	-- This Tool as Upper Tester of IUT
	iut_handle_r = tl_usb_init2(0x05d8);
	iut_handle_w = tl_usb_init2(0x05d8); 
	tl_usb_bulk_monitor_start(iut_handle_r)
	tl_usb_bulk_bufmode_set(1)

	--[[
		----------    Function Map   -----------
		init:	tl_usb_init2(0x05d8);
		start: 	tl_usb_bulk_monitor_start(iut_handle_r)
		read:  	r_tbl,r_len = tl_usb_bulk_read()
		write: 	len = tl_usb_bulk_out(iut_handle_w,array,array_len)
		end:	tl_usb_bulk_monitor_end()
	]] 

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

	if(iut_handle_r==0 or iut_handle_w==0 or lt_handle==0) then
		log_printf(1,"Open device Failed!",0,0)
		tl_btusb_close(lt_handle)
		tl_usb_bulk_monitor_end()
		return nil,nil,nil,nil
	else
		log_printf(1,"Open device Succeed!",0,0)
	end

	lt_handle_r = lt_handle
	lt_handle_w = lt_handle

	return iut_handle_r, iut_handle_w, lt_handle_r, lt_handle_w
end

function Close_Device(lt_handle)
	print("============ Close Device =============")
	tl_btusb_close(lt_handle)
	tl_usb_bulk_monitor_end()
end

-- Bluetooth address of Lower Tester 
LT_BD_Addr = {0x13,0x71, 0xda, 0x7d, 0x1a, 0x00}

-- Bluetooth address of IUT 
IUT_BD_Addr= {0xe6, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5}

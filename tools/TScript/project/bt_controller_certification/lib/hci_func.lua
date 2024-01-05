-- HCI EVENT Process Function Map
require("project.bt_controller_certification.lib.printf")
require("project.bt_controller_certification.lib.device")
--------------------  HCI command ------------------------------
function HCI_Data_Write(descript,handle,mode,buffer,len)
	if(mode=="dongle") then
		tl_usb_bulk_out(handle,buffer,len) -- dongle
	end
	
	if(mode=="uart") then
		tl_rs232_send(buffer,len) -- uart
	end

	if(mode=="usb") then
		tl_btusb_cmd_write(handle,buffer,len) --csr
	end
	log_printf(HCI_COMMAND_DISPLAY,descript,buffer,len);
end

function HCI_Result_Read(handle,mode)  
    
	r_len = 0;
	r_tbl = {};
	if(mode=="dongle") then
		r_tbl,r_len = tl_usb_bulk_read()
	end

	if(mode=="uart") then
		r_tbl,r_len = tl_rs232_recv()
	end

	if(mode=="usb") then
		r_tbl,r_len = tl_btusb_cmd_read(handle)
	end
	return r_tbl,r_len;
end

function HCI_Wait_Inquiry_Result(handle_r,mode,Timeout)
	t0 = os.time()
	while(1)
	do 
		if(mode=="unused") then
			return true,nil
		end
		--read and parse the corresponding HCI Command Complete Event to the HCI_Reset Command
		result_tbl,result_len  = HCI_Result_Read(handle_r,mode)
		code,name,flag,rst_tbl = HCI_Event_Exec(result_tbl,result_len,mode);	
		if(name == "HCI_Inquiry_Result" or name == "HCI_Extended_Inquiry_Result" or name == "HCI_Inquiry_Result_with_RSSI") then
			if(flag) then
				print_table(HCI_EVENT_TABLE_DISPLAY,rst_tbl);
				return flag,rst_tbl
			end
		end
		if(timeout_sec(t0,Timeout)==1) then
			log_printf(HCI_EVENT_TIMEOUT_DISPLAY,"HCI Inquiry Result Event: Timeout",0,0)
			return nil,nil
		end
	end	
end

function HCI_Wait_Event_Done(handle_r,mode,event_name,timeout)
	t0 = os.time()
	while(1)
	do 
		if(mode=="unused") then
			return true,nil
		end

		--read and parse the corresponding HCI Command Complete Event to the HCI_Reset Command
		result_tbl,result_len  = HCI_Result_Read(handle_r,mode)
		code,name,flag,rst_tbl = HCI_Event_Exec(result_tbl,result_len,mode);	
		if(name == event_name) then
			if(flag) then
				print_table(HCI_EVENT_TABLE_DISPLAY,rst_tbl);
				return flag,rst_tbl
			end
		end

		if(timeout_sec(t0,timeout)==1) then
			log_printf(HCI_EVENT_TIMEOUT_DISPLAY,event_name.." Event: Timeout",0,0)			
			return nil,nil
		end
	end
end

function HCI_Wait_ABEvent_Done(handle_r,mode,event_name,timeout)
	t0 = os.time()
	while(1)
	do 
		if(mode=="unused") then
			return true,nil
		end

		--read and parse the corresponding HCI Command Complete Event to the HCI_Reset Command
		result_tbl,result_len  = HCI_Result_Read(handle_r,mode)
		code,name,flag,rst_tbl = HCI_ABEvent_Exec(result_tbl,result_len,mode);	
		if(name == event_name) then
			if(flag) then
				print_table(HCI_EVENT_TABLE_DISPLAY,rst_tbl);
				return flag,rst_tbl
			end
		end

		if(timeout_sec(t0,timeout)==1) then
			log_printf(HCI_EVENT_TIMEOUT_DISPLAY,event_name.." Event: Timeout",0,0)			
			return nil,nil
		end
	end
end

function HCI_Wait_Other_Event(handle_r,mode)
	t0 = os.time()
	while(1)
	do 
		--read and parse the corresponding HCI Command Complete Event to the HCI_Reset Command
		result_tbl,result_len  = HCI_Result_Read(handle_r,mode)
		code,name,flag,rst_tbl = HCI_Event_Exec(result_tbl,result_len,mode);	
		
		if(timeout_sec(t0,1)==1) then
			log_printf(HCI_EVENT_TIMEOUT_DISPLAY,"No New HCI Event",0,0)
			return nil,nil
		end
	end	
end

-- for kite dongle print 
function  dongle_print_process(tbl,tbl_len)
	local ret_type = 0;
	local code;
	local name;
	local flag;
	local rst_tbl;
	if(tbl_len < 2) then
	    return
	end
	data_str="";
	str_str = "";
	if (tbl[1]==0x22) then  -- my_uart_send_str_data
		data_len = tbl[2] + (tbl[3]*256);
		for i = 1, data_len, 1 do
			data_str = data_str..string.format("%02x ",tbl[3+i]);
		end
		str_len = tbl_len - 3 - data_len;
		if(str_len < 0) then  return;  end
		for i = 1, str_len, 1 do
			str_str = str_str..string.format("%c",tbl[ 3 +data_len+i]);
		end
		print(" [dongle]:"..str_str..":"..data_str);
	end
	
	if(tbl[1]==0x21) then
		data_len = tbl_len - 1;
		for i = 1, data_len, 1 do
			data_str = data_str..string.format("%02x ",tbl[1+i]);
		end
		print(" [dongle]:"..data_str);
	end
	
	if(tbl[1]==0x20) then
		str_len = tbl_len - 1;
		for i = 1, str_len, 1 do
			str_str = str_str..string.format("%c",tbl[1+i]);
		end
		print(" [dongle]:"..str_str);
	end

	if(tbl[1]==0x04) then

	end
end

function HCI_Send2Controller(handle,mode,str,data,len)

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,data,len) --csr
	end

	if(mode=="uart") then
		len = tl_rs232_send(data,len) -- dongle
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle,data,len) -- dongle
	end
	log_printf(HCI_COMMAND_DISPLAY,str,data,len);
end


function HCI_AB_Write_Feature(handle,mode,LMP_Features)
	local a = array.new(32);
	if(mode=="uart" or mode=="dongle") then
		a[1] = 0x10
		a[2] = 0x01
		a[3] = 8
		for i=1,8 do
			a[i+3] = LMP_Features[i]  --para
		end

		if(mode=="uart") then
			len = tl_rs232_send(a,11) -- dongle
		end

		if(mode=="dongle") then
			len = tl_usb_bulk_out(handle,a,11) -- dongle
		end

		log_printf(HCI_COMMAND_DISPLAY,"HCI_AB_Write_Feature:",a,11);
	end
end
function HCI_AB_PW_INCR(handle,mode)
	local a = array.new(32);
	if(mode=="uart" or mode=="dongle") then
		a[1] = 0x10
		a[2] = 0x03

		if(mode=="uart") then
			len = tl_rs232_send(a,2) -- dongle
		end

		if(mode=="dongle") then
			len = tl_usb_bulk_out(handle,a,2) -- dongle
		end

		log_printf(HCI_COMMAND_DISPLAY,"HCI_AB_PW_INCR:",a,2);
	end
end
function HCI_AB_PW_DECR(handle,mode)
	local a = array.new(32);
	if(mode=="uart" or mode=="dongle") then
		a[1] = 0x10
		a[2] = 0x04

		if(mode=="uart") then
			len = tl_rs232_send(a,2) -- dongle
		end

		if(mode=="dongle") then
			len = tl_usb_bulk_out(handle,a,2) -- dongle
		end

		log_printf(HCI_COMMAND_DISPLAY,"HCI_AB_PW_DECR:",a,2);
	end
end
function HCI_AB_PW_CNTL(handle,mode,adjust)
	local a = array.new(32);
	if(mode=="uart" or mode=="dongle") then
		a[1] = 0x10
		a[2] = 0x05
		a[3] = 1
		a[4] = adjust

		if(mode=="uart") then
			len = tl_rs232_send(a,4) -- dongle
		end

		if(mode=="dongle") then
			len = tl_usb_bulk_out(handle,a,4) -- dongle
		end

		log_printf(HCI_COMMAND_DISPLAY,"HCI_AB_PW_CNTL:",a,4);
	end
end
function HCI_AB_EVENT_MASK(handle,mode,eve_code,b_v)
	local a = array.new(32);
	if(mode=="uart" or mode=="dongle") then
		a[1] = 0x10
		a[2] = 0x06
		a[3] = 2
		a[4] = eve_code
		a[5] = b_v

		if(mode=="uart") then
			len = tl_rs232_send(a,4) -- dongle
		end

		if(mode=="dongle") then
			len = tl_usb_bulk_out(handle,a,4) -- dongle
		end

		log_printf(HCI_COMMAND_DISPLAY,"HCI_AB_PW_CNTL:",a,4);
	end
end
function HCI_Disconnect (handle_w,mode,Connection_Handle,Reason)
		
	if(mode=="unused") then
	   return
	end

	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	a[1+offset] = 0x06;  --OCF =6
	a[2+offset] = 0x04;  --OGF =4
	a[3+offset] = 0x03;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = Reason;
	
	if(mode=="uart") then
		len = tl_rs232_send(a,7) -- dongle
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Disconnect:",a,7);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,7) -- dongle
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Disconnect:",a,7);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,6) --csr
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Disconnect:",a,6);
	end
	return len;

end

function HCI_Reset(handle_w,mode)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode == "uart" or mode == "dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x03;  --OCF =3
	a[2+offset] = 0x0c;  --OGF =3
	a[3+offset] = 0x00;  --para_len
	
	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reset:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reset:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reset:",a,3);
	end
	return len;
end

function HCI_Inquiry(handle_w,mode,LAP,Inquiry_Length,Num_Responses)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x01;  --OCF =1
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x05;  --para len
	a[4+offset] = bit32.band(LAP , 0xff);
	a[5+offset] = bit32.band(bit32.rshift(LAP,8) , 0xff);
	a[6+offset] = bit32.band(bit32.rshift(LAP,16), 0xff);
	a[7+offset] = Inquiry_Length;
	a[8+offset] = Num_Responses;
	
	--print(string.format("%02x %02x %02x",a[8+offset],a[9+offset],a[5+offset]))
	
	if(mode=="uart") then
		len = tl_rs232_send(a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Inquiry:",a,9);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Inquiry:",a,9);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Inquiry:",a,8);
	end
	return len;
	
end
function HCI_Write_Inquiry_Scan_Activity(handle_w,mode,Inquiry_Scan_Interval,Inquiry_Scan_Window)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x1e;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x04;  --para len
	a[4+offset] = bit32.band(Inquiry_Scan_Interval , 0xff);;
	a[5+offset] = bit32.band(bit32.rshift(Inquiry_Scan_Interval,8) , 0xff);
	a[6+offset] = bit32.band(Inquiry_Scan_Window , 0xff);;
	a[7+offset] = bit32.band(bit32.rshift(Inquiry_Scan_Window,8) , 0xff);




	
	--print(string.format("%02x %02x %02x",a[8+offset],a[9+offset],a[5+offset]))
	
	if(mode=="uart") then
		len = tl_rs232_send(a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Inquiry_Scan_Activity:",a,9);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Inquiry_Scan_Activity:",a,9);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Inquiry_Scan_Activity:",a,8);
	end
	return len;
	
end
function HCI_Read_Inquiry_Mode(handle_w,mode)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x44;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1


	
	--print(string.format("%02x %02x %02x",a[8+offset],a[9+offset],a[5+offset]))
	
	if(mode=="uart") then
		len = tl_rs232_send(a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Inquiry_Mode:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Inquiry_Mode:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Inquiry_Mode:",a,3);
	end
	return len;
	
end
function HCI_Write_Inquiry_Mode(handle_w,mode,Inquiry_Mode)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x45;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x01;  --para len
	a[4+offset] = Inquiry_Mode;

	
	--print(string.format("%02x %02x %02x",a[8+offset],a[9+offset],a[5+offset]))
	
	if(mode=="uart") then
		len = tl_rs232_send(a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Inquiry_Mode:",a,5);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Inquiry_Mode:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Inquiry_Mode:",a,4);
	end
	return len;
	
end
function HCI_Read_Buffer_size(handle_w,mode)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(4);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x05;  --OCF =05
	a[2+offset] = 0x10;  --OGF =10
	a[3+offset] = 0x00;  --para len

	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Buffer_size:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Buffer_size:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Buffer_size:",a,3);
	end
	return len;

end
function HCI_Read_Local_Extended_Features(handle_w,mode,page_num)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(5);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x04;  --OCF =1
	a[2+offset] = 0x10;  --OGF =1
	a[3+offset] = 0x01;  --para len
	a[4+offset] = page_num;  --para

	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Extended_Features:",a,5);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Extended_Features:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Extended_Features:",a,4);
	end
	return len;

end
function HCI_Read_Local_Support_Features(handle_w,mode)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(4);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x03;  --OCF =1
	a[2+offset] = 0x10;  --OGF =1
	a[3+offset] = 0x00;  --para len

	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Support_Features:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Support_Features:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Support_Features:",a,3);
	end
	return len;

end
function HCI_Read_Page_Timeout(handle_w,mode)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x17;  --OCF =0x17
	a[2+offset] = 0x0c;  --OGF =1

	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Page_Timeout:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Page_Timeout:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Page_Timeout:",a,3);
	end
	return len;

end
function HCI_Write_Page_Timeout(handle_w,mode,Page_timeout)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x18;  --OCF =0x17
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 2;  --OCF =0x17
	a[4+offset] = bit32.band(Page_timeout, 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Page_timeout,8) , 0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Timeout:",a,6);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Timeout:",a,6);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Timeout:",a,5);
	end
	return len;

end
function HCI_Write_Extended_Page_Timeout(handle_w,mode,Extended_page_timeout)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x7f;  --OCF =0x17
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 4;  --OCF =0x17
	a[4+offset] = bit32.band(Extended_page_timeout, 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Extended_page_timeout,8) , 0xff);
	a[6+offset] = bit32.band(bit32.rshift(Extended_page_timeout,16) , 0xff);
	a[7+offset] = bit32.band(bit32.rshift(Extended_page_timeout,24) , 0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Extended_Page_Timeout:",a,7);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Extended_Page_Timeout:",a,7);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Extended_Page_Timeout:",a,6);
	end
	return len;

end
function HCI_Write_Scan_Enable(handle_w,mode,Scan_Enable)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x1a;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x01;  --para len
	a[4+offset] = Scan_Enable;  --para len
	
	if(mode=="uart") then
		len = tl_rs232_send(a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Scan_Enable:",a,5);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Scan_Enable:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Scan_Enable:",a,4);
	end
	return len;

end

function HCI_Write_Page_Scan_Activity(handle_w,mode,Page_Scan_Interval,Page_Scan_Window)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x1c;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x04;  --para len
	a[4+offset] = bit32.band(Page_Scan_Interval, 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Page_Scan_Interval,8) , 0xff);
	a[6+offset] = bit32.band(Page_Scan_Window , 0xff);
	a[7+offset] = bit32.band(bit32.rshift(Page_Scan_Window,8) , 0xff);
	
	if(mode=="uart") then
		len = tl_rs232_send(a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Scan_Activity:",a,8);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Scan_Activity:",a,8);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Scan_Activity:",a,7);
	end
	return len;

end


function  HCI_Create_Connection (handle_w,mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
Clock_Offset,Allow_Role_Switch)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x05;  --OCF =1
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 13;  --para len
	a[4+offset] = BD_ADDR[1]; --address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
		
	a[10+offset] = bit32.band(Packet_Type , 0xff);
	a[11+offset] = bit32.band(bit32.rshift(Packet_Type,8) , 0xff);
	 
	a[12+offset] =  Page_Scan_Repetition_Mode;
	
	a[13+offset] = 0x00;  --reserved 
	
	a[14+offset] = bit32.band(Clock_Offset , 0xff);
	a[15+offset] = bit32.band(bit32.rshift(Clock_Offset,8) , 0xff);
	
	a[16+offset] = Allow_Role_Switch;
	
	if(mode=="uart") then
		len = tl_rs232_send(a,17)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Create_Connection:",a,17);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,17)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Create_Connection:",a,17);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,16)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Create_Connection:",a,16);
	end

	print(string.format("Bluetooth address: %02x %02x %02x %02x %02x %02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));

	return len;

end

function  HCI_Accept_Connection_Request (handle_w,mode,BD_ADDR,Role)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x09;  --OCF =1
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 7;  --para len
	a[4+offset] = BD_ADDR[1]; --address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
	
	a[10+offset] = Role;
	
	if(mode=="uart") then
		len = tl_rs232_send(a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Connection_Request:",a,11);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Connection_Request:",a,11);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Connection_Request:",a,10);
	end

	print(string.format("Bluetooth address %02x %02x %02x %02x %02x %02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));
	
	return len;

end
function  HCI_Change_Connection_Packet_Type(handle_w,mode,Connection_Handle,Packet_Type)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x0f;  --OCF =1
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 4;  --para len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = bit32.band(Packet_Type,0xff);
	a[7+offset] = bit32.band(bit32.rshift(Packet_Type,8),0xff);



	if(mode=="uart") then
		len = tl_rs232_send(a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Change_Connection_Packet_Type:",a,8);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Change_Connection_Packet_Type:",a,8);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Change_Connection_Packet_Type:",a,7);
	end


	return len;

end
function  HCI_Reject_Connection_Request (handle_w,mode,BD_ADDR,Reason)
		
	if(mode=="unused") then
	   return
	end
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x0a;  		--OCF 
	a[2+offset] = 0x04;  		--OGF
	a[3+offset] = 7;  			--para len
	a[4+offset] = BD_ADDR[1]; 	--address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
	a[10+offset] = Reason;
	
	if(mode=="uart") then
		len = tl_rs232_send(a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Connection_Request:",a,11);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Connection_Request:",a,11);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Connection_Request:",a,10);
	end

	print(string.format("Bluetooth address %02x %02x %02x %02x %02x %02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));
	
	return len;

end

function HCI_Setup_Synchronous_Connection(handle_w,mode,Connection_Handle,Transmit_Bandwidth,Receive_Bandwidth,Max_Latency,Voice_Setting,Retransmission_Effort,Packet_Type)
    if(mode=="unused") then
        return
    end
    	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
        a[1] = 0x01;  --HCI cmd
        offset = 1;
    end

    a[1+offset] = 0x28;  --OCF =1
    a[2+offset] = 0x04;  --OGF =1
    a[3+offset] = 17;  --para len

	a[4+offset] = bit32.band(Connection_Handle,0xff); --Connection_Handle
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);

    a[6+offset] = bit32.band(Transmit_Bandwidth,0xff); --Transmit_Bandwidth
    a[7+offset] = bit32.band(bit32.rshift(Transmit_Bandwidth,8),0xff);
    a[8+offset] = bit32.band(bit32.rshift(Transmit_Bandwidth,16),0xff);
    a[9+offset] = bit32.band(bit32.rshift(Transmit_Bandwidth,24),0xff);

    a[10+offset] = bit32.band(Receive_Bandwidth,0xff); --Receive_Bandwidth
    a[11+offset] = bit32.band(bit32.rshift(Receive_Bandwidth,8),0xff);
    a[12+offset] = bit32.band(bit32.rshift(Receive_Bandwidth,16),0xff);
    a[13+offset] = bit32.band(bit32.rshift(Receive_Bandwidth,24),0xff);
        
    a[14+offset] = bit32.band(Max_Latency , 0xff); --Max_Latency
    a[15+offset] = bit32.band(bit32.rshift(Max_Latency,8) , 0xff);

    a[16+offset] = bit32.band(Voice_Setting , 0xff); --Voice_Setting
    a[17+offset] = bit32.band(bit32.rshift(Voice_Setting,8) , 0xff);
     
    a[18+offset] = Retransmission_Effort; --Retransmission_Effort
    
    a[19+offset] = bit32.band(Packet_Type , 0xff); --Packet_Type
    a[20+offset] = bit32.band(bit32.rshift(Packet_Type,8) , 0xff);
    
    if(mode=="uart") then
        len = tl_rs232_send(a,21)
        log_printf(HCI_COMMAND_DISPLAY,"HCI_Setup_Synchronous_Connection:",a,21);
    end
    
    if(mode=="dongle") then
        len = tl_usb_bulk_out(handle_w,a,21)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Setup_Synchronous_Connection:",a,21);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,20)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Setup_Synchronous_Connection:",a,20);
	end

	return len;

end

function HCI_Accept_Synchronous_Connection_Request(handle_w,mode,BD_ADDR,Transmit_Bandwidth,Receive_Bandwidth,Max_Latency,Voice_Setting,Retransmission_Effort,Packet_Type)
    if(mode=="unused") then
        return
    end
    	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
        a[1] = 0x01;  --HCI cmd
        offset = 1;
    end

    a[1+offset] = 0x29;  --OCF =1
    a[2+offset] = 0x04;  --OGF =1
    a[3+offset] = 21;  --para len

    a[4+offset] = BD_ADDR[1]; 	--BD address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];

    a[10+offset] = bit32.band(Transmit_Bandwidth,0xff); --Transmit_Bandwidth
    a[11+offset] = bit32.band(bit32.rshift(Transmit_Bandwidth,8),0xff);
    a[12+offset] = bit32.band(bit32.rshift(Transmit_Bandwidth,16),0xff);
    a[13+offset] = bit32.band(bit32.rshift(Transmit_Bandwidth,24),0xff);

    a[14+offset] = bit32.band(Receive_Bandwidth,0xff); --Receive_Bandwidth
    a[15+offset] = bit32.band(bit32.rshift(Receive_Bandwidth,8),0xff);
    a[16+offset] = bit32.band(bit32.rshift(Receive_Bandwidth,16),0xff);
    a[17+offset] = bit32.band(bit32.rshift(Receive_Bandwidth,24),0xff);
        
    a[18+offset] = bit32.band(Max_Latency , 0xff); --Max_Latency
    a[19+offset] = bit32.band(bit32.rshift(Max_Latency,8) , 0xff);

    a[20+offset] = bit32.band(Voice_Setting , 0xff); --Voice_Setting
    a[21+offset] = bit32.band(bit32.rshift(Voice_Setting,8) , 0xff);
     
    a[22+offset] = Retransmission_Effort; --Retransmission_Effort
    
    a[23+offset] = bit32.band(Packet_Type , 0xff); --Packet_Type
    a[24+offset] = bit32.band(bit32.rshift(Packet_Type,8) , 0xff);
    
    if(mode=="uart") then
        len = tl_rs232_send(a,25)
        log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Synchronous_Connection_Request:",a,25);
    end
    
    if(mode=="dongle") then
        len = tl_usb_bulk_out(handle_w,a,25)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Synchronous_Connection_Request:",a,25);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,24)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Synchronous_Connection_Request:",a,24);
	end

	return len;
end

function HCI_Reject_Synchronous_Connection_Request(handle_w,mode,BD_ADDR,Reason)
    if(mode=="unused") then
        return
    end
    	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
        a[1] = 0x01;  --HCI cmd
        offset = 1;
    end

    a[1+offset] = 0x2a;  --OCF =1
    a[2+offset] = 0x04;  --OGF =1
    a[3+offset] = 7;  --para len

    a[4+offset] = BD_ADDR[1]; 	--BD address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];

    a[10+offset] = Reason; --Reason
    
    if(mode=="uart") then
        len = tl_rs232_send(a,11)
        log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Synchronous_Connection_Request:",a,11);
    end
    
    if(mode=="dongle") then
        len = tl_usb_bulk_out(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Synchronous_Connection_Request:",a,11);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Synchronous_Connection_Request:",a,10);
	end

	return len;
end

function HCI_Authentication_Requested(handle_w,mode,Connection_Handle)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x11;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 0x02;  --para_len
	
	a[4+offset] = bit32.band(Connection_Handle, 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8) , 0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Authentication_Requested:",a,6);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Authentication_Requested:",a,6);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Authentication_Requested:",a,5);
	end

	return len;
end

function HCI_Delete_Stored_Link_Key(handle_w,mode,BD_ADDR,Delete_All)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x12;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 0x07;  --para_len
	
	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	a[10+offset] = Delete_All;    --para

	if(mode=="uart") then
		len = tl_rs232_send(a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Delete_Stored_Link_Key:",a,11);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Delete_Stored_Link_Key:",a,11);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Delete_Stored_Link_Key:",a,10);
	end

	return len;

end


function HCI_Write_PIN_Type(handle_w,mode,PIN_Type)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x0a;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 0x01;  --para_len
	a[4+offset] = PIN_Type;    --para

	if(mode=="uart") then
		len = tl_rs232_send(a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_PIN_Type:",a,5);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_PIN_Type:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_PIN_Type:",a,4);
	end
	return len;
end

function HCI_Write_Authentication_Enable(handle_w,mode,Authentication_Enable)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x20;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 0x01;  --para_len
	a[4+offset] = Authentication_Enable;    --para

	if(mode=="uart") then
		len = tl_rs232_send(a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Authentication_Enable:",a,5);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Authentication_Enable:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Authentication_Enable:",a,4);
	end
	return len;
end

function HCI_Write_Simple_Pairing_Mode(handle_w,mode,Simple_Pairing_Mode)
			
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x56;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 0x01;  --para_len
	a[4+offset] = Simple_Pairing_Mode;    --para


	if(mode=="uart") then
		len = tl_rs232_send(a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Simple_Pairing_Mode:",a,5);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Simple_Pairing_Mode:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Simple_Pairing_Mode:",a,4);
	end

	return len;

end
function HCI_Write_LE_Host_Support(handle_w,mode,le_sup,sim_le_host)
	if(mode=="unused") then
	   return
	end
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	a[1+offset] = 0x6d;  --OCF =0x0e
	a[2+offset] = 0x0c;  --OGF =
	a[3+offset] = 0x02;  --para len
	a[4+offset] = le_sup;  --
	a[5+offset] = sim_le_host;  --
	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_LE_Host_Support:",a,6);
	end
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_LE_Host_Support:",a,6);	
	end	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_LE_Host_Support:",a,5);
	end
	return len;
end
function  HCI_Write_Secure_Connections_Host_Support(handle_w,mode,Secure_Connections_Host_Support)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x7a;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 0x01;  --para_len
	a[4+offset] = Secure_Connections_Host_Support;    --para


	if(mode=="uart") then
		len = tl_rs232_send(a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Secure_Connections_Host_Support:",a,5);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Secure_Connections_Host_Support:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Secure_Connections_Host_Support:",a,4);
	end

	return len;
end

function HCI_User_Confirmation_Request_Reply(handle_w,mode,BD_ADDR)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x2c;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 0x06;  --para_len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Confirmation_Request_Reply:",a,10);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Confirmation_Request_Reply:",a,10);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Confirmation_Request_Reply:",a,9);
	end

	return len;
end

function HCI_User_Confirmation_Request_Negative_Reply(handle_w,mode,BD_ADDR)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x2d;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 0x06;  --para_len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Confirmation_Request_Negative_Reply:",a,10);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Confirmation_Request_Negative_Reply:",a,10);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Confirmation_Request_Negative_Reply:",a,9);
	end

	return len;
end

function HCI_User_Passkey_Request_Reply(handle_w,mode,BD_ADDR,PassKey)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x2e;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 0x0a;  --para_len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	for i=1,4 do
		a[9+offset+i] = PassKey[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,14)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Passkey_Request_Reply:",a,14);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,14)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Passkey_Request_Reply:",a,14);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,13)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Passkey_Request_Reply:",a,13);
	end

	return len;
end

function HCI_User_Passkey_Request_Negative_Reply(handle_w,mode,BD_ADDR)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x2f;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 0x06;  --para_len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Passkey_Request_Negative_Reply:",a,10);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Passkey_Request_Negative_Reply:",a,10);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_User_Passkey_Request_Negative_Reply:",a,9);
	end

	return len;
end


function HCI_Write_Local_Name(handle_w,mode,Local_Name)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(248);
    strlen = #Local_Name

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x13;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = strlen;  --para_len

	for i=1,strlen do
		a[i+3+offset] = string.byte(Local_Name,i,i)  --para
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,strlen+4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Local_Name:",a,strlen+4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,strlen+4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Local_Name:",a,strlen+4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,strlen+3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Local_Name:",a,strlen+3);
	end
	return len;
end
function HCI_Read_Local_Name(handle_w,mode)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(8);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x14;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 0;  --para_len


	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Name:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Name:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Name:",a,3);
	end
	return len;
end
function HCI_LE_Set_Event_Mask(handle_w,mode,Event_Mask)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x01;  --OCF
	a[2+offset] = 0x20;  --OGF
	a[3+offset] = 8;  --para_len

	for i=1,8 do
		a[i+3+offset] = Event_Mask[i]  --para
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,12)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_LE_Set_Event_Mask:",a,12);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,12)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_LE_Set_Event_Mask:",a,12);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_LE_Set_Event_Mask:",a,11);
	end
	return len;	
end	
function HCI_Set_Event_Mask(handle_w,mode,Event_Mask)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x01;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 8;  --para_len

	for i=1,8 do
		a[i+3+offset] = Event_Mask[i]  --para
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,12)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Set_Event_Mask:",a,12);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,12)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Set_Event_Mask:",a,12);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Set_Event_Mask:",a,11);
	end
	return len;	
end	
function HCI_Write_Link_Policy_Settings(handle_w,mode,Connection_Handle,Link_Policy_Settings)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x0d;  --OCF
	a[2+offset] = 0x08;  --OGF
	a[3+offset] = 4;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = bit32.band(Link_Policy_Settings,0xff);
	a[7+offset] = bit32.band(bit32.rshift(Link_Policy_Settings,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,7+offset)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Link_Policy_Settings:",a,7+offset);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,7+offset)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Link_Policy_Settings:",a,7+offset);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,7+offset)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Link_Policy_Settings:",a,7+offset);
	end
	return len;		
end

function HCI_Switch_Role(handle_w,mode,BD_ADDR,Role)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x0b;  --OCF
	a[2+offset] = 0x08;  --OGF
	a[3+offset] = 7;  --para_len
	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end
	a[10+offset] = Role;

	if(mode=="uart") then
		len = tl_rs232_send(a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Switch_Role:",a,11);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Switch_Role:",a,11);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Switch_Role:",a,10);
	end
	return len;
end

function HCI_Read_Clock_Offset(handle_w,mode,Connection_Handle)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x1f;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 2;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Clock_Offset:",a,6);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Clock_Offset:",a,6);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Clock_Offset:",a,5);
	end
	return len;
end
function HCI_Read_Remote_Version_Information(handle_w,mode,Connection_Handle)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x1d;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 2;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Version_Information:",a,6);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Version_Information:",a,6);
	end	

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Version_Information:",a,5);
	end
	return len;
end
function HCI_Read_Remote_Supported_Features(handle_w,mode,Connection_Handle)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x1b;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 2;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Supported_Features:",a,6);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Supported_Features:",a,6);
	end	

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Supported_Features:",a,5);
	end
	return len;
end
function HCI_Read_Remote_Ext_Supported_Features(handle_w,mode,Connection_Handle,page_num)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x1c;  --OCF
	a[2+offset] = 0x04;  --OGF
	a[3+offset] = 3;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = page_num;

	if(mode=="uart") then
		len = tl_rs232_send(a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Ext_Supported_Features:",a,7);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Ext_Supported_Features:",a,7);
	end	

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Remote_Ext_Supported_Features:",a,6);
	end
	return len;
end

function HCI_Remote_Name_Request(handle_w,mode,BD_ADDR,Page_Scan_Repetition_Mode,Clock_Offset)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x19;  	--OCF
	a[2+offset] = 0x04;  	--OGF
	a[3+offset] = 10;  		--para_len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end
	a[10+offset] = Page_Scan_Repetition_Mode;
	a[11+offset] = 0;		-- reserved
	a[12+offset] = bit32.band(Clock_Offset,0xff);
	a[13+offset] = bit32.band(bit32.rshift(Clock_Offset,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,14)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_Name_Request:",a,14);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,14)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_Name_Request:",a,14);
	end
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,13)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_Name_Request:",a,13);
	end
	return len;
end

function HCI_Write_Link_Supervision_Timeout(handle_w,mode,Connection_Handle,Link_Supervision_Timeout)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(12);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x37;  --OCF
	a[2+offset] = 0x0c;  --OGF
	a[3+offset] = 4;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = bit32.band(Link_Supervision_Timeout,0xff);
	a[7+offset] = bit32.band(bit32.rshift(Link_Supervision_Timeout,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Link_Supervision_Timeout:",a,8);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Link_Supervision_Timeout:",a,8);
	end


	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Link_Supervision_Timeout:",a,7);
	end
	return len;	
end

function HCI_Sniff_Mode(handle_w,mode,Connection_Handle,Sniff_Max_Interval,Sniff_Min_Interval,Sniff_Attempt,Sniff_Timeout)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x03;  --OCF
	a[2+offset] = 0x08;  --OGF
	a[3+offset] = 10;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = bit32.band(Sniff_Max_Interval,0xff);
	a[7+offset] = bit32.band(bit32.rshift(Sniff_Max_Interval,8),0xff);
	a[8+offset] = bit32.band(Sniff_Min_Interval,0xff);
	a[9+offset] = bit32.band(bit32.rshift(Sniff_Min_Interval,8),0xff);
	a[10+offset] = bit32.band(Sniff_Attempt,0xff);
	a[11+offset] = bit32.band(bit32.rshift(Sniff_Attempt,8),0xff);
	a[12+offset] = bit32.band(Sniff_Timeout,0xff);
	a[13+offset] = bit32.band(bit32.rshift(Sniff_Timeout,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(handle_w,a,14)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Sniff_Mode:",a,14);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,14)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Sniff_Mode:",a,14);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,13)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Sniff_Mode:",a,13);
	end
	return len;	
end

function HCI_Exit_Sniff_Mode(handle_w,mode,Connection_Handle)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x04;  --OCF
	a[2+offset] = 0x08;  --OGF
	a[3+offset] = 2;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Exit_Sniff_Mode:",a,6);
	end
	
	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Exit_Sniff_Mode:",a,6);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Exit_Sniff_Mode:",a,5);
	end
	return len;	
end

function HCI_Sniff_Subrating(handle_w,mode,Connection_Handle,Maximum_Latency,Minimum_Remote_Timeout,Minimum_Local_Timeout)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x11;  --OCF
	a[2+offset] = 0x08;  --OGF
	a[3+offset] = 8;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = bit32.band(Maximum_Latency,0xff);
	a[7+offset] = bit32.band(bit32.rshift(Maximum_Latency,8),0xff);
	a[8+offset] = bit32.band(Minimum_Remote_Timeout,0xff);
	a[9+offset] = bit32.band(bit32.rshift(Minimum_Remote_Timeout,8),0xff);
	a[10+offset] = bit32.band(Minimum_Local_Timeout,0xff);
	a[11+offset] = bit32.band(bit32.rshift(Minimum_Local_Timeout,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,12)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Sniff_Subrating:",a,12);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,12)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Sniff_Subrating:",a,12);
	end	

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Sniff_Subrating:",a,11);
	end
	return len;
end

function HCI_QoS_Setup(handle_w,mode,Connection_Handle,Service_Type,Token_Rate,Peak_Bandwidth,Latency,Delay_Variation)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x07;  --OCF
	a[2+offset] = 0x08;  --OGF
	a[3+offset] = 20;  --para_len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = 0;   -- Unused
	a[7+offset] = Service_Type;
	a[8+offset] = bit32.band(Token_Rate,0xff);
	a[9+offset] = bit32.band(bit32.rshift(Token_Rate,8),0xff);
	a[10+offset] = bit32.band(bit32.rshift(Token_Rate,16),0xff);
	a[11+offset] = bit32.band(bit32.rshift(Token_Rate,24),0xff);
	a[12+offset] = bit32.band(Peak_Bandwidth,0xff);
	a[13+offset] = bit32.band(bit32.rshift(Peak_Bandwidth,8),0xff);
	a[14+offset] = bit32.band(bit32.rshift(Peak_Bandwidth,16),0xff);
	a[15+offset] = bit32.band(bit32.rshift(Peak_Bandwidth,24),0xff);
	a[16+offset] = bit32.band(Latency,0xff);
	a[17+offset] = bit32.band(bit32.rshift(Latency,8),0xff);
	a[18+offset] = bit32.band(bit32.rshift(Latency,16),0xff);
	a[19+offset] = bit32.band(bit32.rshift(Latency,24),0xff);
	a[20+offset] = bit32.band(Delay_Variation,0xff);
	a[21+offset] = bit32.band(bit32.rshift(Delay_Variation,8),0xff);
	a[22+offset] = bit32.band(bit32.rshift(Delay_Variation,16),0xff);
	a[23+offset] = bit32.band(bit32.rshift(Delay_Variation,24),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,24)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_QoS_Setup:",a,24);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,24)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_QoS_Setup:",a,24);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,23)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_QoS_Setup:",a,23);
	end
	return len;	
end


function  HCI_Link_Key_Request_Reply(handle_w, mode, BD_ADDR, KEY)
	local loop;
	local t_str;

	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x0b;  --OCF =0x0b
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 22;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end
	
	t_str = "Link Key :";

	for loop = 1,16 do
		a[9+offset+loop] = KEY[loop];
		t_str = t_str..string.format(" %02x ",KEY[17-loop]);
	end

	print(t_str);


	if(mode=="uart") then
		len = tl_rs232_send(a,26)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Link_Key_Request_Reply:",a,26);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,26)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Link_Key_Request_Reply:",a,26);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,25)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Link_Key_Request_Reply:",a,25);
	end

	return len;
		
end

function HCI_Link_Key_Request_Negative_Reply(handle_w,mode,BD_ADDR)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x0c;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x06;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Link_Key_Request_Negative_Reply:",a,10);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Link_Key_Request_Negative_Reply:",a,10);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Link_Key_Request_Negative_Reply:",a,9);
	end

	return len;
	
end

function  HCI_PIN_Code_Request_Reply(handle_w, mode, BD_ADDR, pin_len,PIN)
	
	local loop;
	local t_str;
	local t_len;
		
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
		a[1] = 0x01;  --HCI cmd
		offset = 1;
	end
	
	a[1+offset] = 0x0d;  --OCF =0x0d
	a[2+offset] = 0x04;  --OGF =1
	
	if (pin_len > 16) then
		pin_len = 16;
	end

	a[3+offset] = 6+1+pin_len;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	a[10+offset] = pin_len;

	t_str = "Pin Code :";

	for loop = 1,pin_len do
		a[10+offset+loop] = PIN[loop];
		t_str = t_str..string.format("% 02x",PIN[pin_len+1-loop]);
	end

	print(t_str);

	if(mode=="uart") then
		len = tl_rs232_send(a,pin_len+11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_PIN_Code_Request_Reply:",a,pin_len+11);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,pin_len+11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_PIN_Code_Request_Reply:",a,pin_len+11);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,pin_len+10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_PIN_Code_Request_Reply:",a,pin_len+10);
	end

	return len;
		
end

function HCI_PIN_Code_Request_Negative_Reply(handle_w,mode,BD_ADDR)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x0e;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x06;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_PIN_Code_Request_Negative_Reply:",a,10);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_PIN_Code_Request_Negative_Reply:",a,10);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_PIN_Code_Request_Negative_Reply:",a,9);
	end

	return len;
	
end
function HCI_Change_Connection_Link_Key(handle_w,mode,Connection_Handle)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x15;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x02;  --para len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);

	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Change_Connection_Link_Key:",a,6);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Change_Connection_Link_Key:",a,6);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Change_Connection_Link_Key:",a,5);
	end

	return len;
	
end

function HCI_Set_Connection_Encryption(handle_w,mode,Connection_Handle,Encryption_Enable)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x13;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x03;  --para len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);
	a[6+offset] = Encryption_Enable;

	if(mode=="uart") then
		len = tl_rs232_send(a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Set_Connection_Encryption:",a,7);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Set_Connection_Encryption:",a,7);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Set_Connection_Encryption:",a,6);
	end

	return len;
	
end

function HCI_Read_Encryption_Key_Size(handle_w,mode,Connection_Handle)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x14;  --OCF =0x0e
	a[2+offset] = 0x08;  --OGF =1
	a[3+offset] = 0x02;  --para len
	a[4+offset] = bit32.band(Connection_Handle,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8),0xff);


	if(mode=="uart") then
		len = tl_rs232_send(a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Encryption_Key_Size:",a,6);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,6)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Encryption_Key_Size:",a,6);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Encryption_Key_Size:",a,5);
	end

	return len;
	
end

function HCI_IO_Capability_Request_Reply(handle_w,mode,BD_ADDR,IO_Capability,OOB_Data_Present,Authentication_Requirement)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x2b;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x09;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	a[10+offset] = IO_Capability;
	a[11+offset] = OOB_Data_Present;
	a[12+offset] = Authentication_Requirement;

	if(mode=="uart") then
		len = tl_rs232_send(a,13)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_IO_Capability_Request_Reply:",a,13);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,13)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_IO_Capability_Request_Reply:",a,13);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,12)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_IO_Capability_Request_Reply:",a,12);
	end

	return len;
	
end

function HCI_IO_Capability_Request_Negative_Reply(handle_w,mode,BD_ADDR,Reason)
	
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x34;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x07;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	a[10+offset] = Reason;


	if(mode=="uart") then
		len = tl_rs232_send(a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_IO_Capability_Request_Negative_Reply:",a,11);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_IO_Capability_Request_Negative_Reply:",a,11);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_IO_Capability_Request_Negative_Reply:",a,10);
	end

	return len;
	
end

function HCI_Remote_OOB_Data_Request_Reply(handle_w,mode,BD_ADDR,OOB_Commitment,OOB_Random)
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(48);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x30;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x26;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	for i=1,16 do
		a[9+offset+i] = OOB_Commitment[i];
	end

	for i=1,16 do
		a[25+offset+i] = OOB_Random[i];
	end


	if(mode=="uart") then
		len = tl_rs232_send(a,42)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_OOB_Data_Request_Reply:",a,42);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,42)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_OOB_Data_Request_Reply:",a,42);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,41)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_OOB_Data_Request_Reply:",a,41);
	end

	return len;
end

function HCI_Remote_OOB_Data_Request_Negative_Reply(handle_w,mode,BD_ADDR)
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x33;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x06;  --para len

	for i=1,6 do
		a[3+offset+i] = BD_ADDR[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_OOB_Data_Request_Negative_Reply:",a,10);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_OOB_Data_Request_Negative_Reply:",a,10);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Remote_OOB_Data_Request_Negative_Reply:",a,9);
	end

	return len;
end

function HCI_Read_Local_OOB_Data(handle_w,mode)
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x57;  --OCF =0x0e
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x00;  --para len

	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_OOB_Data:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_OOB_Data:",a,4);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_OOB_Data:",a,3);
	end

	return len;
end

function HCI_Write_Simple_Pairing_Debug_Mode(handle_w,mode,sp_debug_mode)
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x04;  --OCF =0x0e
	a[2+offset] = 0x18;  --OGF =1
	a[3+offset] = 0x01;  --para len
	a[4+offset] = sp_debug_mode;  

	if(mode=="uart") then
		len = tl_rs232_send(a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Simple_Pairing_Debug_Mode:",a,5);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Simple_Pairing_Debug_Mode:",a,5);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Simple_Pairing_Debug_Mode:",a,4);
	end

	return len;
end

function HCI_UnSupport_Commands(handle_w,mode,unkonw_cmd_ocf,unkonw_cmd_ogf)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = unkonw_cmd_ocf;  --OCF =1
	a[2+offset] = unkonw_cmd_ogf;  --OGF =1
	a[3+offset] = 0x00;  --para len

	if(mode=="uart") then
		len = tl_rs232_send(a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Support_Features:",a,4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Support_Features:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Read_Local_Support_Features:",a,3);
	end
	return len;

end
----------------------- HCI ACL DATA -------------------------------
function HCI_Acl_data_send(handle_w,mode,Connection_Handle,Pb_Flag,data_len,data)
		
	if(mode=="unused") then
	   return
	end
	
	len = 0;
    a = array.new(32);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	  a[1] = 0x02;  --HCI acl data
	  offset = 1;
	end
	
	a[1+offset] = bit32.band(Connection_Handle,0xff);
	a[2+offset] = bit32.bor(bit32.band(bit32.rshift(Connection_Handle,8),0xff),bit32.lshift(Pb_Flag,1));
	a[3+offset] = bit32.band(data_len+4,0xff);
	a[4+offset] = bit32.band(bit32.rshift(data_len+4,8),0xff);


	a[5+offset] = bit32.band(data_len,0xff);
	a[6+offset] = bit32.band(bit32.rshift(data_len,8),0xff);

	chn_id = 0x0001
	a[7+offset] = bit32.band(chn_id,0xff);
	a[8+offset] = bit32.band(bit32.rshift(chn_id,8),0xff);


    -- qp_array ={0x03 ,0x03 ,0x08 ,0x00 ,0x04 ,0x01 ,0x55 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00}
	-- qp_array={0x04 ,0x01 ,0x08 ,0x00 ,0x46 ,0x00 ,0x00 ,0x00 ,0x01 ,0x02 ,0x64 ,0x02}

	for i=1,data_len do
		a[8+offset+i] = data[i];
	end


	if(mode=="uart") then
		len = tl_rs232_send(a,24)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Acl_data_send:",a,9+data_len+4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,24)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Acl_data_send:",a,9+data_len+4);	
	end	
	
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle_w,a,23)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Acl_data_send:",a,8+data_len+4);
	end
	return len;	
end

----------------------- HCI Synchronous Data packets ---------------------------------
function HCI_Synchronous_Data_packet_TX(handle_w,mode,Sync_Con_Hdl,Length,Data)
    if(mode=="unused" or mode=="usb") then
	    return
	end

	if(Data==nil) then
	    return
	end

	if(Length > 255) then 
	    return
	end

	len = 0;
    a = array.new(512);

	offset = 0;
	if(mode=="uart" or mode=="dongle") then
	    a[1] = 0x03;  --HCI Synchronous Data packets
	    offset = 1;
	end
	
	a[1+offset] = bit32.band(Sync_Con_Hdl,0xff);
	a[2+offset] = bit32.band(bit32.rshift(Sync_Con_Hdl,8),0x0e);
	a[3+offset] = Length;

	for i=1,Length do
		a[3+offset+i] = Data[i];
	end

	if(mode=="uart") then
		len = tl_rs232_send(a,Length+4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Synchronous_Data_packet_TX:",a,Length+4);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle_w,a,Length+4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Synchronous_Data_packet_TX:",a,Length+4);	
	end	

	return len;
end

----------------------- HCI event ---------------------------------
function  EVENT_HCI_Inquiry_Complete(tbl,mode)
	local flag;
	local offset = 0;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x01) then -- correct event code should be 0x01
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
   
	return true,rst_tbl;
end

function  EVENT_HCI_Inquiry_Result(tbl,mode)
	local flag;
	local offset = 0;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x02) then -- correct event code should be 0x02
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Num_Responses = tbl[3+offset];
    rst_tbl.BD_ADDR = {};
	rst_tbl.Page_Scan_Repetition_Mode = {};
	rst_tbl.Reserved = {};
	rst_tbl.Class_of_Device = {};
	rst_tbl.Clock_Offset = {};
	
	for i = 1,Num_Responses,1 do
		rst_tbl.BD_ADDR[i] = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]};
		rst_tbl.Page_Scan_Repetition_Mode[i] = tbl[10+offset];
		rst_tbl.Reserved[i] = {tbl[11+offset],tbl[12+offset]};
		rst_tbl.Class_of_Device[i] = tbl[13+offset]+bit32.lshift(tbl[14+offset],8)+bit32.lshift(tbl[15+offset],16);
		rst_tbl.Clock_Offset[i] = tbl[16+offset] + bit32.lshift(tbl[17+offset],8);
	end
	return true,rst_tbl;
end

function  EVENT_HCI_Connection_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x03) then  -- correct event code should be 0x03
	     return false;
	end
	
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]};
	rst_tbl.BD_ADDR = {tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset],tbl[10+offset],tbl[11+offset]};
	
	rst_tbl.Link_Type = tbl[12+offset];
	rst_tbl.Encryption_Enabled = tbl[13+offset];
	
	return  true,rst_tbl;

end

function  EVENT_HCI_Synchronous_Connection_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x2c) then  -- correct event code should be 0x2c
	     return false;
	end
	
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]};
	rst_tbl.BD_ADDR = {tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset],tbl[10+offset],tbl[11+offset]};
	
	rst_tbl.Link_Type = tbl[12+offset];
	rst_tbl.Transmission_Interval = tbl[13+offset];
	rst_tbl.Retransmission_Window = tbl[14+offset];
	rst_tbl.RX_Packet_Length = tbl[15+offset]+tbl[16+offset]*256;
	rst_tbl.TX_Packet_Length = tbl[17+offset]+tbl[18+offset]*256;
	rst_tbl.Air_Mode = tbl[19+offset];

	return  true,rst_tbl;

end

function  EVENT_HCI_Synchronous_Connection_Changed(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x2d) then  -- correct event code should be 0x2d
	     return false;
	end
	
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]};
	
	rst_tbl.Transmission_Interval = tbl[6+offset];
	rst_tbl.Retransmission_Window = tbl[7+offset];
	rst_tbl.RX_Packet_Length = tbl[8+offset]+tbl[9+offset]*256;
	rst_tbl.TX_Packet_Length = tbl[10+offset]+tbl[11+offset]*256;

	return  true,rst_tbl;

end

function  EVENT_HCI_Connection_Request(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x04) then  -- correct event code should be 0x04
	     return false;
	end
	
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	rst_tbl.Class_Of_Device = {tbl[9+offset],tbl[10+offset],tbl[11+offset]};
	rst_tbl.Link_Type = tbl[12+offset];
	
	return  true,rst_tbl;
end

function EVENT_HCI_Disconnection_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x05) then  -- correct event code should be 0x05
	     return false;
	end
	
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]};
	rst_tbl.Reason = tbl[6+offset]
	
	return  true,rst_tbl;
end

function  EVENT_HCI_Authentication_Complete(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	if(tbl[1+offset] ~= 0x06) then  -- correct event code should be 0x08
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + (tbl[5+offset]*256);

	return  true,rst_tbl;
	
end

function  EVENT_HCI_Remote_Name_Request_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	if(tbl[1+offset] ~= 0x07) then  -- correct event code should be 0x08
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]};
	rst_tbl.Remote_Name={}
	for i=1,248 do
		if(tbl[9+offset+i]~=nil) then
			rst_tbl.Remote_Name[i] = tbl[9+offset+i];
		else
			break;
		end
	end

	return true,rst_tbl;
end


function  EVENT_HCI_Encryption_Change(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	if(tbl[1+offset] ~= 0x08) then  -- correct event code should be 0x08
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]};
	rst_tbl.Encryption_Enabled = tbl[6+offset];
	
	return true,rst_tbl;
end

function  EVENT_HCI_Change_Connection_Link_Key_Complete(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	if(tbl[1+offset] ~= 0x09) then  -- correct event code should be 0x08
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + (tbl[5+offset]*256);

	return  true,rst_tbl;
end

function  EVENT_HCI_Encryption_Key_Refresh_Complete(tbl,mode)

	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	if(tbl[1+offset] ~= 0x30) then  -- correct event code should be 0x08
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + (tbl[5+offset]*256);

	return  true,rst_tbl;
end

function  EVENT_HCI_Command_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x0e) then  -- correct event code should be 0x0e
	     return false;
	end
    -- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Num_HCI_Command_Packets = tbl[3+offset];
	rst_tbl.Command_Opcode = {tbl[4+offset], tbl[5+offset]};
	rst_tbl.Return_Parameters = {};
	local ret_para_len = tbl[2+offset] - 3;
	if (ret_para_len >= 1) then 
        for i = 1,ret_para_len,1 do
        	rst_tbl.Return_Parameters[i] = tbl[5+offset+i];
        end
	end

	return true, rst_tbl;
end

function  EVENT_HCI_Command_Status(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x0f) then  -- correct event code should be 0x0f
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Num_HCI_Command_Packets = tbl[4+offset];
	rst_tbl.Command_Opcode = {tbl[5+offset], tbl[6+offset]};

	return true, rst_tbl;
end

function EVENT_HCI_Inquiry_Result_with_RSSI(tbl,mode)
	local flag;
	local offset = 0;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x22) then -- correct event code should be 0x02
	     return false;
	end

	rst_tbl.Num_Responses = tbl[3+offset];
    rst_tbl.BD_ADDR = {};
	rst_tbl.Page_Scan_Repetition_Mode = {};
	rst_tbl.Reserved = {};
	rst_tbl.Class_of_Device = {};
	rst_tbl.Clock_Offset = {};
	rst_tbl.RSSI = {}
	for i = 1,Num_Responses,1 do
		rst_tbl.BD_ADDR[i] = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]};
		rst_tbl.Page_Scan_Repetition_Mode[i] = tbl[10+offset];
		rst_tbl.Reserved[i] = tbl[11+offset];
		rst_tbl.Class_of_Device[i] = tbl[12+offset]+bit32.lshift(tbl[13+offset],8)+bit32.lshift(tbl[14+offset],16);
		rst_tbl.Clock_Offset[i] = tbl[15+offset] + bit32.lshift(tbl[16+offset],8);
		rst_tbl.RSSI[i] = tbl[17+offset];
	end
	

	return true,rst_tbl;
end

function EVENT_HCI_Extended_Inquiry_Result(tbl,mode)
	local flag;
	local offset = 0;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x2f) then -- correct event code should be 0x02
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Num_Responses = tbl[3+offset];
    rst_tbl.BD_ADDR = {};
	rst_tbl.Page_Scan_Repetition_Mode = {};
	rst_tbl.Reserved = {};
	rst_tbl.Class_of_Device = {};
	rst_tbl.Clock_Offset = {};
	rst_tbl.RSSI = {}
	rst_tbl.Extended_Inquiry_response = {}
	for i = 1,Num_Responses,1 do
		rst_tbl.BD_ADDR[i] = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]};
		rst_tbl.Page_Scan_Repetition_Mode[i] = tbl[10+offset];
		rst_tbl.Reserved[i] = tbl[11+offset];
		rst_tbl.Class_of_Device[i] = tbl[12+offset]+bit32.lshift(tbl[13+offset],8)+bit32.lshift(tbl[14+offset],16);
		rst_tbl.Clock_Offset[i] = tbl[15+offset] + bit32.lshift(tbl[16+offset],8);
		rst_tbl.RSSI[i] = tbl[17+offset];
		rst_tbl.Extended_Inquiry_response[i]= array.new(240)
		Extended_Inquiry_response_len = rst_tbl.para_len-16
		for j=1,Extended_Inquiry_response_len do
			rst_tbl.Extended_Inquiry_response[i][j] = tbl[18+offset+j]
		end
	end

	return true,rst_tbl;	
end

function Event_HCI_Role_Change(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x12) then  -- correct event code should be 0x12
	     return false;
	end
	rst_tbl.Status   = tbl[3+offset]
	rst_tbl.BD_ADDR  = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]}
	rst_tbl.New_Role = tbl[10+offset]

	return true, rst_tbl;
end

function Event_HCI_Num_Of_Commplete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x13) then  -- correct event code should be 0x12
	     return false;
	end
	rst_tbl.num_handle   = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.num_commplete = {tbl[6+offset],tbl[7+offset]}
	


	return true, rst_tbl;
end
function Event_HCI_Mode_Change(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x14) then  -- correct event code should be 0x14
	     return false;
	end
	rst_tbl.Status   = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.Current_Mode = tbl[6+offset]
	rst_tbl.Interval = {tbl[7+offset],tbl[8+offset]}
	return true, rst_tbl;
end

function Event_HCI_Max_Slots_Change(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x1b) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.Connection_Handle = {tbl[3+offset],tbl[4+offset]}
	rst_tbl.LMP_Max_Slots = tbl[5+offset]

	return true, rst_tbl;
end

function Event_HCI_Read_Clock_Offset_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x1c) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.Clock_Offset = tbl[6+offset]

	return true, rst_tbl;
end


function  EVENT_HCI_Connection_Packet_Type_Changed(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	local evt_code = 0x1d; -- HCI_Connection_Packet_Type_Changed Event code
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= evt_code) then  -- correct event code should be checked
	     return false;
	end

	--andy:fill in the return tbl here 
    rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status= tbl[5+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + (tbl[5+offset]*256);
	rst_tbl.Packet_Type = tbl[6+offset] + (tbl[7+offset]*256);  --LMP_Features has 8 bytes 

	return true, rst_tbl;
	
end

function EVENT_HCI_Read_Remote_Supported_Features_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x0b) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.LMP_Features = {tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset],tbl[10+offset],tbl[11+offset],tbl[12+offset],tbl[13+offset]}

	return true, rst_tbl;
end

function EVENT_HCI_Read_Remote_Ext_Supported_Features_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	local evt_code = 0x23; -- both 0x00 and 0xff are invaild code
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end

	if(tbl[1+offset] ~= evt_code) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.LMP_Features = {tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset],tbl[10+offset],tbl[11+offset],tbl[12+offset],tbl[13+offset]}

	return true, rst_tbl;
end
function EVENT_HCI_Remote_Name_Request_Ext_feature_notification_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	local evt_code = 0x3d; -- both 0x00 and 0xff are invaild code
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end

	if(tbl[1+offset] ~= evt_code) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.LMP_Features = {tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset],tbl[10+offset],tbl[11+offset],tbl[12+offset],tbl[13+offset]}

	return true, rst_tbl;
end

function  EVENT_HCI_Read_Remote_Version_Information_Complete(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	local evt_code = 0x0c; -- both 0x00 and 0xff are invaild code
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= evt_code) then  -- correct event code should be checked
	     return false;
	end

	--andy:fill in the return tbl here 
    	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status= tbl[3+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + (tbl[5+offset]*256);
	rst_tbl.Version = tbl[6+offset];
	rst_tbl.Manufacturer_Name = tbl[7+offset] + (tbl[8+offset]*256);
	rst_tbl.Subversion = tbl[9+offset] + (tbl[10+offset]*256);


	return true, rst_tbl;
end
function EVENT_HCI_QoS_Setup_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x0d) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.Unused = tbl[6+offset]
	rst_tbl.Service_Type = tbl[7+offset]
	rst_tbl.Token_Rate = {tbl[8+offset],tbl[9+offset],tbl[10+offset],tbl[11+offset]}
	rst_tbl.Peak_Bandwidth = {tbl[12+offset],tbl[13+offset],tbl[14+offset],tbl[15+offset]}
	rst_tbl.Latency = {tbl[16+offset],tbl[17+offset],tbl[18+offset],tbl[19+offset]}
	rst_tbl.Delay_Variation = {tbl[20+offset],tbl[21+offset],tbl[22+offset],tbl[23+offset]}
	return true, rst_tbl;
end

function EVENT_HCI_Flow_Specification_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x21) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.Unused = tbl[6+offset]
	rst_tbl.Flow_direction = tbl[7+offset]
	rst_tbl.Service_Type = tbl[8+offset]
	rst_tbl.Token_Rate = {tbl[9+offset],tbl[10+offset],tbl[11+offset],tbl[12+offset]}
	rst_tbl.Token_Bucket_Size = {tbl[13+offset],tbl[14+offset],tbl[15+offset],tbl[16+offset]}	
	rst_tbl.Peak_Bandwidth = {tbl[17+offset],tbl[18+offset],tbl[19+offset],tbl[20+offset]}
	rst_tbl.Access_Latency = {tbl[21+offset],tbl[22+offset],tbl[23+offset],tbl[24+offset]}
	return true, rst_tbl;
end

function EVENT_HCI_Link_Supervision_Timeout_Changed(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x38) then  -- correct event code should be 0x38
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.Link_Supervision_Timeout = {tbl[6+offset],tbl[7+offset]}
	return true, rst_tbl;
end

function EVENT_HCI_Sniff_Subrating(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x2e) then  -- correct event code should be 0x38
	     return false;
	end
	rst_tbl.Status = tbl[3+offset]
	rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]}
	rst_tbl.Maximum_Transmit_Latency = {tbl[6+offset],tbl[7+offset]}
	rst_tbl.Maximum_Receive_Latency = {tbl[8+offset],tbl[9+offset]}
	rst_tbl.Minimum_Remote_Timeout = {tbl[10+offset],tbl[11+offset]}
	rst_tbl.Minimum_Local_Timeout = {tbl[12+offset],tbl[13+offset]}
	return true, rst_tbl;
end

function  EVENT_HCI_Link_Key_Request(tbl,mode)
	
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x17) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	
	return true, rst_tbl;
end

function EVENT_HCI_IO_Capability_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x31) then  -- correct event code should be 0x0f
	     return false;
	end
	
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	
	return true, rst_tbl;
end

function EVENT_HCI_IO_Capability_Response(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x32) then  -- correct event code should be 0x0f
	     return false;
	end
	
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	rst_tbl.IO_Capability =  tbl[9+offset];
	rst_tbl.OOB_Data_Present =  tbl[10+offset];
	rst_tbl.Authentication_Requirements =  tbl[11+offset];

	return true, rst_tbl;
end

function EVENT_HCI_User_Confirmation_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x33) then  -- correct event code should be 0x33
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	rst_tbl.Numeric_Value = tbl[9+offset] + (tbl[10+offset]*256) + (tbl[11+offset]*256*256)+(tbl[12+offset]*256*256*256);
	
	return true, rst_tbl;

end

function EVENT_HCI_User_Passkey_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x34) then  -- correct event code should be 0x33
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	return true, rst_tbl;

end

function EVENT_HCI_Remote_OOB_Data_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x35) then  -- correct event code should be 0x33
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	return true, rst_tbl;
end



function EVENT_HCI_User_Passkey_Notification(tbl,mode)
	local flag;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x3b) then  -- correct event code should be 0x38
	     return false;
	end
	
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	rst_tbl.PassKey = {tbl[9+offset],tbl[10+offset],tbl[11+offset],tbl[12+offset]} ;
	return true, rst_tbl;
end

function  EVENT_HCI_Simple_Pairing_Complete(tbl,mode)

	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x36) then  -- correct event code should be 0x36
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.BD_ADDR = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]};
	
	return true, rst_tbl;

end


function  EVENT_HCI_Link_Key_Notification(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x18) then  -- correct event code should be 0x36
	     return false;
	end
	
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	rst_tbl.Link_Key = {tbl[9+offset],tbl[10+offset],tbl[11+offset],tbl[12+offset],
						tbl[13+offset],tbl[14+offset],tbl[15+offset],tbl[16+offset],
						tbl[17+offset],tbl[18+offset],tbl[19+offset],tbl[20+offset],
						tbl[21+offset],tbl[22+offset],tbl[23+offset],tbl[24+offset]};
	
	rst_tbl.Key_Type = tbl[25+offset];
	
	return true, rst_tbl;

end

function  EVENT_HCI_PIN_Code_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x16) then  -- correct event code should be 0x36
	     return false;
	end
	
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	return true, rst_tbl;
end

function EVENT_HCI_Read_Timing_Accuracy_Res(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	local evt_code = 0xff; -- HCI_Read_Clock_Offset_Complete Event code
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= evt_code) then  -- correct event code should be checked
	     return false;
	end

	--andy:fill in the return tbl here 
    rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status= tbl[3+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + (tbl[5+offset]*256);
	rst_tbl.drift = tbl[6+offset]; 
	rst_tbl.jitter = tbl[7+offset]

	return true, rst_tbl;
	
end
-----------   HCI event table  -------------------

g_hci_event_tbl = {
	{ code = 0x01, name = "HCI_Inquiry_Complete", func = EVENT_HCI_Inquiry_Complete},
	{ code = 0x02, name = "HCI_Inquiry_Result", func = EVENT_HCI_Inquiry_Result},
	{ code = 0x03, name = "HCI_Connection_Complete", func = EVENT_HCI_Connection_Complete },
	{ code = 0x04, name = "HCI_Connection_Request", func = EVENT_HCI_Connection_Request },
	{ code = 0x05, name = "HCI_Disconnection_Complete", func = EVENT_HCI_Disconnection_Complete },
	{ code = 0x06, name = "HCI_Authentication_Complete", func = EVENT_HCI_Authentication_Complete},
	{ code = 0x07, name = "HCI_Remote_Name_Request_Complete", func = EVENT_HCI_Remote_Name_Request_Complete},
	{ code = 0x08, name = "HCI_Encryption_Change", func = EVENT_HCI_Encryption_Change},
	{ code = 0x09, name = "HCI_Change_Connection_Link_Key_Complete", func = EVENT_HCI_Change_Connection_Link_Key_Complete},
	{ code = 0x0b, name = "HCI_Read_Remote_Supported_Features_Complete", func = EVENT_HCI_Read_Remote_Supported_Features_Complete},
	{ code = 0x0c, name = "HCI_Read_Remote_Version_Information_Complete", func = EVENT_HCI_Read_Remote_Version_Information_Complete},
	{ code = 0x0d, name = "HCI_QoS_Setup_Complete", func = EVENT_HCI_QoS_Setup_Complete},
	{ code = 0x0e, name = "HCI_Command_Complete", func = EVENT_HCI_Command_Complete},
	{ code = 0x0f, name = "HCI_Command_Status", func = EVENT_HCI_Command_Status},
	{ code = 0x12, name = "HCI_Role_Change", func = Event_HCI_Role_Change},
	{ code = 0x13, name = "HCI_Num_Of_Commplete", func = Event_HCI_Num_Of_Commplete},
	{ code = 0x14, name = "HCI_Mode_Change", func = Event_HCI_Mode_Change},
	{ code = 0x16, name = "HCI_PIN_Code_Request",func = EVENT_HCI_PIN_Code_Request},
	{ code = 0x17, name = "HCI_Link_Key_Request", func = EVENT_HCI_Link_Key_Request },
	{ code = 0x18, name = "HCI_Link_Key_Notification", func = EVENT_HCI_Link_Key_Notification },
	{ code = 0x1b, name = "HCI_Max_Slots_Change", func = Event_HCI_Max_Slots_Change},
	{ code = 0x1c, name = "HCI_Read_Clock_Offset_Complete", func = Event_HCI_Read_Clock_Offset_Complete},
	{ code = 0x1d, name = "HCI_Connection_Packet_Type_Changed", func = EVENT_HCI_Connection_Packet_Type_Changed },
	{ code = 0x21, name = "HCI_Flow_Specification_Complete", func = EVENT_HCI_Flow_Specification_Complete},
	{ code = 0x22, name = "HCI_Inquiry_Result_with_RSSI", func = EVENT_HCI_Inquiry_Result_with_RSSI},
	{ code = 0x23, name = "HCI_Read_Remote_Ext_Supported_Features_Complete",func = EVENT_HCI_Read_Remote_Ext_Supported_Features_Complete},
	{ code = 0x2c, name = "HCI_Synchronous_Connection_Complete",func = EVENT_HCI_Synchronous_Connection_Complete},
	{ code = 0x2d, name = "HCI_Synchronous_Connection_Changed",func = EVENT_HCI_Synchronous_Connection_Changed},
	{ code = 0x2e, name = "HCI_Sniff_Subrating_Result", func = EVENT_HCI_Sniff_Subrating},
	{ code = 0x2f, name = "HCI_Extended_Inquiry_Result", func = EVENT_HCI_Extended_Inquiry_Result},
	{ code = 0x30, name = "HCI_Encryption_Key_Refresh_Complete", func = EVENT_HCI_Encryption_Key_Refresh_Complete},
	{ code = 0x31, name = "HCI_IO_Capability_Request", func = EVENT_HCI_IO_Capability_Request },
	{ code = 0x32, name = "HCI_IO_Capability_Response", func = EVENT_HCI_IO_Capability_Response },
	{ code = 0x33, name = "HCI_User_Confirmation_Request",func = EVENT_HCI_User_Confirmation_Request }, 
	{ code = 0x34, name = "HCI_User_Passkey_Request",func = EVENT_HCI_User_Passkey_Request }, 
	{ code = 0x35, name = "HCI_Remote_OOB_Data_Request",func = EVENT_HCI_Remote_OOB_Data_Request}, 
	{ code = 0x36, name = "HCI_Simple_Pairing_Complete",func = EVENT_HCI_Simple_Pairing_Complete },
	{ code = 0x38, name = "HCI_Link_Supervision_Timeout_Changed", func = EVENT_HCI_Link_Supervision_Timeout_Changed},
	{ code = 0x3b, name = "HCI_User_Passkey_Notification", func = EVENT_HCI_User_Passkey_Notification},
	{ code = 0x3d, name = "HCI_Remote_Name_Request_Ext_feature_notification_Complete", func = EVENT_HCI_Remote_Name_Request_Ext_feature_notification_Complete},
	{ code = 0xff, name = "HCI_Read_Timing_Accuracy_res",func = EVENT_HCI_Read_Timing_Accuracy_Res},
};



function  HCI_Event_Exec(tbl,tbl_len,mode)
	local code;
	local flag;
	local rst_tbl;

	tl_usb_bulk_print_buf_flush();

	if(mode=="uart" or mode =="dongle") then
		if(tbl[1]~=0x04) then 
			return
		end

		code = tbl[2];
	end

	if(mode=="usb") then
		code = tbl[1];
	end

	if(mode=="unused") then
		return 0,"",nil,nil
	end

	
	for i,v in ipairs(g_hci_event_tbl) do
		if(g_hci_event_tbl[i].code == code) then
			log_printf(HCI_EVENT_RESULT_DISPLAY,g_hci_event_tbl[i].name..":",tbl,tbl_len)
			flag, rst_tbl = g_hci_event_tbl[i].func(tbl,mode);  --call the event function
			return  code,g_hci_event_tbl[i].name,flag,rst_tbl;
		end
	end
	
	return  0,"",nil,nil;
end



------------------------HCI abnormal Test event--------------------------
function EVENT_HCI_Abnormal_Write_Feature_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   if(tbl[1] ~= 0x10) then 
	     return false;
	   end
	end
	
	if(tbl[2] ~= 0x01) then  -- correct event code should be 0x01
	     return false;
	end
	rst_tbl.Status = tbl[3]
	rst_tbl.Connection_Handle = {tbl[4],tbl[5]}
	return true, rst_tbl;
end
function EVENT_HCI_Abnormal_PW_INCR_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   if(tbl[1] ~= 0x10) then 
	     return false;
	   end
	end
	
	if(tbl[2] ~= 0x03) then  -- correct event code should be 0x03
	     return false;
	end
	rst_tbl.Status = tbl[3]
	rst_tbl.Connection_Handle = {tbl[4],tbl[5]}
	return true, rst_tbl;
end
function EVENT_HCI_Abnormal_PW_DECR_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   if(tbl[1] ~= 0x10) then 
	     return false;
	   end
	end
	
	if(tbl[2] ~= 0x04) then  -- correct event code should be 0x04
	     return false;
	end
	rst_tbl.Status = tbl[3]
	rst_tbl.Connection_Handle = {tbl[4],tbl[5]}
	return true, rst_tbl;
end
function EVENT_HCI_Abnormal_PW_CNTL_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   if(tbl[1] ~= 0x10) then 
	     return false;
	   end
	end
	
	if(tbl[2] ~= 0x05) then  -- correct event code should be 0x05
	     return false;
	end
	rst_tbl.Status = tbl[3]
	rst_tbl.Connection_Handle = {tbl[4],tbl[5]}
	return true, rst_tbl;
end

function EVENT_HCI_Abnormal_EVENT_MASK_Complete(tbl,mode)
	local flag;
	local rst_tbl={};
	if(mode == "uart" or mode=="dongle") then    -- uart need one more byte 0x04
	   if(tbl[1] ~= 0x10) then 
	     return false;
	   end
	end
	
	if(tbl[2] ~= 0x06) then  -- correct event code should be 0x06
	     return false;
	end
	rst_tbl.Status = tbl[3]
	rst_tbl.Connection_Handle = {tbl[4],tbl[5]}
	return true, rst_tbl;
end
------------------------HCI abnormal Test event table------------------------------------------------------


hci_abnormal_event_tbl = {

	{code = 0x01, name = "HCI_Abnormal_Write_Feature_Complete", func = EVENT_HCI_Abnormal_Write_Feature_Complete},
	{code = 0x03, name = "HCI_Abnormal_PW_INCR_Complete", func = EVENT_HCI_Abnormal_PW_INCR_Complete},
	{code = 0x04, name = "HCI_Abnormal_PW_DECR_Complete", func = EVENT_HCI_Abnormal_PW_DECR_Complete},
	{code = 0x05, name = "HCI_Abnormal_PW_CNTL_Complete", func = EVENT_HCI_Abnormal_PW_CNTL_Complete},
	{code = 0x06, name = "HCI_Abnormal_EVENT_MASK_Complete", func = EVENT_HCI_Abnormal_EVENT_MASK_Complete},

}


function  HCI_ABEvent_Exec(tbl,tbl_len,mode)
	local code;
	local flag;
	local rst_tbl;

	tl_usb_bulk_print_buf_flush();

	if(mode=="uart" or mode =="dongle") then
		if(tbl[1]~=0x10) then 
			return
		end

		code = tbl[2];
	end

	if(mode=="unused") then
		return 0,"",nil,nil
	end

	for i,v in ipairs(hci_abnormal_event_tbl) do
		if(hci_abnormal_event_tbl[i].code == code) then
			log_printf(HCI_EVENT_RESULT_DISPLAY,hci_abnormal_event_tbl[i].name..":",tbl,tbl_len)
			flag, rst_tbl = hci_abnormal_event_tbl[i].func(tbl,mode);  --call the event function
			return  code,hci_abnormal_event_tbl[i].name,flag,rst_tbl;
		end
	end
	
	return  0,"",nil,nil;
end


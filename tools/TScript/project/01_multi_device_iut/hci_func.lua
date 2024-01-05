-- HCI EVENT Process Function Map
require"device"
require"printf"

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

function HCI_Disconnect (handle,mode,Connection_Handle,Reason)
		
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
	a[4+offset] = bit32.band(Connection_Handle ,0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8) , 0xff);
	a[6+offset] = Reason;
	
	if(mode=="uart") then
		len = tl_rs232_send(a,7) -- dongle
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Disconnect:",a,7);
	end

	if(mode=="dongle") then
		len = tl_usb_bulk_out(handle,a,7) -- dongle
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Disconnect:",a,7);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,6) --csr
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Disconnect:",a,6);
	end
	return len;

end

function HCI_Reset(handle,mode)
		
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
		len = tl_usb_bulk_out(handle,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reset:",a,4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reset:",a,3);
	end
	return len;
end

function HCI_Inquiry(handle,mode,LAP,Inquiry_Length,Num_Responses)
		
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
		len = tl_usb_bulk_out(handle,a,9)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Inquiry:",a,9);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Inquiry:",a,8);
	end
	return len;
	
end

function HCI_Write_Scan_Enable(handle,mode,Scan_Enable)
		
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
		len = tl_usb_bulk_out(handle,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Scan_Enable:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Scan_Enable:",a,4);
	end
	return len;

end

function HCI_Write_Page_Scan_Activity(handle,mode,Page_Scan_Interval,Page_Scan_Window)
		
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
		len = tl_usb_bulk_out(handle,a,8)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Scan_Activity:",a,8);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,7)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Page_Scan_Activity:",a,7);
	end
	return len;

end


function  HCI_Create_Connection (handle,mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
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
		len = tl_usb_bulk_out(handle,a,17)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Create_Connection:",a,17);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,16)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Create_Connection:",a,16);
	end

	print(string.format("Bluetooth address: %02x %02x %02x %02x %02x %02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));

	return len;

end

function  HCI_Accept_Connection_Request (handle,mode,BD_ADDR,Role)
		
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
		len = tl_usb_bulk_out(handle,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Connection_Request:",a,11);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Accept_Connection_Request:",a,10);
	end

	print(string.format("Bluetooth address %02x %02x %02x %02x %02x %02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));
	
	return len;

end

function  HCI_Reject_Connection_Request (handle,mode,BD_ADDR,Reason)
		
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
		len = tl_usb_bulk_out(handle,a,11)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Connection_Request:",a,11);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,10)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Reject_Connection_Request:",a,10);
	end

	print(string.format("Bluetooth address %02x %02x %02x %02x %02x %02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));
	
	return len;

end

function HCI_Write_Authentication_Enable(handle,mode,Authentication_Enable)
		
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
		len = tl_usb_bulk_out(handle,a,5)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Authentication_Enable:",a,5);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Authentication_Enable:",a,4);
	end
	return len;
end

function HCI_Write_Local_Name(handle,mode,Local_Name)
		
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
		len = tl_usb_bulk_out(handle,a,strlen+4)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Local_Name:",a,strlen+4);
	end

	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,strlen+3)
		log_printf(HCI_COMMAND_DISPLAY,"HCI_Write_Local_Name:",a,strlen+3);
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
		len = tl_rs232_send(handle_w,a,8)
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
		len = tl_rs232_send(handle_w,a,12)
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

-----------   HCI event table  -------------------

g_hci_event_tbl = {
{ code = 0x01, name = "HCI_Inquiry_Complete", func = EVENT_HCI_Inquiry_Complete},
{ code = 0x02, name = "HCI_Inquiry_Result", func = EVENT_HCI_Inquiry_Result},
{ code = 0x03, name = "HCI_Connection_Complete", func = EVENT_HCI_Connection_Complete },
{ code = 0x04, name = "HCI_Connection_Request", func = EVENT_HCI_Connection_Request },
{ code = 0x05, name = "HCI_Disconnection_Complete", func = EVENT_HCI_Disconnection_Complete },
{ code = 0x07, name = "HCI_Remote_Name_Request_Complete", func = EVENT_HCI_Remote_Name_Request_Complete},
{ code = 0x08, name = "HCI_Encryption_Change", func = EVENT_HCI_Encryption_Change},
{ code = 0x0b, name = "HCI_Read_Remote_Supported_Features_Complete", func = EVENT_HCI_Read_Remote_Supported_Features_Complete},
{ code = 0x0d, name = "HCI_QoS_Setup_Complete", func = EVENT_HCI_QoS_Setup_Complete},
{ code = 0x0e, name = "HCI_Command_Complete", func = EVENT_HCI_Command_Complete},
{ code = 0x0f, name = "HCI_Command_Status", func = EVENT_HCI_Command_Status},
{ code = 0x12, name = "HCI_Role_Change", func = Event_HCI_Role_Change},
{ code = 0x14, name = "HCI_Mode_Change", func = Event_HCI_Mode_Change},
{ code = 0x1b, name = "HCI_Max_Slots_Change", func = Event_HCI_Max_Slots_Change},
{ code = 0x1c, name = "HCI_Read_Clock_Offset_Complete", func = Event_HCI_Read_Clock_Offset_Complete},
{ code = 0x21, name = "HCI_Flow_Specification_Complete", func = EVENT_HCI_Flow_Specification_Complete},
{ code = 0x22, name = "HCI_Inquiry_Result_with_RSSI", func = EVENT_HCI_Inquiry_Result_with_RSSI},
{ code = 0x2e, name = "HCI_Sniff_Subrating_Result", func = EVENT_HCI_Sniff_Subrating},
{ code = 0x2f, name = "HCI_Extended_Inquiry_Result", func = EVENT_HCI_Extended_Inquiry_Result},
{ code = 0x38, name = "HCI_Link_Supervision_Timeout_Changed", func = EVENT_HCI_Link_Supervision_Timeout_Changed},

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
	
	if(tbl[2] ~= 0x01) then  -- correct event code should be 0x38
	     return false;
	end
	rst_tbl.Status = tbl[3]
	rst_tbl.Connection_Handle = {tbl[4],tbl[5]}
	return true, rst_tbl;
end
------------------------HCI abnormal Test event table------------------------------------------------------


hci_abnormal_event_tbl = {

{ code = 0x01, name = "HCI_Abnormal_Write_Feature_Complete", func = EVENT_HCI_Abnormal_Write_Feature_Complete},

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


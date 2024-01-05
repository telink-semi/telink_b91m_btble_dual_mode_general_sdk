

dofile(".\\lib\\hci.lua")
print("LUA Version: ".._VERSION)
print("============HCI TEST demo=============")

-- gui init --
tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
tl_form_show(50,50,200,100)
tl_button_show(0,20,80,32,"stop",1)
tl_button_show(100,20,80,32,"test",2)

-- kite usb dongle init --
handle_r = tl_usb_init2(0x05d8);
handle_w = tl_usb_init2(0x05d8); 

-- csr dongle init ---
handle_tb = tl_btusb_init();

if(handle_r ==0  or handle_w==0 or handle_tb==0) then
	print("usb error!!");
	tl_error(1);
	tl_form_close();
	tl_stop(100);
end

tl_usb_bulk_bufmode_set(1)
tl_usb_bulk_monitor_start(handle_r)


-- step 1 : inqiury and inquiry scan --
--HCI_Inquiry(handle_tb,"usb",0x9E8B33,10,10);
--HCI_Write_Scan_Enable(handle_w,"uart",0x03); --inquiry scan and page scan
-- Default setting
step = 1;
HCI_Reset(handle_tb,"usb");
HCI_Write_Scan_Enable(handle_w,"uart",3);
-- get result --
flag = false;
while(1)
do 
	result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
	ret_type2,code2,name2,flag2,rst_tbl2=dongle_print_process(result_tbl2,result_len2);
	tl_usb_bulk_print_buf_flush();

	--result_tbl,result_len = tl_usb_bulk_read() 
	result_tbl,result_len = HCI_result_read(handle_tb,"usb")
	code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");
	
	if(step == 1) then
		if(name == "HCI_Command_Complete") then
			if(flag) then
				  if( rst_tbl.Command_Opcode == 0x0c03) then
				  --print("CSR dongle 
					step = 2;
					HCI_Write_Simple_Pairing_Mode(handle_tb,"usb",1);
				  end
				--print(string.format("step 1: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
		
		
		
	
	end
	
	if(step == 2) then
		if(name == "HCI_Command_Complete") then
			if(flag) then
				  if( rst_tbl.Command_Opcode == 0x0c56) then
					step = 3;
					HCI_Write_Authentication_Enable(handle_tb,"usb",1);
				  end
				--print(string.format("step 2: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
	
	end
	
	if(step == 3) then
		if(name == "HCI_Command_Complete") then
			if(flag) then
				  if( rst_tbl.Command_Opcode == 0x0c20) then
				    print("send HCI_Set_Event_Mask");
					--break;
					local event_mask = {0xff,0x9f,0xfb,0xbf,0x07,0xf8,0xbf,0x1d};
					HCI_Set_Event_Mask(handle_tb,"usb",event_mask);
					step = 4;
					--HCI_Write_Authentication_Enable(handle_tb,"usb",1);
				  end
				--print(string.format("step 2: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
	
	end
	
	if(step == 4) then
		if(name == "HCI_Command_Complete") then
			if(flag) then
				  if( rst_tbl.Command_Opcode == 0x0c01) then
				    --HCI_Write_Secure_Connections_Host_Support(handle_tb,"usb",1);
				    print("default setting done!!");
					break;
					--step = 5;
				  end
				--print(string.format("step 2: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
	
	end
	
	if(step == 5) then -- not support by CSR donlge!!
		if(name == "HCI_Command_Complete") then
			if(flag) then
				  if( rst_tbl.Command_Opcode == 0x0c7a) then
				    --HCI_Write_Secure_Connections_Host_Support(handle_tb,"usb",1);
				    print("default setting done!!");
					break;
				  end
				--print(string.format("step 2: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
		if(name == "HCI_Command_Status") then
			if(flag) then
				table.print(rst_tbl);
			end
		end 
	
	end
	
	--gui process
	--tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
	gui_result_tbl,btn_idx = tl_message_get();
	if btn_idx == 1 then
		break;
	end
	if btn_idx == 88 then  --timer
	   --print("timer")
	end 
end


--]]
-- step 2:  page, and page scan ---
-- 注意这里的BD_ADDR是个二维数组
addr ={};
addr[1] = 0xe6;
addr[2] = 0xd1;
addr[3] = 0xd2;
addr[4] = 0xd3;
addr[5] = 0xd4;
addr[6] = 0xd5;

csr_addr = {};

HCI_Create_Connection(handle_tb,"usb",addr,0xcc18,0x02,
0x00,0x00)

flag = false;
step = 1;
acl_handle = 0;
while(1)
do 

    
	result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
	ret_type2,code2,name2,flag2,rst_tbl2=dongle_print_process(result_tbl2,result_len2);
	tl_usb_bulk_print_buf_flush();

	--result_tbl,result_len = tl_usb_bulk_read() 
	result_tbl,result_len = HCI_result_read(handle_tb,"usb")
	code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");
	
	--table.print(rst_tbl);
	
	if(step == 1) then
		if(name == "HCI_Connection_Complete") then
			 table.print(rst_tbl);
			 if(flag) then
				if(rst_tbl.Status==0) then
				   print("acl create ok!");
				   --acl_handle = rst_tbl.Connection_Handle;
				   print(string.format("CSR ACL handle = 0x%04x",rst_tbl.Connection_Handle));
				   --step = 2;
			 end
			end
		end
		if(ret_type2==1) then -- hci event
			table.print(rst_tbl2);
		end
		
		if(name2 == "HCI_Connection_Request" ) then
				if(flag2) then
				
					HCI_Accept_Connection_Request(handle_w,"uart",rst_tbl2.BD_ADDR,1);
					
				end
		
		end
		
		if(name2 == "HCI_Connection_Complete") then
			 table.print(rst_tbl2);
			 if(flag2) then
				if(rst_tbl2.Status==0) then
				   print("caracara acl create ok!");
				   acl_handle = rst_tbl2.Connection_Handle;
				   csr_addr = rst_tbl2.BD_ADDR;
				   print(string.format("caracara ACL handle = 0x%04x",rst_tbl2.Connection_Handle));
				   step = 2;
			 end
			end
		end
	end
	
	
	if step == 2 then
		HCI_Authentication_Requested(handle_w,"uart",acl_handle); 
		step = 3;
	end
	
	if(step == 3) then
		if(name2 == "HCI_Command_Status") then
				table.print(rst_tbl2);
				if(flag2) then
					if((rst_tbl2.Status==0) and (rst_tbl2.Command_Opcode == 0x411)) then
					   print("Auth response rcv");
					   
					   step = 4;
					end
				end
		
		end
	end
	
	if( step == 4) then
		if(name2 == "HCI_Link_Key_Request") then
			table.print(rst_tbl2);
			if(flag2) then
			print("-------- step 5 ----------");
				HCI_Link_Key_Request_Negative_Reply(handle_w,"uart",rst_tbl2.BD_ADDR);
				step = 5; -- to be continue...
			end
		end
	end
	
	if(step == 5) then
		if(name2 == "HCI_Command_Complete") then
			if(flag2) then
				  if( rst_tbl2.Command_Opcode == 0x040c) then
					step = 6;
				  end
				print(string.format("step 5: 0x%04x",rst_tbl2.Command_Opcode));
			end
		end
	
	end
	
	if(step == 6) then
		if(name2 == "HCI_IO_Capability_Request") then
			if(flag2) then
			     --[[ 
				 (IO capability =DisplayYesNo (0x01), OOB Data Present = Not Present (0x00), 
				04 MITM Protection Not Required – General Bonding. Numeric Compari-
son with automatic accept allowed.
				--]]
				HCI_IO_Capability_Request_Reply(handle_w,"uart",rst_tbl2.BD_ADDR,0x03,0x00,0x04);
				print("step 6---> step 7");
				step = 7;
				table.print(rst_tbl2);
			end
		end
	end
	
	if(step == 7) then 
		if(name2 == "HCI_Command_Complete") then
			if(flag2) then
				  if( rst_tbl2.Command_Opcode == 0x042b) then
					step = 8;
					print("step 7---> step 8");
				  end
				--print(string.format("step 5: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
	
	end
	
	if(step == 8) then
		if(name2 == "HCI_User_Confirmation_Request") then
			if(flag2) then
				  print(string.format("CARACARA number = %d",rst_tbl2.Numeric_Value));
				  print("step 8---> step 9");
				  HCI_User_Confirmation_Request_Reply(handle_w,"uart",csr_addr);
				  step = 9;
				--print(string.format("step 5: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
		
		if(name == "HCI_IO_Capability_Request") then
			if(flag) then
			     --[[ 
				 (IO capability =DisplayYesNo (0x01), OOB Data Present = Not Present (0x00), 
				04 MITM Protection Not Required – General Bonding. Numeric Compari-
son with automatic accept allowed.
				--]]
				HCI_IO_Capability_Request_Reply(handle_tb,"usb",rst_tbl.BD_ADDR,0x01,0x00,0x04);
				table.print(rst_tbl);
				name = " ";
			end
		end
		
		if(name == "HCI_User_Confirmation_Request") then
			if(flag) then
				  print(string.format("CSR number = %d",rst_tbl.Numeric_Value));
				  
				  HCI_User_Confirmation_Request_Reply(handle_tb,"usb",addr);
				  --step = 9;
				--print(string.format("step 5: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
	
	end
	-- 2020-03-07 --debug
	
	if(step == 9) then
	
		if(name2 == "HCI_Command_Complete") then
			if(flag2) then
					table.print(rst_tbl2);
				  if( rst_tbl2.Command_Opcode == 0x042c) then
				  print("step 8---> step 9");
					step = 10;
				  end
				--print(string.format("step 5: 0x%04x",rst_tbl.Command_Opcode));
			end
		end
	end
	
	if(step == 10) then
		if(name == "HCI_Simple_Pairing_Complete") then
			if(flag) then
				print("CSR : simple pairing complete");
				table.print(rst_tbl);
				--step = 11;
			end
		end
		
		if(name2 == "HCI_Simple_Pairing_Complete") then
			if(flag2) then
				print("caracara : simple pairing complete");
				table.print(rst_tbl2);
				print("step 10---> step 11");
				step = 11;
			end
		end
	end
	-- 2020 03-10 debug
	if(step == 11) then
		if(name == "HCI_Link_Key_Notification") then
			if(flag) then
				table.print(rst_tbl);
				print("CSR Link key coreated !");
				--step = 12;
			end
		end
		
		if(name2 == "HCI_Link_Key_Notification") then
			if(flag2) then
				table.print(rst_tbl2);
				print("CARACARA Link key coreated !");
				print("step 11---> step 12");
				step = 12;
			end
		end
	
	end
	
	if(step == 12) then
		if(name == "HCI_Authentication_Complete") then
			if(flag) then
				table.print(rst_tbl);
				print("Authentication OK !");
				--HCI_Set_Connection_Encryption(handle_tb,"usb",acl_handle,1);
				--step = 13;
			end
		end
		
		if(name2 == "HCI_Authentication_Complete") then
			if(flag2) then
				table.print(rst_tbl2);
				print("Authentication OK !");
				HCI_Set_Connection_Encryption(handle_w,"uart",acl_handle,1);
				step = 13;
			end
		end
	end
	
	if(step == 13) then --end
		if(name2 == "HCI_Command_Status") then
			if(flag2) then
				table.print(rst_tbl2);
				print("HCI_Set_Connection_Encryption done!");
				--break;
			end
		end
		if(name2 == "HCI_Encryption_Change") then
			if(flag2) then
				table.print(rst_tbl2);
				print("CARACARA encryption done!");
				--break;
			end
		end
		if(name == "HCI_Encryption_Change") then
			if(flag) then
				table.print(rst_tbl);
				print("CSR encryption done!");
				break;
			end
		end
	end
	
	tl_progress((step * 100) /13);
	
	
	--result_tbl2,result_len2 = HCI_result_read(handle_r,"uart")
		
	--gui process
	--tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
	gui_result_tbl,btn_idx = tl_message_get();
	if btn_idx == 1 then
		break;
	end
	if btn_idx == 88 then  --timer
	   --print("timer")
	end 
end

--print(string.format("ACL handle = 0x%04x",Connection_Handle));
-- important !! --
-- we need to close handles --
tl_btusb_close(handle_tb)
tl_usb_bulk_monitor_end()
tl_form_close()
-- script ok , process bar = 100%
tl_progress(100)



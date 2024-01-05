

dofile(".\\lib\\hci.lua")
print("LUA Version: ".._VERSION)
print("============HCI CON CREATE CARACARA SLAVE=============")

-- gui init --
tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
tl_form_show(50,50,200,100)
tl_button_show(0,20,80,32,"stop",1)
tl_button_show(100,20,80,32,"test",2)

-- kite usb dongle init --
handle_r = tl_usb_init2(0x05d8);
handle_w = tl_usb_init2(0x05d8); 

-- csr dongle init ---
---handle_tb = tl_btusb_init();

if(handle_r ==0  or handle_w==0 ) then
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
HCI_Reset(handle_w,"uart");
-- get result --
flag = false;
while(1)
do 
	result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
	ret_type2,code2,name2,flag2,rst_tbl2=dongle_print_process(result_tbl2,result_len2);
	tl_usb_bulk_print_buf_flush();

	--result_tbl,result_len = tl_usb_bulk_read() 
	-- result_tbl,result_len = HCI_result_read(handle_tb,"usb")
	-- code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");
	
	if(step == 1) then
		if(name2 == "HCI_Command_Complete") then
			if(flag2) then
				  if( rst_tbl2.Command_Opcode == 0x0c03) then 
					print("cara RESET")
					step = 2;
					HCI_Write_Scan_Enable(handle_w,"uart",3);
				  end
				--print(string.format("step 1: 0x%04x",rst_tbl.Command_Opcode));
			end
		end	
	end
	
	if(step == 2) then
		if(name2 == "HCI_Command_Complete") then
			if(flag2) then
				  if( rst_tbl2.Command_Opcode == 0x0c1a) then 
					print("cara Scan_Enable")
					step = 3;
					HCI_Write_Simple_Pairing_Mode(handle_w,"uart",1);
				  end
				--print(string.format("step 1: 0x%04x",rst_tbl.Command_Opcode));
			end
		end	
	end
	
	if(step == 3) then
		if(name2 == "HCI_Command_Complete") then
			if(flag2) then
				  if( rst_tbl2.Command_Opcode == 0x0c56) then 
					print("cara Scan_Enable")
					step = 5;
					---HCI_Write_Connection_Accept_Timeout(handle_w,"uart",0x1FA0);
				  end
				--print(string.format("step 1: 0x%04x",rst_tbl.Command_Opcode));
			end
		end	
	end
	
	if(step == 4) then
		if(name2 == "HCI_Command_Complete") then
			if(flag2) then
				if( rst_tbl2.Command_Opcode == 0x0c16) then 
					print("cara Write_Connection_Accept_Timeout")
					step = 5
				end
				--print(string.format("step 1: 0x%04x",rst_tbl.Command_Opcode));
			end
		end	
	end
	
	if(step == 5) then
		if(name2 == "HCI_Connection_Request" ) then
			if(flag2) then
				HCI_Accept_Connection_Request(handle_w,"uart",rst_tbl2.BD_ADDR,1);
			end
		
		end
		
		if(name2 == "HCI_Connection_Complete") then
			 -- table.print(rst_tbl2);
			 if(flag2) then
				if(rst_tbl2.Status==0) then
				   print("caracara acl create ok!");
				   handle_cara = rst_tbl2.Connection_Handle;
				   csr_addr = rst_tbl2.BD_ADDR;
				   print(string.format("handle_cara = 0x%04x",rst_tbl2.Connection_Handle));
				   step = 6;
				end
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


-- --]]
-- -- step 2:  page, and page scan ---
-- -- 注意这里的BD_ADDR是个二维数组
-- addr ={};
-- addr[1] = 0xe6;
-- addr[2] = 0xd1;
-- addr[3] = 0xd2;
-- addr[4] = 0xd3;
-- addr[5] = 0xd4;
-- addr[6] = 0xd5;

-- csr_addr = {};
-- Packet_Type  = 0x0408
-- HCI_Create_Connection(handle_tb,"usb",addr,Packet_Type,0x02,
-- 0x00,0x00)

-- print("**** 1 ******")

-- flag = false;
-- step = 1;
-- --acl_handle = 0;
-- handle_CSR = 0;
-- handle_cara = 0;
-- while(1)
-- do 

    
	-- result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
	-- ret_type2,code2,name2,flag2,rst_tbl2=dongle_print_process(result_tbl2,result_len2);
	-- tl_usb_bulk_print_buf_flush();

	-- --result_tbl,result_len = tl_usb_bulk_read() 
	-- result_tbl,result_len = HCI_result_read(handle_tb,"usb")
	-- code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");
	
	-- --table.print(rst_tbl);
	
	-- if(step == 1) then
		-- if(name == "HCI_Connection_Complete") then
			-- print("**** 2 ******")
			-- table.print(rst_tbl);
			-- if(flag) then
				-- if(rst_tbl.Status==0) then
				   -- print("acl create ok!");
				   -- handle_CSR = rst_tbl.Connection_Handle;
				   -- print(string.format("handle_CSR = 0x%04x",rst_tbl.Connection_Handle));
				   -- --step = 2;
				-- end
			-- end
		-- end
		
		-- -- if(ret_type2==1) then -- hci event
			-- -- table.print(rst_tbl2);
		-- -- end
		
		-- if(name2 == "HCI_Connection_Request" ) then
			-- if(flag2) then
				-- HCI_Accept_Connection_Request(handle_w,"uart",rst_tbl2.BD_ADDR,1);
			-- end
		
		-- end
		
		-- if(name2 == "HCI_Connection_Complete") then
			 -- -- table.print(rst_tbl2);
			 -- if(flag2) then
				-- if(rst_tbl2.Status==0) then
				   -- print("caracara acl create ok!");
				   -- handle_cara = rst_tbl2.Connection_Handle;
				   -- csr_addr = rst_tbl2.BD_ADDR;
				   -- print(string.format("handle_cara = 0x%04x",rst_tbl2.Connection_Handle));
				   -- step = 2;
				-- end
			-- end
		-- end
	-- end
	
	-- if step == 2 then
		-- print("**** 3 ******")
		-- --step = 3;
		-- break;
	-- end
	
	-- --tl_progress(70);
	
	
	-- --result_tbl2,result_len2 = HCI_result_read(handle_r,"uart")
		
	-- --gui process
	-- --tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
	-- gui_result_tbl,btn_idx = tl_message_get();
	-- if btn_idx == 1 then
		-- break;
	-- end
	-- if btn_idx == 88 then  --timer
	   -- --print("timer")
	-- end 
-- end

-- -------------mark d, init setup done---------------
-- ---sent a HCI cmd to cara to request remote supported features from csr---
-- HCI_Enable_Device_Under_Test_Mode (handle_tb,"uart")
-- while(1)
-- do
    -- result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
    -- ret_type2,code2,name2,flag,rst_tbl2 = dongle_print_process(result_tbl2,result_len2);
    -- tl_usb_bulk_print_buf_flush();
	
	-- --result_tbl,result_len = tl_usb_bulk_read() 
	-- result_tbl,result_len = HCI_result_read(handle_tb,"usb")
	-- code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");
	
	-- ---step 1, check cmd status rsp
	-- if (name == "HCI_Command_Complete") then
		-- if(rst_tbl.Command_Opcode == 0x1803) then
			-- if (rst_tbl.Status == 0) then
				-- print("HCI_Command_Complete");
				-- break;
			-- end 
		-- end
	-- end
	
    -- --gui process
	-- --tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
	-- gui_result_tbl,btn_idx = tl_message_get();
	-- if btn_idx == 1 then
		-- break;
	-- end
	-- if btn_idx == 88 then  --timer
	   -- --print("timer")
	-- end 



    
-- end


-- -- important !! --
-- -- we need to close handles --
-- tl_btusb_close(handle_tb)
-- tl_usb_bulk_monitor_end()
-- tl_form_close()
-- -- script ok , process bar = 100%
-- tl_progress(100)
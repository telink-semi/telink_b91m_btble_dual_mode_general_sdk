

dofile(".\\lib\\hci.lua")
print("LUA Version: ".._VERSION)
print("============HCI CON CREATE CARACARA MASTER CSR SLAVE=============")

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


--trigger csr dongle into page scan state
HCI_Reset(handle_tb,"usb");
while(1)
do 
    --read and parse the corresponding HCI Command Complete Event to the HCI_Reset Command
    result_tbl,result_len = HCI_result_read(handle_tb,"usb")
    code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");  
    if(name == "HCI_Command_Complete") then
        if(flag) then
            table.print(rst_tbl);
            break;
        end
    end
end

HCI_Write_Page_Scan_Activity(handle_tb,"usb",0x0800,0x0800)
while(1)
do 
    --read and parse the corresponding HCI Command Complete Event to the HCI_Write_Page_Scan_Activity Command
    result_tbl,result_len = HCI_result_read(handle_tb,"usb")
    code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");  
    if(name == "HCI_Command_Complete") then
        if(flag) then
            table.print(rst_tbl);
            break;
        end
    end
end

HCI_Write_Scan_Enable(handle_tb,"usb",0x02)
while(1)
do 
    --read and parse the corresponding HCI Command Complete Event to the HCI_Write_Scan_Enable Command
    result_tbl,result_len = HCI_result_read(handle_tb,"usb")
    code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");  
    if(name == "HCI_Command_Complete") then
        if(flag) then
            table.print(rst_tbl);
            break;
        end
    end
end

--trigger caracara evb into paging state
csr_addr = {0x13, 0x71, 0xda, 0x7d, 0x1a, 0x00};
HCI_Create_Connection(handle_w,"uart",csr_addr,0x0008,0x01,0x0011,0x00)			   ----Packet Type : 0x0418 -->0x0018

flag = false;
csr_connection_done = 0;
caracara_connection_done = 0;

while(1)
do 
    result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
    ret_type,code,name,flag,rst_tbl = dongle_print_process(result_tbl2,result_len2);
    tl_usb_bulk_print_buf_flush();
    if (ret_type == 1) then
        if (name == "HCI_Connection_Complete") then
            if (rst_tbl.Status == 0) then
                print("caracara acl create ok!");
                print(string.format("caracara ACL handle = 0x%04x",rst_tbl.Connection_Handle));
				handle_cara = rst_tbl.Connection_Handle;
                caracara_connection_done = 1;
            end 
        end
    end
 
    result_tbl,result_len = HCI_result_read(handle_tb,"usb")
    code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");
    if(name == "HCI_Connection_Complete") then
        if(flag) then
            if(rst_tbl.Status==0) then
                print("csr acl create ok!");
                print(string.format("ACL handle = 0x%04x",rst_tbl.Connection_Handle));
				handle_CSR = rst_tbl.Connection_Handle
                csr_connection_done = 1;
            end
        end
    elseif (name == "HCI_Connection_Request") then
        HCI_Accept_Connection_Request(handle_tb,"usb",rst_tbl.BD_ADDR,1);
    end

    if (caracara_connection_done == 1) then
        if (csr_connection_done == 1) then
            break;
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

step = 1;
Packet_Type  = 0x0408
HCI_Change_Connection_Packet_Type(handle_w,"uart",handle_cara,Packet_Type)   ----Packet Type : 0x0408
while(1)
do 
    result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
    ret_type2,code2,name2,flag2,rst_tbl2 = dongle_print_process(result_tbl2,result_len2);
    tl_usb_bulk_print_buf_flush();
	
	result_tbl,result_len = HCI_result_read(handle_tb,"usb")
    code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");

	if(step == 1) then
		if(ret_type == 1)then
			if (name2 == "HCI_Command_Status") then
				if(flag2) then
					print("HCI Command Status event");
					--table.print(rst_tbl2);
					step = 2;
				end
			end
		end
	end
	
    if(step == 2)then
		--print("w2");
		if (name2 == "HCI_Max_Slots_Change") then
			if(flag2) then
                print("HCI Max Slots Change event");
				--table.print(rst_tbl2);
			end
		end
		if (name2 == "HCI_Connection_Packet_Type_Changed") then
			if(flag2) then
                print("HCI Connection Packet Type Changed event");
				--table.print(rst_tbl2);
				HCI_Read_Buffer_Size(handle_w,"uart");
				step = 3;
			end
		end
		
	end
	
	if(step == 3)then
		--print("w3");
		if (name2 == "HCI_Command_Complete") then
			if(flag2) then
                print("HCI Comand Compete event");
				--table.print(rst_tbl2);
				
				lenth = 0x0009
				acl_buffer = array.new(0x90)
				
				-- --- L2CAP header
				-- acl_buffer[1] = 0x10;   ---length  
				-- acl_buffer[2] = 0x20;   ---length
				-- acl_buffer[3] = 0x10;	---CID
				-- acl_buffer[4] = 0x20;   ---CID
				
				-- --- L2CAP information payload
				-- acl_buffer[5] = 0x10;
				-- acl_buffer[6] = 0x20; --handle
				-- acl_buffer[7] = bit32.band(lenth, 0xff);
				-- acl_buffer[8] = bit32.band(bit32.rshift(lenth,8) , 0xff);
				-- lenth = lenth+0x08
				
				
				lenth = 0x0009
				acl_buffer = array.new(0x90)
				acl_buffer[1] = 0x10;
				acl_buffer[2] = 0x20; --handle
				acl_buffer[3] = bit32.band(lenth, 0xff);
				acl_buffer[4] = bit32.band(bit32.rshift(lenth,8) , 0xff);
				lenth = lenth+0x04
				
				for i=0x05,lenth do
					acl_buffer[i] = i
				end
				
				HCI_ACL_Data_packet(handle_w,"uart",acl_buffer,lenth)
				step = 4;
			end
		end
		
	end

    if(step == 4)then
		Packet_Type  = 0x0408
        HCI_Change_Connection_Packet_Type(handle_w,"uart",handle_cara,Packet_Type)   ----Packet Type : 0x0408
		step = 5;
    end
	
	if(step == 5) then
		if(ret_type == 1)then
			if (name2 == "HCI_Command_Status") then
				if(flag2) then
					print("HCI Command Status event");
					--table.print(rst_tbl2);
					step = 6;
				end
			end
		end
	end
	
	if(step == 6)then
		tmp = 0
		if (name2 == "HCI_Max_Slots_Change") then
			if(flag2) then
                print("HCI Max Slots Change event");
				--table.print(rst_tbl2);
				tmp = tmp + 1
				tmp = bit32.bor(tmp, 0x01);
			end
		end
		if (name2 == "HCI_Connection_Packet_Type_Changed") then
			if(flag2) then
                print("HCI Connection Packet Type Changed event");
				--table.print(rst_tbl2);
				tmp = bit32.bor(tmp, 0x10)
			end
		end
		if(tmp==0x11)then
			lenth = 0x0009
			acl_buffer = array.new(0x90)
			
			-- --- L2CAP header
			-- acl_buffer[1] = 0x10;   ---length  
			-- acl_buffer[2] = 0x20;   ---length
			-- acl_buffer[3] = 0x10;	---CID
			-- acl_buffer[4] = 0x20;   ---CID
			
			-- --- L2CAP information payload
			-- acl_buffer[5] = 0x10;
			-- acl_buffer[6] = 0x20; --handle
			-- acl_buffer[7] = bit32.band(lenth, 0xff);
			-- acl_buffer[8] = bit32.band(bit32.rshift(lenth,8) , 0xff);
			-- lenth = lenth+0x08
			
			
			lenth = 0x0009
			acl_buffer = array.new(0x90)
			acl_buffer[1] = 0x10;
			acl_buffer[2] = 0x20; --handle
			acl_buffer[3] = bit32.band(lenth, 0xff);
			acl_buffer[4] = bit32.band(bit32.rshift(lenth,8) , 0xff);
			lenth = lenth+0x04
			
			for i=0x05,lenth do
				acl_buffer[i] = i
			end
			
			HCI_ACL_Data_packet(handle_w,"uart",acl_buffer,lenth)
			step = 7;
		end 
	end
	
	if(step == 6)then
		print("Test Result: OK!")
		break;
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


-- important !! --
-- we need to close handles --
tl_btusb_close(handle_tb)
tl_usb_bulk_monitor_end()
tl_form_close()
-- script ok , process bar = 100%
tl_progress(100)
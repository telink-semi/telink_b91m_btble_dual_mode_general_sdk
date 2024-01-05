

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

---andy, start of sequence
print("ak try start:")

--1st we create a connection with csr dongle


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


-----------mark c, ak---------
handle_cara =0
handle_csr =0

--trigger caracara evb into paging state
csr_addr = {0x13, 0x71, 0xda, 0x7d, 0x1a, 0x00};
HCI_Create_Connection(handle_w,"uart",csr_addr,0x0418,0x01,0x0011,0x00)

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
                caracara_connection_done = 1;
		handle_cara = rst_tbl.Connection_Handle;
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
		handle_csr = rst_tbl.Connection_Handle;
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
end

-------------mark d, init setup done---------------
---sent a HCI cmd to cara to request remote supported features from csr---
HCI_Read_Remote_Supported_Features(handle_w,"uart",handle_cara)
while(1)
do
    result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
    ret_type,code,name,flag,rst_tbl = dongle_print_process(result_tbl2,result_len2);
    tl_usb_bulk_print_buf_flush();
    if (ret_type == 1) then
	---step 1, check cmd status rsp
        if (name == "HCI_Command_Status") then
	if(rst_tbl.Command_Opcode == 0x041b) then
            if (rst_tbl.Status == 0) then
                print("cara echo cmd RRSF status ok");
                --print(string.format("caracara ACL handle = 0x%04x",rst_tbl.Connection_Handle));
		--handle_cara = rst_tbl.Connection_Handle;
            end 
         end
         end

	---step 2, check evt: 
        if (name == "HCI_Read_Remote_Supported_Features_Complete") then
	if(flag) then
            --if (rst_tbl.Status == 0) then --???
                print("the remote rsp RRSF ok");
		table.print(rst_tbl);
                --print(string.format("caracara ACL handle = 0x%04x",rst_tbl.Connection_Handle));
		--handle_cara = rst_tbl.Connection_Handle;
            --end 
         end
         end



    end
end

--
--andy: ending
print("ak try end;")

--print(string.format("ACL handle = 0x%04x",Connection_Handle));
-- important !! --
-- we need to close handles --
tl_btusb_close(handle_tb)
tl_usb_bulk_monitor_end()
tl_form_close()
-- script ok , process bar = 100%
tl_progress(100)





dofile(".\\lib\\hci.lua")
print("LUA Version: ".._VERSION)
print("============4.7.1.2 LMP/LIH/BV-79-C [Role Switch at Setup, Slave]=============")

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
result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");

--trigger caracara into page scan state
HCI_Write_Scan_Enable(handle_w,"uart",0x02);
while(1)
do 
    --read and parse the corresponding HCI Command Complete Event to the HCI_Write_Scan_Enable Command
    result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
    ret_type,code,name,flag,rst_tbl = dongle_print_process(result_tbl2,result_len2);
    
    if (ret_type == 1) then
        if (flag) then
            table.print(rst_tbl);
            break;
        end
    end
end

-- --trigger csr dongle into page state
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

addr ={};
addr[1] = 0xe6;
addr[2] = 0xd1;
addr[3] = 0xd2;
addr[4] = 0xd3;
addr[5] = 0xd4;
addr[6] = 0xd5;
HCI_Create_Connection(handle_tb,"usb",addr,0xcc18,0x02,0x00,0x01);--allow role switch

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
            end 
        elseif(name == "HCI_Connection_Request") then
            HCI_Accept_Connection_Request(handle_w,"uart",rst_tbl.BD_ADDR,0);
        elseif(name == "HCI_Role_Change") then
            if(rst_tbl.Status==0) then
               print("caracara role change OK!");
               print(string.format("New Role = 0x%02x",rst_tbl.New_Role));
            end
        end
    end

    --result_tbl,result_len = tl_usb_bulk_read() 
    result_tbl,result_len = HCI_result_read(handle_tb,"usb")
    code,name,flag,rst_tbl = g_hci_event_process(result_tbl,result_len,"usb");
    if(name == "HCI_Connection_Complete") then
         --table.print(rst_tbl);
         if(flag) then
            if(rst_tbl.Status==0) then
               print("acl create ok!");
               print(string.format("ACL handle = 0x%04x",rst_tbl.Connection_Handle));
               csr_connection_done = 1;
               -- break;
         end
        end
    elseif(name == "HCI_Role_Change") then
        if(rst_tbl.Status==0) then
           print("CSR role change OK!");
           print(string.format("New Role = 0x%02x",rst_tbl.New_Role));
        end
    end
    
    if (caracara_connection_done == 1) then
        if (csr_connection_done == 1) then
            break;
        end
    end 
        
    --gui process
    tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
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


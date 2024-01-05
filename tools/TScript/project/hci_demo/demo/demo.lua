

dofile(".\\lib\\hci.lua")
print("LUA Version: ".._VERSION)
print("============HCI TEST demo=============")

-- gui init --
tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
tl_form_show(50,50,200,100)
tl_button_show(0,20,80,32,"stop",1)
tl_button_show(100,20,80,32,"test",2)

-- kite usb dongle init --
handle_w = tl_usb_init_pid(0x19218);
handle_r = tl_usb_init_pid(0x19218);

if(handle_r ==0  or handle_w==0) then
	print("usb error!!");
	tl_error(1);
	tl_form_close();
	tl_stop(100);
end

tl_usb_bulk_bufmode_set(1)

print(string.format("usb id = 0x%x",handle_r));



tl_usb_bulk_monitor_start(handle_r)


HCI_Write_Scan_Enable(handle_w,"uart",3);

while(1)
do 
   
   result_tbl2,result_len2 = HCI_result_read(handle_r,"uart");
	ret_type2,code2,name2,flag2,rst_tbl2=dongle_print_process(result_tbl2,result_len2);
   
   tl_usb_bulk_print_buf_flush();
   
	gui_result_tbl,btn_idx = tl_message_get();
	if btn_idx == 1 then
		break;
	end
	if btn_idx == 88 then  --timer
	   --print("timer")
	end 
	
	
	
end
-- step 1 : inqiury and inquiry scan --
--HCI_Inquiry(handle_tb,"usb",0x9E8B33,10,10);
--HCI_Write_Scan_Enable(handle_w,"uart",0x03); --inquiry scan and page scan
-- Default setting

-- get result --

tl_usb_bulk_monitor_end()
tl_sleep_ms(500)
tl_form_close()
tl_progress(100)

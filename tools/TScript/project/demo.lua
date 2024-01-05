


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
tl_usb_bulk_bufmode_set(1)

print(string.format("usb id = 0x%x",handle_r));



tl_usb_bulk_monitor_start(handle_r)

while(1)
do 
   
   tl_sleep_ms(50);
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


dofile(".\\lib\\hci.lua")
print("LUA Version: ".._VERSION)
print("============HCI TEST BV-01-C=============")

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

tl_usb_bulk_monitor_start(handle_r)

-- send command --
HCI_Reset(handle_w,"uart");

-- get result --
while(1)
do 
	--result_tbl,result_len = tl_usb_bulk_read() 
	result_tbl,result_len = HCI_result_read(handle_r,"uart")
	if(result_len ~=0) 
	then
		for i,v in ipairs(result_tbl)
		do
			print(string.format("[%d] = 0x%02x",i,result_tbl[i]))
			--io.write(string.format("%c ",result_tbl[i]));
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
-- important !! --
-- close handles --
tl_btusb_close(handle_tb)
tl_usb_bulk_monitor_end()
tl_form_close()
-- script ok , process bar = 100%
tl_progress(100)



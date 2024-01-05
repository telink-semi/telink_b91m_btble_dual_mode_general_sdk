dofile(".\\lib\\hci.lua")
print("LUA Version: ".._VERSION)

-- gui init --
tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
tl_form_show(50,50,200,100)
tl_button_show(0,20,80,32,"stop",1)
tl_button_show(100,20,80,32,"test",2)

-- kite usb dongle init --
handle_r = tl_usb_init2(0x05d8);
handle_w = tl_usb_init2(0x05d8);
handle_tb = tl_btusb_init();
print("print test");

if(handle_r ==0  or handle_w==0 or handle_tb==0) then
	print("usb error!!");
	tl_error(1);
	tl_form_close();
	tl_stop(100);
end

tl_usb_bulk_monitor_start(handle_r)

acl_buffer = array.new(32)
    acl_buffer[1] = 0x48;
	acl_buffer[2] = 0x00; --handle
	acl_buffer[3] = 10;
	acl_buffer[4] = 0x00; -- data len


	
	acl_buffer[5] = 0x06;
	acl_buffer[6] = 0x00;
	acl_buffer[7] = 0x01;
	acl_buffer[8] = 0x00;
	acl_buffer[9] = 0x0A;
	acl_buffer[10] = 0x9D;
	acl_buffer[11] = 0x02;
	acl_buffer[12] = 0x00;
	acl_buffer[13] = 0x02;
	acl_buffer[14] =  0x00;

tl_btusb_acl_write(handle_tb,acl_buffer,14)

while(1)
do 

	result_tbl,result_len = tl_btusb_acl_read(handle_tb)
	for i,v in ipairs(result_tbl)
	do
			print(string.format("[%d] = 0x%02x",i,result_tbl[i]))
			--io.write(string.format("%c ",result_tbl[i]));
	end

	result_tbl2,result_len2 =HCI_result_read(handle_r,"uart")
	--[[
	for i,v in ipairs(result_tbl2)
		do
			print(string.format("[%d] = 0x%02x",i,result_tbl2[i]))
			--io.write(string.format("%c ",result_tbl[i]));
		end
		--]]
	dongle_print_process(result_tbl2,result_len2);
	
	
	gui_result_tbl,btn_idx = tl_message_get();
	if btn_idx == 1 then
		break;
	end
	if btn_idx == 88 then  --timer
	   --print("timer")
	end 

end

tl_btusb_close(handle_tb)
tl_usb_bulk_monitor_end()
tl_form_close()




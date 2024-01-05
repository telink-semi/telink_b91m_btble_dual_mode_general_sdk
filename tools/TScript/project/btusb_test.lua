print("LUA Version: ".._VERSION)
handle = tl_btusb_init();
print(string.format("usb id = 0x%x",handle));
-- 01 01 04 05 33 8B 9E 06 0A
a = array.new(16)
a[1] = 0x01
a[2] = 0x01
a[3] = 0x04
a[4] = 0x05
a[5] = 0x33
a[6] = 0x8b
a[7] = 0x9e
a[8] = 0x06
a[9] = 0x0a
len = tl_btusb_cmd_write(handle,a,9)
while(1)
do 
     result_tbl,result_len = tl_btusb_cmd_read(handle)
     if(result_len>0)
	 then
		for i,v in ipairs(result_tbl)
		do
		print(string.format("[%d] = 0x%02x",i,result_tbl[i]))
		--io.write(string.format("0x%02x ",result_tbl[i]));
		end
		break;
	 end

end

tl_btusb_close(handle)


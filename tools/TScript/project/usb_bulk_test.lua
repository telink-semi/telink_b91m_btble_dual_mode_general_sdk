print("LUA Version: ".._VERSION)
print("lua begin:")
handle = 1;
print(handle);
handle = tl_usb_init2(0x05d8);
handlew = tl_usb_init2(0x05d8); --write handle should different with read handle
print(string.format("usb id = 0x%x",handle));

r={}
r,r2 = tl_usb_read2(handle,0x007e,4)
print ("len:",r2)
print ("r[1]",r[1])
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end




a = array.new(32);
result_tbl ={};
--a[1] = 10;  --len
--a[2] = 0;
a[1] = 0x28; --cmd
a[2] = 0;   --type
a[3] = 0x10; --addr[0]
a[4] = 0x00; --addr[1]
a[5] = 0x00; --addr[2]
a[6] = 0x80; --addr[3]
a[7] = 0x04; --len[0]
a[8] = 0x00; --len[1]


tl_usb_bulk_monitor_start(handle)
print("bulk thread start...")
len = tl_usb_bulk_out(handlew,a,8)
print(string.format("out len = %d",len));

tl_sys_info_set(1);
tl_form_draw_ratangle(0,0,600,400,0xffffff,0xffffff)
tl_form_show(50,50,200,100)
tl_button_show(0,10,100,32,"stop",1)
gui_result_tbl = {};


while(1)
do 
   result_tbl,result_len = tl_usb_bulk_read() 
   tl_sleep_ms(50);
   if(result_len ~=0) 
   then
       if(result_tbl[3]==0x29) --if the read result!
	   then
		   print("read result = ");
		   print(string.format("adr = 0x%02x%02x%02x%02x",result_tbl[8],result_tbl[7],result_tbl[6],result_tbl[5]))
		   print(string.format("data = 0x%02x%02x%02x%02x",result_tbl[12],result_tbl[11],result_tbl[10],result_tbl[9]))
		   --[[
		   for i,v in ipairs(result_tbl)
			do
			print(string.format("[%d] = 0x%02x",i,result_tbl[i]))
			--io.write(string.format("%c ",result_tbl[i]));
			end
			]]--
	        --break;
		end
	end
	gui_result_tbl,btn_idx = tl_message_get();
	if btn_idx == 1 then
		break;
	end
	if btn_idx == 88 then  --timer
	   --print("timer")
	end 
	
	
	--tl_sleep_ms(50)
   --if(result_len ~=0)
   --then
	 --break
   --end	 
-- tl_sleep_ms(50);
end

--tl_usb_bulk_monitor_start(0);
tl_usb_bulk_monitor_end()
tl_form_close()







print("LUA Version: ".._VERSION)
print("---read R0~R16----")
handle = tl_usb_init(0xffff)
a = array.new(1024)
a[1] = 0x05
len = tl_usb_write(handle,0x0602,a,1)  -- stop mcu
print("stop mcu!");

for j=0,15,1
do
	for i=1,1024,1 
	do
	a[i]=0
	end
	--len = tl_usb_write(handle,0x8000+(1024*j),a,1024)
	
end

print("restart mcu!");
a[1] = 0x3f
len = tl_usb_write(handle,0x060e,a,1)  -- resore mcu ic
--a[1] = 0x00
--len = tl_usb_write(handle,0x0062,a,1)  -- restart mcu

a[1] = 0x84
len = tl_usb_write(handle,0x0602,a,1)  -- restart mcu

print("done!");




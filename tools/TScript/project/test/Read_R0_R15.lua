print("LUA Version: ".._VERSION)
print("---read R0~R16----")
handle = tl_usb_init(0x5320)
a = array.new(3)
a[1] = 0x0a
len = tl_usb_write(handle,0x0602,a,1)  -- stop mcu
r={}
r,r2 = tl_usb_read(handle,0x0680,64) -- read r0~r15

for i=1,16,1   
do 
print(string.format("R%d =\t 0x%02x%02x%02x%02x",i-1,r[i*4],r[i*4-1],r[i*4-2],r[i*4-3]))
end



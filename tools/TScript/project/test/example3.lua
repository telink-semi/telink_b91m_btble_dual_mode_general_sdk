print("LUA Version: ".._VERSION)
print("lua begin:")
dofile(".\\lib\\justtest.lua")
handle = 1;
print(handle);
handle = tl_usb_init(0x1234)
--print(handle);
r={};
r,r2 = tl_usb_read(handle,0x8008,4)
print ("len:",r2)
print ("r[1]",r[1])
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end
handle = tl_usb_init(0x9abc)
r,r2 = tl_usb_read(handle,0x8008,4)
print ("len:",r2)
print ("r[1]",r[1])
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end
--tl_error(0x01);




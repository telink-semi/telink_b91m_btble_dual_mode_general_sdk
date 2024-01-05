print("LUA Version: ".._VERSION)
print("usb remap:")
--dofile(".\\lib\\justtest.lua")
--print(handle)
handle = tl_usb_init(0xf800)
a = array.new(30)
a[1] = 0x53
len = tl_usb_write(handle,0x0074,a,1)
a[1] = 0x34
a[2] = 0x12
len = tl_usb_write(handle,0x007e,a,2)
a[1] = 0x00
len = tl_usb_write(handle,0x0074,a,1)

handle = tl_usb_init(0xf800)
a = array.new(30)
a[1] = 0x53
len = tl_usb_write(handle,0x0074,a,1)
a[1] = 0x78
a[2] = 0x56
len = tl_usb_write(handle,0x007e,a,2)
a[1] = 0x00
len = tl_usb_write(handle,0x0074,a,1)

handle = tl_usb_init(0xf800)
a = array.new(30)
a[1] = 0x53
len = tl_usb_write(handle,0x0074,a,1)
a[1] = 0xbc
a[2] = 0x9a
len = tl_usb_write(handle,0x007e,a,2)
a[1] = 0x00
len = tl_usb_write(handle,0x0074,a,1)

handle = tl_usb_init(0xf800)
a = array.new(30)
a[1] = 0x53
len = tl_usb_write(handle,0x0074,a,1)
a[1] = 0xf0
a[2] = 0xde
len = tl_usb_write(handle,0x007e,a,2)
a[1] = 0x00
len = tl_usb_write(handle,0x0074,a,1)
--print(handle);

print("LUA Version: ".._VERSION)
print("lua begin:")
dofile(".\\lib\\justtest.lua")
--handle = 1
--print(handle)
handle = tl_usb_init(0x1234)
r={}
r,r2 = tl_usb_read(handle,0x8008,4)
print ("len:",r2)
print ("r[1]",r[1])
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end
handle = tl_usb_init(0x5678)
r,r2 = tl_usb_read(handle,0x8008,4)
print ("len:",r2)
print ("r[1]",r[1])
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end
handle = tl_usb_init(0x9abc)
r={}
r,r2 = tl_usb_read(handle,0x8008,4)
print ("len:",r2)
print ("r[1]",r[1])
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end
handle = tl_usb_init(0xdef0)
r,r2 = tl_usb_read(handle,0x8008,4)
print ("len:",r2)
print ("r[1]",r[1])
for i,v in ipairs(r)
do
print(string.format("0x%02x",v))
end
--tl_error(0x01);








print("LUA Version: ".._VERSION)
print("breakpoint run")
handle = tl_usb_init(0x0202)
a = array.new(2)
a[1] = 0x55
len = tl_usb_write(handle,0x0000,a,1)
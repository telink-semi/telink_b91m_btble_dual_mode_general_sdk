print("LUA Version: ".._VERSION)
print("lua begin:")

rs232_tbl = {}
rs232_tbl,rs232_tbl_cnt = tl_rs232_list()

print ("len:",rs232_tbl_cnt)
for i,v in ipairs(rs232_tbl)
do
print(string.format("%s",v))
end
print(string.format("open the first device:%s",rs232_tbl[1]))
tl_rs232_open(rs232_tbl[1], 1)

alen = 10
a = array.new(alen)
for i=1,alen do
a[i] = 0x55;
end

tl_rs232_send(a,10)

rs232_rcv_tbl={}
repeat
rs232_rcv_tbl,len = tl_rs232_recv()
until len>0

for i,v in ipairs(rs232_rcv_tbl)
do
print(string.format(" %x",rs232_rcv_tbl[i]))
tl_sleep_ms(50)
end


print("close uart port now")
tl_rs232_close()
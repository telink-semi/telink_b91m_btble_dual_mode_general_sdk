
tl_log_color(0x00ff)
print("hello")
tl_sleep_ms(100)
tl_log_color(0xff00ff)
print("hello")

for loop = 1,10 do

tl_log_color(loop*100)
tl_sleep_ms(100)
print(loop)
print("---->>");

end

print("color ")
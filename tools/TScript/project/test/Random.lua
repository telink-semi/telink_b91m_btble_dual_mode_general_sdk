a={}
b={}
c={}
name = {}
name_idx = {}
f = io.open("abc.txt",'r')
idx = 1
for i in f:lines() do
--table.insert(c,i)
c[idx] = i
idx=idx+1
end
-- get the information we want
for i=1,#c do
--print(c[i])
a[i],b[i] = string.match(c[i],"([%a.]+)%s+(%d+)")
print(a[i])
print(b[i])
end
f:close()

print("########## Find valid ################")
idx = 1
for i=1,#a do
  if(b[i] == "1") then
     name[idx] = a[i]
	 name_idx[idx] = i
	 idx = idx + 1
	 print(a[i])
  end
end
cnt = #name
tl_form_show(100,0,300,250)
if(cnt==0) then
    print("*******************")
	print("the new turn begin!")
	print("*******************")
    for i=1,#b do
	    b[i] = "1"
	end
	
    tl_form_draw_ratangle(0,0,300,250,0xffffff,0xffffff)
	tl_form_draw_text(10,10,"new turn begin!",25,0x00ff)
else
	print("cnt=",cnt)
	math.randomseed(tostring(os.time()):reverse():sub(1,10))
	luckcnt = math.random(cnt)
	print(luckcnt)
	print("the luck one---", name[luckcnt])
	b[name_idx[luckcnt]] = "0"
	--tl_form_show(100,0,320,280)
    tl_form_draw_ratangle(0,0,300,250,0xffffff,0xffffff)
	tl_form_draw_text(10,10,name[luckcnt],25,0x00ff)
	tl_form_draw_text(10,90,os.date("%c"),25,0xff00)

end
-- write back the result
f = io.open("abc.txt",'w')
str = ""
for i=1,#a do
  str = a[i] .. "			" .. b[i] .."\n"
  print(a[i],"=",b[i])
  f:write(str)
end
f:close()




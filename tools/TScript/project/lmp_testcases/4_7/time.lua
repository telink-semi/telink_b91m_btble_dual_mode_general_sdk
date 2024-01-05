
function timeout_sec(t,sec)
	local time = os.time()
	if((time - t)>sec) then
		return 1
	end
	return 0
end

function delay_sec(sec)
	local t0 = os.time()
	while(1)
	do
		if(timeout_sec(t0,sec)==1) then
			break
		end
	end
end
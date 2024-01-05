
HCI_EVENT_RESULT_DISPLAY 	= 1
HCI_EVENT_TABLE_DISPLAY 	= 0
HCI_EVENT_TIMEOUT_DISPLAY	= 1
HCI_COMMAND_DISPLAY         = 1
HCI_PROCESS_STATUS_DISPLAY  = 1
HCI_READ_EVENT_DISPLAY      = 1

function log_printf(en,str,data,data_len)
	if(en==1) then
		data_str=""
		for i=1,data_len do
			data_str = data_str.." "..string.format("%.2x",data[i])
		end
		data_str  = str..data_str
		print(data_str)
	end
end

function print_table (en, t )  
	if(en==1) then
	    local print_r_cache={}
	    local function sub_print_r(t,indent)
	        if (print_r_cache[tostring(t)]) then
	            print(indent.."*"..tostring(t))
	        else
	            print_r_cache[tostring(t)]=true
	            if (type(t)=="table") then
	                for pos,val in pairs(t) do
	                    if (type(val)=="table") then
	                        print(indent.."["..pos.."] => "..tostring(t).." {")
	                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
	                        print(indent..string.rep(" ",string.len(pos)+6).."}")
	                    elseif (type(val)=="string") then
	                        print(indent.."["..pos..'] => "'..val..'"')
	                    else
	                        --print(indent.."["..pos.."] => "..tostring(val))
							print(indent.."["..pos.."] => "..string.format("%d(0x%02x)",val,val))
	                    end
	                end
	            else
	                print(indent..tostring(t))
	            end
	        end
	    end
	    if (type(t)=="table") then
	        print(tostring(t).." {")
	        sub_print_r(t,"  ")
	        print("}")
	    else
	        sub_print_r(t,"  ")
	    end
	    print()
	end
end


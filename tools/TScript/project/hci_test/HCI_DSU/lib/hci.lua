--require "bit32"
--print(bit32)
-- if mode=

--[[
g_hci_event_tbl = {
{ code= 0x01, name = "Inquiry Complete", func = function (tbl,mode)   end },
{ code= 0x0f, name = "Command Status", func = function (tbl,mode) return CHECK_EVENT_Command_Status(tbl,mode) end},

};
--]] 


-------------------- common function ---------------------------
-- function print_r ( t )  
--     local print_r_cache={}
--     local function sub_print_r(t,indent)
--         if (print_r_cache[tostring(t)]) then
--             print(indent.."*"..tostring(t))
--         else
--             print_r_cache[tostring(t)]=true
--             if (type(t)=="table") then
--                 for pos,val in pairs(t) do
--                     if (type(val)=="table") then
--                         print(indent.."["..pos.."] => "..tostring(t).." {")
--                         sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
--                         print(indent..string.rep(" ",string.len(pos)+6).."}")
--                     elseif (type(val)=="string") then
--                         print(indent.."["..pos..'] => "'..val..'"')
--                     else
--                         --print(indent.."["..pos.."] => "..tostring(val))
-- 						print(indent.."["..pos.."] => "..string.format("%d(0x%02x)",val,val))
--                     end
--                 end
--             else
--                 print(indent..tostring(t))
--             end
--         end
--     end
--     if (type(t)=="table") then
--         print(tostring(t).." {")
--         sub_print_r(t,"  ")
--         print("}")
--     else
--         sub_print_r(t,"  ")
--     end
--     print()
-- end

function print_r ( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        print(indent.."["..pos.."] => "..tostring(val).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        print(indent.."["..pos..'] => "'..val..'"')
					elseif (type(val)=="function") then
						print(indent.."["..pos..'] => "'..tostring(val)..'"')
					elseif (type(val)=="boolean") then
						print(indent.."["..pos..'] => "'..tostring(val)..'"')
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

table.print = print_r;


function  g_hci_event_process(tbl,tbl_len,mode)
	local code;
	local flag;
	local rst_tbl;
	if(mode=="uart") then
		code = tbl[2];
	else
		code = tbl[1];
	end
	for i,v in ipairs(g_hci_event_tbl)
	do
		if(g_hci_event_tbl[i].code == code) then
			print("<EVENT:>"..g_hci_event_tbl[i].name);
			flag, rst_tbl = g_hci_event_tbl[i].func(tbl,mode);  --call the event function
			return  code,g_hci_event_tbl[i].name,flag,rst_tbl;
			
		end
	end

	return  0,"",false,nil;
end

function  g_hci_acl_data_process(tbl,tbl_len,mode)
    local rst_tbl = {};
    rst_tbl.Data = {};
    
    rst_tbl.Connection_Handle = tbl[2] + tbl[3]*256;
    rst_tbl.Data_Length = tbl[4] + tbl[5]*256;
    for i = 1, rst_tbl.Data_Length, 1 do
        rst_tbl.Data[i] = tbl[5+i];
    end

    return  0,"",true,rst_tbl;
end


function  dongle_print_process(tbl,tbl_len)
	local ret_type = 0;
	local code;
	local name;
	local flag;
	local rst_tbl = {};
	if(tbl_len < 2) then
	    return
	end
	data_str="";
	str_str = "";
	if (tbl[1]==0x22) then  -- my_uart_send_str_data
		data_len = tbl[2] + (tbl[3]*256);
		for i = 1, data_len, 1 do
			data_str = data_str..string.format("%02x ",tbl[3+i]);
		end
		str_len = tbl_len - 3 - data_len;
		if(str_len < 0) then  return;  end
		for i = 1, str_len, 1 do
			str_str = str_str..string.format("%c",tbl[ 3 +data_len+i]);
		end
		print("\t[0x22]:"..str_str..":"..data_str);

	elseif(tbl[1]==0x21) then
		data_len = tbl_len - 1;
		for i = 1, data_len, 1 do
			data_str = data_str..string.format("%02x ",tbl[1+i]);
		end
		print("\t[0x21]:"..data_str);

	elseif(tbl[1]==0x20) then
		str_len = tbl_len - 1;
		for i = 1, str_len, 1 do
			str_str = str_str..string.format("%c",tbl[1+i]);
		end
		print("\t[0x20]:"..str_str);
	
	-- hci event
	elseif (tbl[1]==0x04) then 
		ret_type = 1;
		code,name,flag,rst_tbl = g_hci_event_process(tbl,tbl_len,"uart"); --print the hci event name
	-- hci acl data
    elseif (tbl[1]==0x02) then
        ret_type = 2;
        code,name,flag,rst_tbl = g_hci_acl_data_process(tbl,tbl_len,"uart");
    end

    return ret_type,code,name,flag,rst_tbl;
end




--------------------  HCI command ------------------------------

function HCI_result_read(handle,mode)  
    
	r_len = 0;
	r_tbl = {};
	if(mode=="uart") then
		result_tbl,result_len = tl_usb_bulk_read()
		if(result_len > 2) 
		then
			if (result_tbl[1] == 0x10) then
			   r_len = result_len-2;
			   for loop = 1,result_len-2 do
					r_tbl[loop] = result_tbl[loop+2];
			   end	
			end
		end
	end
	if(mode=="usb") then
		r_tbl,r_len = tl_btusb_cmd_read(handle)
	end
	
	return r_tbl,r_len;
end

function HCI_Disconnect (handle,mode,Connection_Handle,Reason)
	print("HCI_Disconnect");
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = bit32.band(Connection_Handle ,0xff);
	a[2+offset] = bit32.band(bit32.rshift(Connection_Handle,8) , 0xff);
	a[3+offset] = Reason;
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,4) -- dongle
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,3) --csr
	end
	return len;

end

function HCI_Reset(handle,mode)
	print("HCI_Reset");
	len = 0;
	local a = array.new(32);
	offset = 0;
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x03;  --OCF =3
	a[2+offset] = 0x0c;  --OGF =3
	a[3+offset] = 0x00;  --para_len
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,4)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,3)
	end
	return len;
end

function HCI_Inquiry(handle,mode,LAP,Inquiry_Length,Num_Responses)
	print(string.format("HCI_Inquiry 0x%06x, %d, %d",LAP,Inquiry_Length,Num_Responses));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x01;  --OCF =1
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x05;  --para len
	a[4+offset] = bit32.band(LAP , 0xff);
	a[5+offset] = bit32.band(bit32.rshift(LAP,8) , 0xff);
	a[6+offset] = bit32.band(bit32.rshift(LAP,16), 0xff);
	a[7+offset] = Inquiry_Length;
	a[8+offset] = Num_Responses;
	
	--print(string.format("%02x %02x %02x",a[8+offset],a[9+offset],a[5+offset]))
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,9)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,8)
	end
	return len;
	
end

function HCI_Write_Scan_Enable(handle,mode,Scan_Enable)
	print(string.format("HCI_Write_Scan_Enable 0x%x",Scan_Enable));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x1a;  --OCF =1a
	a[2+offset] = 0x0c;  --OGF =3
	a[3+offset] = 0x01;  --para len
	a[4+offset] = Scan_Enable;  --para len
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,5)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,4)
	end
	return len;

end

function HCI_Write_Page_Scan_Activity(handle,mode,Page_Scan_Interval,Page_Scan_Window)
	print(string.format("HCI_Write_Page_Scan_Activity 0x%04x, 0x%04x",Page_Scan_Interval,Page_Scan_Window));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x1c;  --OCF =1c
	a[2+offset] = 0x0c;  --OGF =3
	a[3+offset] = 0x04;  --para len
	a[4+offset] = bit32.band(Page_Scan_Interval, 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Page_Scan_Interval,8) , 0xff);
	a[6+offset] = bit32.band(Page_Scan_Window , 0xff);
	a[7+offset] = bit32.band(bit32.rshift(Page_Scan_Window,8) , 0xff);
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,8)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,7)
	end
	return len;

end


function  HCI_Create_Connection (handle,mode,BD_ADDR,Packet_Type,Page_Scan_Repetition_Mode,
Clock_Offset,Allow_Role_Switch)
	print(string.format("HCI_Create_Connection"));
	
	
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x05;  --OCF =5
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 13;  --para len
	a[4+offset] = BD_ADDR[1]; --address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
	
	print(string.format("address 0x%02x%02x%02x%02x%02x%02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));
	
	a[10+offset] = bit32.band(Packet_Type , 0xff);
	a[11+offset] = bit32.band(bit32.rshift(Packet_Type,8) , 0xff);
	 
	a[12+offset] =  Page_Scan_Repetition_Mode;
	
	a[13+offset] = 0x00;  --reserved 
	
	a[14+offset] = bit32.band(Clock_Offset , 0xff);
	a[15+offset] = bit32.band(bit32.rshift(Clock_Offset,8) , 0xff);
	
	a[16+offset] = Allow_Role_Switch;
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,17)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,16)
	end
	return len;

end

function  HCI_Accept_Connection_Request (handle,mode,BD_ADDR,Role)
	print(string.format("HCI_Accept_Connection_Request"));

	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x09;  --OCF =9
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 7;  --para len
	a[4+offset] = BD_ADDR[1]; --address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
	
	print(string.format("address 0x%02x%02x%02x%02x%02x%02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));
	
	a[10+offset] = Role;
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,11)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,10)
	end
	return len;

end

function HCI_Authentication_Requested(handle,mode,Connection_Handle)
	print(string.format("HCI_Authentication_Requested 0x%x",Connection_Handle));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x11;  --OCF =1
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x02;  --para len
	--a[4+offset] = Scan_Enable;  --para len
	
	a[4+offset] = bit32.band(Connection_Handle, 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8) , 0xff);
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,6)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,5)
	end
	return len;

end

function HCI_Link_Key_Request_Negative_Reply(handle,mode,BD_ADDR)
	print(string.format("HCI_Link_Key_Request_Negative_Reply 0x%02x%02x%02x%02x%02x%02x",BD_ADDR[6],BD_ADDR[5],BD_ADDR[4]
	,BD_ADDR[3],BD_ADDR[2],BD_ADDR[1]));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x0c;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x06;  --para len
	a[4+offset] = BD_ADDR[1];
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,10)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,9)
	end
	return len;
	

end


function HCI_IO_Capability_Request_Reply(handle,mode,BD_ADDR,IO_Capability,OOB_Data_Present,Authentication_Requirements)
	print(string.format("HCI_Link_Key_Request_Negative_Reply 0x%02x%02x%02x%02x%02x%02x",BD_ADDR[6],BD_ADDR[5],BD_ADDR[4]
	,BD_ADDR[3],BD_ADDR[2],BD_ADDR[1]));
	print(string.format("%02x,%02x,%02x",IO_Capability,OOB_Data_Present,Authentication_Requirements));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x2b;  --OCF =0x0e
	a[2+offset] = 0x04;  --OGF =1
	a[3+offset] = 0x09;  --para len  6+1+1+1
	
	a[4+offset] = BD_ADDR[1];
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
	
	a[10+offset] = IO_Capability;
	a[11+offset] = OOB_Data_Present;
	a[12+offset] = Authentication_Requirements;
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,13)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,12)
	end
	return len;

end

function  HCI_Set_Event_Mask(handle,mode,Event_Mask)
	print(string.format("HCI_Set_Event_Mask 0x%02x%02x%02x%02x %02x%02x%02x%02x",
	Event_Mask[8],Event_Mask[7],Event_Mask[6],Event_Mask[5],Event_Mask[4],Event_Mask[3],Event_Mask[2],Event_Mask[1]));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end
	
	a[1+offset] = 0x01;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x08;  --para len
	
	a[4+offset] = Event_Mask[1];
	a[5+offset] = Event_Mask[2];
	a[6+offset] = Event_Mask[3];
	a[7+offset] = Event_Mask[4];
	
	a[8+offset] = Event_Mask[5];
	a[9+offset] = Event_Mask[6];
	a[10+offset] = Event_Mask[7];
	a[11+offset] = Event_Mask[8];
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,12)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,11)
	end
	return len;
	

end

-- OGF = 0x03 --
-- OCF = 0x20 --

function HCI_Write_Authentication_Enable(handle,mode,Authentication_Enable)
	print(string.format("HCI_Write_Authentication_Enable 0x%x",Authentication_Enable));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x20;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x01;  --para len
	a[4+offset] = Authentication_Enable;  --para len
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,5)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,4)
	end
	return len;

end

-- OGF = 0x03 --
-- OCF = 0x56 --
function HCI_Write_Simple_Pairing_Mode(handle,mode,Simple_Pairing_Mode)
	print(string.format("HCI_Write_Simple_Pairing_Mode 0x%x",Simple_Pairing_Mode));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x56;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x01;  --para len
	a[4+offset] = Simple_Pairing_Mode;  --para len
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,5)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,4)
	end
	return len;

end

function  HCI_Write_Secure_Connections_Host_Support(handle,mode,Secure_Connections_Host_Support)
	print(string.format("HCI_Write_Authentication_Enable 0x%x",Secure_Connections_Host_Support));
	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x7a;  --OCF =1
	a[2+offset] = 0x0c;  --OGF =1
	a[3+offset] = 0x01;  --para len
	a[4+offset] = Secure_Connections_Host_Support;  --para len
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,5)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,4)
	end
	return len;

end

function  HCI_QoS_Setup(handle,mode,Connection_Handle,Unused,Service_Type,Token_Rate,Peak_Bandwidth,Latency,Delay_Variation)
	print(string.format("HCI_QoS_Setup"));

	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x07;  --OCF =0x0B
	a[2+offset] = 0x08;  --OGF =0x02
	a[3+offset] = 20;  --para len
	a[4+offset] = bit32.band(Connection_Handle , 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8) , 0xff);
	a[6+offset] = Unused;
	a[7+offset] = Service_Type;
	a[8+offset] = bit32.band(Token_Rate, 0xff);;
	a[9+offset] = bit32.band(bit32.rshift(Token_Rate,8), 0xff);
	a[10+offset] = bit32.band(bit32.rshift(Token_Rate,16), 0xff);
	a[11+offset] = bit32.band(bit32.rshift(Token_Rate,24), 0xff);
	a[12+offset] = bit32.band(Peak_Bandwidth, 0xff);;
	a[13+offset] = bit32.band(bit32.rshift(Peak_Bandwidth,8), 0xff);
	a[14+offset] = bit32.band(bit32.rshift(Peak_Bandwidth,16), 0xff);
	a[15+offset] = bit32.band(bit32.rshift(Peak_Bandwidth,24), 0xff);
    a[16+offset] = bit32.band(Latency, 0xff);;
	a[17+offset] = bit32.band(bit32.rshift(Latency,8), 0xff);
	a[18+offset] = bit32.band(bit32.rshift(Latency,16), 0xff);
	a[19+offset] = bit32.band(bit32.rshift(Latency,24), 0xff);
	a[20+offset] = bit32.band(Delay_Variation, 0xff);;
	a[21+offset] = bit32.band(bit32.rshift(Delay_Variation,8), 0xff);
	a[22+offset] = bit32.band(bit32.rshift(Delay_Variation,16), 0xff);
	a[23+offset] = bit32.band(bit32.rshift(Delay_Variation,24), 0xff);
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,24)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,23)
	end
	return len;
end

function  HCI_Switch_Role(handle,mode,BD_ADDR,Role)
	print(string.format("HCI_Switch_Role"));

	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x0B;  --OCF =0x0B
	a[2+offset] = 0x08;  --OGF =0x02
	a[3+offset] = 7;  --para len
	a[4+offset] = BD_ADDR[1]; --address
	a[5+offset] = BD_ADDR[2];
	a[6+offset] = BD_ADDR[3];
	a[7+offset] = BD_ADDR[4];
	a[8+offset] = BD_ADDR[5];
	a[9+offset] = BD_ADDR[6];
	
	print(string.format("address 0x%02x%02x%02x%02x%02x%02x",a[4+offset],
	a[5+offset],a[6+offset],a[7+offset],a[8+offset],a[9+offset]));
	
	a[10+offset] = Role;
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,11)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,10)
	end
	return len;
end

function  HCI_Read_Link_Policy_Settings(handle,mode,Connection_Handle)
	print(string.format("HCI_Read_Link_Policy_Settings"));

	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x0C;  --OCF =0x0c
	a[2+offset] = 0x08;  --OGF =0x02
	a[3+offset] = 2;  --para len
	a[4+offset] = bit32.band(Connection_Handle , 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8) , 0xff);
	
	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,6)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,5)
	end
	return len;
end

function  HCI_Write_Link_Policy_Settings(handle,mode,Connection_Handle,Link_Policy_Settings)
	print(string.format("HCI_Write_Link_Policy_Settings"));

	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

	a[1+offset] = 0x0D;  --OCF =0x0D
	a[2+offset] = 0x08;  --OGF =0x08
	a[3+offset] = 4;  --para len
	a[4+offset] = bit32.band(Connection_Handle , 0xff);
	a[5+offset] = bit32.band(bit32.rshift(Connection_Handle,8) , 0xff);
	a[6+offset] = bit32.band(Link_Policy_Settings , 0xff);
	a[7+offset] = bit32.band(bit32.rshift(Link_Policy_Settings,8) , 0xff);

	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,8)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,7)
	end
	return len;
end

function HCI_Set_AFH_Host_Channel_Classification(handle,mode,AFH_Host_Channel_Classification)
	print(string.format("HCI_Set_AFH_Host_Channel_Classification"));

	len = 0;
	offset = 0;
	local a = array.new(32);
	
	if(mode=="uart") then
	  a[1] = 0x01;  --HCI cmd
	  offset = 1;
	end

    a[1+offset] = 0x3F;  --OCF =0x3f
    a[2+offset] = 0x0C;  --OGF =0x0c
    a[3+offset] = 10;  --para len
    a[4+offset] = AFH_Host_Channel_Classification[1];
    a[5+offset] = AFH_Host_Channel_Classification[2];
    a[6+offset] = AFH_Host_Channel_Classification[3];
    a[7+offset] = AFH_Host_Channel_Classification[4];
    a[8+offset] = AFH_Host_Channel_Classification[5];
    a[9+offset] = AFH_Host_Channel_Classification[6];
    a[10+offset] = AFH_Host_Channel_Classification[7];
    a[11+offset] = AFH_Host_Channel_Classification[8];
    a[12+offset] = AFH_Host_Channel_Classification[9];
    a[13+offset] = AFH_Host_Channel_Classification[10];

	if(mode=="uart") then
		len = tl_usb_bulk_out(handle,a,8)
	end
	if(mode=="usb") then
		len = tl_btusb_cmd_write(handle,a,7)
	end
	return len;
end

----------------------- HCI event ---------------------------------
function  EVENT_HCI_Inquiry_Complete(tbl,mode)
	local flag = true;
	local offset = 0;
	local rst_tbl={};
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x01) then -- correct event code should be 0x01
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
   
	return true,rst_tbl;
end

function  EVENT_HCI_Inquiry_Result(tbl,mode)
	local flag = true;
	local offset = 0;
	local rst_tbl={};
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x02) then -- correct event code should be 0x02
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Num_Responses = tbl[3+offset];
    rst_tbl.BD_ADDR = {};
	rst_tbl.Page_Scan_Repetition_Mode = {};
	rst_tbl.Reserved = {};
	rst_tbl.Class_of_Device = {};
	rst_tbl.Clock_Offset = {};
	
	for i = 1,Num_Responses,1 do
		rst_tbl.BD_ADDR[i] = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]};
		rst_tbl.Page_Scan_Repetition_Mode[i] = tbl[10+offset];
		rst_tbl.Reserved[i] = {tbl[11+offset],tbl[12+offset]};
		rst_tbl.Class_of_Device[i] = tbl[13+offset]+bit32.lshift(tbl[14+offset],8)+bit32.rshift(tbl[15+offset],16);
		rst_tbl.Clock_Offset[i] = tbl[16+offset] + bit32.rshift(tbl[17+offset],8);
	end
	return true,rst_tbl;
end

function  EVENT_HCI_Connection_Complete(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x03) then  -- correct event code should be 0x03
	     return false;
	end
	
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + tbl[5+offset]*256;
	rst_tbl.BD_ADDR = {tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset],tbl[10+offset],tbl[11+offset]};
	
	rst_tbl.Link_Type = tbl[12+offset];
	rst_tbl.Encryption_Enabled = tbl[13+offset];
	
	return  true,rst_tbl;

end

function  EVENT_HCI_Connection_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x04) then  -- correct event code should be 0x04
	     return false;
	end
	
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	rst_tbl.Class_Of_Device = {tbl[9+offset],tbl[10+offset],tbl[11+offset]};
	rst_tbl.Link_Type = tbl[12+offset];
	
	return  true,rst_tbl;
end

function  EVENT_HCI_Encryption_Change(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	if(tbl[1+offset] ~= 0x08) then  -- correct event code should be 0x08
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	--rst_tbl.Connection_Handle = {tbl[4+offset],tbl[5+offset]};
	rst_tbl.Connection_Handle = tbl[4+offset]+(tbl[5+offset]*256);
	rst_tbl.Encryption_Enabled = tbl[6+offset];
	
	return true,rst_tbl;
end

function  EVENT_HCI_QoS_Setup_Complete(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x0d) then  -- correct event code should be 0x0d
	     return false;
	end
    -- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Connection_Handle = tbl[4+offset] + (tbl[5+offset]*256);
	rst_tbl.Unused = tbl[6+offset];
	rst_tbl.Service_Type = tbl[7+offset];
	rst_tbl.Token_Rate = {tbl[8+offset], tbl[9+offset], tbl[10+offset], tbl[11+offset]};
	rst_tbl.Peak_Bandwidth = {tbl[12+offset], tbl[13+offset], tbl[14+offset], tbl[15+offset]};
	rst_tbl.Latency = {tbl[16+offset], tbl[17+offset], tbl[18+offset], tbl[19+offset]};
    rst_tbl.Delay_Variation = {tbl[20+offset], tbl[21+offset], tbl[22+offset], tbl[23+offset]};

	return true, rst_tbl;
end

function  EVENT_HCI_Command_Complete(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x0e) then  -- correct event code should be 0x0e
	     return false;
	end
    -- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Num_HCI_Command_Packets = tbl[3+offset];
	rst_tbl.Command_Opcode = {tbl[4+offset], tbl[5+offset]};
	rst_tbl.Return_Parameters = {};
	local ret_para_len = tbl[2+offset] - 3;
	if (ret_para_len >= 1) then 
        for i = 1,ret_para_len,1 do
        	rst_tbl.Return_Parameters[i] = tbl[5+offset+i];
        end
	end

	return true, rst_tbl;
end

function  EVENT_HCI_Command_Status(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x0f) then  -- correct event code should be 0x0f
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.Num_HCI_Command_Packets = tbl[4+offset];
	rst_tbl.Command_Opcode = {tbl[5+offset], tbl[6+offset]};

	return true, rst_tbl;
end

function  EVENT_HCI_Link_Key_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x17) then  -- correct event code should be 0x0f
	     return false;
	end
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	

	return true, rst_tbl;
end

function EVENT_HCI_IO_Capability_Request(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x31) then  -- correct event code should be 0x0f
	     return false;
	end
	
	rst_tbl.para_len = tbl[2+offset];
	rst_tbl.BD_ADDR = {tbl[3+offset],tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset]};
	
	return true, rst_tbl;
end

function  EVENT_HCI_Role_Change(tbl,mode)
	local flag = true;
	local rst_tbl={};
	local offset = 0;
	if(mode == "uart") then    -- uart need one more byte 0x04
	   offset = 1;
	   if(tbl[1] ~= 0x04) then 
	     return false;
	   end
	end
	
	if(tbl[1+offset] ~= 0x12) then  -- correct event code should be 0x12
	     return false;
	end
	-- rst_tbl.para_len = tbl[2+offset];
	rst_tbl.Status = tbl[3+offset];
	rst_tbl.BD_ADDR = {tbl[4+offset],tbl[5+offset],tbl[6+offset],tbl[7+offset],tbl[8+offset],tbl[9+offset]};
	rst_tbl.New_Role = tbl[10+offset];

	return true, rst_tbl;
end

-----------   HCI event table  -------------------

g_hci_event_tbl = {
{ code = 0x01, name = "HCI_Inquiry_Complete", func = EVENT_HCI_Inquiry_Complete},
{ code = 0x02, name = "HCI_Inquiry_Result", func = EVENT_HCI_Inquiry_Result},
{ code = 0x03, name = "HCI_Connection_Complete", func = EVENT_HCI_Connection_Complete },
{ code = 0x04, name = "HCI_Connection_Request", func = EVENT_HCI_Connection_Request },
{ code = 0x08, name = "HCI_Encryption_Change", func = EVENT_HCI_Encryption_Change},
{ code = 0x0d, name = "HCI_QoS_Setup_Complete", func = EVENT_HCI_QoS_Setup_Complete},
{ code = 0x0e, name = "HCI_Command_Complete", func = EVENT_HCI_Command_Complete},
{ code = 0x0f, name = "HCI_Command_Status", func = EVENT_HCI_Command_Status},
{ code = 0x12, name = "HCI_Role_Change", func = EVENT_HCI_Role_Change},
{ code = 0x17, name = "HCI_Link_Key_Request", func = EVENT_HCI_Link_Key_Request },
{ code = 0x31, name = "HCI_IO_Capability_Request", func = EVENT_HCI_IO_Capability_Request },
};



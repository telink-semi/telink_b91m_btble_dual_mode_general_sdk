
require("printf")

baudrate_tbl={"300","600","1200","2400","4800","9600","14400","19200","38400","56000","57600","115200","128000","256000","921600"}

function tl_uart_open(device_number,baudrate)
	device_list,device_num=tl_rs232_list()
	handle_r = nil
	handle_w = nil
	if(device_num==0) then
		log_printf(1," Not Find UART!",0,0)
	else
		baudrate_index =nil
		for i=1,15 do
			if(baudrate_tbl[i]==baudrate) then
				baudrate_index = i
			end
		end

		if(baudrate_index~=nil) then
			log_printf(1,"Open UART device:"..device_list[device_number],0,0)
			tl_rs232_open(device_list[device_number],baudrate_index)
			handle_r = 1
			handle_w = 1
			--[[
				----------    Function Map   -----------
				find:	tl_rs232_list();
				read:  	r_tbl,r_len = tl_rs232_recv()
				write: 	len = tl_rs232_send(array,array_len)
				end:	tl_rs232_close()
			]]
		else
			log_printf(1,"UART baudrate NOT exist",0,0)
		end
	end

	return handle_r, handle_w
end

function tl_uart_write(handle_w,buffer,len)
	if(handle_w==1) then
		tl_rs232_send(buffer,len)
	else
		log_printf(1,"UART handle_w NOT exist",0,0)
	end
end

--[[



tl_uart_fifo={}
tl_wptr=1
tl_rptr=1
tl_unit_wptr=1

function tl_uart_read(handle_r)
	if(handle_r==1) then
		res,len = tl_rs232_recv()
		-- read data 
		if(res[1]~=2 and res[1]~=3 and res[1]~=4) then
			log_printf(1,"HCI OPCODE NOT exist",0,0)
		else
			while(1)
			do
				if(res[1]==2) then --  HCI ACL
					hci_len = res[4]
					tl_uart_fifo[tl_wptr] ={}
					for i=1,hci_len do
						tl_uart_fifo[tl_wptr][i] = res[i]
					end
					
					tl_wptr = tl_wptr + 1  --  receive one complete packet

					hci_len = hci_len + 4  --  get the length of this complete packet
				end

				if(res[1]==3) then --  HCI SCO

				end

				if(res[1]==4) then --  HCI EVENT

				end

				if(len>hci_len) then
					tl_uart_fifo[tl_wptr] ={}
					
					for i=1,hci_len do
						tl_uart_fifo[tl_wptr][i] = res[i]
					end
				else
					break
				end
			end
		end
		-- analyze data [HCI COMMAND/ACL/SCO]

		-- move data to fifo

		-- return the first data in fifo 

	else
		log_printf(1,"UART handle_r NOT exist",0,0)
	end
	return r_tbl,r_len
end
]]
function tl_uart_close()
	tl_rs232_close()
end
require("printf")
require("device")
require("DefaultSetting-LinkHandling")
require("Preamble-IUT-Master")
-- IUT  						 Lower Tester
-- caracara<kite dongle> 	<->  caracara<kite dongle> 
-- caracara<uart>			<->  caracara<uart>
-- caracara<kite dongle>	<->  csr<usb>       


Testcase = "LMP/LIH/BV-01-C"
iut_mode = "uart"
lt_mode  = "usb"
print("--------------------<<   "..Testcase.."   >>--------------------")

-- Open Device
iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w = Open_Device()
if(iut_handle_r==nil or iut_handle_w==nil or lt_handle_r==nil or lt_handle_w==nil) then
	-- Close Device
	Close_Device(lt_handle)
	return
end

-- <= LT:HCI Reset
	HCI_Reset(lt_handle_w,lt_mode)
-- => LT:HCI Command Complete event
	HCI_Wait_Command_Complete(lt_handle_r,lt_mode)


-- Close Device
Close_Device(lt_handle)
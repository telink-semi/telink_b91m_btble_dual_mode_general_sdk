----config：
Chip = "CaraCara"
Version = "A0"
CMW500_Resources = "TCPIP0::CMW50050-100513::inst0::INSTR"    --仪器编号：15-0025

DEBUG   = 0
VI_TRUE =  1
VI_FALSE = 0





function CMW500_CNTL_Test()

	print ("----------- CMW500 connection  test ------------")

	print(string.format("Chip : %s",Chip))
	print(string.format("Chip Version : %s\n",Version))
	print (string.format("CMW500 Resource Name : %s\n",CMW500_Resources))
	print(string.format("Now time : %s\n",os.date("%Y-%m-%d %H-%M-%S")))
	
	tl_progress(10)
	print ("----------- Loading CMW500.dll ------------")
	mylib = require("CMW500")	
	print ("Load the dynamic library file : CMW500.dll")
	CMW500_ID = mylib.CMW500_OpenDefaultRM()
	print (string.format("CMW500_ID : 0x%x",CMW500_ID))
	 
	CMW500_Handle = mylib.CMW500_Open(CMW500_ID,CMW500_Resources)
	print (string.format("CMW500_Handle : 0x%x",CMW500_Handle))
	
	print ("----------- Inq CMW500 ------------")
	print ("CMD : *IDN?\n ")
	mylib.CMW500_Printf(CMW500_Handle,"*IDN?\n")
	
	local r 
	local ret
	r,ret = mylib.CMW500_Scanf(CMW500_Handle, "%t");
	print(string.format("CMW500 information : %s\n",r))
	
	if(ret ~= 0) then
		print("Check Instrument connecting !!!!!!!\r\n");
		print(string.format("0x%08x",bit32.band(ret,0x7fffffff)))
		mylib.CMW500_Close(CMW500_ID)
		mylib.CMW500_Close(CMW500_Handle)
		print ("CMD :CMW500_Close")
		do
			return
		end
	else
		print("CMW500 connection successeed\n");
	end
	
	print ("----------- Init CMW500 ------------")
	print ("CMD : *RST; *OPC?\n ")
	mylib.CMW500_Printf(CMW500_Handle,"*RST; *OPC?")
	tl_progress(20)
	tl_sleep_ms(100)
	tl_progress(30)
	
	print ("CMD : *CLS; *OPC?\n ")
	mylib.CMW500_Printf(CMW500_Handle,"*CLS; *OPC?")
	tl_sleep_ms(20)
	
	print ("CMD : SYSTEM: REMOTE\n ")
	mylib.CMW500_Printf(CMW500_Handle,"SYSTEM:REMOTE\n")
	
	print ("----------- Setting CMW500 ------------")
	print ("CMD : CONFigure:BLUetooth:MEAS:MEValuation:TOUT 1\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:MEValuation:TOUT 1\n")
	
	print ("CMD : ROUTe:BLUetooth:MEAS:SCENario:SALone RF1C,RX1\n ")
	mylib.CMW500_Printf(CMW500_Handle,"ROUTe:BLUetooth:MEAS:SCENario:SALone RF1C,RX1\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:RFSettings:EATTenuation 2\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:RFSettings:EATTenuation 2\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:RFSettings:ENPower 0\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:RFSettings:ENPower 0\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:RFSettings:UMARgin 3\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:RFSettings:UMARgin 3\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:RFSettings:FREQuency 2402E+6\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:RFSettings:FREQuency 2402E+6\n")
	
	print ("CMD : ROUTe:BLUetooth:MEAS:SCENario:CSPath 'Bluetooth Sig1\n ")
	mylib.CMW500_Printf(CMW500_Handle,"ROUTe:BLUetooth:MEAS:SCENario:CSPath 'Bluetooth Sig1\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:RFSettings:MMODe SING\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:RFSettings:MMODe SING\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:RFSettings:MCHannel 78\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:RFSettings:MCHannel 78\n")
	
	
	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:DMODe MANual\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:DMODe MANual\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:BTYPe EDR\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:BTYPe EDR\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:PTYPe:EDRate E25P\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:PTYPe:EDRate E25P\n")
	
	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:PATTern OTHer\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:PATTern OTHer\n")
	

	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:PLENgth:EDRate?\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:PLENgth:EDRate?\n")
	
	r,ret = mylib.CMW500_Scanf(CMW500_Handle, "%t");
	print(string.format("CMW500 EDRate : %s",r))

	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:LAP #H121212\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:LAP #H121212\n")

	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:UAP #H34\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:UAP #H34\n")

	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:NAP #H5656\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:NAP #H5656\n")

	print ("CMD : CONFigure:BLUetooth:MEAS:ISIGnal:BDADdress?\n ")
	mylib.CMW500_Printf(CMW500_Handle,"CONFigure:BLUetooth:MEAS:ISIGnal:BDADdress?\n")
	
	r,ret = mylib.CMW500_Scanf(CMW500_Handle, "%t");
	print(string.format("CMW500 BDADdress : %s",r))
	
	tl_progress(50)
	tl_sleep_ms(2000)
	tl_progress(90)
	
	mylib.CMW500_Close(CMW500_ID)
	mylib.CMW500_Close(CMW500_Handle)
	print ("CMD :CMW500_Close")
	
	
end

require "bit32"

CMW500_CNTL_Test()
require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")
-- IUT                       Lower Tester
-- eagle dongle(Slave)   <->  csr<usb> (Master)     

function LMP_LIH_BV_102_C(iut_mode,lt_mode,role,Timeout)

    Testcase = "LMP/LIH/BV-102-C"
    -- iut_mode = "dongle"
    -- lt_mode  = "usb"
    OPTIONAL = ALT1
    print("--------------------<<   "..Testcase.."   >>--------------------")

    local Test_Status="Pass"
    -- Open Device
    iut_handle_r,iut_handle_w,lt_handle_r,lt_handle_w = Open_Device(iut_mode,lt_mode)

    if(lt_mode~="unused")then
        if(lt_handle_r==nil or lt_handle_w==nil) then
            Test_Status= "Fail"
            goto Test_Case_End
        end
    end

    if(iut_mode~="unused")then
        if(iut_handle_r==nil or iut_handle_w==nil) then
            Test_Status= "Fail"
            goto Test_Case_End
        end
    end


    -- Default Settings
    flag = Default_Setting(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,role,Timeout)

    if(flag==false) then
        Test_Status= "Fail"
        goto Test_Case_End
    end

    if(role=="master") then
        -- Preamble-IUT-Master
        Allow_Role_Switch = 0  -- allow role switch
        lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Master(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

        if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
            Test_Status="Fail"
            goto Test_Case_End
        end
    end


    if(role=="slave") then
        -- Preamble-IUT-Master
        Allow_Role_Switch = 0  -- allow role switch
        lt_Connection_Handle,iut_Connection_Handle = Preamble_IUT_Slave(iut_handle_r, iut_handle_w,lt_handle_r,lt_handle_w,iut_mode,lt_mode,Allow_Role_Switch,Timeout)

        if(lt_Connection_Handle==nil and iut_Connection_Handle==nil) then
            Test_Status="Fail"
            goto Test_Case_End
        end
    end

    -- trigger LT(csr dongle) to initiate the eSCO link setup procedure with IUT(kite dongle)
    print("============ LT initiates the eSCO link setup procedure =============")
    Transmit_Bandwidth    = 8000; --SCO_BANDWIDTH
    Receive_Bandwidth     = 8000; --SCO_BANDWIDTH
    Max_Latency           = 18;     --18ms
    Voice_Setting         = 0x0043;     --AIR_COD_TRANS|INPUT_COD_LIN|INPUT_SAMP_8BIT|INPUT_DATA_2COMP
    Retransmission_Effort = 0x01;       --At least one retransmission, optimize for power consumption (eSCO connection required)
    Packet_Type           = 0x03E0;     --only EV5 is allowed, SYNC_PACKET_TYPE_EV5_FLAG|SYNC_PACKET_TYPE_NO_EV3_2_FLAG|SYNC_PACKET_TYPE_NO_EV3_3_FLAG|SYNC_PACKET_TYPE_NO_EV5_2_FLAG|SYNC_PACKET_TYPE_NO_EV5_3_FLAG
    HCI_Setup_Synchronous_Connection(lt_handle_w,lt_mode,lt_Connection_Handle,Transmit_Bandwidth,Receive_Bandwidth,Max_Latency,Voice_Setting,Retransmission_Effort,Packet_Type);
    flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
    if(flag~=true) then
        Test_Status = "Fail"
        goto Test_Case_End
    end

-- => IUT:HCI_Connection_Request
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Connection_Request",Timeout)
    if(flag~=true or rst_tbl.Link_Type~=0x02) then
        Test_Status = "Fail"
        goto Test_Case_End
    end

--  <= IUT:HCI_Accept_Connection_Request
    if(flag==true and iut_mode~="unused") then
        HCI_Accept_Synchronous_Connection_Request(iut_handle_w,iut_mode,rst_tbl.BD_ADDR,Transmit_Bandwidth,Receive_Bandwidth,Max_Latency,Voice_Setting,Retransmission_Effort,Packet_Type);
        -- => IUT:HCI Command Status event  
        flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
        if(flag~=true) then
            Test_Status = "Fail"
            goto Test_Case_End
        end     
    end

--  <= IUT:wait for HCI_Synchronous_Connection_Complete Event
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Synchronous_Connection_Complete",Timeout)
    if(flag~=true) then
        Test_Status = "Fail"
        goto Test_Case_End
    elseif(rst_tbl~=nil) then
        iut_sync_con_hdl = rst_tbl.Connection_Handle[1] + rst_tbl.Connection_Handle[2]*256;
        log_printf(HCI_PROCESS_STATUS_DISPLAY,"iut_sync_con_hdl",rst_tbl.Connection_Handle,2);
        iut_sync_rx_pkt_len = rst_tbl.RX_Packet_Length;
        iut_sync_tx_pkt_len = rst_tbl.TX_Packet_Length;
        print("iut_sync_rx_pkt_len",iut_sync_rx_pkt_len);
        print("iut_sync_tx_pkt_len",iut_sync_tx_pkt_len);
    end

--  <= LT:wait for HCI_Synchronous_Connection_Complete Event
    flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Synchronous_Connection_Complete",Timeout)
    if(flag~=true) then
        Test_Status = "Fail"
        goto Test_Case_End
    elseif(rst_tbl~=nil) then
        lt_sync_con_hdl = rst_tbl.Connection_Handle[1] + rst_tbl.Connection_Handle[2]*256;
        log_printf(HCI_PROCESS_STATUS_DISPLAY,"lt_sync_con_hdl",rst_tbl.Connection_Handle,2);
        lt_sync_rx_pkt_len = rst_tbl.RX_Packet_Length;
        lt_sync_tx_pkt_len = rst_tbl.TX_Packet_Length;
        print("lt_sync_rx_pkt_len",lt_sync_rx_pkt_len);
        print("lt_sync_tx_pkt_len",lt_sync_tx_pkt_len);
    end

    print("============ IUT send HCI Synchronous Data packets =============")
    Data = array.new(iut_sync_tx_pkt_len);
    for i=1,iut_sync_tx_pkt_len do
        Data[i] = i;
    end
    HCI_Synchronous_Data_packet_TX(iut_handle_w,iut_mode,iut_sync_con_hdl,iut_sync_tx_pkt_len,Data);

    delay_sec(1);
    Data[1] = Data[1] + 1;
    HCI_Synchronous_Data_packet_TX(iut_handle_w,iut_mode,iut_sync_con_hdl,iut_sync_tx_pkt_len,Data);

    ::Test_Case_End::
    print("============ Test Result:"..Test_Status.." =============")
    -- Close Device
    Close_Device(iut_mode,lt_mode)
    return Test_Status
end

LMP_LIH_BV_102_C("dongle","usb","slave",10)
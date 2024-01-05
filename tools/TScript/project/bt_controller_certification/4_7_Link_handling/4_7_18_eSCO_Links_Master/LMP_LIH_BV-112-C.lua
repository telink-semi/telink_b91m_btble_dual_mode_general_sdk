require("project.bt_controller_certification.4_7_Link_handling.DefaultSetting-LinkHandling")
-- IUT                       Lower Tester
-- eagle dongle(Slave)  <->  csr<usb> (Master)     

function LMP_LIH_BV_112_C(iut_mode,lt_mode,role,Timeout)

    Testcase = "LMP/LIH/BV-112-C"
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

    -- trigger IUT(eagle dongle) to initiate the eSCO link setup procedure with LT(csr dongle)
    print("============ IUT initiates the eSCO link setup procedure =============")
    Transmit_Bandwidth    = 8000; --SCO_BANDWIDTH
    Receive_Bandwidth     = 8000; --SCO_BANDWIDTH
    Max_Latency           = 7;     --7ms
    Voice_Setting         = 0x0043;     --AIR_COD_TRANS|INPUT_COD_LIN|INPUT_SAMP_8BIT|INPUT_DATA_2COMP
    Retransmission_Effort = 0x01;       --At least one retransmission, optimize for power consumption (eSCO connection required)
    Packet_Type           = 0x03C8;     --only EV3 is allowed, SYNC_PACKET_TYPE_EV3_FLAG|SYNC_PACKET_TYPE_NO_EV3_2_FLAG|SYNC_PACKET_TYPE_NO_EV3_3_FLAG|SYNC_PACKET_TYPE_NO_EV5_2_FLAG|SYNC_PACKET_TYPE_NO_EV5_3_FLAG
    HCI_Setup_Synchronous_Connection(iut_handle_w,iut_mode,iut_Connection_Handle,Transmit_Bandwidth,Receive_Bandwidth,Max_Latency,Voice_Setting,Retransmission_Effort,Packet_Type);
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
    if(flag~=true) then
        Test_Status = "Fail"
        goto Test_Case_End
    end

-- => LT:HCI_Connection_Request
    flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Connection_Request",Timeout)
    if(flag~=true or rst_tbl.Link_Type~=0x02) then
        Test_Status = "Fail"
        goto Test_Case_End
    end

-- <= LT:HCI_Accept_Connection_Request
    if(flag==true and lt_mode~="unused") then
        Max_Latency = 0xFFFF; --don't care
        Packet_Type = 0x03F8; --Accept EV3/EV4/EV5 packets
        HCI_Accept_Synchronous_Connection_Request(lt_handle_w,lt_mode,rst_tbl.BD_ADDR,Transmit_Bandwidth,Receive_Bandwidth,Max_Latency,Voice_Setting,Retransmission_Effort,Packet_Type);
        -- => LT:HCI Command Status event  
        flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Command_Status",Timeout)
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

    delay_sec(2);

    print("============ IUT initiates Synchronous Link Parameters Change =============")
    if (iut_sync_con_hdl==nil) then
        Test_Status = "Fail"
        goto Test_Case_End
    end
    Max_Latency           = 0xFFFF; --don't care
    Packet_Type           = 0x03F0; --EV4 or EV5 is allowed, SYNC_PACKET_TYPE_EV4_FLAG|SYNC_PACKET_TYPE_EV5_FLAG|SYNC_PACKET_TYPE_NO_EV3_2_FLAG|SYNC_PACKET_TYPE_NO_EV3_3_FLAG|SYNC_PACKET_TYPE_NO_EV5_2_FLAG|SYNC_PACKET_TYPE_NO_EV5_3_FLAG
    HCI_Setup_Synchronous_Connection(iut_handle_w,iut_mode,iut_sync_con_hdl,Transmit_Bandwidth,Receive_Bandwidth,Max_Latency,Voice_Setting,Retransmission_Effort,Packet_Type);
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Command_Status",Timeout)
    if(flag~=true) then
        Test_Status = "Fail"
        goto Test_Case_End
    end

    -- => IUT:Wait for HCI_Synchronous_Connection_Changed Event
    flag,rst_tbl = HCI_Wait_Event_Done(iut_handle_r,iut_mode,"HCI_Synchronous_Connection_Changed",Timeout)
    if(flag~=true or rst_tbl == nil or rst_tbl.Status~=0) then
        Test_Status = "Fail"
        goto Test_Case_End
    end
    log_printf(HCI_PROCESS_STATUS_DISPLAY,"iut_sync_con_hdl",rst_tbl.Connection_Handle,2);
    iut_sync_rx_pkt_len = rst_tbl.RX_Packet_Length;
    iut_sync_tx_pkt_len = rst_tbl.TX_Packet_Length;
    print("iut_sync_rx_pkt_len",iut_sync_rx_pkt_len);
    print("iut_sync_tx_pkt_len",iut_sync_tx_pkt_len);
    print("iut_sync_transmission_interval",rst_tbl.Transmission_Interval);
    print("iut_sync_retransmission_window",rst_tbl.Retransmission_Window);

    -- => LT:Wait for HCI_Synchronous_Connection_Changed Event
    flag,rst_tbl = HCI_Wait_Event_Done(lt_handle_r,lt_mode,"HCI_Synchronous_Connection_Changed",Timeout)
    if(flag~=true or rst_tbl == nil or rst_tbl.Status~=0) then
        Test_Status = "Fail"
        goto Test_Case_End
    end
    log_printf(HCI_PROCESS_STATUS_DISPLAY,"lt_sync_con_hdl",rst_tbl.Connection_Handle,2);
    lt_sync_rx_pkt_len = rst_tbl.RX_Packet_Length;
    lt_sync_tx_pkt_len = rst_tbl.TX_Packet_Length;
    print("lt_sync_rx_pkt_len",lt_sync_rx_pkt_len);
    print("lt_sync_tx_pkt_len",lt_sync_tx_pkt_len);
    print("lt_sync_transmission_interval",rst_tbl.Transmission_Interval);
    print("lt_sync_retransmission_window",rst_tbl.Retransmission_Window);
    
    ::Test_Case_End::
    print("============ Test Result:"..Test_Status.." =============")
    -- Close Device
    Close_Device(iut_mode,lt_mode)
    return Test_Status
end

LMP_LIH_BV_112_C("dongle","usb","master",10)
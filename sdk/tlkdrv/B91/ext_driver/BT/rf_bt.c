/********************************************************************************************************
 * @file	rf_bt.c
 *
 * @brief	This is the source file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#include "rf_bt.h"
#if SYNC_20210329
static inline void rf_set_power_manual()
{
	// set power by power index in BR
	reg_mode_cfg_tx3 &= ~FLD_MODE_CFG_TX3_TXC_PWR_SRL;
}
#endif

static void sub_wr(unsigned int addr, unsigned char value, unsigned char e, unsigned char s);
static void sub_wr_word(unsigned int addr, unsigned int value, unsigned char e, unsigned char s);

extern void ext_btrf_radioInit(void);
extern void ext_btrf_brManualInit(void);
void bt_master_clk_enable(unsigned char en)
{
	unsigned char dat = read_reg8(SC_BASE_ADDR+0x27);
	if(en)
	{
		write_reg8(SC_BASE_ADDR+0x27,dat|BIT(1)); // enable ZB master clock
	}
	else
	{
		write_reg8(SC_BASE_ADDR+0x27,dat&(~BIT(1))); // disable ZB master clock
	}
}
void rf_radio_init1(void)
{
	//To modify DCOC parameters
	sub_wr(RADIOADDR+0xd2 , 0x1, 4, 4); //DCOC_SFIIP
	sub_wr(RADIOADDR+0xd2 , 0x0, 5, 5); //DCOC_SFQQP
	sub_wr(RADIOADDR+0xd2 , 0x2, 7 , 6); //DCOC_SFII_L
	sub_wr(RADIOADDR+0xd3 , 0x1, 1, 0); //DCOC_SFII_H
	sub_wr(RADIOADDR+0xd3 , 0x6, 5, 2); //DCOC_SFQQ

	//Setting for blanking
	sub_wr(MODEMADDR+0x7b, 0x3, 3, 2); //BLANK_WINDOW

	//To set AGC thresholds
	sub_wr(RADIOADDR+0x4a, 0x0e, 6, 0); //POW_000_001
	sub_wr(RADIOADDR+0x4a, 0x0, 7, 7); //POW_001_010_L
	sub_wr(RADIOADDR+0x4b, 0x09, 5, 0); //POW_001_010_H

	sub_wr(RADIOADDR+0x4e, 0x09, 6, 0); //POW_100_101
	sub_wr(RADIOADDR+0x4e, 0x0, 7, 7); //POW_101_100_L
	sub_wr(RADIOADDR+0x4f, 0x0f, 5, 0); //POW_101_100_H

	sub_wr(RADIOADDR+0x54, 0x0e, 6, 0); //POW_000_001
	sub_wr(RADIOADDR+0x54, 0x0, 7, 7); //POW_001_010_L
	sub_wr(RADIOADDR+0x55, 0x09, 5, 0); //POW_001_010_H

	sub_wr(RADIOADDR+0x56, 0x0c, 6, 0); //POW_010_011
	sub_wr(RADIOADDR+0x56, 0x0, 7, 7); //POW_011_100_L
	sub_wr(RADIOADDR+0x57, 0x08, 5, 0); //POW_011_100_H

	sub_wr(RADIOADDR+0x58, 0x09, 6, 0); //POW_100_101
	sub_wr(RADIOADDR+0x58, 0x0, 7, 7); //POW_101_100_L
	sub_wr(RADIOADDR+0x59, 0x0f, 5, 0); //POW_101_100_H

	sub_wr(RADIOADDR+0x79, 1, 7, 7);// added by xuqiang

	//For optimum preamble detection
	sub_wr(MODEMADDR+0x76, 0x50, 7, 0); //RX_PE_DET_MIN_LO_THRESH
	sub_wr(MODEMADDR+0x77, 0x73, 7, 0); //RX_PE_DET_MIN_HI_THRESH

	//2019/10/29 Junwen add, to fix per floor
	//2020/04/26 Qiangkai remove, move this setting to aura_1m script
	//sub_wr(MODEMADDR+0x79, 1, 3, 3) //RX_DIS_PDET_BLANK
#if SYNC_20210329
	reg_modem_tx_tl_tctrl |= BIT(3);
#endif
}
void br_test_init(void)
{
//	bt_rwbtcntl_rwbten_setf(0);
//	rf_set_freq_chn_manual(19); //frequency and power level controlled by manual mode
	rf_radio_init1();
	ext_btrf_brManualInit();

	// set exchange map address:
	memset((unsigned char*)EM_ADDR,0,0x2000);
	rf_set_em_base_address(BUS_ADDR + OFFSET_ADDR);
//	bt_master_clk_enable(1);
//	bt_rwbtcntl_rwbten_setf(1);
}



void bt_init(void)
{
	br_test_init();// in test mode, frequency and power is controllable
	bt_master_clk_enable(1);
//	btc_bb_dbg_init();
//	bt_core_en(1);// in order to generate interrupt(eg. clkn) ,you must enable bt core at first
}

void br_rx_init(unsigned int rawstep,unsigned int winsize)
{
//  unsigned char i = 0;
//	unsigned char bdaddr[6]={0};

	//init EM radio table
	write_reg32(EM_ADDR+0x100,0);
	write_reg32(EM_ADDR+0x104,0);
	write_reg32(EM_ADDR+0x108,0);
	write_reg32(EM_ADDR+0x10c,0);
	write_reg32(EM_ADDR+0x110,0);
	write_reg32(EM_ADDR+0x114,0);
	write_reg32(EM_ADDR+0x118,0);
	write_reg32(EM_ADDR+0x11c,0);
	write_reg32(EM_ADDR+0x120,0);
	write_reg32(EM_ADDR+0x124,0);
	write_reg32(EM_ADDR+0x128,0);
	write_reg32(EM_ADDR+0x12c,0);
	write_reg32(EM_ADDR+0x130,0);
	write_reg32(EM_ADDR+0x134,0);
	write_reg32(EM_ADDR+0x138,0);
	write_reg32(EM_ADDR+0x13c,0);
	write_reg32(EM_ADDR+0x140,0);
	write_reg32(EM_ADDR+0x144,0);
	write_reg32(EM_ADDR+0x148,0);
	write_reg32(EM_ADDR+0x14c,0);

	//Extrc RF setting
	write_reg32(DM_ADDR+0x70, 0);
	//set RFSEL to 0x2
	write_reg32(DM_ADDR+0x74, 0x4020);

	//set sync Error to 0x7
	write_reg32(BT_ADDR+0x78, 0x4070100);

	//set tx/rx rate <0/1/2> to <0/1/2> value
	write_reg32(BT_ADDR+0x7c, 0x39003900);

#if !DEBUG_JUNWEI
	//set tx power on 112us, rxpower on 85us
	write_reg32(BT_ADDR+0x8c, 0x510755);

	//set rx path delay to 0x2
	write_reg32(BT_ADDR+0x90, 0xd04);
#endif

#if DEBUG_JUNWEI
	write_reg32(BT_ADDR+0x28, 0x1a000612);	// guard time 5us -> 6us ,increased by xuqiang
	sub_wr(MODEMADDR+0x84, 0x4a, 6, 0);	   		// modem br data flow delay 10 cycle *  1/24M (24M:fixed clock for modem)
#else
		//EDR control register
		write_reg32(BT_ADDR+0x28, 0x1a000512);
#endif
	//BT_PREFETCHABORT_TIME_US 200us
	//BT_PREFETCH_TIME_US 160us
	write_reg32(BT_ADDR+0xe0, 0x1900140);

	//Encryption working space @ [0x160 - 0x16f]
	write_reg32(E0_ADDR, 0);
	write_reg32(E0_ADDR+0x4, 0);
	write_reg32(E0_ADDR+0x8, 0);
	write_reg32(E0_ADDR+0xc, 0);

	//control structure @ 0x0200
	sub_wr_word(CS_ADDR,0x3,4,0); //format-> slave:3
	sub_wr_word(CS_ADDR,0,7,7); //SAM_EN
	sub_wr_word(CS_ADDR,0,8,8); //DNABORT
	sub_wr_word(CS_ADDR,0,9,9); //RXBSY_EN
	sub_wr_word(CS_ADDR,0,10,10); //TXBSY_EN
	sub_wr_word(CS_ADDR,0,31,16); //CLKOFF[15:0]

	sub_wr_word(CS_ADDR+0x4,0,11,0); //CLKOFF[27:16]
	sub_wr_word(CS_ADDR+0x4,0,20,16); //linklb
	sub_wr_word(CS_ADDR+0x4,1,26,24); //aclltaddr
	//sub_wr_word(CS_ADDR+0x4,0,27,27); //acledr->br:0/edr:1  controlled by script
	sub_wr_word(CS_ADDR+0x4,1,28,28); //whdsb
	sub_wr_word(CS_ADDR+0x4,0,29,29); //laap
	sub_wr_word(CS_ADDR+0x4,0,30,30); //afhena
	sub_wr_word(CS_ADDR+0x4,0,31,31); //aknena

//	for(i=0;i<6;i++)
//	{
//		bdaddr[i] = read_reg8(CS_ADDR+0x8+i);// controlled by script
//	}

	sub_wr_word(CS_ADDR+0x10,0x3ff,31,22); //rxmaxbuf[9:0]
	sub_wr_word(CS_ADDR+0x14,0x07,7,0); //txpwr[7:0]
	sub_wr_word(CS_ADDR+0x14,0x00,14,8); //freq[6:0]
	sub_wr_word(CS_ADDR+0x14,0x1,15,15); //fh_en
	sub_wr_word(CS_ADDR+0x14,0x0,17,16); //txcrypt[1:0]
	sub_wr_word(CS_ADDR+0x14,0x0,18,18); //txbcry
	sub_wr_word(CS_ADDR+0x14,0x0,19,19); //txeir
	sub_wr_word(CS_ADDR+0x14,0x0,22,22); //nullflt
	sub_wr_word(CS_ADDR+0x14,0x0,23,23); //stopmod
	sub_wr_word(CS_ADDR+0x14,0x0,25,24); //rxcrypt
	sub_wr_word(CS_ADDR+0x14,0x0,26,26); //rxbcry
	sub_wr_word(CS_ADDR+0x14,0x0,27,27); //rxeir
	sub_wr_word(CS_ADDR+0x14,0x0,31,29); //rxthr

	sub_wr_word(CS_ADDR+0x18,winsize&0x1fff,12,0); //rxwinsz[12:0] -> 1.28s
	sub_wr_word(CS_ADDR+0x18,0x1,15,15);   //rxwide
	sub_wr_word(CS_ADDR+0x18,0x2a0,30,16); //txdescptr

	sub_wr_word(CS_ADDR+0x1c,0xffffffff,31,0); //chmap[31:0]

	sub_wr_word(CS_ADDR+0x20,0xffffffff,31,0); //chmap[63:32]

	sub_wr_word(CS_ADDR+0x24,0x7fff,14,0); //chmap[78:64]
	sub_wr_word(CS_ADDR+0x24,0x01,23,16); //att_nb[7:0]
	sub_wr_word(CS_ADDR+0x24,0x00,31,24); //to_nb[7:0]

	sub_wr_word(CS_ADDR+0x28,0x1fff,12,0); //maxfrmtime[12:0]
	sub_wr_word(CS_ADDR+0x28,0x0000,31,16); //sam_ptr[15:0]

	sub_wr_word(CS_ADDR+0x2c,0x00,7,0); //sam_offset[7:0]
	sub_wr_word(CS_ADDR+0x2c,0x00,15,8);//sam_length[7:0]
	sub_wr_word(CS_ADDR+0x2c,0x0000,31,16); //sk[15:0]

	sub_wr_word(CS_ADDR+0x30,0,31,0);//sk[47:16]

	sub_wr_word(CS_ADDR+0x34,0,31,0); //sk[79:48]

	sub_wr_word(CS_ADDR+0x38,0,31,0); //sk[111:80]

	sub_wr_word(CS_ADDR+0x3c,0,15,0); //sk[127:112]
	sub_wr_word(CS_ADDR+0x3c,0,31,16); //lv[15:0]

	sub_wr_word(CS_ADDR+0x40,0,31,0); //lv[47:16]

	sub_wr_word(CS_ADDR+0x44,0,15,0); //lv[63:48]
	sub_wr_word(CS_ADDR+0x44,0,31,16); //txccmpldcnt[15:0]

	sub_wr_word(CS_ADDR+0x48,0,19,0); //txccmpldcnt[35:16]

	sub_wr_word(CS_ADDR+0x4c,0,31,0); //rxccmpldcnt[31:0]

	sub_wr_word(CS_ADDR+0x50,0,3,0); //rxccmpldcnt[35:32]

	sub_wr_word(CS_ADDR+0x5c,0,0,0); //escolastseqn
	sub_wr_word(CS_ADDR+0x5c,0,1,1); //escolastack
	sub_wr_word(CS_ADDR+0x5c,0,2,2); //escowaitack
	sub_wr_word(CS_ADDR+0x5c,0,8,8); //escoretx
	sub_wr_word(CS_ADDR+0x5c,0,9,9); //escorsvd
	sub_wr_word(CS_ADDR+0x5c,0,16,16); //lasttxseqn
	sub_wr_word(CS_ADDR+0x5c,0,17,17); //lasttxack
	sub_wr_word(CS_ADDR+0x5c,0,23,23); //last_null
	sub_wr_word(CS_ADDR+0x5c,0,26,26); //waitack
	sub_wr_word(CS_ADDR+0x5c,0,27,27); //rswitch
	sub_wr_word(CS_ADDR+0x5c,0,28,28); //fnack
	sub_wr_word(CS_ADDR+0x5c,0,29,29); //fcrc
	sub_wr_word(CS_ADDR+0x5c,0,30,30); //txflush
	sub_wr_word(CS_ADDR+0x5c,0,31,31); //txtog


	sub_wr_word(CS_ADDR+0x60,0,0,0); //lastrxseqn
	sub_wr_word(CS_ADDR+0x60,0,1,1); //lastrxack
	sub_wr_word(CS_ADDR+0x60,1,2,2); //lastrxflow

	//Tx descriptor 0 for Control structure
	sub_wr_word(TD0_ADDR,0x2a0,14,0); //nextptr[14:0]
	sub_wr_word(TD0_ADDR,0,25,25); //txseqn
	sub_wr_word(TD0_ADDR,0,24,24); //txarqn
	sub_wr_word(TD0_ADDR,0,23,23); //txflow
	sub_wr_word(TD0_ADDR,0x4,22,19); //txtype[3:0]
	sub_wr_word(TD0_ADDR,0x1,18,16); //txltaddr[2:0]

	sub_wr_word(TD0_ADDR+0x4,0,15,13); //txundef
	sub_wr_word(TD0_ADDR+0x4,0x01,12,3); //txlength[9:0]
	sub_wr_word(TD0_ADDR+0x4,0,2,2); //txpflow
	sub_wr_word(TD0_ADDR+0x4,0,1,0); //txllid[1:0]
	sub_wr_word(TD0_ADDR+0x4,0x0300,31,16); //txaclbufptr

	sub_wr_word(TD0_ADDR+0x8,0x0700,15,0); //txlmbufptr

	//Tx buffer 0 for control structure
	write_reg32(EM_ADDR+0x300,0x000000ff);
	write_reg32(EM_ADDR+0x304,0xffffffff);
	write_reg32(EM_ADDR+0x308,0xab2d5610);

	//Tx LM buffer 0 for control structure
	write_reg32(EM_ADDR+0x700,0xcd000023);
	write_reg32(EM_ADDR+0x704,0xab690201);
	write_reg32(EM_ADDR+0x708,0x891004ef);

	//Rx descriptor 0 for control structure
	sub_wr_word(RD0_ADDR,0x2c0,14,0); //nextptr[14:0]

	sub_wr_word(RD0_ADDR+0x8,0x0500,31,16); //rxaclbufptr[15:0]

	sub_wr_word(RD0_ADDR+0xc,0x0900,15,0); //rxlmbufptr[15:0]

	//rx buffer0 for control structure
	for(int i=0;i< 0x20;i++)
	{
		write_reg32(RXBUF0_ADDR+i*4,0);
	}

	//rx buffer1 for control structure
	for(int j=0;j< 0x20;j++)
	{
		write_reg32(RXBUF1_ADDR+j*4,0);
	}

	//configure currentrxdescptr
	write_reg32(BT_ADDR+0x2c,0x2c0);

	//enable rw-bt core
//	write_reg32(BT_ADDR+0x00,0xde107);
	write_reg32(BT_ADDR+0x00,0xff107);
//	write_reg32(BT_ADDR+0x00,0xdf105);
//	write_reg32(BT_ADDR,0xdf107);  // enable crc, Added by LiXiang_sw

	//ET0
	sub_wr_word(ET_ADDR + ET0,0x1,2,0); //mode[2:0]
	sub_wr_word(ET_ADDR + ET0,0x0,5,3); //status[2:0]
	sub_wr_word(ET_ADDR + ET0,0,6,6); //esco
	sub_wr_word(ET_ADDR + ET0,0,7,7); //rsvd
	sub_wr_word(ET_ADDR + ET0,0,8,8); //sniff
	sub_wr_word(ET_ADDR + ET0,0,9,9); //csb
	sub_wr_word(ET_ADDR + ET0,0,10,10); //spa
	sub_wr_word(ET_ADDR + ET0,0,15,11); //sch_prio1[4:0]
	sub_wr_word(ET_ADDR + ET0,(rawstep&0xffff),31,16); //rawstp[15:0]

	sub_wr_word(ET_ADDR + ET0+0x4,((rawstep>>16)&0xfff),11,0); //rawstp[27:16]
	sub_wr_word(ET_ADDR + ET0+0x4,0x270,25,16);//finestp[9:0]

	sub_wr_word(ET_ADDR + ET0+0x8,((CS_ADDR-ET_ADDR)>>1),14,0); //csptr[15:1]
	sub_wr_word(ET_ADDR + ET0+0x8,0,30,16); //priobw[14:0]
	sub_wr_word(ET_ADDR + ET0+0x8,0,31,31); //priobw_unit

	sub_wr_word(ET_ADDR + ET0+0xc,0,4,0); //sch_prio2[4:0]
	sub_wr_word(ET_ADDR + ET0+0xc,0,12,8); //sch_prio3[4:0]
	sub_wr_word(ET_ADDR + ET0+0xc,0,28,24); //pti_prio[4:0]
	sub_wr_word(ET_ADDR + ET0+0xc,0x3,17,16); //vxchan[1:0]

}
unsigned char mode_set_nslot;
u32 BT_Mode_PKT_Init(unsigned char BTMode,unsigned char PktType)
{
	//------------edr txtpye br(0,4) edr2(1,4),edr3(1,8)------------//
	u32 packet_length = 0;
    switch(BTMode)
    {
    	case 0:     // BR
    		switch(PktType)
    		{
    		   case 1:
				   sub_wr_word(CS_ADDR+0x4,0,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,4,22,19);  			//txtype[3:0]
				   packet_length = 27;
				   mode_set_nslot = 4;
    			   break;  // DH1
    		   case 3:
				   sub_wr_word(CS_ADDR+0x4,0,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,0xb,22,19);  			//txtype[3:0]
				   packet_length = 183;
				   mode_set_nslot = 8;
    			   break;  // DH3
    		   case 5:
				   sub_wr_word(CS_ADDR+0x4,0,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,0xf,22,19);  			//txtype[3:0]
				   packet_length = 339;
				   mode_set_nslot = 12;
				   break;  // DH5
    		   default:break;
    		}
    		break;

    	case 2:     // EDR2
    		switch(PktType)
    		{
			   case 1:
				   sub_wr_word(CS_ADDR+0x4,1,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,4,22,19);  			//txtype[3:0]
				   packet_length = 54;
				   mode_set_nslot = 4;
				   break;  // DH1
			   case 3:
				   sub_wr_word(CS_ADDR+0x4,1,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,0xa,22,19);  			//txtype[3:0]
				   packet_length = 367;
				   mode_set_nslot = 8;
				   break;  // DH3
			   case 5:
				   sub_wr_word(CS_ADDR+0x4,1,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,0xe,22,19);  			//txtype[3:0]
				   packet_length = 679;
				   mode_set_nslot = 12;
				   break;  // DH5
			   default:break;
    		}
    		break;

		case 3:     // EDR3
			switch(PktType)
			{
			   case 1:
				   sub_wr_word(CS_ADDR+0x4,1,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,8,22,19);  			//txtype[3:0]
				   packet_length = 83;
				   mode_set_nslot = 4;
				   break;  // DH1
			   case 3:
				   sub_wr_word(CS_ADDR+0x4,1,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,0xb,22,19);  			//txtype[3:0]
				   packet_length = 552;
				   mode_set_nslot = 8;
				   break;  // DH3
			   case 5:
				   sub_wr_word(CS_ADDR+0x4,1,27,27);  			//br:0, edr2:2 edr3:3
				   sub_wr_word(TD0_ADDR,0xf,22,19);  			//txtype[3:0]
				   packet_length = 1021;
				   mode_set_nslot = 12;
				   break;  // DH5
			   default:break;
			}
			break;

    	default:break;
    }
    return packet_length;
}

void BT_Payload_Init(unsigned char PktLoad)
{
	switch(PktLoad)
	{
	case 0:  // prbs9
		write_reg32(EM_ADDR+0x300,0xe8fbc1ff);  // 0
		write_reg32(EM_ADDR+0x304,0x8b72904c);  // 1
		write_reg32(EM_ADDR+0x308,0x8951b3e7);  // 2
		write_reg32(EM_ADDR+0x30c,0x2323ab63);
		write_reg32(EM_ADDR+0x310,0x72188402);
		write_reg32(EM_ADDR+0x314,0x3b2f61aa);
		write_reg32(EM_ADDR+0x318,0x37e5a851);
		write_reg32(EM_ADDR+0x31c,0xcac9fb49);
		write_reg32(EM_ADDR+0x320,0x2c53180c);
		write_reg32(EM_ADDR+0x324,0x9ae345fd);
		write_reg32(EM_ADDR+0x328,0xb05df1e6);
		write_reg32(EM_ADDR+0x32c,0xbeb41bb6);
		write_reg32(EM_ADDR+0x330,0xe9ea502a);
		write_reg32(EM_ADDR+0x334,0x5e4b9c0e);
		write_reg32(EM_ADDR+0x338,0xa1cc2457);
		write_reg32(EM_ADDR+0x33c,0x87b859b7);
		write_reg32(EM_ADDR+0x340,0x747de0ff);
		write_reg32(EM_ADDR+0x344,0xc5b94826);
		write_reg32(EM_ADDR+0x348,0xc4a8d9f3);
		write_reg32(EM_ADDR+0x34c,0x1191d5b1);
		write_reg32(EM_ADDR+0x350,0x390c4201);
		write_reg32(EM_ADDR+0x354,0x9d97b0d5);
		write_reg32(EM_ADDR+0x358,0x9bf2d428);
		write_reg32(EM_ADDR+0x35c,0x6564fda4);
		write_reg32(EM_ADDR+0x360,0x96298c06);
		write_reg32(EM_ADDR+0x364,0x4d71a2fe);
		write_reg32(EM_ADDR+0x368,0x582ef8f3);
		write_reg32(EM_ADDR+0x36c,0x5f5a0ddb);
		write_reg32(EM_ADDR+0x370,0x74f52815);
		write_reg32(EM_ADDR+0x374,0xaf25ce07);
		write_reg32(EM_ADDR+0x378,0xd0e6122b);
		write_reg32(EM_ADDR+0x37c,0xc3dc2cdb); // 128 byte
		write_reg32(EM_ADDR+768+128,0x3a3ef07f);
		write_reg32(EM_ADDR+768+132,0xe2dca413);
		write_reg32(EM_ADDR+768+136,0xe2546cf9);
		write_reg32(EM_ADDR+768+140,0x88c8ead8);
		write_reg32(EM_ADDR+768+144,0x9c862100);
		write_reg32(EM_ADDR+768+148,0x4ecbd86a);
		write_reg32(EM_ADDR+768+152,0x4df96a14);
		write_reg32(EM_ADDR+768+156,0x32b27ed2);
		write_reg32(EM_ADDR+768+160,0x4b14c603);
		write_reg32(EM_ADDR+768+164,0xa6b8d17f);
		write_reg32(EM_ADDR+768+168,0xac177c79);
		write_reg32(EM_ADDR+768+172,0xafad06ed);
		write_reg32(EM_ADDR+768+176,0xba7a940a);
		write_reg32(EM_ADDR+768+180,0xd792e703);
		write_reg32(EM_ADDR+768+184,0xe8730915);
		write_reg32(EM_ADDR+768+188,0xe1ee166d);
		write_reg32(EM_ADDR+768+192,0x9d1f783f);
		write_reg32(EM_ADDR+768+196,0xf16e5209);
		write_reg32(EM_ADDR+768+200,0x712a367c);
		write_reg32(EM_ADDR+768+204,0x4464756c);
		write_reg32(EM_ADDR+768+208,0x4e431080);
		write_reg32(EM_ADDR+768+212,0x2765ec35);
		write_reg32(EM_ADDR+768+216,0x26fcb50a);
		write_reg32(EM_ADDR+768+220,0x99593f69);
		write_reg32(EM_ADDR+768+224,0xa58a6301);
		write_reg32(EM_ADDR+768+228,0xd35c68bf);
		write_reg32(EM_ADDR+768+232,0xd60bbe3c);
		write_reg32(EM_ADDR+768+236,0x57d68376);
		write_reg32(EM_ADDR+768+240,0xdd3d4a05);
		write_reg32(EM_ADDR+768+244,0xebc97381);
		write_reg32(EM_ADDR+768+248,0xf439848a);
		write_reg32(EM_ADDR+768+252,0xf0f70b36);
		write_reg32(EM_ADDR+768+256,0xce8fbc1f);
		write_reg32(EM_ADDR+768+260,0x78b72904);
		write_reg32(EM_ADDR+768+264,0x38951b3e);
		write_reg32(EM_ADDR+768+268,0x22323ab6);
		write_reg32(EM_ADDR+768+272,0xa7218840);
		write_reg32(EM_ADDR+768+276,0x13b2f61a);
		write_reg32(EM_ADDR+768+280,0x937e5a85);
		write_reg32(EM_ADDR+768+284,0xccac9fb4);
		write_reg32(EM_ADDR+768+288,0xd2c53180);
		write_reg32(EM_ADDR+768+292,0x69ae345f);
		write_reg32(EM_ADDR+768+296,0x6b05df1e);
		write_reg32(EM_ADDR+768+300,0xabeb41bb);
		write_reg32(EM_ADDR+768+304,0xee9ea502);
		write_reg32(EM_ADDR+768+308,0x75e4b9c0);
		write_reg32(EM_ADDR+768+312,0x7a1cc245);
		write_reg32(EM_ADDR+768+316,0xf87b859b);
		write_reg32(EM_ADDR+768+320,0x6747de0f);
		write_reg32(EM_ADDR+768+324,0x3c5b9482);
		write_reg32(EM_ADDR+768+328,0x1c4a8d9f);
		write_reg32(EM_ADDR+768+332,0x11191d5b);
		write_reg32(EM_ADDR+768+336,0x5390c420);
		write_reg32(EM_ADDR+768+340,0x89d97b0d);
		write_reg32(EM_ADDR+768+344,0x49bf2d42);
		write_reg32(EM_ADDR+768+348,0x66564fda);
		write_reg32(EM_ADDR+768+352,0xe96298c0);
		write_reg32(EM_ADDR+768+356,0x34d71a2f);
		write_reg32(EM_ADDR+768+360,0xb582ef8f);
		write_reg32(EM_ADDR+768+364,0x55f5a0dd);
		write_reg32(EM_ADDR+768+368,0x774f5281);
		write_reg32(EM_ADDR+768+372,0xbaf25ce0);
		write_reg32(EM_ADDR+768+376,0xbd0e6122);
		write_reg32(EM_ADDR+768+380,0xfc3dc2cd);
		write_reg32(EM_ADDR+768+384,0x33a3ef07);
		write_reg32(EM_ADDR+768+388,0x9e2dca41);
		write_reg32(EM_ADDR+768+392,0x8e2546cf);
		write_reg32(EM_ADDR+768+396,0x088c8ead);
		write_reg32(EM_ADDR+768+400,0xa9c86210);
		write_reg32(EM_ADDR+768+404,0x44ecbd86);
		write_reg32(EM_ADDR+768+408,0x24df96a1);
		write_reg32(EM_ADDR+768+412,0x332b27ed);
		write_reg32(EM_ADDR+768+416,0xf4b14c60);
		write_reg32(EM_ADDR+768+420,0x9a6b8d17);
		write_reg32(EM_ADDR+768+424,0xdac177c7);
		write_reg32(EM_ADDR+768+428,0xaafad06e);
		write_reg32(EM_ADDR+768+432,0x3ba7a940);
		write_reg32(EM_ADDR+768+436,0x5d792e70);
		write_reg32(EM_ADDR+768+440,0xde873091);
		write_reg32(EM_ADDR+768+444,0xfe1ee166);
		write_reg32(EM_ADDR+768+448,0x99d1f783);
		write_reg32(EM_ADDR+768+452,0xcf16e520);
		write_reg32(EM_ADDR+768+456,0xc712a367);
		write_reg32(EM_ADDR+768+460,0x04464756);
		write_reg32(EM_ADDR+768+464,0x54e43108);
		write_reg32(EM_ADDR+768+468,0xa2765ec3);
		write_reg32(EM_ADDR+768+472,0x926fcb50);
		write_reg32(EM_ADDR+768+476,0x199593f6);
		write_reg32(EM_ADDR+768+480,0xfa58a630);
		write_reg32(EM_ADDR+768+484,0xcd35c68b);
		write_reg32(EM_ADDR+768+488,0x6d60bbe3);
		write_reg32(EM_ADDR+768+492,0x557d6837);
		write_reg32(EM_ADDR+768+496,0x1dd3d4a0);
		write_reg32(EM_ADDR+768+500,0xaebc9738);
		write_reg32(EM_ADDR+768+504,0x6f439848);
		write_reg32(EM_ADDR+768+508,0xff0f70b3);
		write_reg32(EM_ADDR+768+512,0x4ce8fbc1);
		write_reg32(EM_ADDR+768+516,0xe78b7290);
		write_reg32(EM_ADDR+768+520,0x638951b3);
		write_reg32(EM_ADDR+768+524,0x022323ab);
		write_reg32(EM_ADDR+768+528,0xaa721884);
		write_reg32(EM_ADDR+768+532,0x513b2f61);
		write_reg32(EM_ADDR+768+536,0x4937e5a8);
		write_reg32(EM_ADDR+768+540,0x0ccac9fb);
		write_reg32(EM_ADDR+768+544,0xfd2c5318);
		write_reg32(EM_ADDR+768+548,0xe69ae345);
		write_reg32(EM_ADDR+768+552,0xb6b05df1);
		write_reg32(EM_ADDR+768+556,0x2abeb41b);
		write_reg32(EM_ADDR+768+560,0x0ee9ea50);
		write_reg32(EM_ADDR+768+564,0x575e4b9c);
		write_reg32(EM_ADDR+768+568,0xb7a1cc24);
		write_reg32(EM_ADDR+768+572,0xff87b859);
		write_reg32(EM_ADDR+768+576,0x26747de0);
		write_reg32(EM_ADDR+768+580,0xf3c5b948);
		write_reg32(EM_ADDR+768+584,0xb1c4a8d9);
		write_reg32(EM_ADDR+768+588,0x011191d5);
		write_reg32(EM_ADDR+768+592,0xd5390c42);
		write_reg32(EM_ADDR+768+596,0x289d97b0);
		write_reg32(EM_ADDR+768+600,0xa49bf2d4);
		write_reg32(EM_ADDR+768+604,0x066564fd);
		write_reg32(EM_ADDR+768+608,0xfe96298c);
		write_reg32(EM_ADDR+768+612,0xf34d71a2);
		write_reg32(EM_ADDR+768+616,0xdb582ef8);
		write_reg32(EM_ADDR+768+620,0x155f5a0d);
		write_reg32(EM_ADDR+768+624,0x0774f528);
		write_reg32(EM_ADDR+768+628,0x2baf25ce);
		write_reg32(EM_ADDR+768+632,0xdbd0e612);
		write_reg32(EM_ADDR+768+636,0x7fc3dc2c);
		write_reg32(EM_ADDR+768+640,0x133a3ef0);
		write_reg32(EM_ADDR+768+644,0xf9e2dca4);
		write_reg32(EM_ADDR+768+648,0xd8e2546c);
		write_reg32(EM_ADDR+768+652,0x0088c8ea);
		write_reg32(EM_ADDR+768+656,0x6a9c8621);
		write_reg32(EM_ADDR+768+660,0x144ecbd8);
		write_reg32(EM_ADDR+768+664,0xd24df96a);
		write_reg32(EM_ADDR+768+668,0x0332b27e);
		write_reg32(EM_ADDR+768+672,0x7f4b14c6);
		write_reg32(EM_ADDR+768+676,0x79a6b8d1);
		write_reg32(EM_ADDR+768+680,0xedac177c);
		write_reg32(EM_ADDR+768+684,0x0aafad06);
		write_reg32(EM_ADDR+768+688,0x03ba7a94);
		write_reg32(EM_ADDR+768+692,0x15d792e7);
		write_reg32(EM_ADDR+768+696,0x6de87309);
		write_reg32(EM_ADDR+768+700,0x3fe1ee16);
		write_reg32(EM_ADDR+768+704,0x099d1f78);
		write_reg32(EM_ADDR+768+708,0x7cf16e52);
		write_reg32(EM_ADDR+768+712,0x6c712a36);
		write_reg32(EM_ADDR+768+716,0x80446475);
		write_reg32(EM_ADDR+768+720,0x354e4310);
		write_reg32(EM_ADDR+768+724,0x0a2765ec);
		write_reg32(EM_ADDR+768+728,0x6926fcb5);
		write_reg32(EM_ADDR+768+732,0x0199593f);
		write_reg32(EM_ADDR+768+736,0xbfa58a63);
		write_reg32(EM_ADDR+768+740,0x3cd35c68);
		write_reg32(EM_ADDR+768+744,0x76d60bbe);
		write_reg32(EM_ADDR+768+748,0x0557d683);
		write_reg32(EM_ADDR+768+752,0x81dd3d4a);
		write_reg32(EM_ADDR+768+756,0x8aebc973);
		write_reg32(EM_ADDR+768+760,0x36f43984);
		write_reg32(EM_ADDR+768+764,0x1ff0f70b);
		write_reg32(EM_ADDR+768+768,0x04ce8fbc);
		write_reg32(EM_ADDR+768+772,0x3e78b729);
		write_reg32(EM_ADDR+768+776,0xb638951b);
		write_reg32(EM_ADDR+768+780,0x4022323a);
		write_reg32(EM_ADDR+768+784,0x1aa72188);
		write_reg32(EM_ADDR+768+788,0x8513b2f6);
		write_reg32(EM_ADDR+768+792,0xb4937e5a);
		write_reg32(EM_ADDR+768+796,0x80ccac9f);
		write_reg32(EM_ADDR+768+800,0x5fd2c531);
		write_reg32(EM_ADDR+768+804,0x1e69ae34);
		write_reg32(EM_ADDR+768+808,0xbb6b05df);
		write_reg32(EM_ADDR+768+812,0x02abeb41);
		write_reg32(EM_ADDR+768+816,0xc0ee9ea5);
		write_reg32(EM_ADDR+768+820,0x4575e4b9);
		write_reg32(EM_ADDR+768+824,0x9b7a1cc2);
		write_reg32(EM_ADDR+768+828,0x0ff87b85);
		write_reg32(EM_ADDR+768+832,0x826747de);
		write_reg32(EM_ADDR+768+836,0x9f3c5b94);
		write_reg32(EM_ADDR+768+840,0x5b1c4a8d);
		write_reg32(EM_ADDR+768+844,0x2011191d);
		write_reg32(EM_ADDR+768+848,0x0d5390c4);
		write_reg32(EM_ADDR+768+852,0x4289d97b);
		write_reg32(EM_ADDR+768+856,0xda49bf2d);
		write_reg32(EM_ADDR+768+860,0xc066564f);
		write_reg32(EM_ADDR+768+864,0x2fe96298);
		write_reg32(EM_ADDR+768+868,0x8f34d71a);
		write_reg32(EM_ADDR+768+872,0xddb582ef);
		write_reg32(EM_ADDR+768+876,0x8155f5a0);
		write_reg32(EM_ADDR+768+880,0xe0774f52);
		write_reg32(EM_ADDR+768+884,0x22baf25c);
		write_reg32(EM_ADDR+768+888,0xcdbd0e61);
		write_reg32(EM_ADDR+768+892,0x07fc3dc2);
		write_reg32(EM_ADDR+768+896,0x4133a3ef);
		write_reg32(EM_ADDR+768+900,0xcf9e2dca);
		write_reg32(EM_ADDR+768+904,0xad8e2546);
		write_reg32(EM_ADDR+768+908,0x10088c8e);
		write_reg32(EM_ADDR+768+912,0x86a9c862);
		write_reg32(EM_ADDR+768+916,0xa144ecbd);
		write_reg32(EM_ADDR+768+920,0xed24df96);
		write_reg32(EM_ADDR+768+924,0x60332b27);
		write_reg32(EM_ADDR+768+928,0x17f4b14c);
		write_reg32(EM_ADDR+768+932,0xc79a6b8d);
		write_reg32(EM_ADDR+768+936,0x6edac177);
		write_reg32(EM_ADDR+768+940,0x40aafad0);
		write_reg32(EM_ADDR+768+944,0x703ba7a9);
		write_reg32(EM_ADDR+768+948,0x915d792e);
		write_reg32(EM_ADDR+768+952,0x66de8730);
		write_reg32(EM_ADDR+768+956,0x83fe1ee1);
		write_reg32(EM_ADDR+768+960,0x2099d1f7);
		write_reg32(EM_ADDR+768+964,0x67cf16e5);
		write_reg32(EM_ADDR+768+968,0x56c712a3);
		write_reg32(EM_ADDR+768+972,0x08044647);
		write_reg32(EM_ADDR+768+976,0xc354e431);
		write_reg32(EM_ADDR+768+980,0x50a2765e);
		write_reg32(EM_ADDR+768+984,0xf6926fcb);
		write_reg32(EM_ADDR+768+988,0x30199593);
		write_reg32(EM_ADDR+768+992,0x8bfa58a6);
		write_reg32(EM_ADDR+768+996,0xe3cd35c6);
		write_reg32(EM_ADDR+768+1000,0x376d60bb);
		write_reg32(EM_ADDR+768+1004,0xa0557d68);
		write_reg32(EM_ADDR+768+1008,0x381dd3d4);
		write_reg32(EM_ADDR+768+1012,0x48aebc97);
		write_reg32(EM_ADDR+768+1016,0xb36f4398);
		write_reg32(EM_ADDR+768+1020,0xc1ff0f70);
		break;
	case 1:  // 0x55
		for(int i = 0; i < 256; i++)
		{
			write_reg32(EM_ADDR+0x300+i*4, 0x55555555);
		}
		break;
	case 2:  // 0x0f
		for(int i = 0; i < 256; i++)
		{
			write_reg32(EM_ADDR+0x300+i*4, 0x0f0f0f0f);
		}
		break;
	case 3:
		for(int i = 0; i < 256; i++)
		{
			write_reg32(EM_ADDR+0x300+i*4, 0x00000000);
		}
		break;
	default:
		for(int i = 0; i < 256; i++)
		{
			write_reg32(EM_ADDR+0x300+i*4, 0xffffffff);
		}
		break;
	}
}


void br_tx_init(u32 rawstep)
{

	//init EM radio table
	write_reg32(EM_ADDR+0x100,0);
	write_reg32(EM_ADDR+0x104,0);
	write_reg32(EM_ADDR+0x108,0);
	write_reg32(EM_ADDR+0x10c,0);
	write_reg32(EM_ADDR+0x110,0);
	write_reg32(EM_ADDR+0x114,0);
	write_reg32(EM_ADDR+0x118,0);
	write_reg32(EM_ADDR+0x11c,0);
	write_reg32(EM_ADDR+0x120,0);
	write_reg32(EM_ADDR+0x124,0);
	write_reg32(EM_ADDR+0x128,0);
	write_reg32(EM_ADDR+0x12c,0);
	write_reg32(EM_ADDR+0x130,0);
	write_reg32(EM_ADDR+0x134,0);
	write_reg32(EM_ADDR+0x138,0);
	write_reg32(EM_ADDR+0x13c,0);
	write_reg32(EM_ADDR+0x140,0);
	write_reg32(EM_ADDR+0x144,0);
	write_reg32(EM_ADDR+0x148,0);
	write_reg32(EM_ADDR+0x14c,0);

	//Extrc RF setting
	write_reg32(DM_ADDR+0x70, 0);//RADIOCNTL00	0x70
	//set RFSEL to 0x2
	write_reg32(DM_ADDR+0x74, 0x4020);//RADIOCNTL10	0x74

	//set sync Error to 0x7
	write_reg32(BT_ADDR+0x78, 0x4070100);

	//set tx/rx rate <0/1/2> to <0/1/2> value
	write_reg32(BT_ADDR+0x7c, 0x39003900);//RADIOCNTL30	0x7c

#if !DEBUG_JUNWEI
	//set tx power on 112us, rxpower on 85us
	write_reg32(BT_ADDR+0x8c, 0x510755);

	//set rx path delay to 0x2
	write_reg32(BT_ADDR+0x90, 0xd04);
#endif

#if DEBUG_JUNWEI
	write_reg32(BT_ADDR+0x28, 0x1a000612);	// guard time 5us -> 6us ,increased by xuqiang
	sub_wr(0x80140c84, 0x4a, 6, 0);	   		// modem br data flow delay 10 cycle *  1/24M (24M:fixed clock for modem)GUARD_ENABLE_0	0x84
#else
		//EDR control register
	write_reg32(BT_ADDR+0x28, 0x1a000512);
#endif
	write_reg32(BT_ADDR+0x28, 0x1a000512);

	//BT_PREFETCHABORT_TIME_US 200us
	//BT_PREFETCH_TIME_US 160us
	write_reg32(BT_ADDR+0xe0, 0x1900140);//TIMGENCNTL0	0xe0
	//Encryption working space @ [0x160 - 0x16f]
	write_reg32(E0_ADDR, 0);
	write_reg32(E0_ADDR+0x4, 0);
	write_reg32(E0_ADDR+0x8, 0);
	write_reg32(E0_ADDR+0xc, 0);
//	write_reg32(BT_ADDR+0xc4, 0x160);
	//control structure @ 0x0200
	sub_wr_word(CS_ADDR,0x2,4,0); //format-> master:2
	sub_wr_word(CS_ADDR,0,7,7);//SAM_EN
	sub_wr_word(CS_ADDR,0,8,8); //DNABORT
	sub_wr_word(CS_ADDR,0,9,9); //RXBSY_EN
	sub_wr_word(CS_ADDR,0,10,10); //TXBSY_EN
	sub_wr_word(CS_ADDR,0,31,16); //CLKOFF[15:0]

	sub_wr_word(CS_ADDR+0x4,0,11,0); //CLKOFF[27:16]
	sub_wr_word(CS_ADDR+0x4,0,20,16); //linklb
	sub_wr_word(CS_ADDR+0x4,1,26,24); //aclltaddr
//	sub_wr_word(CS_ADDR+0x4,0,27,27); //acledr->br:0/edr:1
	sub_wr_word(CS_ADDR+0x4,1,28,28); //whdsb
	sub_wr_word(CS_ADDR+0x4,0,29,29); //laap
	sub_wr_word(CS_ADDR+0x4,0,30,30); //afhena
	sub_wr_word(CS_ADDR+0x4,0,31,31); //aknena

	sub_wr_word(CS_ADDR+0x8,0x8674af00,31,0); //bdaddr[31:0]
	sub_wr_word(CS_ADDR+0xc,0x0576,15,0); //bdaddr[47:32]
	sub_wr_word(CS_ADDR+0xc,0x9339,31,16); //bch[15:0]
	sub_wr_word(CS_ADDR+0x10,0x00168,17,0); //bch[33:16]

	sub_wr_word(CS_ADDR+0x10,0x3FF,31,22); //rxmaxbuf[9:0]
	sub_wr_word(CS_ADDR+0x14,0x07,7,0); //txpwr[7:0]
	sub_wr_word(CS_ADDR+0x14,0x00,14,8); //freq[6:0]
	sub_wr_word(CS_ADDR+0x14,0x1,15,15); //fh_en
	sub_wr_word(CS_ADDR+0x14,0x0,17,16); //txcrypt[1:0]
	sub_wr_word(CS_ADDR+0x14,0x0,18,18);//txbcry
	sub_wr_word(CS_ADDR+0x14,0x0,19,19); //txeir
	sub_wr_word(CS_ADDR+0x14,0x0,22,22); //nullflt
	sub_wr_word(CS_ADDR+0x14,0x0,23,23); //stopmod
	sub_wr_word(CS_ADDR+0x14,0x0,25,24); //rxcrypt
	sub_wr_word(CS_ADDR+0x14,0x0,26,26); //rxbcry
	sub_wr_word(CS_ADDR+0x14,0x0,27,27); //rxeir
	sub_wr_word(CS_ADDR+0x14,0x0,31,29); //rxthr

	sub_wr_word(CS_ADDR+0x18,0x001a,12,0); //rxwinsz[12:0]
	sub_wr_word(CS_ADDR+0x18,0x0,15,15); //rxwide
	sub_wr_word(CS_ADDR+0x18,0x2a0,30,16);//txdescptr

	sub_wr_word(CS_ADDR+0x1c,0xffffffff,31,0); //chmap[31:0]

	sub_wr_word(CS_ADDR+0x20,0xffffffff,31,0); //chmap[63:32]

	sub_wr_word(CS_ADDR+0x24,0x7fff,14,0); //chmap[78:64]
	sub_wr_word(CS_ADDR+0x24,0x01,23,16); //att_nb[7:0]
	sub_wr_word(CS_ADDR+0x24,0x00,31,24); //to_nb[7:0]

	sub_wr_word(CS_ADDR+0x28,0x1fff,12,0); //maxfrmtime[12:0]
	sub_wr_word(CS_ADDR+0x28,0x0000,31,16); //sam_ptr[15:0]

	sub_wr_word(CS_ADDR+0x2c,0x00,7,0); //sam_offset[7:0]
	sub_wr_word(CS_ADDR+0x2c,0x00,15,8);//sam_length[7:0]
	sub_wr_word(CS_ADDR+0x2c,0x0000,31,16); //sk[15:0]

	sub_wr_word(CS_ADDR+0x30,0,31,0);//sk[47:16]

	sub_wr_word(CS_ADDR+0x34,0,31,0); //sk[79:48]

	sub_wr_word(CS_ADDR+0x38,0,31,0); //sk[111:80]

	sub_wr_word(CS_ADDR+0x3c,0,15,0); //sk[127:112]
	sub_wr_word(CS_ADDR+0x3c,0,31,16); //lv[15:0]

	sub_wr_word(CS_ADDR+0x40,0,31,0); //lv[47:16]

	sub_wr_word(CS_ADDR+0x44,0,15,0) ;//lv[63:48]
	sub_wr_word(CS_ADDR+0x44,0,31,16) ;//txccmpldcnt[15:0]

	sub_wr_word(CS_ADDR+0x48,0,19,0); //txccmpldcnt[35:16]

	sub_wr_word(CS_ADDR+0x4c,0,31,0); //rxccmpldcnt[31:0]

	sub_wr_word(CS_ADDR+0x50,0,3,0); //rxccmpldcnt[35:32]

	sub_wr_word(CS_ADDR+0x5c,0,0,0); //escolastseqn
	sub_wr_word(CS_ADDR+0x5c,0,1,1); //escolastack
	sub_wr_word(CS_ADDR+0x5c,0,2,2); //escowaitack
	sub_wr_word(CS_ADDR+0x5c,0,8,8); //escoretx
	sub_wr_word(CS_ADDR+0x5c,0,9,9); //escorsvd
	sub_wr_word(CS_ADDR+0x5c,0,16,16); //lasttxseqn
	sub_wr_word(CS_ADDR+0x5c,0,17,17); //lasttxack
	sub_wr_word(CS_ADDR+0x5c,0,23,23); //last_null
	sub_wr_word(CS_ADDR+0x5c,0,26,26); //waitack
	sub_wr_word(CS_ADDR+0x5c,0,27,27); //rswitch
	sub_wr_word(CS_ADDR+0x5c,0,28,28); //fnack
	sub_wr_word(CS_ADDR+0x5c,0,29,29); //fcrc
	sub_wr_word(CS_ADDR+0x5c,0,30,30); //txflush
	sub_wr_word(CS_ADDR+0x5c,0,31,31); //txtog


	sub_wr_word(CS_ADDR+0x60,0,0,0); //lastrxseqn
	sub_wr_word(CS_ADDR+0x60,0,1,1); //lastrxack
	sub_wr_word(CS_ADDR+0x60,1,2,2); //lastrxflow

	//Tx descriptor 0 for Control structure
	sub_wr_word(TD0_ADDR,0x2a0,14,0); //nextptr[14:0]
	sub_wr_word(TD0_ADDR,0,25,25); //txseqn
	sub_wr_word(TD0_ADDR,0,24,24); //txarqn
	sub_wr_word(TD0_ADDR,0,23,23);//txflow
//	sub_wr_word(TD0_ADDR,0x4,22,19); //txtype[3:0]
	sub_wr_word(TD0_ADDR,0x1,18,16); //txltaddr[2:0]

	sub_wr_word(TD0_ADDR+0x4,0,15,13); //txundef
	sub_wr_word(TD0_ADDR+0x4,0x0a,12,3); //txlength[9:0]
	sub_wr_word(TD0_ADDR+0x4,0,2,2); //txpflow
	sub_wr_word(TD0_ADDR+0x4,0,1,0); //txllid[1:0]
	sub_wr_word(TD0_ADDR+0x4,0x0300,31,16); //txaclbufptr

	sub_wr_word(TD0_ADDR+0x8,0x0700,15,0); //txlmbufptr

//////////////////////change by lixiang ////////////////////////////////////
	//Tx buffer 0 for control structure
	write_reg32(EM_ADDR+0x300,0xffffffff);
	write_reg32(EM_ADDR+0x304,0xffffffff);
	write_reg32(EM_ADDR+0x308,0xab2d5610);
//////////////////////////////////////////////////////////////////////////
	//Tx LM buffer 0 for control structure
	write_reg32(EM_ADDR+0x700,0xcd000023);
	write_reg32(EM_ADDR+0x704,0xab690201);
	write_reg32(EM_ADDR+0x708,0x891004ef);

	//Rx descriptor 0 for control structure
	sub_wr_word(RD0_ADDR,0x000,14,0); //nextptr[14:0]//error

	sub_wr_word(RD0_ADDR+0x8,0x0500,31,16); //rxaclbufptr[15:0]

	sub_wr_word(RD0_ADDR+0xc,0x0900,15,0); //rxlmbufptr[15:0]

	//rx buffer0 for control structure
	for(int i=0;i< 0x100;i++)
	{
		write_reg32(RXBUF0_ADDR+i*4,0);
	}

	//rx buffer1 for control structure
	for(int j=0;j< 0x100;j++)
	{
		write_reg32(RXBUF1_ADDR+j*4,0);
	}

	//configure currentrxdescptr
	write_reg32(BT_ADDR+0x2c,0x2c0);

//	write_reg32(BT_ADDR+0xD0,0x8000);//infinite TX
//	write_reg32(BT_ADDR+0xD0,0x0000);//infinite TX
	//enable rw-bt core
	write_reg32(BT_ADDR+0x00,0xff107);
//	write_reg32(BT_ADDR+0x00,0xdf107); ////////////////change by lixiang RWBTCNTL0	0x00
//	sub_wr_word(CS_ADDR+0x14,0x0,25,24); //rxcrypt
//	sub_wr_word(CS_ADDR+0x14,0x2,17,16); //txcrypt[1:0]
	//enable end and slot and start irqs
	write_reg32(BT_ADDR+0xc,0x31);
	write_reg32(DM_ADDR+0xc,0x1);
	//ET0
	sub_wr_word(ET_ADDR  + ET0,0x1,2,0); //mode[2:0]
	sub_wr_word(ET_ADDR  + ET0,0x0,5,3); //status[2:0]
	sub_wr_word(ET_ADDR  + ET0,0,6,6); //esco
	sub_wr_word(ET_ADDR  + ET0,0,7,7); //rsvd
	sub_wr_word(ET_ADDR  + ET0,0,8,8); //sniff
	sub_wr_word(ET_ADDR  + ET0,0,9,9); //csb
	sub_wr_word(ET_ADDR  + ET0,0,10,10); //spa
	sub_wr_word(ET_ADDR  + ET0,0,15,11); //sch_prio1[4:0]

	sub_wr_word(ET_ADDR  + ET0,(rawstep&0xffff),31,16); //rawstp[15:0]
	sub_wr_word(ET_ADDR  + ET0+0x4,((rawstep>>16)&0xfff),11,0); //rawstp[27:16]
	sub_wr_word(ET_ADDR  + ET0+0x4,0x270,25,16);//finestp[9:0]

	sub_wr_word(ET_ADDR  + ET0+0x8,((CS_ADDR-ET_ADDR)>>1),14,0); //csptr[15:1]
	sub_wr_word(ET_ADDR  + ET0+0x8,0,30,16); //priobw[14:0]
	sub_wr_word(ET_ADDR  + ET0+0x8,0,31,31); //priobw_unit

	sub_wr_word(ET_ADDR  + ET0+0xc,0,4,0); //sch_prio2[4:0]
	sub_wr_word(ET_ADDR  + ET0+0xc,0,12,8); //sch_prio3[4:0]
	sub_wr_word(ET_ADDR  + ET0+0xc,0,28,24); //pti_prio[4:0]
	sub_wr_word(ET_ADDR  + ET0+0xc,0x3,17,16); //vxchan[1:0]
}

extern sys_clk_t sys_clk;
void BT_Clock_Init(ZB_mst_clock_e clk)
{
	unsigned char div = 0;
	if((sys_clk.hclk % 32 == 0) && (clk == ZB_MST_CLOCK_32M))
	{
		div = sys_clk.hclk / 32;
	}
	else if((sys_clk.hclk % 24 == 0) && (clk == ZB_MST_CLOCK_24M))
	{
		div = sys_clk.hclk / 24;
	}
	else if((sys_clk.hclk % 16 == 0) && (clk == ZB_MST_CLOCK_16M))
	{
		div = sys_clk.hclk / 16;
	}
	else
	{
		if(sys_clk.hclk % 8)
			return;
		div = sys_clk.hclk / 8;
		clk = ZB_MST_CLOCK_8M;
	}
	write_reg8(0x1401d2,(div-1)); // hclk -> br master hgclk
	write_reg8(0x1401d3,clk); // 1 tick -> 1us
}


/**
 * @brief      This function writes a byte data to a specified digital register
 * @param[in]  addr - the address of the digital register needs to write
 * @param[in]  value  - the data will be written to the digital register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
static void sub_wr(unsigned int addr, unsigned char value, unsigned char e, unsigned char s)
{
	unsigned char v, mask, tmp1, target, tmp2;

	v = read_reg8(addr);
	mask = BIT_MASK_LEN(e - s + 1);
	tmp1 = value & mask;

	tmp2 = v & (~BIT_RNG(s,e));

	target = (tmp1 << s) | tmp2;
	write_reg8(addr, target);
}
/**
 * @brief      This function writes a word data to a specified digital register
 * @param[in]  addr - the address of the digital register needs to write
 * @param[in]  value  - the data will be written to the digital register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
static void sub_wr_word(unsigned int addr, unsigned int value, unsigned char e, unsigned char s)
{
	unsigned int v, mask, tmp1, target, tmp2;

	v = read_reg32(addr);

	if((e==31)&&(s==0))
	{
		target = value;
	}
	else
	{
		mask = BIT_MASK_LEN(e - s + 1);
		tmp1 = value & mask;

		tmp2 = v & (~BIT_RNG(s,e));

		target = (tmp1 << s) | tmp2;
	}

	write_reg32(addr, target);
}

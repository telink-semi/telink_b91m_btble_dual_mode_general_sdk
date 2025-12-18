/*
 * bt_emi.c
 *
 *  Created on: 2023年4月4日
 *      Author: Admin
 */
#include "../BT/rf_bt.h"
#include "../../emi.h"
#include "bt_emi.h"
#include "reg_include/soc.h"

extern u32 bt_hw_get_et_next_slot(uint32_t hslot);

static u32 bt_get_clkn(u16* fcnt);
static void sub_wr_ram32(unsigned int addr, unsigned int value, unsigned char e, unsigned char s);



volatile int paylaod_length_set = 0;
unsigned int fst_rawstep=0;
#define RAW_STEP 32
//extern void bt_master_clk_enable(unsigned char en);


extern unsigned char  g_manual_setting;
extern unsigned char  g_scale;
extern unsigned char  g_slice;

unsigned short bt_pwr_list[BT_PWR_LIST_MAX_LENGTH] = {
		//scale << 8 | slice
		//BR
		63,//		EXT_BTRF_TP_POWER_INDEX_MAX_P9p33dBm
		53,//		EXT_BTRF_TP_POWER_INDEX_P8p35dBm,
		45,//		EXT_BTRF_TP_POWER_INDEX_P7p30dBm,
		40,//		EXT_BTRF_TP_POWER_INDEX_P6p46dBm,
		36,//		EXT_BTRF_TP_POWER_INDEX_P5p70dBm,
		30,//		EXT_BTRF_TP_POWER_INDEX_P4p40dBm,
		27,//		EXT_BTRF_TP_POWER_INDEX_P3p57dBm,
		25,//		EXT_BTRF_TP_POWER_INDEX_P2p95dBm,
		22,//		EXT_BTRF_TP_POWER_INDEX_P1p92dBm,
		19,//		EXT_BTRF_TP_POWER_INDEX_P0p76dBm,
		16,//		EXT_BTRF_TP_POWER_INDEX_N0p00dBm,
		14,//		EXT_BTRF_TP_POWER_INDEX_N1p38dBm,
		12,//		EXT_BTRF_TP_POWER_INDEX_N3p00dBm,
		8,//		EXT_BTRF_TP_POWER_INDEX_N6p30dBm,
		2,//		EXT_BTRF_TP_POWER_INDEX_N17p16dBm,
		0,//		EXT_BTRF_TP_POWER_INDEX_N54p03dBm,

		63<<8|63,//		RF_IQ_POWER_INDEX_P9P50_dBm,
		63<<8 | 40,//		RF_IQ_POWER_INDEX_P7P63_dBm,
		53<<8 | 39,//		RF_IQ_POWER_INDEX_P4P63_dBm,
		45<<8 | 35,//		RF_IQ_POWER_INDEX_P1P18_dBm,
		42<<8 | 31,//		RF_IQ_POWER_INDEX_P0P00_dBm,
		40<<8 | 30,//		RF_IQ_POWER_INDEX_N1P63_dBm,
		5<<8 | 22,//		EXT_BTRF_IQ_POWER_INDEX_N4P50_dBm,
		//1.8V
		//BR
		63,//		EXT_BTRF_TP_POWER_INDEX_MAX_P5p72dBm
		53,//		EXT_BTRF_TP_POWER_INDEX_P4p77dBm,
		45,//		EXT_BTRF_TP_POWER_INDEX_P3p75dBm,
		40,//		EXT_BTRF_TP_POWER_INDEX_P2p96dBm,
		36,//		EXT_BTRF_TP_POWER_INDEX_P2p39dBm,
		30,//		EXT_BTRF_TP_POWER_INDEX_P1p60dBm,
		27,//		EXT_BTRF_TP_POWER_INDEX_P1p03dBm,
		25,//		EXT_BTRF_TP_POWER_INDEX_P0p55dBm,
		22,//		EXT_BTRF_TP_POWER_INDEX_N0p20dBm,
		19,//		EXT_BTRF_TP_POWER_INDEX_N1p17dBm,
		16,//		EXT_BTRF_TP_POWER_INDEX_N2p45dBm,
		14,//		EXT_BTRF_TP_POWER_INDEX_N3p37dBm,
		12,//		EXT_BTRF_TP_POWER_INDEX_N4p55dBm,
		8,//		EXT_BTRF_TP_POWER_INDEX_N7p81dBm,
		2,//		EXT_BTRF_TP_POWER_INDEX_N19p37dBm,
		0,//		EXT_BTRF_TP_POWER_INDEX_N54p03dBm,

		63<<8|63,//		RF_IQ_POWER_INDEX_P7P87_dBm,
		63<<8 | 40,//		RF_IQ_POWER_INDEX_P5P89_dBm,
		53<<8 | 39,//		RF_IQ_POWER_INDEX_P3P08_dBm,
		45<<8 | 35,//		RF_IQ_POWER_INDEX_N0P91_dBm,
		42<<8 | 31,//		RF_IQ_POWER_INDEX_N3P31_dBm,
		40<<8 | 30,//		RF_IQ_POWER_INDEX_N4P8_dBm,
		5<<8 | 22,//		EXT_BTRF_IQ_POWER_INDEX_N7P84_dBm,
};


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

	tmp2 = v & (~ BIT_RNG(s,e));

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
void bt_rf_set_chn(signed char chn);

void rf_bt_rx_init(unsigned char _mode, signed short _chn)
{
	unsigned char filter = 0x30;
	bt_rf_set_chn(_chn);
	//set bd address
	sub_wr_ram32(CS_ADDR+0x8,0x8674af00,31,0); //bdaddr[31:0]  00  af  74  86  76  05
	sub_wr_ram32(CS_ADDR+0xc,0x0576,15,0); //bdaddr[47:32]
	sub_wr_ram32(CS_ADDR+0xc,0x9339,31,16); //bch[15:0]
	sub_wr_ram32(CS_ADDR+0x10,0x00168,17,0); //bch[33:16]
	sub_wr_ram32(CS_ADDR+0x4,_mode,27,27); //br:0, edr:1

	if(_mode == 1)	// add by pengcheng, suggest by xuqiang, 20201103
	{
		write_reg8(MODEMADDR+0xa0,0x1e); //br sync threshold [31:0]
	}
	write_reg8(0x140e7a,filter);  // filt
	br_rx_init(RAW_STEP,4096);

//	//////start rx////////////
//	// reset clkn
//	write_reg32(DM_ADDR,0x40000000); //reset master_clock domain
//	// clear status
//	sub_wr_ram32(ET_ADDR + ET0,0x0,5,3); //status[2:0]
//	// clear rx_done
//	sub_wr_ram32(RD0_ADDR,0x0,15,15);
//	// start ET0
//	write_reg32(BT_ADDR+0x100,0x80000000);
}

volatile unsigned char BTMode,Pkt_Type, scale,slice;
signed short chnidex;
void rf_bt_tx_init(unsigned char _btMode, unsigned char _pktType, signed short _chn, unsigned char _scale, unsigned char _slice, unsigned char _dataType)
{
    chnidex = _chn;
    unsigned char PowerMode = 0;  // Vbat/Vant
    BTMode = _btMode;
    Pkt_Type = _pktType;
    unsigned char PktLoad = _dataType;    // prbs9/0xaa/0xf0
#if !SYNC_20210329
    unsigned char tp_mode = 1;
#endif
    scale = _scale;
    slice = _slice;
	unsigned char tx_pa_pwr_l = 0;
	unsigned char tx_pa_pwr_h = 0;

	unsigned int PktLength = BT_Mode_PKT_Init(BTMode,Pkt_Type);
	if(PowerMode == 0)  // vbat
	{
		sub_wr(RADIOADDR+0x25, 0, 6, 6);    //MODE VBAT TX
	}
	else
	{
		sub_wr(RADIOADDR+0x25, 1, 6, 6);    //MODE VANT TX
	}
//	write_reg32(BT_ADDR,0x40000000); //reset master_clock domain
	br_tx_init(RAW_STEP);//rawstep=3200
	sub_wr_ram32(TD0_ADDR+0x4,PktLength,12,3); //txlength[9:0]
	BT_Payload_Init(PktLoad);
//	write_reg8(0x170480, 0xaa); //
//	bt_radiopwrupdn_pack(BT_RADIO_RX_POWER_UP, BT_RADIO_POWER_DOWN, BT_RADIO_TX_POWER_UP);
//	bt_radiotxrxtim_pack(BT_RADIO_SYNC_OFFSET, BT_RADIO_RX_EIR_ADJ_EN, BT_RADIO_RX_PATH_DELAY, BT_RADIO_TX_PATH_DELAY(BTMode));
#if SYNC_20210329
	//write_reg8(0x140c80, 0xaa); //suggest by xuqiang, add by pengcheng 20210127, to solve problem: sync Error(preamble not found).
	emi_radiopwrupdn_pack(BT_RADIO_RX_POWER_UP, BT_RADIO_POWER_DOWN, BT_RADIO_TX_POWER_UP);
	emi_radiotxrxtim_pack(BT_RADIO_SYNC_OFFSET, BT_RADIO_RX_EIR_ADJ_EN, BT_RADIO_RX_PATH_DELAY, BT_RADIO_TX_PATH_DELAY(BTMode));
	if(BTMode == BT_MODE_BR)
	{
		sub_wr(RADIOADDR+0x26, 0x48, 7, 0); //disable iq and enable TP
		sub_wr(RADIOADDR+0x3e, 0x29, 7, 0); //modulation index h29-0.32
		emi_radiopwrupdn_txpwrupct_setf(0x6e);
//		gpio_set_level(GPIO_PD1,0);
//		gpio_set_level(GPIO_PD0,1);
	}
	else if(BTMode == BT_MODE_EDR2 || BTMode == BT_MODE_EDR3)
	{
		sub_wr(RADIOADDR+0x26, 0x4c, 7, 0); //enable iq
		sub_wr(MODEMADDR+0x91,1,5,5); // scale enable
		sub_wr(MODEMADDR+0x91,scale&0x1f,4,0);
//		gpio_set_level(GPIO_PD1,1);
//		gpio_set_level(GPIO_PD0,0);
	}
	tx_pa_pwr_l = slice & 0x01;
	tx_pa_pwr_h = (slice>>1) & 0x1f;
	sub_wr(RADIOADDR+0x26,tx_pa_pwr_l,7,7);
	sub_wr(RADIOADDR+0x27,tx_pa_pwr_h,4,0);
	bt_rf_set_chn(chnidex);
#else
	//****set tp mode***********//
	//disable Iq(0x140e26 0x48) ,modulation index h29-0.32(0x140e3e 0x29)  0x16048C 0x0051076e
	if(BTMode == BT_MODE_BR)
	{
		ext_btrf_changeRadioMode(RADIO_TX_MODE_TP);
	}
	else
	{
		ext_btrf_changeRadioMode(RADIO_TX_MODE_IQ);
	}
	sub_wr(MODEMADDR+0x91,scale&0x1f,4,0);
//	write_reg8(MODEM_TX_CTRL_0, 0xaa); //suggest by xuqiang, add by pengcheng 20210127, to solve problem: sync Error(preamble not found).
//	write_reg8(0x140c27, 0xa2);
//	write_reg32(0x16048c,0x00560c64);

	//************ btChannel***************//
	bt_rf_set_chn(chnidex);
	//************ btPower***************//
//	if(BTMode != 0)
//	{
//		sub_wr(MODEMADDR+0x91,1,5,5); // scale enable
//		sub_wr(MODEMADDR+0x91,scale&0x1f,4,0);
//	}
	tx_pa_pwr_l = slice & 0x01;
	tx_pa_pwr_h = (slice>>1) & 0x1f;
	sub_wr(RADIOADDR+0x26,tx_pa_pwr_l,7,7);
	sub_wr(RADIOADDR+0x27,tx_pa_pwr_h,4,0);
#endif
//		u32 rawstep_init = (0x0a) & MAX_HSLOT_CLOCK;
//
//		sub_wr_ram32(ET_ADDR  + ET0,(rawstep_init&0xffff),31,16); //rawstp[15:0]
//		sub_wr_ram32(ET_ADDR  + ET0+0x4,((rawstep_init>>16)&0xfff),11,0); //rawstp[27:16]
//
//		sub_wr_ram32(ET_ADDR  + ET0+0x4,624,25,16);//finestp[9:0]
//	//	write_reg32(DM_ADDR,0x40000000); //reset master_clock domain
//		// clear status
//		sub_wr_ram32(ET_ADDR + ET0,0x0,5,3); //status[2:0]
//		// clear tx_done
//		sub_wr_ram32(TD0_ADDR,0x0,15,15);
//		// start ET0
//		write_reg32(BT_ADDR+0x100,0x80000000);
}

void rf_bt_kick()
{
	//------------- bt_kick -----------------------------------//
	// reset clkn
//	write_reg32(BT_ADDR,0x40000000); //reset master_clock domain
	unsigned int rawstep = bt_hw_get_et_next_slot(0)+32;
	fst_rawstep = rawstep;
	sub_wr_ram32(ET_ADDR  + ET0,(rawstep&0xffff),31,16); //rawstp[15:0]
	sub_wr_ram32(ET_ADDR  + ET0+0x4,((rawstep>>16)&0xfff),11,0); //rawstp[27:16]
	sub_wr_ram32(ET_ADDR + ET0+0x4,0x270,25,16);//finestp[9:0]

	// clear status
	sub_wr_ram32(ET_ADDR + ET0,0x0,5,3); //status[2:0]
	// clear tx_done
	sub_wr_ram32(TD0_ADDR,0x0,15,15);
	// start ET0
	write_reg32(BT_ADDR+0x100,0x80000000);
}

void rf_bt_tx_init_conitnue(unsigned char _btMode, unsigned char _pktType, signed short _chn, unsigned char _scale, unsigned char _slice, unsigned char _dataType)
{

//	if(_btMode)
//	{
//		rf_bt_tx_init(_btMode, _pktType, _chn, 0x10, 0x30, _dataType);
//	}
//	else
//	{
		rf_bt_tx_init(_btMode, _pktType, _chn, _scale, _slice, _dataType);
//	}
	write_reg32(BT_ADDR+0xD0,0x8000);//infinite TX;
}

void bt_emi_tx_continue_mode_run(RF_MODE_BT_e mode, signed short chn, unsigned char pwr_index, BT_PKT_Data_type_e type)
{
	bt_init();
//	if(mode&3)
//	{
//	rf_bt_tx_init_conitnue(mode & 3, mode >> 2 & 0xff, chn, bt_pwr_list[pwr_index]>>8&0xff, bt_pwr_list[pwr_index]&0xff, type);
//	}else
//	{
//		rf_bt_tx_init_conitnue(mode & 3, mode >> 2 & 0xff, chn, bt_pwr_list[pwr_index]>>8&0xff, bt_pwr_list[pwr_index]&0xff, type);
//	}
	if(g_manual_setting){
		rf_bt_tx_init_conitnue(mode & 3, mode >> 2 & 0xff, chn, g_scale, g_slice, type);
	}else{
		rf_bt_tx_init_conitnue(mode & 3, mode >> 2 & 0xff, chn, bt_pwr_list[pwr_index]>>8&0xff, bt_pwr_list[pwr_index]&0xff, type);
	}
	rf_bt_kick();
}

extern unsigned char  g_hop;
void bt_emi_tx_burst_mode_init(RF_MODE_BT_e mode, signed short chn, unsigned char pwr_index, BT_PKT_Data_type_e type)
{
	bt_init();

//	if(mode&3)
//	{
		if(g_manual_setting){
			rf_bt_tx_init(mode & 3, mode >> 2 & 0xff, chn, g_scale, g_slice, type);
		}else{
//		if(g_hop)
		rf_bt_tx_init(mode & 3, mode >> 2 & 0xff, chn, bt_pwr_list[pwr_index]>>8&0xff, bt_pwr_list[pwr_index]&0xff, type);
//		rf_bt_tx_init(mode & 3, mode >> 2 & 0xff, chn, 0x10, 0x30, type);
		}
//	}
//	else
//	{
//		rf_bt_tx_init(mode & 3, mode >> 2 & 0xff, chn, bt_pwr_list[pwr_index]>>8&0xff, bt_pwr_list[pwr_index]&0xff, type);
//	}
	//cv interrupt
	write_reg32(BT_ADDR+0x0c, EMI_CLKNINTSTAT_BIT|EMI_ENDFRMINTSTAT_BIT|EMI_RXDONEINTSTAT_BIT|EMI_SKIPFRMINTSTAT_BIT|EMI_ERRORINTSTAT_BIT|EMI_STARTFRMINTSTAT_BIT);
	rf_bt_kick();

//	extern void debug_port_init(void);
//	debug_port_init();
}
volatile int BT_error = 0;
void bt_emi_tx_burst_mode_loop(unsigned int *finish_times, unsigned char nslot)
{
	unsigned int irq_stat;
	irq_stat = read_reg32(BT_ADDR+0x10);
	if(irq_stat&EMI_CLKNINTSTAT_BIT)
	{
		write_reg32(BT_ADDR+0x18, EMI_CLKNINTSTAT_BIT);//clear ip instat
	}
	if(irq_stat&EMI_ERRORINTSTAT_BIT){
		write_reg32(BT_ADDR+0x18, EMI_ERRORINTSTAT_BIT);//clear ip instat
		BT_error = read_reg32(BT_ADDR+0x60);
	}
	if(irq_stat&EMI_ENDFRMINTSTAT_BIT)
	{
		write_reg32(BT_ADDR+0x18, EMI_ENDFRMINTSTAT_BIT);//clear bt instat
		(*finish_times)++;
		// start again
		unsigned int  rawstep = bt_hw_get_et_next_slot(0);
//		unsigned int rawstep = fst_rawstep + (nslot * (*finish_times));
		sub_wr_ram32(ET_ADDR + ET0,rawstep,31,16); //rawstp[15:0]
		sub_wr_ram32(ET_ADDR + ET0+0x4,(rawstep>>16)&0xfff,11,0); //rawstp[27:16]
		// clear status
		sub_wr_ram32(ET_ADDR + ET0,0x0,5,3); //status[2:0]
		// clear tx_done
		sub_wr_ram32(TD0_ADDR,0x0,15,15);
		// start ET0
		write_reg32(BT_ADDR+0x100,0x80000000);
	}

}

unsigned char hop_chnidex = 2;

void bt_emi_tx_burst_mode_loop_hoop(unsigned int *finish_times,unsigned char nslot)
{
	unsigned int irq_stat;
	irq_stat = read_reg32(BT_ADDR+0x10);
	if(irq_stat&EMI_CLKNINTSTAT_BIT)
	{
		write_reg32(BT_ADDR+0x18, EMI_CLKNINTSTAT_BIT);//clear ip instat
	}
	if(irq_stat&EMI_ERRORINTSTAT_BIT){
		write_reg32(BT_ADDR+0x18, EMI_ERRORINTSTAT_BIT);//clear ip instat
		BT_error = read_reg32(BT_ADDR+0x60);
	}
	if(irq_stat&EMI_ENDFRMINTSTAT_BIT)
	{
		write_reg32(BT_ADDR+0x18, EMI_ENDFRMINTSTAT_BIT);//clear bt instat
		(*finish_times)++;

		// start again
		unsigned int  rawstep = bt_hw_get_et_next_slot(0);
//		unsigned int rawstep = fst_rawstep + (nslot * (*finish_times));
		sub_wr_ram32(ET_ADDR + ET0,rawstep,31,16); //rawstp[15:0]
		sub_wr_ram32(ET_ADDR + ET0+0x4,(rawstep>>16)&0xfff,11,0); //rawstp[27:16]
		// clear status
		sub_wr_ram32(ET_ADDR + ET0,0x0,5,3); //status[2:0]
		// clear tx_done
		sub_wr_ram32(TD0_ADDR,0x0,15,15);
		// start ET0
		write_reg32(BT_ADDR+0x100,0x80000000);
		if(g_hop){
		if(hop_chnidex>80)
			hop_chnidex = 2;
		bt_rf_set_chn(hop_chnidex);
		hop_chnidex++;
		}
	}

}


void bt_emi_rx_mode_init(RF_MODE_BT_e mode, signed short chn)
{
	bt_init();
	rf_bt_rx_init(((mode&0x03) == 0)?0:1, chn);
	//cv interrupt
	write_reg32(BT_ADDR+0x0c, EMI_CLKNINTSTAT_BIT|EMI_ENDFRMINTSTAT_BIT|EMI_STARTFRMINTSTAT_BIT|EMI_RXINTSTAT_BIT|EMI_ERRORINTSTAT_BIT);
	rf_bt_kick();
}

volatile unsigned int crc_err_cnt = 0;
void bt_emi_rx_mode_loop(unsigned int *finish_times)
{
	unsigned int irq_stat;

	irq_stat = read_reg32(BT_ADDR+0x10);
	if(irq_stat&EMI_CLKNINTSTAT_BIT)
	{
		write_reg32(BT_ADDR+0x18, EMI_CLKNINTSTAT_BIT);//clear ip instat

	}
	if(irq_stat&EMI_ERRORINTSTAT_BIT){
		write_reg32(BT_ADDR+0x18, EMI_ERRORINTSTAT_BIT);//clear ip instat
		BT_error = read_reg32(BT_ADDR+0x60);
	}
//	irq_stat = read_reg32(BT_ADDR+0x10);
	if(irq_stat&EMI_ENDFRMINTSTAT_BIT)
	{
//		gpio_set_level(GPIO_PE7,1);
		write_reg32(BT_ADDR+0x18, EMI_ENDFRMINTSTAT_BIT);//clear bt instat
		if(!(read_sram16(RD0_ADDR+2)&(RXCRCERR|RXHECERR|RXSYNCERR)))
		{
			(*finish_times)++;
		}
		else if(read_sram16(RD0_ADDR+2)&(RXCRCERR))
		{
			crc_err_cnt++;
		}
		unsigned short finestp = 0;
		unsigned int rawstep = bt_get_clkn(&finestp) + 1;
//		unsigned int rawstep = bt_hw_get_et_next_slot(0);
		sub_wr_ram32(ET_ADDR + ET0,rawstep,31,16); //rawstp[15:0]
		sub_wr_ram32(ET_ADDR + ET0+0x4,(rawstep>>16)&0xfff,11,0); //rawstp[27:16]
		sub_wr_ram32(ET_ADDR + ET0+0x4,finestp,25,16);//finestp[9:0]

		// clear status
		sub_wr_ram32(ET_ADDR + ET0,0x0,5,3); //status[2:0]
		// clear rx_done
		sub_wr_ram32(RD0_ADDR,0x0,15,15);
		// start ET0
		write_reg32(BT_ADDR+0x100,0x80000000);
		write_sram16(RD0_ADDR+2,0);
	}
}

void bt_emi_reset_cv()
{
	sub_wr_word(BT_ADDR, 0x01, 31, 31);
	while(read_reg32(BT_ADDR) & (unsigned int)(0x80000000));
	sub_wr_word(BT_ADDR, 0x01, 30, 30);
	while(read_reg32(BT_ADDR) & (unsigned int)(0x40000000));
	sub_wr_word(BT_ADDR, 0x01, 29, 29);
	while(read_reg32(BT_ADDR) & (unsigned int)(0x20000000));
	sub_wr_word(BT_ADDR, 0x01, 28, 28);
	while(read_reg32(BT_ADDR) & (unsigned int)(0x10000000));
}

void bt_emi_stop()
{
	bt_emi_reset_cv();
	rf_emi_reset_baseband();
	analog_write_reg8(0x7d,0x81);
	analog_write_reg8(0x7d,0x80);//reset baseband
}

void bt_emi_single_tone(signed char rf_chn, unsigned char pwr)
{
	rf_access_code_comm(0x29417671);
	if(g_manual_setting){
		rf_emi_tx_single_tone(g_slice, rf_chn);
	}else{
	rf_emi_tx_single_tone(bt_pwr_list[pwr]&0xff, rf_chn);
	}
}

void bt_emi_init()
{
	BT_Clock_Init(ZB_MST_CLOCK_8M);
}

signed char bt_get_rssi(unsigned char index)
{
	return (read_sram16(BT_RXCHASS_ADDR + index * BT_RXDESC_SIZE) & 0xff) - 110;
}


static u32 bt_get_clkn(u16* fcnt)
{
	sub_wr_word(DM_ADDR + 0x1c,1,31,31);
	while(read_reg32(DM_ADDR + 0x1c)&0x80000000);
	*fcnt = read_reg32(DM_ADDR + 0x20);
	return read_reg32(DM_ADDR + 0x1c);
}
/**
 * @brief      This function writes a word data to sram
 * @param[in]  addr - the address of the digital register needs to write
 * @param[in]  value  - the data will be written to the digital register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
static void sub_wr_ram32(unsigned int addr, unsigned int value, unsigned char e, unsigned char s)
{
	unsigned int v, mask, tmp1, target, tmp2;

	v = read_sram32(addr);

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

	write_sram32(addr, target);
}
volatile unsigned char  bt_g_single_tong_freqoffset = 0;//for eliminate single carrier frequency offset.
void bt_rf_set_chn(signed char chn)
{
	unsigned int freq_low;
	unsigned int freq_high;
	unsigned int chnl_freq;
	unsigned int freq;

	freq = 2400+chn;
	chnl_freq = freq*2 + bt_g_single_tong_freqoffset;
	freq_low  = (chnl_freq & 0x7f);
	freq_high = ((chnl_freq>>7)&0x3f);

	write_reg8(0x170644,  (read_reg8(0x170644) | 0x01 ));
	write_reg8(0x170644,  (read_reg8(0x170644) & 0x01) | freq_low << 1);
	write_reg8(0x170645,  (read_reg8(0x170645) & 0xc0) | freq_high);
	write_reg8(0x170629,  0xc0);
}

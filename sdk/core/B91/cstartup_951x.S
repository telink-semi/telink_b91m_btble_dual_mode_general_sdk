/********************************************************************************************************
 * @file	cstartup_9518.S
 *
 * @brief	This is the boot file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
 #if (MCU_CORE_TYPE == MCU_CORE_B91)

    .section .vectors, "ax"
    //.org  and linker's relaxation (-flto) cannot be used at the same time
    //Pop corresponds to push. Before using .option norelax, use push to save the current .option configuration
    //and then modify .option. After using norelax, use pop to restore
	.option push
	.option norelax
    .org 0x0



	.global synth_8_dct_asm
	.global synth_8_bfilter_asm
    .global _RESET_ENTRY
 	.type _RESET_ENTRY,@function

    .align 2
_RESET_ENTRY:
	j 		 _START
	//free the 6th ~ 7th byte to store the crc type of the bin file
#if SUPPORT_USER_CONFIG
	.org 0x4
	.word(_CONFIG_DATA_POS)
#endif

    .org 0x18
    .word (BIN_SIZE)

    .org 0x20
    .word ('T'<<24 | 'L'<<16 | 'N'<<8 | 'K')

    //.org 0x24
    //.short (0x0000)             //mutiboot address 0k
    //.short (0x1F01)           //mutiboot address 128k
    //.short (0x3F02)           //mutiboot address 256k
    //.short (0x7F04)           //mutiboot address 512k

    .org 0x26
    //.short (0x0003)           //READ:    cmd:1x, addr:1x, data:1x, dummy:0
    //.short (0x070B)           //FREAD:   cmd:1x, addr:1x, data:1x, dummy:8
    .short (0x173B)           //DREAD:   cmd:1x, addr:1x, data:2x, dummy:8
    //.short (0x53BB)           //X2READ:  cmd:1x, addr:2x, data:2x, dummy:4
    //.short (0x276B)           //QREAD:   cmd:1x, addr:1x, data:4x, dummy:8
    //.short (0x65EB)             //X4READ:  cmd:1x, addr:4x, data:4x, dummy:6
	.option pop
    .align 2

_START:

	/* Initialize global pointer */
    .option push
    .option norelax
    la     gp, __global_pointer$
    .option pop

	/* Initialize stack pointer */
    la     t0, _STACK_TOP
    mv     sp, t0

#ifdef __nds_execit
	/* Initialize EXEC.IT table */
	la t0, _ITB_BASE_
	csrw uitb, t0
#endif

#ifdef __riscv_flen
	/* Enable FPU */
	li t0, 0x00006000
	csrrs t0, mstatus, t0
	/* Initialize FCSR */
	fscsr zero
#endif

/* GPIO_DEBUG */
#if 0
	// add debug, PE1 output 1
	lui		t0,	0x80140 			//0x80140322
	li		t1, 0xfd
	li		t2, 0x02
	sb		t1 , 0x322(t0)		 //0x8014030a  Pe oen     =  0xfd
	sb		t2 , 0x323(t0)		 //0x8014030b  Pe output  =  0x02
#endif


    /* Initial machine trap-vector Base */
    la     t0, __vectors
    csrw   mtvec, t0

    /* Enable vectored external plic interrupt */
    csrsi  mmisc_ctl, 2

    /* Enable I/D-Cache */
    csrr   t0,  mcache_ctl
    ori    t0,  t0,  1  #/I-Cache
    ori    t0,  t0,  2  #/D-Cache
    csrw   mcache_ctl,  t0
    fence.i
	/* Move retention reset from flash to sram */
_RETENTION_RESET_INIT:

    la     t1, _RETENTION_RESET_LMA_START
    la     t2, _RETENTION_RESET_VMA_START
    la     t3, _RETENTION_RESET_VMA_END
_RETENTION_RESET_BEGIN:
    bleu    t3, t2, _RETENTION_DATA_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _RETENTION_RESET_BEGIN

	/* Move retention from flash to sram */
_RETENTION_DATA_INIT:

    la     t1, _RETENTION_DATA_LMA_START
    la     t2, _RETENTION_DATA_VMA_START
    la     t3, _RETENTION_DATA_VMA_END
_RETENTION_DATA_INIT_BEGIN:
    bleu   t3, t2, _RAMCODE_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _RETENTION_DATA_INIT_BEGIN


	/* Move ramcode from flash to sram */
_RAMCODE_INIT:

    la     t1, _RETENTION_CODE_LMA_START
    la     t2, _RETENTION_CODE_VMA_START
    la     t3, _RAMCODE_VMA_END
_RAMCODE_INIT_BEGIN:
    bleu    t3, t2, _DATA_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _RAMCODE_INIT_BEGIN

#if SUPPORT_USER_CONFIG

	/* Move Data from flash to sram */
_DATA_INIT:
    la     t1, _DATA_LMA_START
    la     t2, _DATA_VMA_START
    la     t3, _DATA_VMA_END
_DATA_INIT_BEGIN:
    bleu   t3, t2, _CONFIG_DATA_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _DATA_INIT_BEGIN


	/* Move Config Data from flash to sram */
_CONFIG_DATA_INIT:
    la     t1, _CONFIG_DATA_LMA_START
    la     t2, _CONFIG_DATA_VMA_START
    la     t3, _CONFIG_DATA_VMA_END
_CONFIG_DATA_INIT_BEGIN:
    bleu   t3, t2, _ZERO_BSS
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _CONFIG_DATA_INIT_BEGIN

#else

	/* Move Data from flash to sram */
_DATA_INIT:
    la     t1, _DATA_LMA_START
    la     t2, _DATA_VMA_START
    la     t3, _DATA_VMA_END
_DATA_INIT_BEGIN:
    bleu   t3, t2, _ZERO_BSS
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _DATA_INIT_BEGIN

#endif


	/* Zero .bss section in sram */
_ZERO_BSS:
    lui    t0, 0
    la     t2, _BSS_VMA_START
    la     t3, _BSS_VMA_END
_ZERO_BSS_BEGIN:
    bleu   t3, t2, _FILL_STK
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _ZERO_BSS_BEGIN

	/* Fill the remaining section in sram */
_FILL_STK:
#if 0
    lui    t0, 0x55555
    addi   t0, t0, 0x555
    la     t2, _BSS_VMA_END
    la     t3, _STACK_TOP
_FILL_STK_BEGIN:
    bleu   t3, t2, _MAIN_FUNC
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _FILL_STK_BEGIN
#endif
	/* Jump to the main function */
_MAIN_FUNC:
    nop

    la     t0, main
    jalr   t0

    nop
    nop
    nop
    nop
    nop
_END:
    j    _END




.section .retention_reset, "ax"

	.global env_clear
	.global synth_8_dct_asm
	.global synth_8_bfilter_asm
    .global _IRESET_ENTRY
 	.type _IRESET_ENTRY,@function

    .align 2
_IRESET_ENTRY:
	/* Decide whether this is an NMI or cold reset */
	j 		_ISTART

    .align 2

_ISTART:


	/* Initialize global pointer */
    .option push
    .option norelax
    la     gp, __global_pointer$
    .option pop

	/* Initialize stack pointer */
    la     t0, _STACK_TOP
    mv     sp, t0

#ifdef __nds_execit
	/* Initialize EXEC.IT table */
	la t0, _ITB_BASE_
	csrw uitb, t0
#endif

#ifdef __riscv_flen
	/* Enable FPU */
	li t0, 0x00006000
	csrrs t0, mstatus, t0
	/* Initialize FCSR */
	fscsr zero
#endif

/* GPIO_DEBUG */
#if 0
	// add debug, PE1 output 1
	lui		t0,	0x80140 			//0x80140322
	li		t1, 0xfd
	li		t2, 0x02
	sb		t1 , 0x322(t0)		 //0x8014030a  Pe oen     =  0xfd
	sb		t2 , 0x323(t0)		 //0x8014030b  Pe output  =  0x02
#endif

    /* Initial machine trap-vector Base */
    la     t0, __vectors
    csrw   mtvec, t0

    /* Enable vectored external plic interrupt */
    csrsi  mmisc_ctl, 2



    /* Enable I/D-Cache */
    csrr   t0,  mcache_ctl
    ori    t0,  t0,  1  #/I-Cache
    ori    t0,  t0,  2  #/D-Cache
    csrw   mcache_ctl,  t0
    fence.i


/* flash wakeup */
_WAKEUP_FLASH:
	lui		t0 , 0x80140
	li		t1 , 0xff
	li		t2 , 0x0
	li		t3 , 0xab
	sb		t1 , 0x329(t0)			 //mspi ie enable	:0x140329:0x1f
	sb		t2 , 0x101(t0)			 //cs_low			:0x140101:0x00
	sb		t3 , 0x100(t0)			 //wakeup_cmd		:0x140100:0xab
_MSPI_WAIT:
	lui		t0 , 0x80140
	lb		t2 , 0x102(t0)			//read reg_mspi_status FLD_MSPI_BUSY(bit0)
	li		t3 , 0x1
	li		t4 , 0x10
	beq 	t3 , t2 ,_MSPI_WAIT
	sb		t4 , 0x101(t0)			//cs_high			:0x140101:0x10





/*efuse load need delay about 18us */
	li		t0 , 0
	li		t1 , 226
_WAIT_EFUSE_LOAD_FINISH:
	addi 	t0 , t0 , 1
	bgeu 	t1 , t0 , _WAIT_EFUSE_LOAD_FINISH








_MULTI_ADDRESS_BEGIN:
	lui		t0 , 0x80140
	la		t1 , g_pm_multi_addr
	lw		t2 , 0(t1)
	sw		t2 , 0x104(t0)			 //g_pm_multi_addr->0x80140104



#if 0
	/* Move ramcode from flash to sram */
_IRAMCODE_INIT:

    la     t1, _RAMCODE_LMA_START
    la     t2, _RAMCODE_VMA_START
    la     t3, _RAMCODE_VMA_END
_IRAMCODE_INIT_BEGIN:
    bleu   t3, t2, _IDATA_INIT
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _IRAMCODE_INIT_BEGIN
#endif




	/* Move Data from flash to sram */
_IDATA_INIT:
    la     t1, _DATA_LMA_START
    la     t2, _DATA_VMA_START
    la     t3, _DATA_VMA_END
_IDATA_INIT_BEGIN:
    bleu   t3, t2, _IZERO_BSS
    lw     t0, 0(t1)
    sw     t0, 0(t2)
    addi   t1, t1, 4
    addi   t2, t2, 4
    j      _IDATA_INIT_BEGIN



	/* Zero .bss section in sram */
_IZERO_BSS:
#if 0  /* SiHui: should remove bss clear later. now some bss data not processed correctly, so need clear */
    lui    t0, 0
    la     t2, _BSS_VMA_START
    la     t3, _BSS_VMA_END
_IZERO_BSS_BEGIN:
    bleu   t3, t2, _IFILL_STK
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _IZERO_BSS_BEGIN
#endif

	/* Fill the remaining section in sram */
_IFILL_STK:
#if 0
    lui    t0, 0x55555
    addi   t0, t0, 0x555
    la     t2, _BSS_VMA_END
    la     t3, _STACK_TOP
_IFILL_STK_BEGIN:
    bleu   t3, t2, _IMAIN_FUNC
    sw     t0, 0(t2)
    addi   t2, t2, 4
    j      _IFILL_STK_BEGIN
#endif
	/* Jump to the main function */
_IMAIN_FUNC:
    nop

    la     t0, main
    jalr   t0

    nop
    nop
    nop
    nop
    nop
_IEND:
    j    _IEND



.text
	.global default_irq_entry
	.align 2

default_irq_entry:
1:	j 1b

synth_8_dct_asm:
	c.addi	sp,-16
	c.swsp	s2,4(sp)
	c.swsp	s0,12(sp)
	c.swsp	s1,8(sp)
	sw	s3,0(sp)

	lw	s1,0(a1)
	lw	s0,4(a1)
	lw	t2,8(a1)
	lw	t0,12(a1)
	lw	t6,16(a1)
	lw	t5,20(a1)
	lw	t4,24(a1)
	lw	t3,28(a1)
	addi	s2,a0,256
SYNC8_DCT_LOOP:
	lh	a5,2(a0)
	lh	s3,0(a0)
	mul	a5,a5,s0
	lh	t1,4(a0)
	maddr32	a5,s3,s1
	lh	a7,6(a0)
	maddr32	a5,t1,t2
	lh	a6,8(a0)
	maddr32	a5,a7,t0
	lh	a1,10(a0)
	maddr32	a5,a6,t6
	lh	a3,12(a0)
	maddr32	a5,a1,t5
	lh	a4,14(a0)
	c.addi	a2,2
	c.addi	a0,16
	maddr32	a5,a3,t4
	maddr32	a5,a4,t3

	li 	a6,32768			//2^(NUM_FRAC_BITS_COS-1)
	add 	a5,a5,a6

	c.srai	a5,0x10		//NUM_FRAC_BITS_COS f
	sh	a5,-2(a2)
	bne	a0,s2,SYNC8_DCT_LOOP
	c.lwsp	s0,12(sp)
	c.lwsp	s1,8(sp)
	c.lwsp	s2,4(sp)
	c.lwsp	s3,0(sp)
	c.addi	sp,16
	ret

synth_8_bfilter_asm:
	c.addi	sp,-32
	addi	t2,a1,160
	c.li	t0,0
SYNC8_BFILTER_LOOP:
	lh	a4,48(a2)
	lh	a3,2(a1)

	lh	a7,0(a2)
	mul	a5,a4,a3
	lh	a6,0(a1)

	lh	a4,64(a2)
	maddr32	a5,a7,a6
	lh	a3,4(a1)

	lh	a7,112(a2)
	maddr32	a5,a4,a3
	lh	a6,6(a1)

	lh	a4,128(a2)
	maddr32	a5,a7,a6
	lh	a3,8(a1)

	lh	a7,176(a2)
	maddr32	a5,a4,a3
	lh	a6,10(a1)

	lh	a4,192(a2)
	maddr32	a5,a7,a6
	lh	a3,12(a1)

	lh	a7,240(a2)
	maddr32	a5,a4,a3
	lh	a6,14(a1)

	lh	a4,256(a2)
	maddr32	a5,a7,a6
	lh	a3,16(a1)

	lh	a7,304(a2)
	maddr32	a5,a4,a3
	lh	a6,18(a1)

	c.addi	a0,2
	maddr32	a5,a7,a6
	c.addi	a1,20
	c.addi	a2,2

	c.srai	a5,0xd	// MY_EXTER_BIT: 1-0xb; 2-0xa; 3-0x9 + SBCDEC_EXTRA_BITS e
	ksubh	a5,t0,a5
	sh	a5,-2(a0)
	bne	a1,t2, SYNC8_BFILTER_LOOP
	c.addi16sp	sp,32
	ret


env_clear:
    /* Enable I/D-Cache */
    csrw mcause, 0
	ret

       .weak trap_handler

 trap_handler:
1:	j 1b

    .macro INTERRUPT num
	.weak entry_irq\num
	.set entry_irq\num, default_irq_entry
	.long entry_irq\num
	.endm



#define VECTOR_NUMINTRS         63

   	.section .rodata

	.global __vectors
	.balign 256


__vectors:
	/* Trap vector */
	.long trap_entry

	/* PLIC interrupt vector */
	.altmacro
	.set irqno, 1
	.rept VECTOR_NUMINTRS/*  .rept  .endr  */
	INTERRUPT %irqno
	.set irqno, irqno+1
	.endr


#endif


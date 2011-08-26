#include <config.h>

.globl s_init

#define LOW_LEVEL_SRAM_STACK 0x0003c000

#ifdef CONFIG_SPL_EARLY_DEBUG
#define CLKPWR_BASE 0x40004000
#define CLKPWR_UART_CLK_CTRL 0xe4

#define UART5_BASE 0x40090000

#define UART_RX 0
#define UART_TX 0
#define UART_DLL 0
#define UART_IER 1
#define UART_DLM 1
#define UART_FCR 2
#define UART_IIR 2
#define UART_LCR 3
#define UART_LCR_DLAB 0x80
#define UART_LSR 5
#define UART_LSR_TEMT 0x40
#define UART_LSR_THRE 0x20
#define UART_SHIFT 2

.macro addruart,rx,rd
mrc	p15, 0, \rx, c1, c0
tst	\rx, #1
ldreq	\rx, =UART5_BASE
ldrne	\rx, =0xf4090000
.endm

.macro senduart,rd,rx
	strb	\rd, [\rx, #UART_TX << UART_SHIFT]
.endm

.macro busyuart,rd,rx
1002:	ldrb	\rd, [\rx, #UART_LSR << UART_SHIFT]
	and	\rd, \rd, #UART_LSR_TEMT | UART_LSR_THRE
	teq	\rd, #UART_LSR_TEMT | UART_LSR_THRE
	bne	1002b
.endm

.macro waituart,rd,rx
.endm

setup_uart:
	ldr	r3, =CLKPWR_BASE
	ldr	r0, [r3, #CLKPWR_UART_CLK_CTRL]
	orr	r0, r0, #4
	str	r0, [r3, #CLKPWR_UART_CLK_CTRL]

	ldr	r3, =0x40054000
	ldr	r0, [r3, #4]
	and	r0, r0, #0xfffffcff
	orr	r0, r0, #0x00000200
	str	r0, [r3, #4]

	addruart r3, r1

	/* configure divider */
	ldrb	r0, [r3, #UART_LCR << UART_SHIFT]
	orr	r0, r0, #UART_LCR_DLAB
	strb	r0, [r3, #UART_LCR << UART_SHIFT]

	mov	r0, #1
	strb	r0, [r3, #UART_DLL << UART_SHIFT]
	mov	r0, #0
	strb	r0, [r3, #UART_DLM << UART_SHIFT]

	ldrb	r0, [r3, #UART_LCR << UART_SHIFT]
	and	r0, #0x7f
	strb	r0, [r3, #UART_LCR << UART_SHIFT]

	/* configure transport mode */
	mov	r0, #0x03
	strb	r0, [r3, #UART_LCR << UART_SHIFT]

	/* TODO: set UART clock divider */
	ldr	r4, =0x400040d8
	ldr	r0, =0x00001386
	str	r0, [r4, #0]

	/* clear fifos */
	mov	r0, #0x0f
	strb	r0, [r3, #UART_FCR << UART_SHIFT]

	ldrb	r0, [r3, #UART_RX << UART_SHIFT]
	ldrb	r0, [r3, #UART_IIR << UART_SHIFT]
	ldrb	r0, [r3, #UART_LSR << UART_SHIFT]

	mov	pc, lr

#define ENTRY(name) \
	.globl name; \
	.align 0; \
	name:

#define END(name) \
	.size name, .-name

#define ENDPROC(name) \
	.type name, %function; \
	END(name)

ENTRY(printhex)
	adr	r2, hexbuf
	add	r3, r2, r1
	mov	r1, #0
	strb	r1, [r3]
1:	and	r1, r0, #15
	mov	r0, r0, lsr #4
	cmp	r1, #10
	addlt	r1, r1, #'0'
	addge	r1, r1, #'a' - 10
	strb	r1, [r3, #-1]!
	teq	r3, r2
	bne	1b
	mov	r0, r2
	b	printascii
ENDPROC(printhex)

ENTRY(printascii)
	addruart r3, r1
	b	2f
1:	waituart r2, r3
	senduart r1, r3
	busyuart r2, r3
	teq	r1, #'\n'
	moveq	r1, #'\r'
	beq	1b
2:	teq	r0, #0
	ldrneb	r1, [r0], #1
	teqne	r1, #0
	bne	1b
	mov	pc, lr
ENDPROC(printascii)
#endif

.globl lowlevel_init
lowlevel_init:
#ifdef CONFIG_SPL_EARLY_DEBUG
	bl	setup_uart
	adr	r0, str_a1
	bl	printascii
#endif

	ldr	sp, =LOW_LEVEL_SRAM_STACK
	push	{ip, lr}
	bl	s_init
	pop	{ip, pc}

#ifdef CONFIG_SPL_EARLY_DEBUG
str_a1:	.asciz "Avionic Design GmbH\n"
hexbuf:	.space 16
#endif

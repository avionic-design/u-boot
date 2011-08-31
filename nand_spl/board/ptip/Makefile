CONFIG_NAND_SPL = y

include $(TOPDIR)/config.mk
#include $(TOPDIR)/nand_spl/board/$(BOARDDIR)/config.mk

nandobj := $(OBJTREE)/nand_spl/

LDSCRIPT = $(TOPDIR)/nand_spl/board/$(BOARDDIR)/u-boot.lds
LDFLAGS := -T $(nandobj)u-boot.lds -Ttext $(CONFIG_SYS_TEXT_BASE) $(LDFLAGS) $(LDFLAGS_FINAL)
AFLAGS += -DCONFIG_SPL_BUILD -DCONFIG_NAND_SPL
CFLAGS += -DCONFIG_SPL_BUILD -DCONFIG_NAND_SPL

SOBJS = start.o lowlevel_init.o _udivsi3.o _divsi3.o
COBJS = div64.o div0.o
COBJS += clkpwr.o timer.o board.o serial.o lpc32xx_nand.o lpc32xx_dma.o
COBJS += nand_boot.o nand_ecc.o

SRCS := $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS := $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS = $(SOBJS) $(COBJS)
LNDIR = $(nandobj)board/$(BOARDDIR)

ALL = $(nandobj)u-boot-spl $(nandobj)u-boot-spl.bin

all: $(obj).depend $(ALL)

$(nandobj)u-boot-spl.bin: $(nandobj)u-boot-spl
	$(OBJCOPY) $(OBJCFLAGS) --pad-to=2048 -O binary $< $@

$(nandobj)u-boot-spl: $(OBJS) $(nandobj)u-boot.lds
	cd $(LNDIR) && $(LD) $(LDFLAGS) $(__OBJS) -Map $(nandojb)u-boot-spl.map -o $@

$(nandobj)u-boot.lds: $(LDSCRIPT)
	$(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - < $< > $@

$(obj)start.S:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/cpu/arm926ejs/start.S $@

$(obj)lowlevel_init.S:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/cpu/arm926ejs/lpc3250/lowlevel_init.S $@

$(obj)_udivsi3.S:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/lib/_udivsi3.S $@

$(obj)_divsi3.S:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/lib/_divsi3.S $@

$(obj)div64.c:
	@rm -f $@ && ln -s $(TOPDIR)/lib/div64.c $@

$(obj)div0.c:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/lib/div0.c $@

$(obj)clkpwr.c:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/cpu/arm926ejs/lpc3250/clkpwr.c $@

$(obj)timer.c:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/cpu/arm926ejs/lpc3250/timer.c $@

$(obj)serial.c:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/cpu/arm926ejs/lpc3250/serial.c $@

$(obj)board.c:
	@rm -f $@ && ln -s $(TOPDIR)/arch/arm/cpu/arm926ejs/lpc3250/board.c $@

$(obj)nand_boot.c:
	@rm -f $@ && ln -s $(TOPDIR)/nand_spl/nand_boot.c $@

$(obj)nand_ecc.c:
	@rm -f $@ && ln -s $(TOPDIR)/drivers/mtd/nand/nand_ecc.c $@

$(obj)lpc32xx_nand.c:
	@rm -f $@ && ln -s $(TOPDIR)/drivers/mtd/nand/lpc32xx_nand.c $@

$(obj)lpc32xx_dma.c:
	@rm -f $@ && ln -s $(TOPDIR)/drivers/dma/lpc32xx_dma.c $@

$(obj)%.o: $(obj)%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o: $(obj)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

include $(SRCTREE)/rules.mk

sinclude $(obj).depend

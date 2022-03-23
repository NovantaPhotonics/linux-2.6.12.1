cmd_arch/arm/mm/cache-v4wt.o := arm-linux-gcc -Wp,-MD,arch/arm/mm/.cache-v4wt.o.d  -nostdinc -isystem /home/jwarren/projects/elcheapo/tools/bin/../lib/gcc-lib/arm-linux-uclibc/3.3.4/include -D__KERNEL__ -Iinclude  -mlittle-endian -D__ASSEMBLY__ -mapcs-32 -D__LINUX_ARM_ARCH__=4 -march=armv4 -mtune=arm9tdmi -msoft-float    -c -o arch/arm/mm/cache-v4wt.o arch/arm/mm/cache-v4wt.S

deps_arch/arm/mm/cache-v4wt.o := \
  arch/arm/mm/cache-v4wt.S \
  include/linux/linkage.h \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  include/asm/linkage.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/compiler.h \
  include/asm/hardware.h \
  include/asm/arch/hardware.h \
  include/asm/sizes.h \
  include/asm/arch/AT91RM9200.h \
  include/asm/arch/AT91RM9200_SYS.h \
  include/asm/page.h \
    $(wildcard include/config/cpu/copy/v3.h) \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
  arch/arm/mm/proc-macros.S \
  include/asm/constants.h \
  include/asm/thread_info.h \
  include/asm/fpstate.h \
    $(wildcard include/config/iwmmxt.h) \

arch/arm/mm/cache-v4wt.o: $(deps_arch/arm/mm/cache-v4wt.o)

$(deps_arch/arm/mm/cache-v4wt.o):

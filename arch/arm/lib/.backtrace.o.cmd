cmd_arch/arm/lib/backtrace.o := arm-linux-gcc -Wp,-MD,arch/arm/lib/.backtrace.o.d  -nostdinc -isystem /home/jwarren/projects/elcheapo/tools/bin/../lib/gcc-lib/arm-linux-uclibc/3.3.4/include -D__KERNEL__ -Iinclude  -mlittle-endian -D__ASSEMBLY__ -mapcs-32 -D__LINUX_ARM_ARCH__=4 -march=armv4 -mtune=arm9tdmi -msoft-float    -c -o arch/arm/lib/backtrace.o arch/arm/lib/backtrace.S

deps_arch/arm/lib/backtrace.o := \
  arch/arm/lib/backtrace.S \
    $(wildcard include/config/cpu.h) \
    $(wildcard include/config/frame/pointer.h) \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  include/linux/linkage.h \
  include/asm/linkage.h \
  include/asm/assembler.h \
  include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/smp.h) \

arch/arm/lib/backtrace.o: $(deps_arch/arm/lib/backtrace.o)

$(deps_arch/arm/lib/backtrace.o):

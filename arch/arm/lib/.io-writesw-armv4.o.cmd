cmd_arch/arm/lib/io-writesw-armv4.o := arm-linux-gcc -Wp,-MD,arch/arm/lib/.io-writesw-armv4.o.d  -nostdinc -isystem /home/jwarren/projects/elcheapo/tools/bin/../lib/gcc-lib/arm-linux-uclibc/3.3.4/include -D__KERNEL__ -Iinclude  -mlittle-endian -D__ASSEMBLY__ -mapcs-32 -D__LINUX_ARM_ARCH__=4 -march=armv4 -mtune=arm9tdmi -msoft-float    -c -o arch/arm/lib/io-writesw-armv4.o arch/arm/lib/io-writesw-armv4.S

deps_arch/arm/lib/io-writesw-armv4.o := \
  arch/arm/lib/io-writesw-armv4.S \
  include/linux/linkage.h \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  include/asm/linkage.h \
  include/asm/assembler.h \
  include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/smp.h) \

arch/arm/lib/io-writesw-armv4.o: $(deps_arch/arm/lib/io-writesw-armv4.o)

$(deps_arch/arm/lib/io-writesw-armv4.o):

cmd_arch/arm/lib/udivdi3.o := arm-linux-gcc -Wp,-MD,arch/arm/lib/.udivdi3.o.d  -nostdinc -isystem /home/jwarren/projects/elcheapo/tools/bin/../lib/gcc-lib/arm-linux-uclibc/3.3.4/include -D__KERNEL__ -Iinclude  -mlittle-endian -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -ffreestanding -Os     -fno-omit-frame-pointer -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mapcs-32 -D__LINUX_ARM_ARCH__=4 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -Uarm      -DKBUILD_BASENAME=udivdi3 -DKBUILD_MODNAME=udivdi3 -c -o arch/arm/lib/udivdi3.o arch/arm/lib/udivdi3.c

deps_arch/arm/lib/udivdi3.o := \
  arch/arm/lib/udivdi3.c \
  arch/arm/lib/gcclib.h \
  arch/arm/lib/longlong.h \

arch/arm/lib/udivdi3.o: $(deps_arch/arm/lib/udivdi3.o)

$(deps_arch/arm/lib/udivdi3.o):

cmd_arch/arm/lib/ucmpdi2.o := arm-linux-gcc -Wp,-MD,arch/arm/lib/.ucmpdi2.o.d  -nostdinc -isystem /home/jwarren/projects/elcheapo/tools/bin/../lib/gcc-lib/arm-linux-uclibc/3.3.4/include -D__KERNEL__ -Iinclude  -mlittle-endian -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -ffreestanding -Os     -fno-omit-frame-pointer -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mapcs-32 -D__LINUX_ARM_ARCH__=4 -march=armv4 -mtune=arm9tdmi -mshort-load-bytes -msoft-float -Uarm      -DKBUILD_BASENAME=ucmpdi2 -DKBUILD_MODNAME=ucmpdi2 -c -o arch/arm/lib/ucmpdi2.o arch/arm/lib/ucmpdi2.c

deps_arch/arm/lib/ucmpdi2.o := \
  arch/arm/lib/ucmpdi2.c \
  arch/arm/lib/gcclib.h \

arch/arm/lib/ucmpdi2.o: $(deps_arch/arm/lib/ucmpdi2.o)

$(deps_arch/arm/lib/ucmpdi2.o):

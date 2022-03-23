cmd_scripts/kconfig/kconfig_load.o := gcc -Wp,-MD,scripts/kconfig/.kconfig_load.o.d -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer       -c -o scripts/kconfig/kconfig_load.o scripts/kconfig/kconfig_load.c

deps_scripts/kconfig/kconfig_load.o := \
  scripts/kconfig/kconfig_load.c \
  /usr/include/dlfcn.h \
  /usr/include/features.h \
  /usr/include/sys/cdefs.h \
  /usr/include/gnu/stubs.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stddef.h \
  /usr/include/bits/dlfcn.h \
  /usr/include/stdio.h \
  /usr/include/bits/types.h \
  /usr/include/bits/wordsize.h \
  /usr/include/bits/typesizes.h \
  /usr/include/libio.h \
  /usr/include/_G_config.h \
  /usr/include/wchar.h \
  /usr/include/bits/wchar.h \
  /usr/include/gconv.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdarg.h \
  /usr/include/bits/stdio_lim.h \
  /usr/include/bits/sys_errlist.h \
  /usr/include/bits/stdio.h \
  /usr/include/stdlib.h \
  /usr/include/sys/types.h \
  /usr/include/time.h \
  /usr/include/endian.h \
  /usr/include/bits/endian.h \
  /usr/include/sys/select.h \
  /usr/include/bits/select.h \
  /usr/include/bits/sigset.h \
  /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sched.h \
  /usr/include/alloca.h \
  scripts/kconfig/lkc.h \
  scripts/kconfig/expr.h \
  /usr/lib/gcc-lib/i386-redhat-linux/3.2.2/include/stdbool.h \
  /usr/include/libintl.h \
  /usr/include/locale.h \
  /usr/include/bits/locale.h \
  scripts/kconfig/lkc_defs.h \
  scripts/kconfig/lkc_proto.h \

scripts/kconfig/kconfig_load.o: $(deps_scripts/kconfig/kconfig_load.o)

$(deps_scripts/kconfig/kconfig_load.o):

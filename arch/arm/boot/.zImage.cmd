cmd_arch/arm/boot/zImage := arm-linux-objcopy -O binary -R .note -R .comment -S  arch/arm/boot/compressed/vmlinux arch/arm/boot/zImage

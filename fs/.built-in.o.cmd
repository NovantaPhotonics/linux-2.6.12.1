cmd_fs/built-in.o :=  arm-linux-ld -EL   -r -o fs/built-in.o fs/open.o fs/read_write.o fs/file_table.o fs/buffer.o fs/bio.o fs/super.o fs/block_dev.o fs/char_dev.o fs/stat.o fs/exec.o fs/pipe.o fs/namei.o fs/fcntl.o fs/ioctl.o fs/readdir.o fs/select.o fs/fifo.o fs/locks.o fs/dcache.o fs/inode.o fs/attr.o fs/bad_inode.o fs/file.o fs/filesystems.o fs/namespace.o fs/aio.o fs/seq_file.o fs/xattr.o fs/libfs.o fs/fs-writeback.o fs/mpage.o fs/direct-io.o fs/eventpoll.o fs/binfmt_script.o fs/binfmt_elf.o fs/dnotify.o fs/proc/built-in.o fs/partitions/built-in.o fs/sysfs/built-in.o fs/devpts/built-in.o fs/ext2/built-in.o fs/ramfs/built-in.o fs/nls/built-in.o fs/cifs/built-in.o fs/jffs2/built-in.o

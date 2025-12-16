#!/bin/bash
set -e

# Clean
rm -rf bin iso *.o src/*.o

# Create directories
mkdir -p bin
mkdir -p iso/boot/grub

# Assemble bootloader
nasm -f elf32 boot.asm -o boot.o

# Compile kernel and libs
CFLAGS="-m32 -ffreestanding -fno-stack-protector -fno-pie -fno-pic -nostdlib -nostartfiles -nodefaultlibs"

gcc $CFLAGS -c kernel.c -o kernel.o
gcc $CFLAGS -c src/kb.c -o src/kb.o
gcc $CFLAGS -c src/print.c -o src/print.o
gcc $CFLAGS -c src/cstr.c -o src/cstr.o
gcc $CFLAGS -c src/fs.c -o src/fs.o

# Link kernel (ELF)
ld -m elf_i386 -T linker.ld \
    -o bin/kernel.elf \
    boot.o kernel.o src/kb.o src/print.o src/cstr.o src/fs.o

# Copy kernel to ISO tree
cp bin/kernel.elf iso/boot/kernel.elf

# Create GRUB config
cat > iso/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "minos version 1" {
    multiboot /boot/kernel.elf
    boot
}
EOF

# Build ISO
grub-mkrescue -o bin/minos.iso iso

# Run in QEMU
qemu-system-x86_64 -cdrom bin/minos.iso

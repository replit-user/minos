rm -rf ./bin/*
rm -rf ./*.o
nasm -f elf32 boot.asm -o boot.o
gcc -m32 -c -ffreestanding -fno-stack-protector -fno-pie -fno-pic -nostdlib -nostartfiles -nodefaultlibs kernel.c -o kernel.o
gcc -m32 -c -ffreestanding -fno-stack-protector -fno-pie -fno-pic -nostdlib -nostartfiles -nodefaultlibs ./src/kb.c -o ./src/kb.o
gcc -m32 -T linker.ld -o ./bin/kernel.bin kernel.o ./src/kb.o ./src/print.o

ld -m elf_i386 -T link.ld -o ./bin/kernel.bin boot.o kernel.o ./src/print.o ./src/kb.o ./src/cstr.o
qemu-system-x86_64 -kernel ./bin/kernel.bin
rm -rf ./*.o
# sudo apt install qemu-system-i386 qemu-utils
qemu-system-i386 -m 512M -drive file=build/os.img,format=raw -boot c -vga std -serial file:debug-log.txt # -s -S
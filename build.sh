#!/bin/bash

export TARGET=i686-elf
export PATH=$HOME/opt/cross/bin:$PATH
export COMP_ERROR="0"

export BUILD_BIN_PATH=build/.bin

INCLUDES="-Iinclude"

cleanBuild(){
    echo -e "\nCleaning up previous build files..."
    ./clean.sh
}

asmCompile(){
    for file in "$@"; do
        # Extract the relative path from the file (excluding the extension)
        relative_path=$(dirname "$file")
        filename=$(basename "$file" .s)
        output_file="${BUILD_BIN_PATH}/GAS_${relative_path//\//_}_$filename.o"

        echo -e "ASSEMBLY: Compiling \033[1m$file\033[0m to $output_file"
        $TARGET-as "$file" -o "$output_file"
        if ! [ -f "$output_file" ]; then
            export COMP_ERROR="1"
        fi
    done
}

nasmCompile(){
    for file in "$@"; do
        # Extract the relative path from the file (excluding the extension)
        relative_path=$(dirname "$file")
        filename=$(basename "$file" .asm)
        output_file="${BUILD_BIN_PATH}/NASM_${relative_path//\//_}_$filename.o"

        echo -e "NASSEMBLY: Compiling \033[1m$file\033[0m to $output_file"
        nasm -f elf32 "$file" -o "$output_file"
        if ! [ -f "$output_file" ]; then
            export COMP_ERROR="1"
        fi
    done
}

# Honestly, this isn't needed anymore, but just add support.
cppCompile(){
    for file in "$@"; do
        # Extract the relative path from the file (excluding the extension)
        relative_path=$(dirname "$file")
        filename=$(basename "$file" .cpp)
        output_file="${BUILD_BIN_PATH}/G++_${relative_path//\//_}_$filename.o"

        echo -e "G++: Compiling \033[1m$file\033[0m to $output_file"
        $TARGET-g++ -c "$file" -o "$output_file" -ffreestanding -O2 -fno-exceptions -fno-rtti -fpermissive $INCLUDES
        if ! [ -f "$output_file" ]; then
            export COMP_ERROR="1"
        fi
    done
}

cCompile(){
    for file in "$@"; do
        # Extract the relative path from the file (excluding the extension)
        relative_path=$(dirname "$file")
        filename=$(basename "$file" .c)
        output_file="${BUILD_BIN_PATH}/GCC_${relative_path//\//_}_$filename.o"

        echo -e "GCC: Compiling \033[1m$file\033[0m to $output_file"
        $TARGET-gcc -c "$file" -o "$output_file" -std=gnu11 -ffreestanding -O2 $INCLUDES
        if ! [ -f "$output_file" ]; then
            export COMP_ERROR="1"
        fi
    done
}

cleanBuild

mkdir build/.bin

clear

echo -e "\n-------------------------------"
echo -e "Compilation Process Started"
echo -e "-------------------------------\n"

# Assembly Compilation
echo -e "\033[1mASSEMBLY Compilation\033[0m"
echo -e "---------------------"
asmCompile $(find src/ -name "*.s")
nasmCompile $(find src/ -name "*.asm")
echo -e "\n"

# GCC Compilation
echo -e "\033[1mGCC Compilation\033[0m"
echo -e "---------------"
cCompile $(find src/ -name "*.c")
echo -e "\n"

# G++ Compilation
echo -e "\033[1mG++ Compilation\033[0m"
echo -e "---------------"
cppCompile $(find src/ -name "*.cpp")
echo -e "\n"

# Linker Compilation
echo -e "Linking with GCC"
echo -e "----------------"
$TARGET-gcc -T linker.ld -o build/build.bin -ffreestanding -O2 -nostdlib build/.bin/*.o -lgcc
echo -e "\n"

echo -e "-------------------------------"
echo -e "Compilation Ended"
echo -e "-------------------------------\n"

if [ -f "build/build.bin" ] && [ $COMP_ERROR == "0" ]; then
    # Check whether if the file has a valid Multiboot2 header.
if grub-file --is-x86-multiboot2 build/build.bin; then
    echo -e "\033[1mCreating GRUB-bootable disk image\033[0m"
    echo -e "--------------------------------------"

    # 16 MB empty image
    dd if=/dev/zero of=build/os.img bs=1M count=16

    # Create DOS partition table and a single FAT16 partition using all space
    echo ",,c" | sudo sfdisk build/os.img

    # Map partitions to loop device
    LOOP_DEVICE=$(sudo losetup -Pf --show build/os.img)
    PARTITION="${LOOP_DEVICE}p1"

    # Verify the partition exists
    if [ ! -e "$PARTITION" ]; then
        echo "Error: partition $PARTITION does not exist."
        sudo losetup -d "$LOOP_DEVICE"
        exit 1
    fi

    # Format the partition as FAT16
    sudo mkfs.fat -F 16 "$PARTITION"

    # Mount partition and copy kernel + optional programs
    sudo mount "$PARTITION" /mnt
    sudo mkdir -p /mnt/boot/grub
    sudo cp build/build.bin /mnt/boot/kernel.bin

    # Optional: copy extra programs
    if [ -d build/programs ]; then
        sudo mkdir -p /mnt/programs
        sudo cp build/programs/* /mnt/programs/
    fi

    sudo mkdir -p /mnt/boot/grub
    sudo cp build/boot/grub/grub.cfg /mnt/boot/grub/

    # **Install GRUB while the partition is still mounted**
    sudo grub-install \
        --target=i386-pc \
        --boot-directory=/mnt/boot \
        --modules="part_msdos fat" \
        "$LOOP_DEVICE"

    # Unmount partition and detach loop device
    sudo umount /mnt
    sudo losetup -d "$LOOP_DEVICE"

    # Clean up build binary
    rm build/build.bin

    echo -e "\nDisk image ready: build/os.img"

    ./qemu.sh

else
    echo -e "\033[1mFile does not have a valid Multiboot header.\033[0m"

    cleanBuild

    echo -e ""
fi

else

echo -e "\033[1mCompilation Issue\033[0m"

cleanBuild

rmdir build/.bin
rm build/build.bin

echo -e ""

fi

export COMP_ERROR="0"
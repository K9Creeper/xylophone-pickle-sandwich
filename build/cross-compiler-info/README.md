<h1> Cross Compiler Setup (i686) </h1>

<h2> File Explorer Prerequisites </h2>

1. Open File Explorer.
2. Locate and Open your WSL folder, ```\\wsl$\Ubuntu```.
3. Open the ```/home/<YOUR_USER>``` directory.

<h2 id="WSLPrerequisites"> WSL Shell / Terminal Prerequisites </h2 > 

1. Open the WSL Shell / Terminal.
2. Make sure you are in the correct path: ```/home/<YOUR_USER>```, by using the ```pwd``` command.
3. In the terminal, <b>(in the specfied order)</b> run: ```export PREFIX=$HOME/opt/cross```, ```export TARGET=i686-elf```, ```export PATH=$PREFIX/bin:$PATH```.
4. In the terminal, run: ```mkdir src```.
5. In the terminal, run: ```cd src```.
6. In the terminal, run: ```mkdir cross-compiler-build-i686```.
7. In the terminal, run: ```cd cross-compiler-build-i686```.

<b>DO NOT CREATE A NEW INSTANCE OF THE SHELL. IF SO, REDO ALL PREVIOUS STEPS. </b>


<h2> BinUtils </h2>

<h3> Download </h3>

1. In the terminal run: ```cd $HOME/src/```.
2. In the terminal run: ```wget https://ftp.gnu.org/gnu/binutils/binutils-2.43.1.tar.gz```.
4. Wait for the download to be completed.
3. In the terminal run: ```tar -xf binutils-2.43.1.tar.gz```.

<h3> Build </h3>

<b> MAKE SURE YOU HAVE COMPLETED the [WSL Shell / Terminal Prerequisites](#WSLPrerequisites) </b>

1. In the terminal, run: ```cd $HOME/src/cross-compiler-build-i686```.
2. In the terminal, run: ```mkdir build-binutils```.
3. In the terminal, run: ```cd build-binutils```.
4. In the terminal, run: ```chmod +x ../../binutils-2.43.1/configure```.
5. In the terminal, run: ```../../binutils-2.43.1/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror```.
6. Wait for the previous command to finish, this may take time.
7. In the terminal, run: ```make -j 4```.
8. Wait for the previous command to finish, this may take time.
9. In the terminal, run: ```make install -j 4```.
10. Wait for the previous command to finish, this may take time.

<h2> GCC </h2>

<h3> Download </h3>

1. In the terminal run: ```cd $HOME/src/```.
2. In the terminal run: ```wget https://ftp.gnu.org/gnu/gcc/gcc-14.2.0/gcc-14.2.0.tar.gz```.
4. Wait for the download to be completed.
3. In the terminal run: ```tar -xf gcc-14.2.0.tar.gz```.

<h3> Build </h3>

<b> MAKE SURE YOU HAVE COMPLETED the [WSL Shell / Terminal Prerequisites](#WSLPrerequisites) </b>

1. In the terminal, run: ```cd $HOME/src/cross-compiler-build-i686```.
2. In the terminal, run: ```mkdir build-gcc```.
3. In the terminal, run: ```cd build-gcc```.
4. In the terminal, run: ```chmod +x ../../gcc-14.2.0/configure```.
5. In the terminal, run: ```../../gcc-14.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers```.
6. Wait for the previous command to finish, this may take time.
7. In the terminal, run: ```make all-gcc -j 4```. 
8. Wait for the previous command to finish, this may take time.
9. In the terminal, run: ```make all-target-libgcc -j 4```. 
10. Wait for the previous command to finish, this may take time.
11. In the terminal, run: ```make install-gcc -j 4```. 
12. Wait for the previous command to finish, this may take time.
13. In the terminal, run: ```make install-target-libgcc -j 4```. 
14. Wait for the previous command to finish, this may take time.

<h2> Post Build </h2>

In the terminal, run: ```export PATH=$HOME/opt/cross/bin:$PATH```. This must be done for the cross-compiler to be recognized in the current instance.

<h2> How To Use </h2>

You can use this Cross-Compiler by invoking something like this: ```$TARGET-gcc --version```.

<h2> Notes </h2>

Every new instance of a shell, will clear these 'exports', you will need to keep invoking them.

<br>
<br>

<b>Source(s): </b>

https://wiki.osdev.org/GCC_Cross-Compiler

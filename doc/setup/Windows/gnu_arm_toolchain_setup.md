
>[!Abstract]
>This section explains how to install GNU Arm Toolchain, which contains the compiler `gcc`, yes, but most of all,  **`gdb`**


I installed the [Arm GNU Toolchain Downloads – Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) from ARM official website. 

There are many different, I took _Windows (mingw-w64-x86_64) hosted cross toolchains_. It is important to select the _AArch32 bare-metal target (arm-none-eabi)_ as we are targeting bare-metal programs (no OS on target)

After installing on Wodiws, all things required for tool-chain are installed here:
```bash
C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.3 rel1\bin>
```

#### Beware of `%PATH%` if you installed STM32CubeIDE

>[!WARNING] Path conflict?
>I got the following error:
>```dos
>[100%] Linking C executable prj_h7.elf
c:/progra~2/gnuarm~1/102021~1.10/bin/../lib/gcc/arm-none-eabi/10.3.1/../../../../arm-none-eabi/bin/ld.exe:C:/Users/jeremie.gallee/Documents/Project_MarbleFrame/prj_blog/prj_h7/link/STM32H723ZGTX_FLASH.ld:91: non constant or forward reference address expression for section .ARM.extab
collect2.exe: error: ld returned 1 exit status
mingw32-make[2]: *** [CMakeFiles\prj_h7.elf.dir\build.make:3621: prj_h7.elf] Error 1
mingw32-make[1]: *** [CMakeFiles\Makefile2:82: CMakeFiles/prj_h7.elf.dir/all] Error 2
mingw32-make: *** [Makefile:90: all] Error 2
> ```
> Which may come from search for an old ARM-GCC, conflicting on my Windows.
> Indeed, this error comes from linker.ld file in my project, which specifies that GCC10 is not supported, only from GCC11:
> ```
> .ARM.extab (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
> {
>    *(.ARM.extab* .gnu.linkonce.armextab.*)
>    ```
> (From linker file)

I had indeed in my `%PATH%`, STM32Cube, which was placed before the brand new installed `gnu-arm-toolchain`. And also an old ARM-GNU compiler:

```dos
C:\Windows\System32\OpenSSH\;
C:\msys64\ucrt64\bin;
// STM32Cube disturbing it all
C:\ST\STM32CubeIDE_1.14.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.101.202401061624\tools\bin;
// Brand new GNU Arm toolchain
C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10;
C:\Program Files\usbipd-win\;
C:\Program Files\TortoiseSVN\bin;
```

Another problem was that I got an error when installing the last ARM-GNU tool-chain, path was too long for Windows, which said to me to manage it by myself the operation of adding the newly installed gnu-arm, version 14 to `%PATH%` variable. 

Funny this path length problem of Windows, and 1000 Mb of RAM for Edge when opening two pdf is ok...

So after correction to last installed ARM-GNU, my path is now:

```dos
C:\msys64\ucrt64\bin;
C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.3 rel1\bin;
C:\ST\STM32CubeIDE_1.14.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.101.202401061624\tools\bin;
C:\Program Files\usbipd-win\;
C:\Program Files\TortoiseSVN\bin;
```

Which is better. Now it all works to compile on Windows.
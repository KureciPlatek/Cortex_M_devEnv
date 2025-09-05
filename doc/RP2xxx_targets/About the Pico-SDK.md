# Dev env on RP2040

>[!Abstract]
>**About the RP2040**:
>- 133MHz Dual ARM Cortex-M0+ Cores
>- 264 kB SRAM
>- full C/C++ SDK available
>- AHB/APB buses, I2C, SPI, ADC, USB and UART peripherals
>Has a brother: RP2350 which has a dual Cortex-M33 and dual RISC-V on same chip
>---
>There are **two main ways** to install and use **Pico-SDK** environment:
>- Either by installing **VSCode extensions** (all integrated altogether, quite nice, quite savy, quite heavy...)
>- Or by installing it **all manually** and use it as you please, with the development environment that suits you the most
> 
>We will go all manually first, then install VSCode extensions.

Many examples may be found here:
[GitHub - raspberrypi/pico-examples](https://github.com/raspberrypi/pico-examples)

## On Windows
### Install of external sources

All important info may be found here: [GitHub - raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk) on how to install and prepare the Pico SDK on your machine.

I added the git repo to my project to use it decoupled from rest of my laptop (keep decoupled and paths relative). I ~~strongly modified~~ adapted the CMakeLists.txt, as Pico-SDK comes integrated with a full CMake managed development environment. Many external programs and tools are automated and required by this CMake managed environment, like TinyUSB, Python and Picotool.

Furthermore, CMake also integrates automatic Github mechanism, which doesn't make things quite decoupled but keeps CMakeLists.txt quite simple.

The most simple example given in Github didn't worked:
```dos
C:\...\prj_rp2040\bin>mingw32-make
[ 14%] Building C object CMakeFiles/prj_rp.dir/app/main.c.obj
C:\...\prj_rp2040\app\main.c:15:10: fatal error: pico/stdlib.h: No such file or directory
   15 | #include "pico/stdlib.h"
      |          ^~~~~~~~~~~~~~~
compilation terminated.
```
Which makes me wondered about my way of installing the SDK and how strong it is coupled/automated. The file do exists, I also tried to add it to include files:

```cmake
set(INCLUDE_DIR
   ${CMAKE_SOURCE_DIR}/app
   ${PICO_SDK_PATH}/src/common/pico_stdlib_headers/include
)

...

add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
```
 But then other error arise:

```dos
C:\...\prj_rp2040\bin>mingw32-make
[ 14%] Building C object CMakeFiles/prj_rp.dir/app/main.c.obj
In file included from C:\...\prj_rp2040\app\main.c:15:
C:\...\pico-sdk\src\common\pico_stdlib_headers\include/pico/stdlib.h:10:10: fatal error: pico.h: No such file or directory
   10 | #include "pico.h"
      |          ^~~~~~~~
compilation terminated.
```

Which is quite strange, because it is Pico-SDK's own environment.

Full automation of CMake doesn't look quite successful until now so I decided to clone the repo, do `git submodule update --init` as asked by Pico-SDK's CMake environment and remove everything from my CMakeLists.txt to reach the simplest, as presented in SDK's Github repository.

Git submodule fetched:
```dos
C:\...\Project_MarbleFrame\pico-sdk>git submodule update --init
Submodule 'lib/btstack' (https://github.com/bluekitchen/btstack.git) registered for path 'lib/btstack'
Submodule 'lib/cyw43-driver' (https://github.com/georgerobotics/cyw43-driver.git) registered for path 'lib/cyw43-driver'
Submodule 'lib/lwip' (https://github.com/lwip-tcpip/lwip.git) registered for path 'lib/lwip'
Submodule 'lib/mbedtls' (https://github.com/Mbed-TLS/mbedtls.git) registered for path 'lib/mbedtls'
Submodule 'tinyusb' (https://github.com/hathach/tinyusb.git) registered for path 'lib/tinyusb'
```

### Windows installer for Pico-SDK

After some research, there is an Installer on Windows that does it all, but will install the following stuff:
- [Arm GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- [CMake](https://cmake.org/download/)
- [Ninja](https://github.com/ninja-build/ninja/releases)
- [Python 3.9](https://www.python.org/downloads/windows/)
- [Git for Windows](https://git-scm.com/download/win)
- [Visual Studio Code](https://code.visualstudio.com/)
- [OpenOCD](https://github.com/openocd-org/openocd/)
So let's docker it all baby...

>[!WARNING] 
>Also, this installer won't be supported anymore and there is no clear procedure about the different elements required so I actually gave up to do it manually on Windows.

### Extension of Visual Studio Code: 

So I decided to try VSCode extension install, to manage required tools to be installed. It ended up to be like STM32Cube IDE. It doesn't download the important: CMake project and Pico-SDK only.

After creation of a project, it will install the `.pico-sdk/` folder which is the github repo with all stuff from Git repo. So could have been done manually.
Also, it presents the disadvantage to have a `/2.1.0/` folder and a headless git repo (not on any branch). On my opinion, it is not the best way to manage external libraries and git repositories. When another version comes, we will have another folder with all external elements, like Ninja, gnu-arm compiler and so on?

Here is an extract on how `~/.pico-sdk/sdk/<version.x.y.z>/` that appeared on my computer after install of Pico-SDK extension for VSCode: 

```bash
jeremie@N619:~/.pico-sdk$ ll
total 36
drwxr-xr-x  9 jeremie jeremie 4096 Dec 12 18:05 ./
drwxr-x--- 18 jeremie jeremie 4096 Dec 12 18:04 ../
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:05 cmake/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:05 ninja/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:05 openocd/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:04 picotool/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:04 sdk/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:04 toolchain/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:04 tools/

jeremie@N619:~/.pico-sdk$ cd sdk/2.1.0/
jeremie@N619:~/.pico-sdk/sdk/2.1.0$ ll
total 108
drwxr-xr-x 12 jeremie jeremie 4096 Dec 12 18:04 ./
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:04 ../
-rw-r--r--  1 jeremie jeremie  133 Dec 12 18:04 .bazelignore
-rw-r--r--  1 jeremie jeremie  201 Dec 12 18:04 .bazelrc
-rw-r--r--  1 jeremie jeremie    6 Dec 12 18:04 .bazelversion
drwxr-xr-x  9 jeremie jeremie 4096 Dec 12 18:10 .git/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:04 .github/
-rw-r--r--  1 jeremie jeremie  168 Dec 12 18:04 .gitignore
-rw-r--r--  1 jeremie jeremie  477 Dec 12 18:04 .gitmodules
-rw-r--r--  1 jeremie jeremie    0 Dec 12 18:04 BUILD.bazel
-rw-r--r--  1 jeremie jeremie 2715 Dec 12 18:04 CMakeLists.txt
-rw-r--r--  1 jeremie jeremie 2289 Dec 12 18:04 CONTRIBUTING.md
-rw-r--r--  1 jeremie jeremie 1489 Dec 12 18:04 LICENSE.TXT
-rw-r--r--  1 jeremie jeremie 6312 Dec 12 18:04 MODULE.bazel
-rw-r--r--  1 jeremie jeremie 8653 Dec 12 18:04 README.md
-rw-r--r--  1 jeremie jeremie    0 Dec 12 18:04 WORKSPACE
drwxr-xr-x  8 jeremie jeremie 4096 Dec 12 18:04 bazel/
drwxr-xr-x  4 jeremie jeremie 4096 Dec 12 18:04 cmake/
drwxr-xr-x  2 jeremie jeremie 4096 Dec 12 18:04 docs/
drwxr-xr-x  2 jeremie jeremie 4096 Dec 12 18:04 external/
drwxr-xr-x  7 jeremie jeremie 4096 Dec 12 18:04 lib/
-rw-r--r--  1 jeremie jeremie 3361 Dec 12 18:04 pico_sdk_init.cmake
-rw-r--r--  1 jeremie jeremie 1781 Dec 12 18:04 pico_sdk_version.cmake
drwxr-xr-x  9 jeremie jeremie 4096 Dec 12 18:04 src/
drwxr-xr-x 15 jeremie jeremie 4096 Dec 12 18:04 test/
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:04 tools/

```

**In `ninja/`** 
we have again another ninja variant: the `/v1.12.1/` as a standalone.

**In `cmake/`** 
```bash
jeremie@N619:~/.pico-sdk/cmake$ ll
total 16
drwxr-xr-x 3 jeremie jeremie 4096 Dec 12 18:05 ./
drwxr-xr-x 9 jeremie jeremie 4096 Dec 12 18:05 ../
-rw-r--r-- 1 jeremie jeremie  886 Dec 12 18:05 pico-vscode.cmake
drwxr-xr-x 6 jeremie jeremie 4096 Dec 12 18:05 v3.29.9/
jeremie@N619:~/.pico-sdk/cmake$ cd v3.29.9/
jeremie@N619:~/.pico-sdk/cmake/v3.29.9$ ll
total 24
drwxr-xr-x  6 jeremie jeremie 4096 Dec 12 18:05 ./
drwxr-xr-x  3 jeremie jeremie 4096 Dec 12 18:05 ../
drwxr-xr-x  2 jeremie jeremie 4096 Nov 21 14:46 bin/
drwxr-xr-x  3 jeremie jeremie 4096 Nov 21 14:46 doc/
drwxr-xr-x  4 jeremie jeremie 4096 Nov 21 14:46 man/
drwxr-xr-x 10 jeremie jeremie 4096 Nov 21 14:46 share/
```

We have another install of CMake **and** a `pico-vscode.cmake` which checks some config with VSCode. again, we are increasing our coupling to the VSCode IDE.

**In `toolchain/`**
Again, we have here a new installed `gnu-arm-none-eabi` tool-chain
```bash
jeremie@N619:~/.pico-sdk/toolchain/13_3_Rel1$ ll
total 584
drwxr-xr-x 8 jeremie jeremie   4096 Dec 12 18:04 ./
drwxr-xr-x 3 jeremie jeremie   4096 Dec 12 18:04 ../
-rw-r--r-- 1 jeremie jeremie   5650 Jun 15 20:46 13.3.rel1-x86_64-arm-none-eabi-manifest.txt
drwxr-xr-x 5 jeremie jeremie   4096 Jun 14 13:52 arm-none-eabi/
drwxr-xr-x 2 jeremie jeremie   4096 Jun 14 14:54 bin/
drwxr-xr-x 3 jeremie jeremie   4096 Jun 14 13:38 include/
drwxr-xr-x 4 jeremie jeremie   4096 Jun 14 13:40 lib/
drwxr-xr-x 3 jeremie jeremie   4096 Jun 14 13:40 libexec/
-rw-r--r-- 1 jeremie jeremie 553129 Jun 15 20:46 license.txt
drwxr-xr-x 8 jeremie jeremie   4096 Jun 15 20:46 share/
```


>[!CONCLUSION]
>The Pico-SDK from VSCode installs also, like STM32CubeIDE, all elements/required libs/programs are re-installed, in the folder.
>I don't like this way of doing it. Yeah it is like "_plug and play_", full development environment is ready. But like STM32CubeIDE, it leads to super heavy development environment, with few sight over which version we have and son on. We just rely on what the IDE gives us and we are not going in the direction of knowing and controlling the inputs of our project.
>No wonder we need 2 TB hard-drive on Windows now...

>[!QUESTION]
>Here it is up to the developer to choose between a fight and control over its environment or letting the machine controlling for us

# Environment of an RP2040 with Raspberry Pi debug probe


## Install of dev env

>[!WARNING]
>When looking for Pico SDK, be careful not to find the `/etc/alternatives/pico -> /bin/nano` which is actually the Nano (Nano's ANOther editor) which is inspired by `Pico` editor (Pine Composer)


## Debug:

Seems to use DebugProbe (CMSIS-DAP) [Default] -> DebugProbe uses DAP
Or selection of SWD -> Pi Host, on Pi 5 requires Linux Kernel > 6.6


### Things important


There are two paths that are actually important from Raspberry for RP2040:
The official SDK itself
[GitHub - raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
and some examples:
[GitHub - raspberrypi/pico-examples](https://github.com/raspberrypi/pico-examples/)

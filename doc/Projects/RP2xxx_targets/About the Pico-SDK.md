
>[!Abstract]
>There are **two main ways** to install and use **Pico-SDK** environment:
>- Either by installing **VSCode extensions** (all integrated altogether, quite nice, quite savy, quite heavy...)
>- Or by installing it **all manually** and use it as you please, with the development environment that suits you the most
> 
>We will go all manually first, then install VSCode extensions.


>[!IMPORTANT]
>pico-sdk massively uses CMake to manage sources to be integrated/compiled/linked to project or not. It leads to very simple CMakeLists.txt files. How to integrate the different features offered by pico-sdk is done in function `target_link_libraries()` :
>
>```CMake
>target_link_libraries(${CMAKE_PROJECT_NAME}
   pico_stdlib
   hardware_adc
   ...
   pico_async_context_freertos
   pico_lwip_mqtt
)
>```


>[!WARNING] No Windows Environment
>As I prefer Linux, I did not made any project examples yet, of an `rp2xxx` target with an environment on Windows. Everything is done on Linux.
>
>I explain however how to install everything on Windows.


**Diagram of my setup**

Before carrying on, I prefer to provide here a diagram of my setup, which connections are made, in a little bit more details. All connections are then explained later in this document. Refer to this diagram to understand what I am doing.

![[rp2xxx_setup.svg]]

This setup is the same for all projects in `rp2xxx` family.

| Element      | Description                                                                                                                                                                                                                                                                                                                   |
| ------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `USB pico`   | Connected to connector `USB` of pico board, it will power our target and the `rp2xxx` SoC on it                                                                                                                                                                                                                               |
| `USB debug`  | Connected to Raspberry Debug Probe. It will power the debugger, forward `UART`/Serial messages and transmit `gdb` commands through `gdb-server`                                                                                                                                                                               |
| `RP2XXX SoC` | The target we want to program, manipulate, debug                                                                                                                                                                                                                                                                              |
| `UART0`      | The default UART used by pico when configuring board to forward stdio to UART. See section [[#Configuration of `CMakeLists.txt`]] for more details.                                                                                                                                                                           |
| `SWD`        | Debug interface of `rp2xxx`, part of CoreSight of ARM Cortex-M architecture. No specific configuration to be done here, except if you want to use the pins for another purpose, but then you are on your own. It is connected on `DEBUG` connector of board, its position may change, but generally it is at the bottom of it |
| `LED green`  | Like `UART^0`, hard wired on pico board. Here, in case of a `pico2_w`, the `CYW43` WiFi/Bluetooth chip took too many pins of `RP2350`, that Raspberry had to control it through `CYW43` chip.                                                                                                                                 |
| `CYW43 chip` | Cypress CYW4343W WiFi and Bluetooth chip for IP and Bluetooth communication. More details here: [Connecting to the Internet with Raspberry Pi Pico W-series.: Getting online with C/C++ or MicroPython on W-series devices.](https://datasheets.raspberrypi.com/picow/connecting-to-the-internet-with-pico-w.pdf)             |


>[!INFO] For more information
>All information about `pico-sdk` and the different pico boards is very well explained here: [Pico-series Microcontrollers - Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html) Select the Getting Started with pico to have the first introduction.



# Install of pico-sdk

## On Linux

It is very simple and straightforward to install it on Linux, simply clone project from github and configure paths so that your `CMakeLists.txt` knows where to find the `pico-sdk`'s sub-`CMakeLists.txt`

How to install it is well explained in section "UNIX Command Line" from pico-sdk's github repository: [GitHub - raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

>[!HINT]
>As for some project we may use some git submodules, clone the git repository and updates also the submodules:
>```bash
>git clone https://github.com/raspberrypi/pico-sdk.git
>git submodule update --init --recursive
>```


## On Windows
### Install of external sources

Same for [[#On Linux]], all important info may be found here: [GitHub - raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk) on how to install and prepare the Pico SDK on your machine.

#### That's where the fun begins

I added the git repo to my project to use it decoupled from rest of my laptop (keep things decoupled and paths relative). I ~~strongly modified~~ adapted the `CMakeLists.txt`, as Pico-SDK comes integrated with a full `CMake` managed development environment. Many external programs and tools are automated and required by this `CMake` managed environment, like `TinyUSB`, `Python` and `Picotool`.

Furthermore, `pico-sdk`'s `CMake` integrates automatic `Github` fetch mechanism. Which doesn't make things quite decoupled but keeps `CMakeLists.txt` quite simple.

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

I am not found of it, as again, many things are installed or re-installed, many different things to manage and if one breaks, the whole chain comes down with it.

>[!WARNING] 
>This installer won't be supported anymore anyway. And there is no clear procedure about the different elements required so I actually gave up to do it manually on Windows.

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


>[!WARNING] CONCLUSION
>The Pico-SDK from VSCode installs also, like STM32CubeIDE, all elements/required libs/programs are re-installed, in the folder.
>I don't like this way of doing it. Yeah it is like "_plug and play_", full development environment is ready. But like STM32CubeIDE, it leads to super heavy development environment, with few sight over which version we have and son on. We just rely on what the IDE gives us and we are not going in the direction of knowing and controlling the inputs of our project.
>No wonder we need 2 TB hard-drive on Windows now...





---
# Configuration of `CMakeLists.txt`

`pico-sdk` massively uses `CMake` and expect us also to use `CMake` to manage our project and integrate `pico-sdk`.

For this reason, CMakeLists.txt are way much smaller and simpler for `rp2xxx` family than for `stm32xx` family. 
The drawback is that we are dependent on how `pico-sdk` is made and managed by `CMake`, which may reduce our flexibility. As said, I tend to keep things as decoupled as possible, and this is a form of coupling.

Anyway, here is how important parts of a `CMakeLists.txt` for a `RP2350` looks like:

```CMake
message("Project RP2350")

# --- Configure pico-sdk
# Set board type, in my case it is a pico2_w
set(PICO_BOARD pico2_w)

# path to picotool. I placed it in /opt/ but can be wherever you want
set(picotool_DIR /opt/picotool)

# path to the pico-sdk git repository
set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/../../pico-sdk)

# path to a pico-sdk CMake file you have to integrate to your project
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)

...
# Call it prior after setting the different paths as show before. It will
# Configure the CMake environment of pico-sdk
pico_sdk_init()

...

# --- Create project's Makefile
# Well, no chocolate, no executable
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})

# Configure some common used peripherals:
# Those one are a little bit specific, as it configures pico projects to use
# UART or USB, or not. I set UART to 1 to have printf() forwarded to the default
# UART, and forwarded again to Debug Probe's virtual COM port (/dev/ttyACM* or
# /dev/ttyUSB*).
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 0)

...

# Add pico-sdk libraries
# This the recommended way from pico-sdk to import and integrate the different
# sources from it.
target_link_libraries(${CMAKE_PROJECT_NAME}
   pico_stdlib
   hardware_adc
   ...
)

# Add .elf and .hex files
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
```

Specificities of this CMakeLists.txt:
- Section `# --- Configure pico-sdk` of this code snipped should be clear enough. Just set the different paths
- `pico_enable_stdio_***()` allows us to forward `printf()` to either default UART port of our pico board, or to USB. I selected UART, as I wanted logs to be forwarded to the UART, then the debugger and read it all from the Virtual COM port offered by the debug probe.
- `pico_sdk_init()` Prepare `CMake` environment of `pico-sdk`
- `target_link_libraries()` is important because it is where the different features offered by `pico-sdk` will be integrated. For example, `pico-stdlib` and `hardware_adc` to read RP2350's ADC values. **Do not try to add with `include_directory()`**
- Call at last, `pico_add_extra_outputs()` to generate the `.elf` file. Otherwise, only an `.uf2` file, that you have to flash with drag and drop on pico2-w board USB mass storage, as explained here: [Getting started with Raspberry Pi Pico-series: C/C++ development with Raspberry Pi Pico-series and other Raspberry Pi microcontroller-based boards.](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)


There are other things to configure on CMakeLists.txt, but as they should be self explanatory enough, I didn't took the time to explain them here.

>[!INFO]
>Some projects have special configuration (like integration of `FreeRTOS`). Those are explained in the project folders, not here.

# Debug of `rp2xxx` family with Raspberry Pi Debug Probe

#todo

### `openOCD` config to flash & debug with RP Debug probe

After an apt-get upgrade of openocd, I did got all rp2350 config file. Executing 
```bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
```

System View Description files may be found in `pico-sdk`, at `src/rp2350/hardware_regs/rp2350.svd`
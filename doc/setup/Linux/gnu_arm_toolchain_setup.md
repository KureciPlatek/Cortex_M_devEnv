
>[!Abstract]
>This section explains how to install GNU Arm Toolchain, which contains the compiler `gcc`, yes, but most of all,  **`gdb`**

## Install GNU-ARM toolchain

**On Linux, either with aptitude:**

```bash
sudo apt install gcc-arm-none-eabi
```

>[!WARNING]
>I discovered that installing GNU ARM toolchain per `apt` package manager, doesn't provide the gnu debugger with it (`arm-none-eabi-gdb`, which is the equivalent of `gdb`, but for ARM based microcontrollers). 
>-> `gdb-multiarch` must be installed, it works fine
>-> Or install ARM-GNU Toolchain from official ARM website as described below


**Or download the last version from official's website directly: **
[Arm GNU Toolchain Downloads – Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

>[!WARNING] Do not download something random
>I selected _**x86_64 Linux hosted cross toolchains**_ / _**AArch32 bare-metal target (arm-none-eabi)**_ as we are targeting "bare-metal" programs (no OS is available on target, run program directly on the Cortex-M core).

Unpack it and place it in `/usr/share/`.
Then add links to all binaries from this toolchain in `/usr/bin`:

```bash
$ sudo tar xJf arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi.tar.xz -C /usr/share/
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
$ sudo ln -s /usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/arm-none-eabi-ld /usr/bin/arm-none-eabi-ld
```

The list of linked executables here is reduced to some, but if your development environment requires more, just add them. You will see that in `/usr/share/arm-gnu-toolchain-<vers>-<host_arch>-arm-none-eabi/bin/` many more tools are available. Like `objdump` or `strip`.

It depends on what you require for your project (compiling, stripping, dumping of object etc...).

This way, you will have an apt-like install of most recent and official gnu-arm tool-chain release. But it keeps the manually installed file separated from Linux's apt installed files.

> [!TIP]
> It is also possible to add `/usr/share/arm-gnu-toolchain<***>` to PATH


>[!IMPORTANT]
>This section is more an opinion than a real explanation. You are not obligated to read it. Nevertheless, I started this project because I see my fellow engineers starting to see Scrum, IDEs, CMake and Docker/Containers as gods that nobody should contradict or question their advantages.
>
>I disagree with this religious way of looking at those mentioned technologies/things


## Long short version of our goals:

We usually use all integrated development environment, generally provided by vendor or company. We just have to focus on developing and understand how the IDE works (how to create a debug session, how to configure project, how to manage external sources, etc...)
But when looking a little bit, most of the IDE (except for IAR and Keil), using gnu for ARM tool-chain, _just_ integrates some different elements/programs that may be used separately.

The aim of this article is to understand what actually lies below those IDE, because, for example, STM32CubeIDE uses all the technologies I will use, but bundles them into the big STMCubeIDE folder and forces you to uses the IDE only.

Why do that? So you could understand actually what is happening, tailor your development chain to your need, reduce price and hard-drive cost and actually increase simplicity.

For that, we will look at the debugger, the compiler mainly, and some other specific parts and look at the common point between all those projects with different targets.

## About IDE
There are IDEs like STM32CubeIDE or other Eclipse based environment, but it is not the aim here. I want to show which parts are actually important. Cleanly split the full development environment into those _parts_ and explain what they do, as much as possible how to configure/use them to tailor them to your environment.

>[!NOTE] About PlatformIO
>It is for me the same as STM32CubeIDE, it integrates it all and adds a GUI layer over it, but at the end it uses all elements that we will explain. 
>
>On my opinion, if there is the need to use a GUI to debug (which is also on my opinion quite useful), then I would go to a professional tool, like JLink and Ozone from SEGGER which covered 100% of debug cases I met until now.


## About CMake

Many projects are now managed with CMake, and on my opinion, with too much modularity. It is good to have modularity, but until a point. For example, in pico-sdk as it will be seen, CMake is on my opinion too intensively used, and managing CMake may start to take more time than developing project itself (again, on my opinion). And it should not be.

For this reason, I did not made many CMakeLists.txt, almost one for each subfolder/file like some project does. Also I did not made one CMakeLists.txt to control all different projects with different libraries and architectures, as the difference between those projects is too high to be all controlled in one CMakeLists.txt.

Instead, I strive for one single, as simple as possible CMakeLists.txt, that manages full project, one per project. Helped by two .cmake files that lists external sources and project's own sources. Not more.

>[!TIP]
>On my experience, each family of target, coming from a vendor should have its CMakeLists.txt. Indeed, each company has its own understanding of CMake, and uses as it pleases it. 
>
>**For example**: 
>I used targets from two different companies: STMicroelectronics and Raspberry Pi, and for both, different targets. For all targets from STMicroelectronics, I could keep a lot of common between the CMakeLists.txt. But almost nothing when I started to work with RP2xxx from Raspberry Pi.


I find generally that people overuse `CMake` and its many functions for same functionality. 
For example, to add `-Wall` flag to compiler, there are different options:

```CMake
set (CMAKE_C_FLAGS "-Wall")
```
Or
```CMake
add_compile_options(-Wall)
```

**But those two ways bring different compilation results**

- In the first case, I have an error thrown:
```bash
/pico-sdk/src/rp2_common/hardware_sync_spin_lock/include/hardware/sync/spin_lock.h:152:2: error: #error no SW_SPIN_LOCK_LOCK available for PICO_USE_SW_SPIN_LOCK on this platform
  152 | #error no SW_SPIN_LOCK_LOCK available for PICO_USE_SW_SPIN_LOCK on this platform
      |  ^~~~~
```

- In the second case (from pico-sdk example), I have only warnings (a lot for an sdk on my opinion)

Git commit with the error: #51778e9

Other way to add GNU-ARM flags:
```CMake
set_source_files_properties(
    ${PICO_LWIP_PATH}/path/to/some/file.c
    PROPERTIES
    COMPILE_OPTIONS "-Wno-unused-result"
    )
```

I mean, how many ways we can do that? On my opinion, it leads to the kind of problem I encountered earlier. Too many options = too many things to manage and maintain = increased risk of non understandable problems.



## Windows pride


>[!TIP] Stop thinking Windows is good
>But we should stop to work on Windows OMO, it just tries to reach the capacity of development of Linux, and so many things are just there to try to imitate Linux, like Cygwin or MinGW, then Microsoft itself did the WSL, which adds more complexity (even if WSL is great) to the stuff.
>
>Windows is just evolving backwards, like the new _"AI powered"_ Outlook is, this thing is the biggest pile of junk I ever saw, 1/2 a Gb for sometimes blocking my e-mail because it thinks I am focusing! Please just stop putting intelligence everywhere, I don't want my hammer to get soft before hitting the nail, because it thinks I may hurt myself...)
>
>The only things that are keeping Windows afloat is because of Outlook (which you know my opinion on), Excel and Office suite, and also only because everybody base it works on it. And also because some program still only work on Windows.
>
>WSL is on my opinion also not much better, because it is not a full kernel and for example, some drivers for PEAK-PCAN may not work (have ever heard of error "HEADER not found"?). Also, at some point virtual memory manager Vmmem will take over 1.5 Gb of RAM space...
>
>Also with WSL, you have to do some trick to forward USB ports to this semi-OS, so that you can flash and debug your program on target with gdb (I had to use usbipd to forward STLinkDebugger to WSL env)

Windows only brings problems. It is always a strange state regarding terminals, it is never clear how it will react, as sometimes it configures itself to be UNIX like, sometimes half, sometimes not. Git reacts differently.

Those backslashes are nothing else but a pain in the ass

It is still crazy to have an space in name `Program Files`.

And those CRLF in files are just horrible. I mean, why?? I don't know how many times, with the flourishing of "smart" programs and IDE, it configures it back to CRLF, I didn't went to control it at each commit (please don't tell me I have to do that) and tadaaaa `^M` chars everywhere! Big Red Commit!

### CMake problems on Windows:


>[!TIP] Stop thinking Windows is good
>They are the only one using "\\" instead of "/", which make all path useless on `CMakeLists.txt`. Yay.
>So when writing decoupled CMake files, as platform independent as possible, I wanted to use `${HOME}` environment variable, sadly, for Windows, `${HOMEPATH}` provides a path with "\\", which will generate errors. So I had to put my full path in `stm32.cmake` file.

To generate makefiles, you also have to sepcify which kind of GNU you installed: ninja, MinGW, cygwin, etc... there are many different style and I am always confused about that. People may say I am an old grumpy cat, but when you have many different programs which are not compatible, for one OS, for the same purpose, compared to one program for all UNIX OS, 

### Generally about Windows and the way Microsoft maintain/develop it

As you can see I really don't like Windows, and when I see the direction it is taking:
- Last Outlook is simply horrible
- 500 MByte when one pdf is open
- vmem which explodes after a couple of hours of use of WSL
- Teams / Microsoft account / Outlook who goes in all directions (why do we get notification that we receive a mail? Why are we notified that someone tries to contact us on Teams? Why are sometimes ). And when we go a little bit deep, we can actually see that this whole monster is poorly finished: for example, by setting an event, each time I modify my event, it changes the status back to "free"!! Why can't you just keep it for example "out of office" as I set it? The list is long and for something that we pay, I am shocked to see how poorly it is finished.
- DOS use is a pain in the a**, for example, add aliases to powershell is a full afternoon of work, with support of chatGPT


# About IDEs
## STM32CubeMX and STM32CubeIDE

Those two big babies are required to generate a couple of files, which won't be heavier at the end, as a couple of kbyte (surely less).

They generate, complex startup and linker file and are quite handy to generate clock, pll and system configuration. It could also be possible to do those parts which are in C (not linker and startup.s which I recommend take some already made) ourselves, but it would take too much time.

>[!WARNING] 
>Every time you create a new project with STM32CubeMX and/or STM32CubeIDE, it will actually also do a git clone of the STM32CubeXX folders we manually cloned!!! Again, bundle all together again. It is, yeah, explained when generating a project with Cube. 
>But now I have a file in my `%HOME%` older which is more than 11 Gbyte!!:
>On `~\STM32Cube\Repository`, size is:
>```bash
>C:\Users\jeremie.gallee\STM32Cube
>11.6 GB (12’458’266’786 Bytes)
>```
>PS: Getting this info on DOS is a farce, you have to do `dir /s` and wait for a couple of ~hours~ minutes for terminal to end print it all and show value...

>[!WARNING]
>After a little bit of flowing into this STM32Cube/Repository mystic folder I discovered some pepitos, truly.
>STM32CubeMX created 3 different STM32F4 HAL folders:
>```dos
>~\STM32Cube\Repository\Packs\STMicroelectronics\X-CUBE-AZRTOS-F4\1.1.0\Drivers\STM32F4xx_HAL_Driver
>~\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.1\Drivers\STM32F4xx_HAL_Driver
>~\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.2\Drivers\STM32F4xx_HAL_Driver
>```
>I mean, whaaaaat??? If they are different, gosh...

>[!WARNING]
>I discovered that for STM32H7, ThreadX is not in the typical `<stm32_hal_git_repo>/Middlewares/ST/threadx/` like for STM32N6 or some others, but another git repo is provided by STMicroelectronics, which is named: `x-cube-azrtos-h7`. Inside this repository, **another STM32H7 HAL** is available, and **is different from the STM32CubeH7**!!
>
>After a diff, I saw only few differences, but differences anyway.


The next sections are about how and what to configure/install on your computer. For both Windows and Linux. Some are the same (like CMake configuration) some are different. I didn't had the time (or the nerves) to fully do it for both OS

Also, as I want to leave free to anyone to build/custom its own environment, I just explain the parts that have to exists for everything to work, but how you put them all together is free of choice.

In this file, I explain my preferences about the development environment setup.

If you are not interested, jump to [[setup/Linux/gnu_arm_toolchain_setup|gnu_arm_toolchain_setup]] for Linux and [[setup/Windows/gnu_arm_toolchain_setup|gnu_arm_toolchain_setup]] for Windows.

---
### My Linux environment

For me, **I mainly worked on Linux** with a Raspberry Pi 5, 8 Gb RAM and quad-core Arm Cortex A76 processor.

>[!WARNING] AArch64 Linux and Pi5
>RPi5 has but only one "_sliiiiiightly_" disadvantage: it is aarch64 Linux, and not x86_64. Which means that many programs that were ported by vendors/companies may have been ported on Linux, but not for Raspberry Pi 5.n 
>For example, Saleae software is not supported

### Working environment setup

To keep some flexibility and show that it is possible, I let my different setups (Target + Debugger) plugged to the Raspberry Pi at the office and worked remotely with VSCode and terminals through SSH. This way, I could work remotely from the office, from home or somewhere else (train, library).

>[!WARNING] 
>This setup is only for Linux, as RPi5 is under Linux OS

VSCode offers a quite handful way to keep a GUI on remote, without requiring to forward the X server. It can indeed connects to a remote session through ssh.

>[!NOTE]
> How to work remotely with VSCode will not be explained here. Please look at https://code.visualstudio.com/docs/remote/ssh

Structure of my Linux working environment setup:

![[WorkingEnvSetup.svg]]

The philosophy is simple, everything is on the RPi5, which stays put somewhere, with all hardware connected to it and containing everything that is to install. The laptop is mere here to offer an interface to human, all is controlled on RPi5 remotely through VSCode and terminal over SSH.

Of course, you may chose a different environment, as long as cables are well plugged my dear Frodo. For example, it will be different for Windows users.


**Next:**

| [[Theory_of_debugging_with_GDB]] < Previous page | Next page ><br>(Linux)[[setup/Linux/gnu_arm_toolchain_setup\|gnu_arm_toolchain_setup]]<br>(Windows)[[setup/Windows/gnu_arm_toolchain_setup\|gnu_arm_toolchain_setup]] |
| :----------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------: |

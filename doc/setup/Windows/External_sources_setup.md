
>[!ABSTRACT]
>Here I explain how I organized the different external/third Party source code and resources. I mainly talk about `ThreadX` and `HAL`  and how they are integrated to project with `sub-CMake.cmake` files, as for `pico-sdk` it is a little bit different and detailed in another file

>[!HINT]
>How to set-up external/third party sources on Windows is very Linux-like so not everything is explained. It is also good that it is Linux-like, it means it is decoupled from OS.

## `ThreadX`

Same as for Linux, but keep in mind that `%PATH%` is different and has to be configured in `cortex_m_rtos.cmake` file

Look here: [[setup/Linux/External_sources_setup|External_sources_setup]]
## `FreeRTOS`

#todo
## HAL for F4 and H7

Same as for Linux, but keep in mind that `%PATH%` is different and has to be configured in `cortex_m_stm32.cmake` file

Look here: [[setup/Linux/External_sources_setup|External_sources_setup]]

## RP2xxx's Pico-SDK

pico-sdk is a little bit different as its install process and use imposes some things. How to set it up on you development environment, then use it on your projects with CMake is explained separately here: [[About_the_Pico-SDK]].


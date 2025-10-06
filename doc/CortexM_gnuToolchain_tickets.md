## Description:

The idea is to show that we can fully control total Cortex-M development environment without any fully integrated black-boxed IDE

Is to show that IDE just do some steps to combine different mandatory elements (compiler, third party source etc... ) and actually brings nothing except more complexity, stronger coupling, and double installation of sources.

>[!SUMMARY]
>- Cortex-M used: M7, M4, dual M33, dual M0+, M55
>- RISC-V Hazard3
>- External sources: ThreadX, FreeRTOS, H7_HAL, N6_HAL, F4_HAL, pico-sdk
>- GNU-ARM toolchain
>- VSCode, CMake
>- Debuggers: STLink, Black Magic Debug Probe, JLink, Raspberry Debug Probe

### To do

>[!INFO] 
>Detailed explanation about what to do for pending WIP tickers

|       Ticket       | Details of work progress                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| :----------------: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|     **SFW_4**      | ==DevEnv7== RP2350, ThreadX<br>Look at official ThreadX git repo, in ports/cm33/example/build/ and look at secure and non secure mode, how TZ is implemented and the function callback story (it seems that some callback have to be implemented when a Cortex-M has a TrustZone, recommended by ARM).<br>Look at how to debug a HardFault and LR corruption problem<br>Seems that RP2350 always boot in Secure code, maybe is NSC and Secure ThreadX required |
|                    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                |

### Steps, workflow



***
## SYSTEM ISSUES

**New issues:**  

| Issue | Description                                                                                                                                                                                                 | state |
| ----- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----- |
| SYS_1 | ==Process== Create a branch for each DevEnv or couple of DevEnv. Avoid a single, long linear Git line                                                                                                       | NEW   |
| DOC_2 | ==ARM-Cortex== Explain and demystify CMSIS and other CMSIS (like DSP) files                                                                                                                                 | NEW   |
| DOC_3 | ==Debug== Explain/demystify/depict the different debug interfaces, _DAP_, _TAP_, _JTAG_, _SWD_ etc..., their role, their physical interfaces, connectors (_SWD + UART_, 20 pins connectors etc..) and so on | NEW   |
| DOC_4 | ==Linker== Understand linke files and how they work together with reset_handlers and startup.s files                                                                                                        | NEW   |
|       |                                                                                                                                                                                                             |       |



**Work In Progress issues:**  

| Issue | Description                                                                            | state |
| ----- | -------------------------------------------------------------------------------------- | ----- |
| DOC_1 | ==Blog== Create all documentation and explain the full work and process on each DevEnv | WIP   |
| DOC_6 | ==File names==. Check all file names, especially for CMake. Look also at all #todo     | WIP   |
|       |                                                                                        |       |



**Resolved issues:**

| Issue | Description                                                                                                                                                                   | state                   |
| ----- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------- |
| DOC_5 | ==Windows Complains== Gather all Windows complain in a Markdown file                                                                                                          | DONE some opinion stays |
| SYS_2 | ==CMake== Restructure all CMake files for them to be better structured and more clean<br>- Improvement: use target_sources() to avoid specifing those at add_executable() cmd | DONE - #0aa5813         |
|       |                                                                                                                                                                               |                         |


**Rejected issues**

| Issue | Description | state |
| ----- | ----------- | ----- |
|       |             |       |

***
## SOFTWARE ISSUES

**New issues:**  

| Issue | Description                                                                                                                                                       | state |
| ----- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----- |
| SFW_1 | ==C/C++ libraries== Check about the different libraries that are available/used/recommended by GNU-ARM toolsuite: _picolib_, _newlib_, _nanolib_, _stdlib_ etc... | NEW   |
| SFW_2 | ==Future work== Show that this development environment is 100% similar on FPGA and/or RISC-V architectures                                                        | NEW   |
| SFW_6 | ==DevEnv3== RP2040, ThreadX (Eclipseless) BlackMagic debug probe, Github/common Pico-SDK                                                                          | NEW   |



**Work In Progress issues:**  

| Issue  | Description                                                                                                                                                                  | state                                                               |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------- |
| SFW_9  | ==DevEnv6== STM32H7, ThreadX (Eclipseless), BlackMagic debug probe, STMCubeless (keep only required specific files)                                                          | WIP - #64e11c9<br>Still have to do JLink and BlackMagic             |
| SFW_4  | ==DevEnv7== RP2350, ThreadX (Eclipseless), Raspberry Debug Probe                                                                                                             | WIP - #1b0f7a7<br>Has a core HardFault IRQ when executing scheduler |



**Resolved issues:**

| Issue  | Description                                                                                                                                                                                           | state           |
| ------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------- |
| SFW_8  | ==DevEnv5== STM32F4, ThreadX (Eclipseless, from official Github repo direct), BlackMagic debug probe, STMCubeless (keep only required specific files)                                                 | DONE            |
| SFW_11 | ==DevEnv8== RP2350, FreeRTOS, Raspberry Debug Probe, MQTT<br>Warning about memory required for lwIP  ed  ed  ed                                                                                       | DONE - #86f3157 |
| SFW_5  | ==DevEnv2== With RP2040, ThreadX (Eclipseless), Raspberry Debug Probe, Github/common Pico-SDK                                                                                                         | DONE #d53d3c3   |
| SFW_10 | ==External Sources== restructure management of external source from GitHub repositories.<br>Get the official Azure_ThreadX repo instead of the one provideed by STMicroelectronics (x-cube-azrtos-h7) | DONE - #0aa5813 |
| SFW_12 | ==DevEnv7== Restructure threads and mqtt connection management. One task to manage MQTT connection, and transfers, one task to read temperature, one task to read gyroscopes                          | DONE - #20746fe |


**Rejected issues**

| Issue | Description                                                                      | state                 |
| ----- | -------------------------------------------------------------------------------- | --------------------- |
| SFW_7 | ==DevEnv4== STM32F4, ThreadX (Eclipse plugin), STLink debug probe, STMCubeIDE/MX | REJECTED - No STMCube |
|       |                                                                                  |                       |

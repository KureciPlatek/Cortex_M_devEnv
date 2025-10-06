
>[!ABSTRACT]
>Here I will list all projects, and technologies they use, or not.

# List of projects

### Different Projects with combination of targets/debuggers:
| Target  | Platform | Debuggers        | gdb-server                      | Architecture          |
| :------ | :------- | :--------------- | :------------------------------ | :-------------------- |
| STM32H7 | Linux    | STLink/BMD/JLink | OpenOCD, st-utils               | Cortex-M7             |
| STM32H7 | Windows  | STLink/BMD/JLink | OpenOCD, st-utils, STLinkServer | Cortex-M7             |
| STM32F4 | Linux    | STLink/BMD/RDP   | OpenOCD, st-utils               | Cortex-M4             |
| RP2040  | Linux    | RDP/BMD/JLink    | OpenOCD                         | Cortex-M0+            |
| RP2350  | Linux    | RDP/BMD/JLink    | OpenOCD                         | Cortex-M33 and RISC-V |
| RP2350  | Windows  | RDP/BMD/JLink    | OpenOCD                         | Cortex-M33 and RISC-V |
With:
- `RDP`: Raspberry Debug Probe
- `BMD`: Black Magic Debug probe

**Less work on Windows**:
I did for Windows only with target STM32H7 and RP2350 as they seems the most complex (presence of cache and MPU on Cortex-M7, dual core/drop on RP2350. TrustZone for both). 

I am assuming that porting on Windows for other project should be easy, as steps should be close to those two projects.

Furthermore, developing on Windows is on my opinion quite restrictive, which makes it all a little bit more annoying. So I focused on Linux.


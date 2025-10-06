
## Hardware Debugger Comparison

| Debugger | Price Range | Pros | Cons | Best For |
|----------|-------------|------|------|----------|
| **STLink** | $0-80 | • Built into Nucleo/Discovery<br>• Virtual COM port<br>• Good OpenOCD support | • ST chips only<br>• Limited advanced features | STM32 development |
| **JLink** | $65+ | • Professional grade<br>• Excellent tooling<br>• Wide MCU support | • Higher cost<br>• Overkill for hobbyists | Professional development |
| **Raspberry Debug Probe** | $12 | • Very affordable<br>• Open hardware<br>• Good for RP2040/RP2350 | • Limited to certain MCUs<br>• Community support | Raspberry Pi projects |
| **Black Magic Probe** | $80 | • Open source hardware/software<br>• Self-contained GDB server<br>• No additional software needed | • Higher cost<br>• Limited availability | Open source enthusiasts |




### Each debugger hardware is different

How to set debugger's hardware (cables, voltages, etc...) is quite easy and also quite different, depending on the target to be installed. To understand how is it done, please refer to your hardware target documentation, as well as the debugger documentation.



# In praxis

There are different gdb and gdb-server programs, as well as debuggers, here is the 

For the `gdb` program, I mainly used `arm-none-eabi-gdb`, from the last official release of `gnu-arm-none-eabi`. `gdb-multiarch` also works fine, but may be old and not support some more recent SoC.





## Details for debuggers I listed
### Debug with STLink debugger 

**Characteristics:**
- Works fine with the following `gdb-server`: `st-util`, `OpenOCD` and `ST-LINK server`
- ST's official `ST-LINK server` works only on Windows though
- Integrated on the PCB of almost all NUCLEO and DISCOVERY kits
- Cheap hardware (25-50$)
- Virtual COM port available (get `putchar()` over UART)

### Debug with Black Magic debug probe

**Website:** [Black Magic Debug: The Plug&Play MCU Debugger — Black Magic Debug documentation](https://black-magic.org/index.html)

**Characteristics:**
- Fully open source, hardware and software
	- Debugger's software may be flashed onto other boards which will be used as debugger (for example, it is possible to use an STMF4-Discovery board as the debugger's hardware)
	- Create your own debugger
	- Port new architectures or new microcontrollers
- Supports a lot of architecture
- MIPI-10 interface, as well as UART port (get `putchar()` over UART)
- Automation of flash and debugging with scripts
- Relatively cheap: ~70-90$

### Debug with J-Link

Todo

### Debug with Raspberry Debug Probe

Todo

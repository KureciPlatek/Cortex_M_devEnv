
>[!WARNING]
>This page is work in progress
>It should gather specificity for each debuggers and how to handle it (example, BMP has an integrated gdb-server and COM port)  

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

There are different `gdb` and `gdb-server` programs, as well as debuggers, here is the 

For the `gdb` program, I mainly used `arm-none-eabi-gdb`, from the last official release of `gnu-arm-none-eabi`. `gdb-multiarch` also works fine, but may be old and not support some more recent SoC.


# Details for debuggers I listed
## Debug with STLink debugger 

**Characteristics:**
- Works fine with the following `gdb-server`: `st-util`, `OpenOCD` and `ST-LINK server`
- ST's official `ST-LINK server` works only on Windows though
- Integrated on the PCB of almost all NUCLEO and DISCOVERY kits
- Cheap hardware (25-50$)
- Virtual COM port available (get `putchar()` over UART)

## Debug with Black Magic debug probe

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


#### Update of BMP

Install of Rustup
```bash
jeremie@laptop:~ $ curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
info: downloading installer

Welcome to Rust!

This will download and install the official compiler for the Rust
programming language, and its package manager, Cargo.

Rustup metadata and toolchains will be installed into the Rustup
home directory, located at:

  /home/jeremie/.rustup

This can be modified with the RUSTUP_HOME environment variable.

The Cargo home directory is located at:

  /home/jeremie/.cargo

This can be modified with the CARGO_HOME environment variable.

The cargo, rustc, rustup and other commands will be added to
Cargo's bin directory, located at:

  /home/jeremie/.cargo/bin

This path will then be added to your PATH environment variable by
modifying the profile files located at:

  /home/jeremie/.profile
  /home/jeremie/.bashrc

You can uninstall at any time with rustup self uninstall and
these changes will be reverted.

Current installation options:


   default host triple: aarch64-unknown-linux-gnu
     default toolchain: stable (default)
               profile: default
  modify PATH variable: yes

1) Proceed with standard installation (default - just press enter)
2) Customize installation
3) Cancel installation
```

Results of installation:

```bash
info: default toolchain set to 'stable-aarch64-unknown-linux-gnu'

  stable-aarch64-unknown-linux-gnu installed - rustc 1.90.0 (1159e78c4 2025-09-14)


Rust is installed now. Great!

To get started you may need to restart your current shell.
This would reload your PATH environment variable to include
Cargo's bin directory ($HOME/.cargo/bin).

To configure your current shell, you need to source
the corresponding env file under $HOME/.cargo.

This is usually done by running one of the following (note the leading DOT):
. "$HOME/.cargo/env"            # For sh/bash/zsh/ash/dash/pdksh
source "$HOME/.cargo/env.fish"  # For fish
source $"($nu.home-path)/.cargo/env.nu"  # For nushell
```


Install of cargo. I had to install `openssl` and `libssl-dev` first:

```bash
# Source .bashrc file after install of rustup (asked by it)
source .bashrc

# Install openssl libraries require by cargo
sudo apt-get install openssl
sudo apt-get install libssl-dev

```

Then install cargo itself.
```bash
curl -L --proto '=https' --tlsv1.2 -sSf https://raw.githubusercontent.com/cargo-bins/cargo-binstall/main/install-from-binstall-release.sh | bash
```


Now we are ready to install `bmputil`:

```bash
cargo binstall bmputil

# Lot of output, ending by:
...
    Finished `release` profile [optimized + debuginfo] target(s) in 5m 16s
  Installing /home/jeremie/.cargo/bin/bmputil-cli
   Installed package `bmputil v1.0.0` (executable `bmputil-cli`)
 INFO Cargo finished successfully
 INFO Done in 320.225216099s
```

Be careful to also update `udev` rules as explained here: [blackmagic/driver at main · blackmagic-debug/blackmagic](https://github.com/blackmagic-debug/blackmagic/tree/main/driver#99-blackmagic-plugdevrules)

Then it is possible to look at the probe, configure and update it. For example, to manage RISC-V targets, it is required to update it with command:

```bash
bmputil-cli probe update
```

Then follow what you want to install.


## Debug with J-Link

#todo

## Debug with Raspberry Debug Probe

#todo

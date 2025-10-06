# Troubleshooting Guide

## Quick Diagnostic Steps

### 1. Verify Your Environment
```bash
# Check toolchain
arm-none-eabi-gcc --version
arm-none-eabi-gdb --version

# Check OpenOCD
openocd --version

# Check hardware (Linux)
lsusb | grep -i st
ls /dev/ttyACM*

# Check hardware (Windows)
# Use Device Manager to verify STLink appears
```

---

## Common Issues & Solutions

### OpenOCD Connection Problems

#### Issue: "Error: open failed"
```
Error: open failed
in procedure 'program'
in procedure 'init' called at file "embedded:startup.tcl", line 495
```

**Causes & Solutions**:
- **Another debugger is active**: Close STM32CubeIDE, other GDB sessions
- **Permissions (Linux)**: Add user to `dialout` group, restart session
- **Wrong config file**: Verify you're using correct interface/target files

#### Issue: "No device found on target"
```
Error: No device found on target.
```

**Solutions**:
1. **Check physical connections**: USB cable, power, SWD/JTAG connections
2. **Verify target power**: Some boards need external power
3. **Try different USB port**: Some USB ports/hubs cause issues
4. **Check jumper settings**: Nucleo boards have ST-LINK/MCU connection jumpers

#### Issue: "JTAG-DP STICKY ERROR"
```
Error: JTAG-DP STICKY ERROR
Error: Could not initialize the debug port
```

**Solutions**:
1. **Reset the board**: Hold RESET button while connecting
2. **Power cycle**: Disconnect USB, reconnect
3. **Check SWD frequency**: Lower the frequency in config:
   ```tcl
   set CLOCK_FREQ 1000  # Reduce from 8000 to 1000 kHz
   ```

---

### GDB Connection Problems

#### Issue: "Remote connection timeout"
```
(gdb) target extended-remote :3333
Remote debugging using :3333
Remote communication timeout.  Target disconnected.: Connection timed out.
```

**Solutions**:
1. **Ensure OpenOCD is running**: Check OpenOCD terminal for "Listening on port 3333"
2. **Check port number**: Verify port matches in config file and GDB command
3. **Firewall issues**: Temporarily disable firewall to test

#### Issue: "Remote refused connection"
```
(gdb) target extended-remote :3333
:3333: Connection refused.
```

**Solutions**:
1. **OpenOCD not started**: Start OpenOCD first
2. **Wrong port**: Check `set GDB_PORT` in your config file
3. **Port already in use**: Kill other processes using port 3333

---

### Compilation Issues

#### Issue: "arm-none-eabi-gcc: command not found"
**Linux Solutions**:
```bash
# Check if installed
which arm-none-eabi-gcc

# If missing, install
sudo apt install gcc-arm-none-eabi
# Or download official release and update PATH
```

**Windows Solutions**:
1. Verify PATH includes ARM toolchain bin directory
2. Restart Command Prompt after PATH changes
3. Use full path as temporary workaround

#### Issue: "Linker script not found"
```
collect2.exe: fatal error: ld returned 1 exit status
/usr/bin/../lib/gcc/arm-none-eabi/xx/../../../../arm-none-eabi/bin/ld: cannot find -lc
```

**Solutions**:
1. **Check CMake configuration**: Verify toolchain file is correct
2. **Verify linker script path**: Ensure `.ld` file exists and is referenced properly
3. **Clean and rebuild**: Remove build directory and rebuild

---

### Hardware-Specific Issues

### STLink Issues

#### Issue: "STLink firmware upgrade required"
**Solution**: 
1. Download STLink Utility from STMicroelectronics
2. Connect board and run firmware upgrade
3. Alternative: Use STM32CubeProgrammer to upgrade

#### Issue: "Multiple ST-LINK detected"
**Solution**:
```tcl
# Add to config file to specify which STLink to use
hla_serial "066DFF495251717867013920"  # Use your STLink serial number
```

### Raspberry Pi Debug Probe Issues

#### Issue: "Picoprobe not detected"
**Solutions**:
1. **Flash correct firmware**: Ensure Debug Probe has picoprobe firmware
2. **Check USB connection**: Try different cables/ports  
3. **Linux permissions**: Add udev rules for picoprobe

---

### Platform-Specific Issues

### Linux Issues

#### Issue: "Permission denied" accessing /dev/ttyACM0
**Solution**:
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER
# Log out and back in

# Or temporary fix
sudo chmod 666 /dev/ttyACM0
```

#### Issue: OpenOCD fails with "libusb error"
**Solution**:
```bash
# Install libusb development files
sudo apt install libusb-1.0-0-dev

# For older systems
sudo apt install libusb-dev
```

### Windows Issues

#### Issue: "The system cannot find the path specified"
**Solutions**:
1. **PATH too long**: Windows has PATH length limits, shorten other entries
2. **Spaces in path**: Use quotes around paths with spaces
3. **Use short path names**: Use `PROGRA~2` instead of `Program Files (x86)`

#### Issue: OpenOCD immediately crashes
**Solutions**:
1. **Antivirus blocking**: Add OpenOCD directory to antivirus exclusions
2. **Run as Administrator**: Right-click Command Prompt → "Run as administrator"
3. **Install Visual C++ Redistributables**: Download from Microsoft

---

## Advanced Diagnostics

### Debug OpenOCD Connection
Add verbose logging to your config file:
```tcl
# Add at top of config file
debug_level 3
log_output openocd.log
```

This creates detailed logs in `openocd.log` for analysis.

### Debug GDB Communication
Start GDB with verbose output:
```bash
arm-none-eabi-gdb -ex "set debug remote 1" program.elf
```

### Check Hardware with System Tools

**Linux**:
```bash
# USB device details
lsusb -v -d 0483:  # STMicroelectronics vendor ID

# Serial port info
dmesg | grep ttyACM

# Check if device is busy
lsof /dev/ttyACM0
```

**Windows**:
```cmd
# Device manager via command line
devmgmt.msc

# Check COM port usage
mode  # Shows all COM ports
```

---

## Getting Additional Help

### Log Collection
When asking for help, include:

1. **Full error messages** (copy-paste from terminal)
2. **OpenOCD config file** contents  
3. **System information**:
   - OS version
   - Toolchain versions (`arm-none-eabi-gcc --version`)
   - Hardware being used

### Useful Resources

- **OpenOCD Documentation**: [OpenOCD User Guide](http://openocd.org/doc/html/index.html)
- **ARM GCC Documentation**: [ARM GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain)
- **STM32 Community**: [STM32 Community Forum](https://community.st.com/)
- **Raspberry Pi**: [Raspberry Pi Debug Probe Documentation](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html)

---

## Prevention Tips

### ✅ **Best Practices**
- Always use latest ARM toolchain releases
- Keep only one ARM toolchain in PATH
- Use vendor-provided OpenOCD configs when available
- Test setup with simple projects before complex ones
- Document your working configurations

### ❌ **Common Mistakes**
- Having multiple conflicting toolchains installed
- Using outdated OpenOCD configurations
- Not checking hardware connections first
- Mixing different debugging tools simultaneously
- Ignoring file permissions on Linux

---

## Still Having Issues?

If problems persist:

1. **Start fresh**: Uninstall everything, reboot, reinstall following setup guides exactly
2. **Test with known-good setup**: Try with a different board or debugger if available
3. **Check hardware**: Try the same setup on a different computer
4. **Community help**: Post in embedded development forums with complete details

**Remember**: Most issues are configuration-related, not hardware failures. Patience and systematic debugging usually resolve problems.

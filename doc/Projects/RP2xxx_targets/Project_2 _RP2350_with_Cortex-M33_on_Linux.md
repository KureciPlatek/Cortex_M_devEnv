
>[!Abstract]
>In this section, we will configure `prj_rp2350` project to run an `RTOS` on it, configure and run Wi-Fi communication with the `CYW43` chip available on `pico2_w` board and publish/subscribe to `MQTT` broker and services 

>[!TIP] Prerequisite
>You configured the `rp2xxx` development environment, as explained in [[About_the_Pico-SDK]]. We will compile and run program here.

>[!WARNING]
>I did not succeed to port `ThreadX` on RP230 yet, so I had to **switch to `FreeRTOS`**. 
>An RTOS is required by our project goals, and also to use `MQTT` and `CYW43`, an asynchron environment is required to handle the full IP stack and `CYW43` chip.
>
>For details about `ThreadX` porting (WIP), please look here [[ThreadX_on_rp2350_cortexM33]]

# Overview

Here is an overview of the different elements of this project:
![[rp2350_overview.svg]]

**There are 4 main parts**: 
- the **smartphone** (actually optional) to publish and subscribe to MQTT topics
- The **Raspberry Pi 5**, which actually offers the possibility to create a Wi-Fi hotspot. It will also have the role of MQTT broker and of MQTT client (publisher and subscriber)
- The **Raspberry Debug Probe** (required for development)
- The **pico2_w board**, which connects to the Wi-Fi hotspots and publish / subscribe to MQTT topics too. In an RTOS environment. For that, it requires the packages/code components as described in picture.

#### `MQTT` Broker

Each `MQTT` communication requires a broker, we will use the free of use `Mosquitto`, as it is available for Raspberry Pi 5 and pretty much well explained

#### `prj_rp2350` components

For `MQTT` to work on the `RP2350`, it requires some components to be included and configured:
- `FreeRTOS`, which will offer the required asynchron context. required by `cyw43` and `lwIP`
- `pico-sdk-mqtt`, the `MQTT` library of `pico-sdk`
- `lwIP`: light weight IP library, also provided by `pico-sdk`
- `cyw43_arch`: drivers for CYW43 Wi-Fi and Bluetooth chip
- `prj_rp2350_program` itself, where we define RTOS tasks, configure Wi-Fi, configure and initiate CYW43 chip, and handle `MQTT` messages

#### Fun with smartphone

To have some fun, discovering that it all works (engineers are big kids wanting to have fun no? Just don't burn the laptop of your boss while short-circuiting the full open-space like I did once...) you may use any MQTT analyzer application and connect your smartphone to the RPi5 Wi-Fi and find the MQTT broker.


# `FreeRTOS` 

## Installation

Warning, clone of official repository of `FreeRTOS` does not necessarily contains all required ports:
https://github.com/FreeRTOS/FreeRTOS-Kernel.git

It is like `openOCD`, it is better to clone `FreeRTOS` from Raspberry:
https://github.com/raspberrypi/FreeRTOS-Kernel.git

In `target_link_libraries()`, se set to dynamic heap for allocations: `FreeRTOS-Kernel-Heap4`


## `FreeRTOS` configuration

#todo explain tasks and async context required by other babies


# Installation of MQTT broker: `Mosquitto`

#### Install `Mosquitto`

Install `mosquitto` with the following steps:

```bash
# Install with aptitude
sudo apt install -y mosquitto mosquitto-clients

# Enable it as a service that starts when RPi5 boots
sudo systemctl enable mosquitto.service

# Check mosquitto installed and works
mosquitto -v
```

#### Configure `Mosquitto`

A config file is available to configure `Mosquitto`, provide list of credentials, port to listen to, interface to connect to an so on. For that, edit the following file with admin credentials:

```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Add the following lines:

```bash
allow_anonymous false
listener 1883
```

Now we will precise that only some users/profiles with passwords are allowed. For that, add the following lines to `mosquitto.conf` file:

```bash
# Set this line at the top of config file
per_listener_settings true

# Precise file where profiles and their passwords are saved
password_file /etc/mosquitto/passwd
```


Now, to create profiles, the program `mosquitto_passwd` comes very handy. For that, just do:
```bash
mosquitto_passwd -c /etc/mosquitto/passwd <new_user>
```

Be careful to give the same path to `passwd` file as in `mosquitto.conf` file. Give the user name you want in `<new_user>`
Then, when hitting ENTER, `mosquitto_passwd` will ask you to give a password for this user, simple as that.

### `Mosquitto` troubleshooting

`Mosquitto` can present some strange problems when not cleanly set down and Rpi5 is shut down. It sends afterwards a magnificently useless message when trying to start/restart` mosquitto.service` with `systemctl`:

```bash
jeremie@laptop:~/work/prj_blog/prj_rp2350 $ sudo systemctl start mosquitto.service
Job for mosquitto.service failed because the control process exited with error code.
See "systemctl status mosquitto.service" and "journalctl -xeu mosquitto.service" for details.
```

I mean, `exited with error code` and no error code provided... it is like "task failed successfully with ERR_SUCCESS". 

I discovered that it is better to force `Mosquitto` to be in IPv4 and also force it in RPi5 hotspot's configuration. For that, add lines `bind_interface wlan0` and `socket_domain ipv4` to `Mosquitto` config file as follow:

```bash
...

allow_anonymous false
listener 1883
# Here force interface to use, as well as use of ipv4
bind_interface wlan0
socket_domain ipv4

...
```

### Start Wi-Fi hotspot on Rpi5:

To start Wi-Fi hotspot on RPi5, without requiring the graphical interface, execute this command in a terminal:
```bash
sudo nmcli device wifi hotspot ssid <hotspot_name> password <password_name> ifname wlan0
```

### Test `Mosquitto` broker

The following commands in Linux may be used to test the Mosquitto broker by publishing and subscribing to topics in local:

```bash
# Subscribe to a topic
mosquitto_sub -d -t rpi5_topic -u RPI5_JEREM -P m05qu1770 -h 10.42.0.1

# Publish on a topic:
mosquitto_pub -d -t rpi5_topic -m "Hello world!" -u RPI5_JEREM -P m05qu1770 -h 10.42.0.1
```

Example:
```bash
# Subscribe to a topic:
mosquitto_sub -d -t rpi5_topic -u RPI5_JEREM -P m05qu1770 -h 10.42.0.1

# When publishing with mosquitto_pub command, we should see the message
Client (null) sending CONNECT
Client (null) received CONNACK (0)
Client (null) sending SUBSCRIBE (Mid: 1, Topic: rpi5_topic, QoS: 0, Options: 0x00)
Client (null) received SUBACK
Subscribed (mid: 1): 0
Client (null) received PUBLISH (d0, q0, r1, m0, 'rpi5_topic', ... (18 bytes))
Hello from Pico W!
```

Also, look at documentation: [test.mosquitto.org](https://test.mosquitto.org/)

# `MQTT` with `pico-sdk`

A deeper understanding on how to configure the pico-sdk to use lwIP, freeRTOS asynchron context and MQTT library will be done later.

For now, information may be found here: [Connecting to the Internet with Raspberry Pi Pico W-series.: Getting online with C/C++ or MicroPython on W-series devices.](https://datasheets.raspberrypi.com/picow/connecting-to-the-internet-with-pico-w.pdf)

As well as information provided by following examples:
- [pico-examples/pico_w/wifi/mqtt at master · raspberrypi/pico-examples · GitHub](https://github.com/raspberrypi/pico-examples/tree/master/pico_w/wifi/mqtt)
- [pico-examples/pico_w/wifi/freertos at master · raspberrypi/pico-examples · GitHub](https://github.com/raspberrypi/pico-examples/tree/master/pico_w/wifi/freertos)

Also, reading code :) even if I am not a fan of people saying "the code is documentation enough", which for me is like "I am too lazy to take time to explain, refactor and reorganize what I did. Just try to follow my unexplained decisions".

# Run program

After all is well configured as explained before, and when your elf file is ready, you may flash and run the program with `gdb` as explained in prior documents. I did it only with openOCD and Raspberry Debug Probe, but it should also be fine with JLink EDU and Black Magic Probe, as explained in [[Project_1_RP2040_on_Linux]]

1. Run `gdb-server` with `openOCD`:
```bash
# Run openOCD
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c "adapter speed 5000"

# Expected prompt:
```

2. Run `gdb` (it should use the `.gdbinit` file inside the `prj_rp2350` project folder):

```bash
arm-none-eabi-gdb -iex "add-auto-load-safe-path ./" ./bin/*.elf
```
 
 Normally you should have the capacity to debug now. the  `.gdbinit` file should have already connect to `openOCD`, load program on target's flash and be ready to run.


### On VSCode

To configure a debug session on VSCode with `openOCD` and Raspberry Debug Probe, I had to modify the `launch.json` file of VSCode to add a configuration as follow:

```json
      {
         "name": "prj_rp2350_debug_openOcd",
         "cwd": "${workspaceRoot}",
         "executable": "${workspaceRoot}/prj_rp2350/bin/prj_rp.elf",
         "request": "launch",
         "type": "cortex-debug",
         "servertype": "external",
         "gdbTarget": "localhost:3333",
         "svdFile": "${workspaceFolder}/../../pico-sdk/src/rp2350/hardware_regs/RP2350.svd",
         "runToEntryPoint": "main",
         "overrideLaunchCommands": [
             "monitor reset init"
         ]
      },
```


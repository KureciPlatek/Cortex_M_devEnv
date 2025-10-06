
>[!Abstract]
>In this section, we will configure `prj_rp2350` project to run an `RTOS` on it, configure and run Wi-Fi communication with the `CYW43` chip available on `pico2_w` board and publish/subscribe to `MQTT` broker and services 

>[!TIP] Prerequisite
>You configured the `rp2xxx` development environment, as explained in [[About the Pico-SDK]]. We will compile and run program here.

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


Warning, clone of official repository of FreeRTOS does not necessarily contains all required ports:
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

Be careful to give the same path to passwd file as in `mosquitto.conf` file. Give the user name you want in `<new_user>`
Then, when hitting ENTER, `mosquitto_passwd` will ask you to give a password for this user, simple as that.

### `Mosquitto` troubleshooting

`Mosquitto` can present some strange problems when not cleanly set down and Rpi5 is shut down. It sends afterwards a magnificently useless message when trying to start/restart` mosquitto.service` with `systemctl`:

```bash
jeremie@jeremDev:~/work/prj_blog/prj_rp2350 $ sudo systemctl start mosquitto.service
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

#todo


# `MQTT` with `pico-sdk`


### Configure and initialize `MQTT` on `RP2350` with `pico-sdk` 


### API's important functions
# ESP32 Wake On Lan

## Introduction
In this project I used an ESP32 to send Wake on Lan packet to power on my Computer. There is a web server where you can send WoL packet and change target MAC address. And for the laziest people I add [ESP RainMaker](https://github.com/espressif/esp-rainmaker/tree/master) project to send WoL packet from an assistant (like Google Home or Alexa).


## Requirements
#### Hardware
- Esp32
- Computer with Windows/Linux/MacOS (to build and setup) and/or Computer with Wake on Lan enabled
- Smartphone IOS/Android
- Cable to connect Esp32 to your Computer
#### Software
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- Python3, CMake, Ninja... (required by ESP-IDF)
- Git
- [ESP RainMaker app](https://github.com/espressif/esp-rainmaker/tree/master) (available in App Store and Play Store)



## Get Started
### Get esp32-WOL

``` bash
git clone --recursive https://github.com/filipporaciti/esp32-WOL.git
cd esp32-WOL
```
### Setup
- #### Target MAC address (Wake on Lan)
You can configure it from the integrated website (after build) or from menuconfig; from terminal run `idf.py menuconfig` &rarr; `WoL Configuration` &rarr; `Destination MAC address`

- #### WiFi SSID and password
You'll configure it when you have to connect your ESP32 with ESP RainMaker app
### Build
``` bash
idf.py build
```
### Flash and Monitor
``` bash
idf.py flash monitor
```
### Connect ESP32 to ESP RainMaker app
If you can't see the QR code in your terminal, you can try to run `idf.py erase-flash` before flash the firmware on ESP32.

To connect ESP RainMaker app with your ESP32 it's just enought to open ESP RainMaker app, push '+' button on top-right and scan the QR code; here you can configure SSID and password thar ESP32 takes to connect to the network.
### Connect ESP RainMaker to Google Home and Alexa
- #### Google Home
Open Google Home app &rarr; Settings &rarr; Works with Google &rarr; Search ESP RainMaker and log in with your account.
- #### Alexa
Open Alexa app &rarr; More &rarr; Things to Try &rarr; Search ESP RainMaker, enable it and log in with your account.
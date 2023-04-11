> ![News 90s GIF](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/news.gif) The [version 1.1.0 was released](https://github.com/jnthas/clockwise/releases/tag/v1.1.0) supporting Improv WiFi and a settings page to set up timezone, RGB/RBG pins, display bright and the 12/24 hours format.


[![Clockwise CI/CD](https://github.com/jnthas/clockwise/actions/workflows/clockwise-ci.yml/badge.svg)](https://github.com/jnthas/clockwise/actions/workflows/clockwise-ci.yml)

![Logo](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/clockwise_logo.png "Logo")

> The DIY smart wall clock device

Clockwise was an idea I had while working with 64x64 LED matrices.
These displays are about the size of a wall clock and with the ESP32, besides controlling the content presented on the display we also gain the functionality of 
WiFi, Bluetooth, touch buttons and other sensors, which gives us basically a smart wall clock. 
From there I started to develop a platform to create the _Clockfaces_, or skins that the clock can have. The possibilities are many and I hope that with help from contributors, we can grow the options even more.

### ⏰ Do you want to create a new cool Clockface?
Start here https://github.com/jnthas/cw-cf-0x00


## Available clockfaces

## Mario Bros. Clock

![Mario Bros. Clockface](https://github.com/jnthas/cw-cf-0x01/blob/main/cf_0x01_thumb.jpg "Mario Bros. Clockface")

https://github.com/jnthas/cw-cf-0x01


## Time in Words

![Time in Words Clockface](https://github.com/jnthas/cw-cf-0x02/blob/main/cf_0x02_thumb.jpg "Time in Words Clockface")

https://github.com/jnthas/cw-cf-0x02

## World Map Clock

![World Map Clockface](https://github.com/jnthas/cw-cf-0x03/blob/main/cf_0x03_thumb.jpg "World Map Clockface")

https://github.com/jnthas/cw-cf-0x03

## Castlevania Clock Tower

![Castlevania Clockface](https://github.com/jnthas/cw-cf-0x04/blob/main/cf_0x04_thumb.jpg "Castlevania Clockface")

https://github.com/jnthas/cw-cf-0x04

## Pacman

![Pacman Clockface](https://github.com/jnthas/cw-cf-0x05/blob/main/cf_0x05_thumb.jpg "Pacman Clockface")

https://github.com/jnthas/cw-cf-0x05


## Driving the led matrix

The three main hardware components of Clockwise are: 
- HUB75/HUB75E compatible LED matrix 64x64
- an ESP32; and 
- a power supply of 3A+

With these components in hand, just follow the wiring instructions according to the library used, by default Clockwise uses the [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA#2-wiring-esp32-with-the-led-matrix-panel) but any Adafruit GFX compatible library should work. The default wiring connection is showed below.

![ESP32-HUB75-MatrixPanel-I2S-DMA wiring](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/display_esp32_wiring_thumb.png "ESP32-HUB75-MatrixPanel-I2S-DMA wiring")

[Full size](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/display_esp32_wiring_bb.png)

- In case you want something ready to use, I recommend Brian Lough's [ESP32 Trinity](https://github.com/witnessmenow/ESP32-Trinity), basically it's connecting the board and uploading the firmware, as simple as that.
- If you want a designed PCB, I recommend this project from @Alexvanheu. It's compatible with HUB75/HUB75E led matrices and already tested with Clockwise https://github.com/Alexvanheu/Mario-Clock-PCB-ESP32


## How to change the clockface (web flashing)

1) Go to https://clockwise.page/ and select the desired clockface
2) Connect the ESP32 device on your computer's USB port 
3) Click on the Flash button
4) A dialog will appear, select the correct USB port and click in Connect ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step1.png))
5) Select the INSTALL and INSTALL again ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step2.png))
6) Wait while the flash tool uploads the firmware and finish ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step3.png))
7) From the version 1.1.0, click in NEXT on step 6, Improv will start looking for available WiFi networks to connect
8) Select your local network (must be a 2.4GHz) and enter with your password ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step4.png))
9) If connection was successful, a message with button VISIT DEVICE will pop up and you can visit the Clockwise setting page  ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step5.png))


### Configuring only WiFi
After flashing your clockface, you will have a step to configure the WiFi. But in case you change your access point or password, you can set up just the WiFi connecting the Clockwise on USB, opening https://clockwise.page and clicking in Flash button, a window will pop up with a few options where you can re-configure your WiFi network ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step6.png)) as well as open the Settings page to change preferences using button VISIT DEVICE. Remember: it is important to use a 2.4GHz WiFi, it will not work on 5GHz.


### Settings page
The settings page have the following options
- *Timezone*: The timezone must be in the format America/New_York, America/Sao_Paulo, Europe/Paris, Asia/Dubai, etc. so that the clock can connect to an NTP server to get the correct time.
- *Swap Blue/Green pins*: Some displays have the RGB order different, in this case RBG. You can use this options to change the order.
- *Display Bright*: Change the display bright.
- *Use 24h format*: You can choose between 20:00 or 8:00PM in your device.


## How to change the clockface (PlatformIO)

Clockwise uses PlatformIO as IDE, so the configuration is already done if you use the same. The Clockwise structure consists mainly of three folders
- clockfaces: contains the collection of available clockfaces. This folder is not included when compiling
- lib: contains the basic code for Clockwise to work and in addition a symbolic link to the current clockface
- src: contains the entry point for the clock code

```
.
├── clockfaces
│   ├── cw-cf-0x01
│   ├── cw-cf-0x02
│   └── cw-cf-0x03
├── lib
│   ├── cw-commons
│   ├── cw-gfx-engine
│   └── timeinwords -> ../clockfaces/cw-cf-0x02/
└── src
    └── main.cpp

```
To create the symbolic link run the following command inside lib/ folder:

``clockwise/firmware/lib$ ln -s ../clockfaces/cw-cf-0x02/ timeinwords``

Or, if you prefer, you can get the same result by copying the desired clockface folder into lib/

The same way as web flashing, when connecting for the first time you will have to configure the wifi, follow the instructions in Configuring WiFi section above. 

## How to change the clockface (esp-idf)

You can use the [official Esspressif IoT Development Framekwork (aka esp-idf)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/) to build and upload this project to an ESP32 device, including the [ESP32-Trinity board](https://esp32trinity.com/).

### Install esp-idf
Follow the [Step By Step installation instructions](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/#installation-step-by-step).

### Setup the environment variables
Follow the [instructions here](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/#step-4-set-up-the-environment-variables).

### Clone and build this project
* `git clone --recurse-submodules https://github.com/jnthas/clockwise.git`
* `idf.py reconfigure`
* `idf.py menuconfig` (select `Clockwise Configuration` and choose the clockface)
* `idf.py flash`
* `idf.py monitor`

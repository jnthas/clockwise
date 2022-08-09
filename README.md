[![Clockwise CI/CD](https://github.com/jnthas/clockwise/actions/workflows/clockwise-ci.yml/badge.svg)](https://github.com/jnthas/clockwise/actions/workflows/clockwise-ci.yml)

![Logo](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/clockwise_logo.png "Logo")

> The DIY smart wall clock device

Clockwise was an idea I had while working with 64x64 LED matrices.
These displays are about the size of a wall clock and with the ESP32, besides controlling the content presented on the display we also gain the functionality of 
WiFi, Bluetooth, touch buttons and other sensors, which gives us basically a smart wall clock. 
From there I started to develop a platform to create the _Clockfaces_, or skins that the clock can have. The possibilities are many and I hope that with help from contributors, we can grow the options even more.

Currently there are three clockface options:

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

In case you want something ready to use, I recommend Brian Lough's [ESP32 Trinity](https://github.com/witnessmenow/ESP32-Trinity), basically it's connecting the board and uploading the firmware, as simple as that.


## How to change the clockface (web flashing)

1) Go to https://clockwise.page/ and select the desired clockface
2) Connect the ESP32 device on your computer's USB port 
3) Click on the Flash button
4) A dialog will appear, select the correct USB port and click in Connect ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step1.png))
5) Select the INSTALL and INSTALL again ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step2.png))
6) Wait while the flash tool uploads the firmware and finish ([screenshot](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/usb-step3.png))

### Configuring WiFi
The first time you run it, you need to configure the WiFi, for that connect to the `Clockwise-Wifi` access point using the password `12345678` via your smartphone or laptop, click “Configure WiFi” and select your AP, put in your password and your timezone and save it. The timezone must be one of the listed here e.g. America/New_York, America/Sao_Paulo, Europe/Paris, Asia/Dubai, etc. so that the clock can connect to an NTP server to get the correct time. It is important to use a 2.4GHz WiFi, it will not work on 5GHz.

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

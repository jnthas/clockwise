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


## Driving the led matrix

To control the LED matrix, you only need an ESP32, which must follow the wiring instructions according to the library used, by default Clockwise uses [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA#2-wiring-esp32-with-the-led-matrix-panel) but any Adafruit GFX compatible library should work.

![ESP32-HUB75-MatrixPanel-I2S-DMA wiring](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/display_esp32_wiring_thumb.png "ESP32-HUB75-MatrixPanel-I2S-DMA wiring")

[Full size](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/display_esp32_wiring_bb.png)



In case you want something ready to use, I recommend Brian Lough's [ESP32 Trinity](https://github.com/witnessmenow/ESP32-Trinity), basically it's connecting the board and uploading the firmware, as simple as that.


## How to change the clockface

Use PlatformIO to run Clockwise. The Clockwise structure consists mainly of three folders
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

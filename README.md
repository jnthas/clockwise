# Clockwise

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


## How to change the clockface?

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
To create the symbolic link run the command in the project root:

``ln -s firmware/clockfaces/cw-cf-0x02/ firmware/lib/timeinwords``

Or, if you prefer, you can get the same result by copying the desired clockface folder into lib/

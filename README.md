![News GIF](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/news.gif) **[Latest version 1.4.2 released!](https://github.com/jnthas/clockwise/releases/tag/v1.4.2)** | [See full change log](https://github.com/jnthas/clockwise/blob/main/CHANGELOG.md#142---2024-04-21)

![Clockwise Logo](https://github.com/jnthas/clockwise/blob/gh-pages/static/images/clockwise_logo.png "Clockwise Logo")

---

**Clockwise** is an open-source smart wall clock that you can easily build yourself.  
It only needs:

- 64x64 RGB LED Matrix (HUB75 or HUB75E)
- ESP32 dev board
- 5V 3A power supply
- Jumpers

To simplify assembly, you can also use the [**WiseShield-32 DIY PCB kit**](https://www.elecrow.com/clockwise-diy-kit.html) — created in partnership with Elecrow especially for Clockwise.

---

## Features

- Real-time clock with customizable themes ("Clockfaces")
- Web-based interface for configuration
- Open-source hardware and firmware
- Compatible with various PCBs or just simple wiring
- Community-driven

---

## Quick Start

### 1. Required Hardware

If you want to build it from scratch, you will need at least these components below. Follow the instructions on [Wiki]() to assemble it. 
- 64x64 RGB LED matrix (HUB75 or HUB75E)
- ESP32 Dev Board
- 5V 3A power supply
- Jumpers

Alternatively, we created a custom PCB that simplifies this process a lot. The kit includes not only the PCB but all components including sensors to make your clock smarter. Check the [WiseShield-32 PCB kit](https://www.elecrow.com/clockwise-diy-kit.html) out now!

### 2. Flash Firmware

- Go to: [https://clockwise.page](https://clockwise.page)
- Follow the on-screen steps to flash Clockwise and configure WiFi

For detailed flashing steps:  
👉 [See Wiki: Flashing Clockwise](https://github.com/jnthas/clockwise/wiki/Flashing-Clockwise)

---

## Clockfaces Gallery

You can choose from many creative Clockfaces — or make your own:

Mario Bros | Words | World Map | Castlevania | Pacman | Pokedex | Canvas
:--:|:--:|:--:|:--:|:--:|:--:|:--:
[![](https://github.com/jnthas/cw-cf-0x01/blob/main/cf_0x01_thumb.jpg)](https://github.com/jnthas/cw-cf-0x01) | [![](https://github.com/jnthas/cw-cf-0x02/blob/main/cf_0x02_thumb.jpg)](https://github.com/jnthas/cw-cf-0x02) | [![](https://github.com/jnthas/cw-cf-0x03/blob/main/cf_0x03_thumb.jpg)](https://github.com/jnthas/cw-cf-0x03) | [![](https://github.com/jnthas/cw-cf-0x04/blob/main/cf_0x04_thumb.jpg)](https://github.com/jnthas/cw-cf-0x04) | [![](https://github.com/jnthas/cw-cf-0x05/blob/main/cf_0x05_thumb.jpg)](https://github.com/jnthas/cw-cf-0x05) | [![](https://github.com/jnthas/cw-cf-0x06/blob/main/cf_0x06_thumb.jpg)](https://github.com/jnthas/cw-cf-0x06) | [![](https://github.com/jnthas/cw-cf-0x07/raw/main/cf_0x07_thumb.jpg)](https://github.com/jnthas/cw-cf-0x07)

> Canvas is a special type of Clockface that is capable of rendering different themes described in a JSON file. More about **Canvas Clockface**: [Wiki page](https://github.com/jnthas/clockwise/wiki/Canvas-Clockface)

---

## How to Build

You can assemble Clockwise in three different ways:

1. **Web Flashing** — the easiest way (recommended)  
    👉 [Wiki: Web Flashing Guide](https://github.com/jnthas/clockwise/wiki/Flashing-Clockwise)

2. **PlatformIO** — advanced usage for developers  
    👉 [Wiki: Building with PlatformIO](https://github.com/jnthas/clockwise/wiki/Building-with-PlatformIO)

3. **ESP-IDF** — for full control  
    👉 [Wiki: Building with ESP-IDF](https://github.com/jnthas/clockwise/wiki/Building-with-ESP-IDF)

---

## Advanced Configuration

Clockwise offers a flexible set of options that you can adjust through its built-in Settings page:

* **WiFi settings** — easily update your network details without reflashing
* **Timezone & NTP server** — sync the clock accurately for your region
* **Display brightness** — manual or automatic control (with optional LDR sensor)
* **Display rotation** — adjust orientation to match your wall setup
* **Clockface selection** — switch between installed Clockfaces or use Canvas themes
* **RGB order correction** — fix color mismatch on certain LED matrices
* **POSIX timezone string** — fine-tune timezone behavior

👉 See the full guide here: [Advanced Configuration Wiki](https://github.com/jnthas/clockwise/wiki/Settings)

---

## Community & Contributions

- Join the [Clock Club](https://github.com/jnthas/clock-club) to create your own Clockfaces
- Visit [Embedded Rocks](https://embedded.rocks) for news & community
- Contributions and pull requests are welcome!  
👉 [Contributing Guide](https://github.com/jnthas/clockwise/wiki/Contributing)

---

## License

Clockwise is released under the MIT License.

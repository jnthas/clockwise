# AGENTS.md - Clockwise GitHub Pages

## Project Overview
This repository hosts the official GitHub Pages site for **Clockwise** ([clockwise.page](https://clockwise.page)), an open-source smart wall clock powered by an ESP32 and a 64x64 RGB LED matrix (HUB75).

Beyond static documentation, this site functions as an **in-browser firmware flashing portal** enabling users to flash different clockfaces to their ESP32 devices via Web Serial.

---

## Architecture & Key Components

### 1. Web Firmware Flashing ("Flash self-service")
* **Library:** [ESP Web Tools](https://esphome.github.io/esp-web-tools/) (`<esp-web-install-button>`), loaded via CDN in `_includes/head-custom.html`.
* **Flow:** 
  1. The user selects a clockface radio button on `index.md`.
  2. JavaScript dynamically updates the `manifest` attribute on `<esp-web-install-button>`.
  3. Clicking "CONNECT" initiates Web Serial flashing (requires HTTPS & desktop Chromium-based browser).
* **Firmware Structure (`static/firmware/`):**
  * `base_firmware/`: Shared ESP32 boot binaries (`bootloader_dio_40m.bin` @ `0x1000`, `partitions.bin` @ `0x8000`, `boot_app0.bin` @ `0xe000`).
  * `cw-cf-0x<id>/`: Individual clockface folders containing `firmware.bin` (@ `0x10000`) and `manifest.json`.
  * `manifest.json`: Defines chip family (`ESP32`) and relative binary offsets.

### 2. Documentation & Pages
* `index.md`: Landing page, firmware web flasher, hardware parts overview, video demos, and project introduction.
* `welcome.md`: Onboarding guide for the WiseShield-32 DIY Kit (BOM, preparation, assembly roadmap).
* `docs/soldering-guide.md`: Step-by-step soldering manual for the WiseShield-32 PCB.
* `docs/troubleshooting-guide.md`: Hardware assembly and runtime diagnosis.
* `docs/api.md` & `docs/openapi.yml`: Reference documentation for the Clockwise ESP32 device's built-in HTTP control API (`/`, `/get`, `/read`, `/set`, `/restart`).

### 3. Site Setup & Tech Stack
* **Engine:** Jekyll with `jekyll-theme-cayman` and `github-pages` gem.
* **Layout & Style:** `_layouts/default.html`, `_includes/head-custom.html`, and `assets/css/style.scss`.
* **Custom Domain:** `CNAME` points to `clockwise.page`.

---

## Local Development

```bash
# Ensure user gem path is loaded
export GEM_HOME="$HOME/gems"
export PATH="$HOME/gems/bin:$PATH"

# Serve locally with live reload
bundle exec jekyll serve --livereload
```
*Site runs at `http://localhost:4000`.*

---

## Instructions for LLM Agents Modifying This Repo

1. **Adding/Updating Clockfaces:**
   - Create/update folder under `static/firmware/cw-cf-0x<id>/`.
   - Keep `manifest.json` pointing to `../base_firmware/` offsets and local `firmware.bin`.
   - Update `index.md` radio selector group and thumbnail image links.
2. **Preserving Flashing Functionality:**
   - Do not remove or break `<esp-web-install-button>` attributes or the `handleClick()` script in `index.md`.
   - ESP Web Tools requires relative path manifests and HTTPS in production.
3. **Docs & API Changes:**
   - When modifying device API documentation, update both `docs/api.md` and `docs/openapi.yml` in tandem.

set(CLOCKFACES cw-cf-0x01 cw-cf-0x02 cw-cf-0x03 cw-cf-0x04 cw-cf-0x05 cw-cf-0x06)

idf_component_register(
			SRC_DIRS "."
			INCLUDE_DIRS ${PROJECT_DIR}/firmware/src
			PRIV_REQUIRES ESP32-HUB75-MatrixPanel-I2S-DMA cw-commons cw-gfx-engine WiFiManager Improv-WiFi-Library arduino Adafruit-GFX-Library ${CLOCKFACES}
                       )

target_compile_options(${COMPONENT_TARGET} PUBLIC -DCW_FW_VERSION="1.2.2" -DCW_FW_NAME="Clockwise")

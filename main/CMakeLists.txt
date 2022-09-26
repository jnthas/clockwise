set(SRC_DIRS)
set(INCLUDE_DIRS)

# WiFiManager
# This can be removed if https://github.com/tzapu/WiFiManager/pull/1489 is merged in.
set(SRC_DIRS ${SRC_DIRS} "${PROJECT_DIR}/components/WiFiManager")
set(INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_DIR}/components/WiFiManager")

set(CLOCKFACES cw-cf-0x01 cw-cf-0x02 cw-cf-0x03 cw-cf-0x04 cw-cf-0x05)

idf_component_register(
			SRC_DIRS "." ${SRC_DIRS}
			INCLUDE_DIRS ${PROJECT_DIR}/firmware/src ${INCLUDE_DIRS}
			# Add ezTime and WiFiManager once they are merged.
			REQUIRES ESP32-HUB75-MatrixPanel-I2S-DMA cw-commons cw-gfx-engine ${CLOCKFACES}
                       )

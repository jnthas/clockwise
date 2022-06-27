## The DIY smart wall clock device

Clockwise was an idea I had while working with 64x64 LED matrices. These displays are about the size of a wall clock and with the ESP32, besides controlling the content presented on the display we also gain the functionality of WiFi, Bluetooth, touch buttons and other sensors, which gives us basically a smart wall clock. 


<esp-web-install-button manifest="static/firmware/cw-cf-0x02/manifest.json">
  
  <button slot="activate">Time in Words</button>
  <span slot="unsupported">Ah snap, your browser does not support WebSerial API! If you are using a mobile browser, this is expected. Please, move to the desktop version.</span>
  <span slot="not-allowed">Ah snap, you are not allowed to use this on HTTP!</span>
</esp-web-install-button>
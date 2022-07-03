## Flash self-service
Here in this section you can upload the desired clockface. A step-by-step how to flash your device can be seen [here](https://github.com/jnthas/clockwise/blob/main/README.md#how-to-change-the-clockface-web-flashing). This feature uses the [ESP Web Tools](https://esphome.github.io/esp-web-tools/) library.

<fieldset>
  <legend>Clockface library</legend>
  <label>
    <input type="radio" name="clockface" value="cw-cf-0x01" checked onclick="handleClick(this);">
    <img id="cw-cf-0x01" src="https://github.com/jnthas/cw-cf-0x01/raw/main/cf_0x01_thumb.jpg" width="128" alt="Mario Bros.">
  </label>
  <label>
    <input type="radio" name="clockface" value="cw-cf-0x02" onclick="handleClick(this);">
    <img id="cw-cf-0x02" src="https://github.com/jnthas/cw-cf-0x02/raw/main/cf_0x02_thumb.jpg" width="128" alt="Time in words">
  </label>
  <label>
    <input type="radio" name="clockface" value="cw-cf-0x03" onclick="handleClick(this);">
    <img id="cw-cf-0x03" src="https://github.com/jnthas/cw-cf-0x03/raw/main/cf_0x03_thumb.jpg" width="128" alt="World map">
  </label>
</fieldset>

<!-- Flash button -->
<div id="esp-web-tool-widget">
  <esp-web-install-button id="esp-web" manifest="">
    <button id="flash-button" slot="activate"></button>
    <span slot="unsupported">Ah snap, your browser does not support WebSerial API! If you are using a mobile browser, this is expected. Please, move to the desktop version.</span>
    <span slot="not-allowed">Ah snap, you are not allowed to use this on HTTP!</span>
  </esp-web-install-button>
</div>

## Hardware
You will need only three components to make a Clockwise device, they are easily found on stores like Amazon and Aliexpress. More details [here](https://github.com/jnthas/clockwise/tree/main#driving-the-led-matrix).


## About

Clockwise was an idea I had while working with 64x64 LED matrices. These displays are about the size of a wall clock and with the ESP32, besides controlling the content presented on the display we also gain the functionality of WiFi, Bluetooth, touch buttons and other sensors, which gives us basically a smart wall clock. 


<script>
  function handleClick(radio) {
    document.getElementById("esp-web").setAttribute("manifest", "static/firmware/" + radio.value + "/manifest.json"); 
    document.getElementById("flash-button").textContent = "Flash '" + document.getElementById(radio.value).alt + "'"; 
  }

  handleClick(document.querySelector('input[name="clockface"]:checked'));
</script>

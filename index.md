> ![News 90s GIF](static/images/news.gif)The [version 1.1.0 was released](https://github.com/jnthas/clockwise/releases/tag/v1.1.0) supporting Improv WiFi and a settings page to set up timezone, RGB/RBG pins, display bright and the 12/24 hours format.

## Flash self-service
Here in this section you can upload the desired clockface to your device. A step-by-step how to flash the Clockwise can be seen [here](https://github.com/jnthas/clockwise/blob/main/README.md#how-to-change-the-clockface-web-flashing). This feature uses the [ESP Web Tools](https://esphome.github.io/esp-web-tools/) library.


<fieldset>
  <legend>📚 Clockface Library</legend>

  <div class="container-radio">
  <label>
    <input type="radio" name="clockface" value="cw-cf-0x01" onclick="handleClick(this);" checked>
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
  <label>
    <input type="radio" name="clockface" value="cw-cf-0x04" onclick="handleClick(this);">
    <img id="cw-cf-0x04" src="https://github.com/jnthas/cw-cf-0x04/raw/main/cf_0x04_thumb.jpg" width="128" alt="Castlevania">
  </label>
  <label>
    <input type="radio" name="clockface" value="cw-cf-0x05" onclick="handleClick(this);">
    <img id="cw-cf-0x05" src="https://github.com/jnthas/cw-cf-0x05/raw/main/cf_0x05_thumb.jpg" width="128" alt="Pacman">
  </label>
  <label>
    <input type="radio" name="clockface" value="cw-cf-0x06" onclick="handleClick(this);">
    <img id="cw-cf-0x06" src="https://github.com/jnthas/cw-cf-0x06/raw/main/cf_0x06_thumb.jpg" width="128" alt="Pokedex">
  </label>
  </div>

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

Clockwise was an idea I had while working with 64x64 LED matrices. These displays are about the size of a wall clock and with the ESP32, besides controlling the content presented on the display we also gain the functionality of WiFi, Bluetooth, touch buttons and other sensors, which gives us basically a smart wall clock. I wrote a post for [instructables](https://www.instructables.com/Mario-Bros-Clock/).

<img class="center" alt="Clockwise on a shelf" src="static/images/clockwise-shelf.jpg"/>


<script>
  function handleClick(radio) {
    document.getElementById("esp-web").setAttribute("manifest", "static/firmware/" + radio.value + "/manifest.json"); 
    document.getElementById("flash-button").textContent = "📤 UPLOAD '" + document.getElementById(radio.value).alt + "'"; 
  }

  handleClick(document.querySelector('input[name="clockface"]:checked'));
</script>

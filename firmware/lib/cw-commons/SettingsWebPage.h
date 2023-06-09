#pragma once

#include <Arduino.h>

const char SETTINGS_PAGE[] PROGMEM = R""""(
<!DOCTYPE html>
<html>
<title>Clockwise Settings</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<link rel="shortcut icon" type="image/x-icon"
  href="https://github.com/jnthas/clockwise/raw/gh-pages/static/images/favicon.png">

<body>
  <div class="w3-container" style="background-image: linear-gradient(120deg, #155799, #159957);">
    <img class="w3-animate-zoom w3-padding w3-image"
      src="data:image/png;base64,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" alt="Logo"
      style="width:20%">
  </div>

  <div class="w3-bar w3-black w3-medium">
    <div id="fw-version" class="w3-bar-item w3-black w3-hover-red"></div>
    <div id="ssid" class="w3-bar-item w3-hover-blue w3-right"></div>
    <div class="w3-bar-item w3-button w3-hover-yellow w3-right" onclick="restartDevice();"><i class='fa fa-power-off'></i> Restart</div>
    <div id="status" class="w3-bar-item w3-green" style="display:none"><i class='fa fa-floppy-o'></i> Saved! Restart your device</div>
  </div>

  <div class="w3-row-padding w3-padding">
    <div id="base" class="w3-col s3 m3 s12" style="display: none;">
      <div class="w3-card-4 w3-margin-bottom">
        <header class="w3-container w3-blue-gray">
          <h4 id="title">{{TITLE}}</h4>
        </header>
        <div class="w3-container">
          <p style="min-height: 45px;" id="description">{{DESCRIPTION}}</p>
          <hr>
          <div class="w3-row w3-section">
            <div class="w3-col" style="width:50px"><i id="icon" class="w3-xxlarge w3-text-dark-grey fa"></i>
            </div>
            <div class="w3-rest" id="formInput">
              {{FORM_INPUT}}
            </div>
          </div>
        </div>
        <button id="cardButton" class="w3-button w3-block w3-light-blue">Save</button>
      </div>
    </div>
  </div>
  <script>
    function createCards(settings) {
      console.log(settings);
      const cards = [
        {
          title: "Display Bright",
          description: "0 = dark (display off) / 255 = super bright | Value: <strong><output id='rangevalue'>" + settings.displaybright + "</output></strong>",
          formInput: "<input class='w3-input w3-border' type='range' min='0' max='255' value='" + settings.displaybright + "' class='slider' id='bright' oninput='rangevalue.value=value'>",
          icon: "fa-adjust",
          save: "updatePreference('displayBright', bright.value)",
          property: "displayBright"
        },
        {
          title: "Use 24h format?",
          description: "Changes the hour format to show 20:00 instead of 8:00PM",
          formInput: "<input class='w3-check' type='checkbox' id='use24h' " + (settings.use24hformat == '1' ? "checked" : "") + "><label for='use24h'> Yep</label>",
          icon: "fa-clock-o",
          save: "updatePreference('use24hFormat', Number(use24h.checked))",
          property: "use24hFormat"
        },
        {
          title: "Swap Blue/Green pins?",
          description: "Swap Blue and Green pins because the panel is RBG instead of RGB",
          formInput: "<input class='w3-check' type='checkbox' id='swapBG' " + (settings.swapbluegreen == '1' ? "checked" : "") + "><label for='swapBG'> Yep</label>",
          icon: "fa-random",
          save: "updatePreference('swapBlueGreen', Number(swapBG.checked))",
          property: "swapBlueGreen"
        },
        {
          title: "Timezone",
          description: "Consult your TZ identifier <a href='https://en.wikipedia.org/wiki/List_of_tz_database_time_zones'>here.</a> Examples: America/Sao_Paulo, Europe/Lisbon",
          formInput: "<input id='tz' class='w3-input w3-light-grey' name='tz' type='text' placeholder='Timezone' value='" + settings.timezone + "''>",
          icon: "fa-globe",
          save: "updatePreference('timeZone', tz.value)",
          property: "timeZone"
        },
        {
          title: "NTP Server",
          description: "Configure your prefered NTP Server. You can use one of the <a href='https://www.ntppool.org'>NTP Pool Project</a> pools or a local one.",
          formInput: "<input id='ntp' class='w3-input w3-light-grey' name='ntp' type='text' placeholder='NTP Server' value='" + settings.ntpserver + "''>",
          icon: "fa-server",
          save: "updatePreference('ntpServer', ntp.value)",
          property: "ntpServer"
        },
        {
          title: "Automatic Bright",
          description: "Inform the values read by the LDR when the room is dark (min value) and bright (max value). Range 0 - 4095",
          formInput: "<input id='autoBrightMin' class='w3-input w3-light-grey w3-cell w3-margin-right' name='autoBrightMin' style='width:45%;' type='number' min='0' max='4095' placeholder='Min value' value='" + settings.autobrightmin + "''>" + 
                     "<input id='autoBrightMax' class='w3-input w3-light-grey w3-cell' name='autoBrightMax' style='width:45%;' type='number' min='0' max='4095' placeholder='Max value' value='" + settings.autobrightmax + "''>",
          icon: "fa-sun-o",
          save: "updatePreference('autoBright', autoBrightMin.value.padStart(4, '0') + ',' + autoBrightMax.value.padStart(4, '0'))",
          property: "autoBright"
        }
      ];

      var base = document.querySelector('#base');
      cards.forEach(c => {

        var clone = base.cloneNode(true);
        clone.id = c.property;
        clone.removeAttribute("style");

        Array.prototype.slice.call(clone.getElementsByTagName('*')).forEach(e => {
          e.id = e.id + "-" + c.property;
        });

        base.before(clone);
        document.getElementById("title-" + c.property).innerHTML = c.title
        document.getElementById("description-" + c.property).innerHTML = c.description
        document.getElementById("formInput-" + c.property).innerHTML = c.formInput
        document.getElementById("icon-" + c.property).classList.add(c.icon);
        document.getElementById("cardButton-" + c.property).setAttribute("onclick", c.save);
      })

      document.getElementById("ssid").innerHTML = "<i class='fa fa-wifi'></i> " + settings.wifissid
      document.getElementById("fw-version").innerHTML = "<i class='fa fa-code-fork'></i> Firmware v" + settings.cw_fw_version
    }

    function updatePreference(key, value) {
      const xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status >= 200 && this.status < 299) {
          document.getElementById('status').style.display = 'block';
        }
      };
      xhr.open('POST', '/set?' + key + '=' + value);
      xhr.send();

      setTimeout(() => {
        document.getElementById('status').style.display = 'none';
      }, 2000);
    }

    function begin() {
      var xmlhttp = new XMLHttpRequest();
      xmlhttp.onreadystatechange = function () {
        if (this.readyState === 2 && this.status === 204) {
          const headers = this.getAllResponseHeaders().trim().split(/[\r\n]+/);
          const headerMap = {};
          headers.forEach((line) => {
            const parts = line.split(": ");
            const header = parts.shift().substring(2);
            const value = parts.join(": ");
            headerMap[header] = value;
          });
          createCards(headerMap);
        }
      };
      xmlhttp.open("GET", "/get", true);
      xmlhttp.send();
    }

    function restartDevice() {
      const xhr = new XMLHttpRequest();
      xhr.open('POST', '/restart');
      xhr.send();
    }

    //Local
    //createCards({ "displayBright": 30, "swapBlueGreen": 1, "use24hFormat": 0, "timeZone": "Europe/Lisbon", "ntpServer": "pool.ntp.org", "wifiSsid": "test", "autoBrightMin":0, "autoBrightMax":800, "cw_fw_version":"1.2.2" });

    //Embedded
    begin();

  </script>
</body>
</html>
)"""";

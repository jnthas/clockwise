#pragma once

#include <Arduino.h>



typedef struct SettingsCards {
  const char* title;
  const char* description;
  const char* formInput;
  const char* icon;
  const char* propName;
} SettingsCard;

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
    <div class="w3-container">
        <img src="https://github.com/jnthas/clockwise/raw/gh-pages/static/images/clockwise_logo.png" alt="Logo"
            class="w3-image w3-padding" style="width:50%;max-width:600px">
    </div>

    <div class="w3-panel w3-black">
        <h3>.:: Settings Page</h3>
    </div>
    <div class="w3-row-padding w3-padding">
        <div id="base" class="w3-col s3 m3 s12" style="display: none;">
            <div class="w3-card-4 w3-margin-bottom">
                <header class="w3-container w3-blue-gray">
                    <h3 id="title">{{TITLE}}</h3>
                </header>
                <div class="w3-container">
                    <p style="min-height: 45px;" id="description">{{DESCRIPTION}}</p>
                    <hr>
                    <div class="w3-row w3-section">
                        <div class="w3-col" style="width:50px"><i id="icon" class="w3-xxlarge w3-text-dark-grey fa"></i></div>
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
                    title:"Display Bright", 
                    description:"0 = dark (display off) / 255 = super bright | Value: <strong><output id='rangevalue'>" + settings.displayBright + "</output></strong>",
                    formInput:"<input class='w3-input w3-border' type='range' min='0' max='255' value='" + settings.displayBright + "' class='slider' id='bright' oninput='rangevalue.value=value'>",
                    icon:"fa-adjust",
                    save:"updatePreference('displayBright', bright.value)",
                    property:"displayBright"
                },                
                { 
                    title:"Use 24h format?", 
                    description:"Changes the hour format to show 20:00 instead of 8:00PM",
                    formInput:"<input class='w3-check' type='checkbox' id='use24h' " + (settings.use24hFormat == 1 ? "checked" : "") + "><label for='use24h'> Yep</label>",
                    icon:"fa-calendar",
                    save:"updatePreference('use24hFormat', Number(use24h.checked))",
                    property:"use24hFormat"
                },
                { 
                    title:"Swap Blue/Green pins?", 
                    description:"Swap Blue and Green pins because the panel is RBG instead of RGB",
                    formInput:"<input class='w3-check' type='checkbox' id='swapBG' " + (settings.swapBlueGreen == 1 ? "checked" : "") + "><label for='swapBG'> Yep</label>",
                    icon:"fa-random",
                    save:"updatePreference('swapBlueGreen', Number(swapBG.checked))",
                    property:"swapBlueGreen"
                },
                { 
                    title:"Timezone", 
                    description:"Consult your TZ identifier <a href='https://en.wikipedia.org/wiki/List_of_tz_database_time_zones'>here.</a> Examples: America/Sao_Paulo, Europe/Lisbon",
                    formInput:"<input id='tz' class='w3-input' name='tz' type='text' placeholder='Timezone' value='"+ settings.timeZone +"''>",
                    icon:"fa-clock-o",
                    save:"updatePreference('timeZone', tz.value)",
                    property:"timeZone"
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
        }

        function updatePreference(key, value) { 
          const xhr = new XMLHttpRequest(); 
          xhr.open('POST', '/set?' + key + '=' + value); 
          xhr.send(); 
        }

        function begin() {
            var xmlhttp = new XMLHttpRequest();
            xmlhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    createCards(JSON.parse(this.responseText));
                }
            };
            xmlhttp.open("GET", "/get", true);
            xmlhttp.send();
        }

        //Local
        //createCards({"displayBright":30,"swapBlueGreen":1,"use24hFormat":0,"timeZone":"Europe/Lisbon","wifiSsid":"test"});
        
        //Embedded
        begin();

    </script>
</body>
</html>
)"""";

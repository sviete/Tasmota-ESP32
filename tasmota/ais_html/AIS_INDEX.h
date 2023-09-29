const char AIS_INDEX[] PROGMEM =
  "<style>a {color:white;text-decoration:none;}</style>"
  "<body>"
  "<center>"
  "<div class=\"card\" style=\"width: 100%%;max-width: 600px;\">"
  "<img class=\"card-img-top\" src=\"https://raw.githubusercontent.com/sviete/Tasmota-ESP32/development/ais_files/ais-smart-home.jpeg\" alt=\"AIS smart home logo\">"
  "<div class=\"card-img-overlay\">"
  "<h5 class=\"card-title\" style=\"text-align:left;color:white\"><span class=\"mdi mdi-memory\"></span><span class=\"mdi mdi-thermometer\"> %s °C</h5>"
  "</div>"

  "<div class=\"card-body\" style=\"padding-top:2em;background-color: #232732;min-height: 9.5em;\">"
  "<div class=\"row row-cols-2 row-cols-md-2 g-4\">"
  "<div class=\"col\">"
  "<div class=\"card\" style=\"background-color:#486397;min-height: 9.5em;\">"
  "<a href=\"ah?\">"
  "<img src=\"https://raw.githubusercontent.com/sviete/Tasmota-ESP32/development/ais_files/ha_logo.png\" style=\"width: 4em; margin: 1em;\"></img>"
  "<h5 class=\"card-title\">Home Assistant</h5>"
  "</a>"
  "</div>"
  "</div>"
  "<div class=\"col\">"
  "<div class=\"card\" style=\"background-color:#486397;min-height: 9.5em;\">"
  "<a href=\"az?\">"
  "<img src=\"https://raw.githubusercontent.com/sviete/Tasmota-ESP32/development/ais_files/z2m_logo.png\" style=\"width: 4em; margin: 1em;\"></img>"
  "<h5 class=\"card-title\">Zigbee2Mqtt</h5>"
  "</a>"
  "</div>"
  "</div>"
  "<div class=\"col\">"
  "<div class=\"card\" style=\"background-color:#637faf;min-height: 9.5em;\">"
  "<a href=\"ble?\">"
  "<span class=\"mdi mdi-bluetooth\" style=\"font-size: 4em;color:#3f51b5;\"></span>"
  "<h5 class=\"card-title\">Bluetooth</h5>"
  "</a>"
  "</div>"
  "</div>"
  "<div class=\"col\">"
  "<div class=\"card\" style=\"background-color:#637faf;min-height: 9.5em;\">"
  "<a href=\"am?\">"
  "<img src=\"https://raw.githubusercontent.com/sviete/Tasmota-ESP32/development/ais_files/mqtt_logo.png\" style=\"width: 4em; margin: 1em;\"></img>"
  "<h5 class=\"card-title\">MQTT</h5>"
  "</a>"
  "</div>"
  "</div>"

  "<div class=\"col\">"
  "<div class=\"card\" style=\"background-color:#7b88b1;min-height: 9.5em;\">"
  "<a href=\"a1?\">"
  "<span class=\"mdi mdi-update\" style=\"font-size: 4em;color:#5b0566;\"></span>"
  "<h5 class=\"card-title\">Upgrade</h5>"
  "</a>"
  "</div>"
  "</div>"

  "<div class=\"col\">"
  "<div class=\"card\" style=\"background-color:#7b88b1;min-height: 9.5em;\">"
  "<a href=\"at?\">"
  "<img src=\"https://raw.githubusercontent.com/sviete/Tasmota-ESP32/development/ais_files/terminal_logo.png\" style=\"width: 4em; margin: 1em;\"></img>"
  "<h5 class=\"card-title\">Terminal</h5>"
  "</a>"
  "</div>"
  "</div>"

  "<div class=\"col\">"
  "<div class=\"card\" style=\"background-color: #97a5d6;min-height: 9.5em;\">"
  "<a href=\"ai?\">"
  "<span class=\"mdi mdi-information-outline\" style=\"font-size: 4em;color:#000000\"></span>"
  "<h5 class=\"card-title\">About</h5>"
  "</a>"
  "</div>"
  "</div>"

  "</div>"
  "</div>"

  "<div style=\"text-align: right; padding-right: 2em; z-index: 999999;background-color: #232732;\">"
  "<span class=\"mdi mdi-memory\" style=\"font-size: 2em;\"></span>" 
  "<span style=\"line-height:2em;vertical-align:super;color:white;\">version %s <a href=\"cn?\" style=\"color:black;text-decoration:none;\">&nbsp;&nbsp;.</a></span>"
  "</div>"
  
;

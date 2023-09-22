const char AIS_HA[] PROGMEM =
" <style>"
"    body {"
"  background-color: #272727;"
"  padding: 10px;"
"}"
"button.btn-back {"
"  margin-left:1em;"
"  border-radius: 50%% !important;"
"  background-color: rgba(255, 255, 255, 0.08);"
"  color: white;"
"  border: none !important;"
"  -webkit-transition: background-color 1s, color 1s, -webkit-transform 0.5s;"
"  transition: background-color 1s, transform 0.5s;"
"}"
"button.btn:btn-back {"
"  background-color: rgba(255, 255, 255, 0.4);"
"  color: black;"
"  -webkit-transform: translateX(-5px);"
"  -webkit-box-shadow: 5px 0px 18px 0px rgba(105,105,105,0.8);"
"  -moz-box-shadow: 5px 0px 18px 0px rgba(105,105,105,0.8);"
"  box-shadow: 5px 0px 18px 0px rgba(105,105,105,0.8);"
"}"
"</style>"
"<body >"
"<button onclick=\"window.location.href='/'\" class=\"btn btn-back btn-lg text-center\"><span><span class=\"mdi mdi-arrow-left\" style=\"font-size: large;\"></span></span></button>"
"<center>"  
"  <div class=\"card\" style=\"width: 100%%;max-width: 600px;margin-top: 2em;\">"
"    <span class=\"mdi mdi-home-assistant\" style=\"font-size: 7em; color: #18bbf2;\"></span>"
"    <div class=\"card-body\">"
"      <h5 class=\"card-title\">Connecting to Home Assistant with ZHA</h5>"
"      <p class=\"card-text\">In Home Assistant (requires version 0.113+) go to <b>Configuration</b> - <b>Integrations</b>, click the <b>+</b> icon, search for <b>ZHA</b> integration and select it.</p>"
"    </div>"
"    <ul class=\"list-group list-group-flush\" style=\"text-align: left;\">"
"      <li class=\"list-group-item\">1. For Radio Type choose <b>“ZNP”</b></li>"
"      <li class=\"list-group-item\">2. Choose <b>“software”</b> for ‘data flow control’</li>"
"      <li class=\"list-group-item\">3. Under <b>“Serial device path”</b> enter <code>socket://%s:8888</code> </li>"
"      <li class=\"list-group-item\">3. When the ais adapter is discovered you will get a confirmation message </li>"
"    </ul>"
"    <div class=\"card-body\">"
"      <a href=\"https://ai-speaker.com/docs/ais_zigbee_index\" class=\"card-link\"><span class=\"mdi mdi-web-box\"></span>Documentation</a>"
"      <a href=\"https://github.com/sviete/Tasmota-ESP32\" class=\"card-link\"><span class=\"mdi mdi-github\"></span>Source code</a>"
"    </div>"
"  </div>"
"</center>"
;

const char HEADER[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang='en'> <body> <a href="/" style="text-decoration: none"><h2 class="rainbow-text">RELAY CONTROLLER</h2></a> 
)rawliteral";
const char FOOTER[] PROGMEM = R"rawliteral(
 <div class='wifi-setting'>Update new version: <input type='checkbox' name='update' /></div> <input type='submit' value='Confirm & Save' /> </form> </body> <style> input[type='text'], input[type='number'], select { width: 100%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; font-size: 30px; max-width: 300px; } input[type='submit'] { width: 100%; background-color: #4caf50; color: white; padding: 14px 20px; margin: 30px 0; border: none; border-radius: 4px; font-size: 30px; cursor: pointer; } input[type='checkbox'] { height: 50px; width: 50px; } input[type='submit']:hover { background-color: #45a049; } form { padding: 0px 20px; } h2 { font-size: 60px; text-align: center; } .wifi-setting { display: flex; text-align: left; font-size: 32px; align-items: center; gap: 10px; margin: 15px 0px; padding: 10px; white-space: nowrap; } .switch { position: relative; display: inline-block; width: 96px; height: 54px; } .switch input { opacity: 0; width: 0; height: 0; } .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: 0.4s; transition: 0.4s; } .slider:before { position: absolute; content: ''; height: 42px; width: 42px; left: 6px; bottom: 6px; background-color: white; -webkit-transition: 0.4s; transition: 0.4s; } input:checked + .slider { background-color: #2196f3; } input:focus + .slider { box-shadow: 0 0 1px #2196f3; } input:checked + .slider:before { -webkit-transform: translateX(42px); -ms-transform: translateX(42px); transform: translateX(42px); } .slider.round { border-radius: 54px; } .slider.round:before { border-radius: 50%; } .rainbow-text { background-image: linear-gradient(to left, #0000ff, #ff0000, #00f7ff, #ff0000, #fdc150, #ff0000); -webkit-background-clip: text; color: transparent; } </style> <script> document.getElementById("relay1").onchange = function () { window.location.href = "/status?relay1=toggle"; }; document.getElementById("relay2").onchange = function () { window.location.href = "/status?relay2=toggle"; }; document.getElementById("relay3").onchange = function () { window.location.href = "/status?relay3=toggle"; }; document.getElementById("relay4").onchange = function () { window.location.href = "/status?relay4=toggle"; }; </script> </html>
)rawliteral";

#define AP_SSID "RELAY_ESP_4"
#define AP_PASS "1234567890"
#define REFRESH_TIME 500

uint8_t RELAY_PINS[4] = {16, 14, 12, 13};

const char *currentVersion = "1.1";
const char *serverUrl = "http://52.220.9.202:8266/esp8266-relay-4-channel-firmware.bin";

const char *HW_ID = "HW-1234";
const char *const ssid = "P08";
const char *const password = "1234567890";
const char *const mqtt_server = "42c1410bece7497a99cfa15427041510.s2.eu.hivemq.cloud";
const char *mqttUser = "dien-tu-tieu-hoc";
const char *mqttPassword = "1234567890";
const int mqttPort = 8883;
const byte DNS_PORT = 53;

String getValuebyKey(String input, String key) {
    uint8_t start = input.indexOf(key) + key.length() + 1;
    uint8_t end = input.indexOf(";", start);
    String value = input.substring(start, end);
    return value;
}
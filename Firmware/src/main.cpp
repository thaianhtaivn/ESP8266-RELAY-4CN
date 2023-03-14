#include <ArduinoJson.h>
#include <Arduino.h>
#include "LittleFS.h"
#include <WiFiUdp.h>
#include <PubSubClient.h>
// For server firmware update
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>

// For Internal Header
#include <settings.h>
#include <CertStoreBearSSL.h>

void launchWeb(void);
void setupAP(void);
void createWebServer();

//-------------------------------------
WiFiUDP ntpUDP;
BearSSL::CertStore certStore;
WiFiClientSecure espClient;
PubSubClient *client;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

String file_setting_name = "/FileSetting.txt", settingData = "", content = "";

uint8_t WifiState = 0, number_connection = 0;
unsigned long mainTask, tickTask;
char STREAM_PATH[30];

String SSID, PASSWORD;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect()
{
  // Loop until we’re reconnected
  while (!client->connected())
  {
    Serial.print("Attempting MQTT connection…");
    String clientId = "ESP8266Client - MyClient";
    // Attempt to connect
    // Insert your password
    if (client->connect(clientId.c_str(), "dien-tu-tieu-hoc", "1234567890"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement…
      client->publish("Hello", "hello world");
      // … and resubscribe
      client->subscribe("Hello");
    }
    else
    {
      Serial.print("failed, rc = ");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nBegin program ...");
  if (LittleFS.begin())
  {
    File fileToRead = LittleFS.open(file_setting_name, "r");
    if (fileToRead)
    {
      settingData = "";
      while (fileToRead.available())
      {
        settingData += fileToRead.readString();
      }
    }
  }
  SSID = getValuebyKey(settingData, "SSID");
  PASSWORD = getValuebyKey(settingData, "PASSWORD");

  if (SSID.length() < 2 && PASSWORD.length() < 2)
    WiFi.begin("P08", "1234567890");
  else
    WiFi.begin(SSID, PASSWORD);

  mainTask = millis();
  tickTask = millis();
  setupAP();

  pinMode(2, OUTPUT);
  for (uint8_t i = 0; i < 4; i++)
  {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
  }
  // MQTT Setup
  // int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  // Serial.printf("Number of CA certs read: %d\n", numCerts);
  // if (numCerts == 0)
  // {
  //   Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
  //   return; // Can't connect to anything w/o certs!
  // }

  // BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
  // // Integrate the cert store with this connection
  // bear->setCertStore(&certStore);

  // client = new PubSubClient(*bear);

  // client->setServer(mqtt_server, 8883);
  // client->setCallback(callback);
}

void loop()
{
  dnsServer.processNextRequest();
  server.handleClient();
  if ((unsigned long)(millis() - tickTask) > REFRESH_TIME)
  {
    digitalWrite(2, !digitalRead(2));
    tickTask = millis();
  }
  if (!client->connected())
  {
    reconnect();
  }
  client->loop();
}

void launchWeb()
{
  createWebServer();
  server.begin();
}

void setupAP(void)
{
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSID, AP_PASS);
  dnsServer.start(DNS_PORT, "*", apIP);
  launchWeb();
}
void createWebServer()
{
  server.onNotFound([]()
                    {
        server.sendHeader("Location", String("/"), true);
        server.send(302, "text/plain", ""); });

  server.on("/", []()
            {
              server.sendHeader("Location", "/status");
              server.send(302); });

  server.on("/status", []()
            {
              char html_buff[800];
              uint8_t relay_states[4] = {digitalRead(RELAY_PINS[0]), digitalRead(RELAY_PINS[1]), digitalRead(RELAY_PINS[2]), digitalRead(RELAY_PINS[3])};

              for (uint8_t i = 0; i < 4; i++)
              {
                String arg = server.arg("relay" + String(i + 1));
                if (arg == "toggle")
                {
                  relay_states[i] = !relay_states[i];
                  digitalWrite(RELAY_PINS[i], relay_states[i]);
                }
              }

              char ip_buff[50];
              IPAddress localIP = WiFi.localIP();
              String LOCAL_IP = localIP.toString();
              sprintf(ip_buff, "<div><h3 style='text-align: center'>%s</h3></div>", LOCAL_IP.c_str());

              sprintf(html_buff, "<form action='setting' method='get'> <div class='wifi-setting'>Wifi: <input name='ssid' type='text' value='%s' />Password: <input name='pass' type='text' value='%s' /></div>", SSID.c_str(), PASSWORD.c_str());
              for (uint8_t i = 0; i < 4; i++)
              {
                char relay_buff[180];
                sprintf(relay_buff, "<div class='wifi-setting'> <label>Relay %d</label> <label class='switch'> <input type='checkbox' id='relay%d' %s /> <span class='slider round'></span> </label> </div>", i + 1, i + 1, relay_states[i]?"checked":"");
                strcat(html_buff, relay_buff);
              }

              server.send(200, "text/html", FPSTR(HEADER) + String(ip_buff) + String(html_buff) + FPSTR(FOOTER)); });

  server.on("/setting", []()
            {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        String qupdate = server.arg("update");

        qsid.trim();
        qpass.trim();
        if (qsid.length() > 0 && qpass.length() > 0) {
            File file = LittleFS.open(file_setting_name, "w");
            if (!file) {
                return;
            }
            int bytesWritten = file.printf("UPDATE:%s;SSID:%s;PASSWORD:%s;", qupdate.c_str(), qsid.c_str(), qpass.c_str());
            if (bytesWritten < 1) {
                content = "<h1>Something wrong on writing FS..!</h1>";
                server.sendHeader("Access-Control-Allow-Origin", "*");
                server.send(200, "text/html", content);
                return;
            }
            file.close();

            content = "<h1>Save data sucessfully !!!</h1>";
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "text/html", content);
            delay(50);

            if (qupdate == "on") {
                WiFiClient client;
                delay(100);
                t_httpUpdate_return ret = ESPhttpUpdate.update(client, serverUrl, currentVersion);
                switch (ret) {
                    case HTTP_UPDATE_FAILED:
                        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(),
                                      ESPhttpUpdate.getLastErrorString().c_str());
                        break;
                    case HTTP_UPDATE_NO_UPDATES:
                        Serial.println("HTTP_UPDATE_NO_UPDATES");
                        break;
                    case HTTP_UPDATE_OK:
                        Serial.println("HTTP_UPDATE_OK");
                        break;
                }
            }
            delay(200);
            ESP.reset();
        } else {
            content = "<h1>You need to input your WiFi info !!!</h1>";
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(404, "text/html", content);
        } });
}

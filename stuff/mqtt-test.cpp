
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char *const ssid = "P08";
const char *const password = "1234567890";
const char *const mqtt_server = "42c1410bece7497a99cfa15427041510.s2.eu.hivemq.cloud";
const char *mqttUser = "dien-tu-tieu-hoc";
const char *mqttPassword = "1234567890";
const int mqttPort = 8883;

WiFiClientSecure espClient;
PubSubClient client(espClient);

void reconnect()
{
    while (!client.connected())
    {
        Serial.println("Attempting MQTT connection...");
        String clientId = "ESP8266Client-"; // create client ID
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUser, mqttPassword))
        { // try to connect
            Serial.println("Connected");
            client.subscribe("Hello");
        }
        else
        {
            Serial.println(" Reconnect in 5 seconds");
            delay(5000);
        }
    }
}

void callback(char *topic, byte *payload, int length)
{
    Serial.print("Message received on topic: ");
    Serial.println(topic);

    String message;
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Serial.print("Message content: ");
    Serial.println(message);
    DynamicJsonDocument jsonDoc(1024);
    DeserializationError error = deserializeJson(jsonDoc, message);
    if (error)
    {
        Serial.println("Failed to parse JSON");
        return;
    }
    const char *name = jsonDoc["name"];
    const char *state = jsonDoc["state"];
    Serial.printf("Set %s to %s", name, state);
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    espClient.setInsecure(); // set certificate check off

    client.setServer(mqtt_server, 8883); // set MQTT server settings
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();
    // do some MQTT stuff here...
}
/*
    ESP8266 Based Temperature and Humidity Sensor
*/

#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "ssid.h"   // My real Wifi SSID and password

#ifndef ssid_h
#define ssid_h
const char ssid[] = "ssid";
const char pass[] = "password";
#endif


int pinDHT22 = 2;  //GPIO2 is pin 2, GPIO0 is pin 0
SimpleDHT22 dht22;

unsigned long lastMQTT = 0;
unsigned long delayMQTT = 300000;

// Print wifi details every 30 seconds for the first 10 minutes
unsigned long lastInfo = 0;
unsigned long delayInfo = 30000;
unsigned long maxInfo = 600000;

bool starting = true;

WiFiClient net;
MQTTClient client(512);

void connect() {
  Serial.print("Checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to wifi.");
  Serial.print("Trying MQTT...");
  while (!client.connect("esp1")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to MQTT");
  client.subscribe("$aws/things/mosquittobroker/shadow/+/accepted");
}


void sendReadings() {
  float temperature = 0;
  float humidity = 0;
  byte data[40] = {0};

  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(pinDHT22, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err); delay(2000);
    return;
  }
  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.print("  Humidity:");
  Serial.println(humidity);

  char mqttPayload[100];
  sprintf(mqttPayload, "{\"temp\":%.2f,\"humid\":%.2f}", temperature, humidity);
  client.publish("arduino/esp1/data", mqttPayload);
}

void messageReceived(String &topic, String &payload) {
  Serial.println(topic);
  Serial.println(payload);
}

void sendWifiDetails() {
  byte mac[6];
  char machex[20];
  WiFi.macAddress(mac);
  sprintf(machex, "%2X:%2X:%2X:%2X:%2X:%2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  IPAddress ip;
  ip = WiFi.localIP();

  unsigned long cd = (delayMQTT - (millis() - lastMQTT)) / 1000;

  Serial.print("Mac:");
  Serial.print(machex);
  Serial.print(" IP:");
  Serial.print(ip);
  Serial.print(" time:");
  Serial.print(cd);
  Serial.println(" seconds");
}

void setup()
{
  Serial.begin(115200);
  Serial.print("Initialising Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  client.begin("pi", net);
  client.onMessage(messageReceived);
  connect();
  Serial.println("Ready");
  lastMQTT = 0;
  lastInfo = 0;
}

void loop()
{
  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
    sendWifiDetails();
  }

  if (starting || (millis() - lastMQTT > delayMQTT)) {
    lastMQTT = millis();
    sendReadings();
    starting = false;
  }

  if ((millis() < maxInfo) && (millis() - lastInfo > delayInfo)) {
    lastInfo = millis();
    sendWifiDetails();
  }
}









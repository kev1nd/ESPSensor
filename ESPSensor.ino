/*
    ESP8266 Based Temperature and Humidity Sensor
*/

#include <SPI.h>
#include <SimpleDHT.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "ssid.h"
#ifndef ssid_h
#define ssid_h
const char ssid[] = "ssid";
const char pass[] = "password";
#endif

int pinDHT11 = 2;
SimpleDHT11 dht11;

int inputLevel = 2;
unsigned long lastMillis = 0;
unsigned long delayMillis = 60000;

boolean newData = false;
WiFiClient net;
MQTTClient client(512);

void connect() {
  Serial.print("Checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    delay(1000);
  }
  Serial.print("Connected to Wifi. Trying MQTT");
  while (!client.connect("arduino", "try", "try")) {
    // Serial.print(".");
    delay(1000);
  }

  Serial.print("Connected to MQTT");

  client.subscribe("$aws/things/mosquittobroker/shadow/+/accepted");
  // client.subscribe("$aws/things/mosquittobroker/shadow/update/accepted");
  // client.unsubscribe("arduino/config");
}


void sendReadings() {
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    Serial.println("Read DHT11 failed");
    return;
  }

  char mqttPayload[100];
  sprintf(mqttPayload, "{\"temp\":%i,\"humid\":%i}", temperature, humidity);
  client.publish("arduino/esp1/data", mqttPayload);
}


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  connect();
  Serial.println("Ready");
  lastMillis = millis();
}

void loop()
{
  client.loop();
  delay(10);
  
  if (!client.connected()) {
    connect();
  }
  if (millis() - lastMillis > delayMillis) {
    lastMillis = millis();
    sendReadings();
  }
}









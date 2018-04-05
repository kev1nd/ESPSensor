/*
    ESP8266 Based Temperature and Humidity Sensor
*/

#include <SimpleDHT.h>
//#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "ssid.h"
#ifndef ssid_h
#define ssid_h
const char ssid[] = "ssid";
const char pass[] = "password";
#endif

int pinDHT22 = 2;  //GPIO2 is pin 2, GPIO0 is pin 1
SimpleDHT22 dht22;

int inputLevel = 2;
unsigned long lastMillis = 0;
unsigned long delayMillis = 15000;

boolean newData = false;
WiFiClient net;
MQTTClient client(512);

void connect() {
  Serial.println("Checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to Wifi.");
  Serial.println("Trying MQTT");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Connected to MQTT");
  client.subscribe("$aws/things/mosquittobroker/shadow/+/accepted");
}


void sendReadings() {
  Serial.println("Sending readings");
  float temperature = 0;
  float humidity = 0;
  byte data[40] = {0};

  // virtual int read2(int pin, float* ptemperature, float* phumidity, byte pdata[40]) = 0;

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
  //  payload.replace("\n","");
  //  payload.replace("\r","");
  // Serial.println("{\"channel\":\"" + topic + "\",\"payload\":" + payload + "}");
  Serial.println(topic);
  Serial.println(payload);
}

void setup()
{
  delay(3000);
  Serial.begin(115200);
  Serial.print("Initialising Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  client.begin("pi", net);
  client.onMessage(messageReceived);
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









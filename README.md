# ESPSensor
Temperature and Humidity from an ESP8266 and DHT22

This experimental project is a cut-down version of my ArduinoSensor project. Rather than use an Arduino interfaced to the ESP8266, here the ESP8266 does everything.

Temperature and Humidity are read by a DHT22 module. This is slightly more expensive, but gives greater accuracy and range than the DHT11. 

## Hardware
![Circuit Diagram](https://github.com/kev1nd/ESPSensor/blob/master/assets/circuit.png)

The curcuit includes a reset button, and also a program button. It is necessary to pull GPIO down during start in order to prepare the ESP for programming. The USB port is used only for programming, since in normal operation all communication is done through wifi.

## Wifi Configudation
The file ssid.h isn't included, because it contains the wifi ssid/password only. It's contents are the same as as the in-line code, except with a valid local ssid/password. This is a simple way of programming a wifi connection, avoiding the need for a web server or serial based configuration. Of course, add your own wifi details.

```
#ifndef ssid_h
#define ssid_h
const char ssid[] = "ssid";
const char pass[] = "password";
#endif
```

## MQTT Comms
The measurements are published using the MQTT protocol, to AWS IoT. Since the Arduino IDE doesn't support TLS1.2, it can't connect directly to the AWS IoT. To work around this, I've used an old Raspberry Pi with Mosquito software installed. I'll write up a separate instruction for installing the Raspberry Pi, but the trick to getting it working was to install a later version of Mosquito than comes in as standard with the installer.


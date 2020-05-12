# Water Tank

Arduino part of solution for measuring water level in tank. Arduino only collects data from ultrasonic sensor and sends them via RESTful api to to backend where all calculation are done.

Ultrasonic sensor is attached to the top cover of tank and measures distance of water level from it.

__Other parts of solution__
* [Mobile Application](https://github.com/martin-pechacek/water-tank-app)
*  [Java (Spring boot) backend](https://github.com/martin-pechacek/water-tank-backend)

## Parts

* Arduino
* Ultrasonic sensor
* ESP8266 Wifi module

## Wifi connection

To connect your wifi you must edit `esp8266_secrets.h` file and set correct SSID and password

## Data transfering

IP address, port and HTTPS certificate SHA1 fingerprint of RESTful API must be also set in `esp8266_secrets.h` file. 

## License
[GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)
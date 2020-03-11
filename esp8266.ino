#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "esp8266_secrets.h"

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = HOST;
const int httpsPort = PORT;
String post_uri = "/api/measurements/";

// SHA1 fingerprint of the certificate
const char fingerprint[] PROGMEM = CERT_FINGERPRINT;

WiFiClientSecure client;

/************************************************************/
/*                       METHODS                            */
/************************************************************/
int getNumberOfTries(int connectionTimeout, int delayBetweenTries){
  return connectionTimeout/delayBetweenTries;
}


void connectToWifi(int connectionTimeout){
  int delayBetweenTries = 250;
  int tries = getNumberOfTries(connectionTimeout, delayBetweenTries);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  for(int i=1; i<=tries; i++){
    if(WiFi.status() == WL_CONNECTED){
      return;
    } else {
      delay(delayBetweenTries);
    }
  }
}


void connectToServer(int connectionTimeout){
  int delayBetweenTries = 250;
  int tries = getNumberOfTries(connectionTimeout, delayBetweenTries);

  client.setFingerprint(fingerprint);

  for(int i=1; i<=tries; i++){
    if(client.connect(host, httpsPort)) {
      // Info to arduino - Successfully connected to server
      Serial.println('0');
      return;
    } else {
      delay(delayBetweenTries);
    } 
  }
}


String receiveWaterLevelDistance(int timeout){
  int delayBetweenTries = 250;
  int tries = getNumberOfTries(timeout, delayBetweenTries);
  
  for(int i=1;i<=tries;i++){
    if(Serial.available()){
      return Serial.readStringUntil('\n');
    }
    delay(delayBetweenTries);
  }
}


void sendWaterLevelDistance(String waterLevelDistance){
  String data = "{\"waterLevelDistance\": " + waterLevelDistance + "}";

  client.print(String("POST ") + post_uri + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Device-ID: test-device\r\n" +
               "Connection: close\r\n" + 
               "Content-Length: " + data.length() + "\r\n\r\n" +
               data);

  while (client.connected()) {
    String line = client.readStringUntil('\n'); 
    if (line.startsWith("HTTP/1.1 201")) {
      // Info to arduino - Successfully sent POST
      Serial.println('1');
      return;
    }
  }
  // Info to arduino - Unsuccessfully sent POST
  Serial.println('2');  
}

/************************************************************/
/*                       BODY                               */
/************************************************************/
void setup() {            
  Serial.begin(9600);

  connectToWifi(1200000);
  connectToServer(1200000);
  
  delay(500);
}


void loop() { 
  String waterLevelDistance = receiveWaterLevelDistance(60000);

  sendWaterLevelDistance(waterLevelDistance);

  delay(5000);
}

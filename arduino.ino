#include <SoftwareSerial.h> 

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
 
#define RELAY (13)

SoftwareSerial ESP_Serial(7, 8);  //Tx,Rx

int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;

 
void enterSleep(void)
{ 
  Serial.println("");
  Serial.println("Sleeping..");
  Serial.println("");
  digitalWrite(RELAY, LOW);

  delay(7200000); //2h
}

long measureDistance(){
  long measuringList[6];

  for(int i = 0; i < 5; i++){
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
   
    // Convert the time into a distance
    cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343 
    measuringList[i] = cm;
    
    delay(100);
  }

  //asc sort measured values
  for(int i=0; i<(5-1); i++) {
    for(int o=0; o<(5-(i+1)); o++) {
      if(measuringList[o] > measuringList[o+1]) {
        int t = measuringList[o];
        measuringList[o] = measuringList[o+1];
        measuringList[o+1] = t;
      }
    }
  }
    
  //median  
  cm = measuringList[2];

  return cm;
} 


int getNumberOfLoops(int delayBetweenTries, int timeout){
  int numberOfLoops = timeout/delayBetweenTries;

  return numberOfLoops;
}

char getLastChar(String text){   
   char lastChar = text.charAt(text.length() - 1);
   
   return lastChar;
}

void waitTillESPIsReady(int timeout){
  int delayBetweenTries = 250;
  int tries = getNumberOfLoops(delayBetweenTries,timeout);

  for(int i=0;i<=tries;i++){
     String receivedData = "";
     
     receivedData = receiveESPData();
     
     char lastChar = getLastChar(receivedData);
     
     delay(150);

     if(lastChar == '0'){    
      Serial.println("ESP Connected");
      break;
     }
     
     if(i==tries){
        enterSleep();
     }
     delay(delayBetweenTries);
  }
}

void sendData(long distance, int timeout){
    int delayBetweenTries = 250;
    int tries = getNumberOfLoops(delayBetweenTries,timeout);
    String receivedData = "";
    
    ESP_Serial.println(distance);

    for(int i=0;i<=tries;i++){
      String receivedData = receiveESPData();
      char lastChar = getLastChar(receivedData);
 
      if(lastChar == '1'){
        Serial.println("Data sucessfuly transfered");
        break;
      }   
      delay(delayBetweenTries);  
    } 
}

String receiveESPData(){
  String receivedData = "";
  String currentLine = "";
  
  while(ESP_Serial.available()){
    char inChar = ESP_Serial.read();
    
    if(inChar == '0' || inChar == '1' || inChar == '2'){
      receivedData += inChar;
    }
  }
  
  receivedData.trim();
  
  return receivedData;
}
 
void setup()
{
  //Serial Port begin
  Serial.begin(9600);
  ESP_Serial.begin(9600);
  pinMode(RELAY,OUTPUT);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
 
 
void loop()
{
    Serial.println("");
    Serial.println("not sleeping");
    Serial.println("");
    digitalWrite(RELAY, HIGH);
 
    long cmDistance = 999999;
  
    waitTillESPIsReady(30000);
       
    delay(50);
  
    cmDistance=measureDistance();
     
    sendData(cmDistance,30000); 
   
    enterSleep();
}

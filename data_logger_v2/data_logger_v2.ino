#include <Bounce.h>
#define BUTTON 3
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "SPI.h"

#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


Adafruit_LIS3DH lis = Adafruit_LIS3DH();
Adafruit_LIS3DH lis_2 = Adafruit_LIS3DH();
File logfile;
RTC_DS3231 rtc;
Bounce bouncer = Bounce( BUTTON, 50 ); 


#define redLEDpin 20
#define greenLEDpin 8
#define LOG_INTERVAL 1 // delay in microseconds
#define SYNC_INTERVAL 1000 // delay in milis

int write_data_bool = LOW;
bool write_init = true;
const int chipSelect = 10;
const int arr_size = 200;


typedef struct
  {
      uint32_t time;
      float x1;
      float y1;
      float z1;
      float x2;
      float y2;
      float z2;
  }  datapoint;

datapoint logs[arr_size];

void setup() {
  pinMode(BUTTON,INPUT);
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  if (! lis.begin(0x18) || (! lis_2.begin(0x19))) { 
    error("sensor is not htere.");
  }
  lis.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);
  lis_2.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);
  
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  
  if (!rtc.begin()) {
    logfile.println("RTC failed");
  }

}

void loop() {
  
  if ( bouncer.update() ) {
     if ( bouncer.read() == HIGH) {
       if (write_data_bool){
         logfile.close();
       }
        write_data_bool = !write_data_bool;
        write_init = true; 
       }
  }
  if (write_data_bool){
    blink();
    if (write_init){
      blink();
      blink();
      blink();
      DateTime now = rtc.now();
      String date;
      date =  String(now.month())+ String(now.day()) + String(now.minute())+ String(now.second())+ ".txt";
      char filename[16] = {0};    
      date.toCharArray(filename, 16);
      if (! SD.exists(filename)) {
          // only open a new file if it doesn't exist
        
        logfile = SD.open(filename, FILE_WRITE); 
      }
      if (! logfile) {
        error("couldnt create file");
      }
      write_init = false; 
    }
  for (int i = 0; i < arr_size; i++){
    delayMicroseconds(LOG_INTERVAL);
    lis.read();      // get X Y and Z data at once
    lis_2.read();
    sensors_event_t event;
    lis.getEvent(&event);
    sensors_event_t event_2;
    lis_2.getEvent(&event_2);
    logs[i].time = millis();
    logs[i].x1 = event.acceleration.x;
    logs[i].y1 = event.acceleration.y;
    logs[i].z1 = event.acceleration.z;
    logs[i].x2 = event_2.acceleration.x;
    logs[i].y2 = event_2.acceleration.y;
    logs[i].z2 = event_2.acceleration.z;
  }
  // dump file baybe
  int timenow = millis();
  logfile.write(logs, sizeof(logs));
  }

}
void blink(){
  delay(200);
  digitalWrite(greenLEDpin, HIGH);
  delay(200);
  digitalWrite(greenLEDpin, LOW);
}
void error()
{ 
  // red LED indicates error
  for (int i= 0; i< 5; i++){
    delay(200);
    digitalWrite(greenLEDpin, HIGH);
    delay(200);
    digitalWrite(greenLEDpin, LOW);

  }

  while(1);
}

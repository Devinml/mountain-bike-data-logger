#include <Bounce.h>
#define BUTTON 3
#include "SdFat.h"
#include <Wire.h>
#include "RTClib.h"
#include "SPI.h"

#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


Adafruit_LIS3DH lis = Adafruit_LIS3DH();
Adafruit_LIS3DH lis_2 = Adafruit_LIS3DH();
SdFat sd;
SdFile logfile;
RTC_DS3231 rtc;
Bounce bouncer = Bounce( BUTTON, 50 ); 


#define redLEDpin 20
#define greenLEDpin 8
#define LOG_INTERVAL 1 // delay in microseconds

int write_data_bool = LOW;
bool write_init = true;
const int chipSelect = 10;
const int arr_size = 1500;


typedef struct
  {
      uint32_t time;
      uint32_t x1;
      uint32_t y1;
      uint32_t z1;
      uint32_t x2;
      uint32_t y2;
      uint32_t z2;
  }  datapoint;

datapoint logs[arr_size];

void setup() {
  
  Serial.begin(9600);
  pinMode(BUTTON,INPUT);
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  Serial.begin("pinmode out ");

  if (! lis.begin(0x18) || (! lis_2.begin(0x19))) { 
    Serial.println("no acc");
    error();
  }
  lis.setDataRate(LIS3DH_DATARATE_LOWPOWER_1K6HZ);
  lis_2.setDataRate(LIS3DH_DATARATE_LOWPOWER_1K6HZ);
  Serial.println("Data rates set");
  
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!sd.begin(chipSelect)) {
    error();
  }
  
  if (!rtc.begin()) {
    logfile.println("RTC failed");
  }
  Serial.println("ready to log");
  Wire.setClock(1000000);  
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
    Serial.println("write data log hit");
    if (write_init){
      Serial.println("Preparing to logg");
      blink();
      blink();
      blink();
      DateTime now = rtc.now();
      String date;
      date =  String(now.month())+ String(now.day()) + String(now.minute())+ String(now.second())+ ".txt";
      char filename[16] = {0};    
      date.toCharArray(filename, 16);
      if (! sd.exists(filename)) {
          // only open a new file if it doesn't exist
          logfile.open(filename, O_WRONLY | O_CREAT | O_EXCL);
      }
      if (! logfile) {
        Serial.println("error with SD card");
        error();
      }
      write_init = false; 
    }
  Serial.println("logging prior to loop");
  for (int i = 0; i < arr_size; i++){
    lis.read();      // get X Y and Z data at once
    lis_2.read();
    logs[i].time = millis();
    logs[i].x1 = lis.x;
    logs[i].y1 = lis.y;
    logs[i].z1 = lis.z;
    logs[i].x2 = lis_2.x;
    logs[i].y2 = lis_2.y;
    logs[i].z2 = lis_2.z;
  }
  // dump file baybe
  digitalWrite(greenLEDpin, HIGH);
  logfile.write(logs, sizeof(logs));
  digitalWrite(greenLEDpin, LOW);
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

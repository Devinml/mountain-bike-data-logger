#include <Bounce.h>
#define BUTTON 3
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "SPI.h"

#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


Adafruit_LIS3DH lis = Adafruit_LIS3DH();
Adafruit_LIS3DH lis_2 = Adafruit_LIS3DH();

// A simple data logger for the Arduino analog pins

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  100 // micros between entries (reduce to take more/faster data)
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define redLEDpin 20
#define greenLEDpin 8

int write_data_bool = LOW;        // the current state of the output pin
bool write_init = true;
// The analog pins that connect to the sensors

RTC_DS3231 rtc;

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;
Bounce bouncer = Bounce( BUTTON, 50 ); 
void error(char *str)
{ 
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);

  while(1);
}

void setup(void)
{

  // use debugging LEDs
  pinMode(BUTTON,INPUT);
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  // digitalWrite(redLEDpin, ledState);
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    
    error("Could not find First accelerometer");
  }
  lis.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);
  

  if (! lis_2.begin(0x19)) {   // change this to 0x19 for alternative i2c address
  
    error("Could Not find Second accelerometer");
  }
  lis_2.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);
  
  // initialize the SD card
  
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  
  if (!rtc.begin()) {
    logfile.println("RTC failed");
  }
  analogReference(EXTERNAL);
}

void loop(void)
{ 
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
    if (write_init){
      DateTime now = rtc.now();
      String date;
      date =  String(now.month())+ String(now.day()) + String(now.minute())+ String(now.second())+ ".csv";
      char filename[16] = {0};    
      date.toCharArray(filename, 16);
        
      if (! SD.exists(filename)) {
          // only open a new file if it doesn't exist
        
        logfile = SD.open(filename, FILE_WRITE); 
      }
      if (! logfile) {
        error("couldnt create file");
      }
      
      logfile.println("millis,x_1,y_1,z_1,x_2,y_2,z_2");
      write_init = false; 
    }

    lis.read();      // get X Y and Z data at once
    lis_2.read();
    sensors_event_t event;
    lis.getEvent(&event);
    sensors_event_t event_2;
    lis_2.getEvent(&event_2);

    // delay for the amount of time we want between readings
    delayMicroseconds(LOG_INTERVAL);
    
    digitalWrite(greenLEDpin, HIGH);
    
    // log milliseconds since starting
    uint32_t m = millis();
    String data_string;
    data_string = m + String(",") + String(event.acceleration.x) + String(",") +  String(event.acceleration.y) + String(",") +  String(event.acceleration.z) + String(",") +  String(event_2.acceleration.x) + String(",") +  String(event_2.acceleration.y) + String(",") +  String(event_2.acceleration.z);
    logfile.println(data_string);
    // logfile.print(m);           // milliseconds since start
    // logfile.print(",");
    // logfile.print(event.acceleration.x);
    // logfile.print(",");
    // logfile.print(event.acceleration.y);
    // logfile.print(",");
    // logfile.print(event.acceleration.z);
    // logfile.print(",");
    // logfile.print(event_2.acceleration.x);
    // logfile.print(",");
    // logfile.print(event_2.acceleration.y);
    // logfile.print(",");
    // logfile.print(event_2.acceleration.z);
    // logfile.println();

    digitalWrite(greenLEDpin, LOW);

    // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
    // which uses a bunch of power and takes time
    if ((millis() - syncTime) < SYNC_INTERVAL) return;
    syncTime = millis();
    
    // blink LED to show we are syncing data to the card & updating FAT!
    digitalWrite(redLEDpin, HIGH);
    // logfile.flush();
    digitalWrite(redLEDpin, LOW);
  }
}
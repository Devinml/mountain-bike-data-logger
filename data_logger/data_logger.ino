
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>


Adafruit_LIS3DH lis = Adafruit_LIS3DH();
Adafruit_LIS3DH lis_2 = Adafruit_LIS3DH();

// A simple data logger for the Arduino analog pins

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  5 // mills between entries (reduce to take more/faster data)

// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to 
// the last 10 reads if power is lost but it uses less power and is much faster!
#define SYNC_INTERVAL 50 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

// the digital pins that connect to the LEDs
#define redLEDpin 13
#define greenLEDpin 8

// The analog pins that connect to the sensors

RTC_PCF8523 rtc;// define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);

  while(1);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println();
  
  // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    error("Could not find First accelerometer");
  }
  Serial.println("LIS3DH found!");

  if (! lis_2.begin(0x19)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt find second acc");
    error("Could Not find Second accelerometer");
  }
  Serial.println("LIS3DH_2 found!");
  
  

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");
  if (!rtc.begin()) {
    logfile.println("RTC failed");
  }
  rtc.start();
  DateTime now = rtc.now();
  String date;
  date =  String(now.month()) + "-" + String(now.day()) + "-"+  String(now.minute())+ ".csv";
  char filename[16] = {0};    
  date.toCharArray(filename, 16);

  Serial.println("filename: " + String(filename));
    
  if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
    logfile = SD.open(filename, FILE_WRITE); 
   }
  
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  logfile.println("millis,x_1,y_1,z_1,x_2,y_2,z_2"); 
  analogReference(EXTERNAL);
}

void loop(void)
{
  lis.read();      // get X Y and Z data at once
  lis_2.read();
  sensors_event_t event;
  lis.getEvent(&event);
  sensors_event_t event_2;
  lis_2.getEvent(&event_2);


  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));
  
  digitalWrite(greenLEDpin, HIGH);
  
  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m);           // milliseconds since start
  logfile.print(",");
  logfile.print(event.acceleration.x);
  logfile.print(",");
  logfile.print(event.acceleration.y);
  logfile.print(",");
  logfile.print(event.acceleration.z);
  logfile.print(",");
  logfile.print(event_2.acceleration.x);
  logfile.print(",");
  logfile.print(event_2.acceleration.y);
  logfile.print(",");
  logfile.print(event_2.acceleration.z);
  logfile.println();

  digitalWrite(greenLEDpin, LOW);

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  logfile.flush();
  digitalWrite(redLEDpin, LOW);
  
}


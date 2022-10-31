
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
#define LOG_INTERVAL  25 // mills between entries (reduce to take more/faster data)
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

#define redLEDpin 13
#define greenLEDpin 8
const int buttonPin = 2;  
int ledState = LOW;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin
bool write_init = true;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


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
  pinMode(buttonPin, INPUT);
  // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  digitalWrite(redLEDpin, ledState);
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
  
  analogReference(EXTERNAL);
}

void loop(void)
{
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        write_init = true;        
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  Serial.println("Write_init value: ");
  Serial.print(write_init);
  if (ledState){
    if (write_init){
      Serial.println("in the init of data logging");
      DateTime now = rtc.now();
      String date;
      date =  String(now.month()) + "-" + String(now.day()) + "-"+  String(now.minute())+ ".csv";
      char filename[16] = {0};    
      date.toCharArray(filename, 16);

      Serial.println("filename: " + String(filename));
        
      if (! SD.exists(filename)) {
          // only open a new file if it doesn't exist
        Serial.println("file does not exist creating a new one");
        logfile = SD.open(filename, FILE_WRITE); 
      }
      if (! logfile) {
        error("couldnt create file");
      }
      
      Serial.print("Logging to: ");
      Serial.println(filename);

      logfile.println("millis,x_1,y_1,z_1,x_2,y_2,z_2");
      write_init = false; 
    }
    Serial.println("logging data");
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
}
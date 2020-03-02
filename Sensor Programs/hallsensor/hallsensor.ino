#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
int digitalPin = 4; // linear Hall magnetic sensor digital interface
int controlAPin = A13; // linear Hall magnetic sensor analog interface
int reading; // analog reading
char controlsurfaceA[16];
String measurement;

File myFile;
const int chipSelect = BUILTIN_SDCARD;

void setup ()
{
  Serial.begin(115200); // Start serial communications 
  pinMode(controlAPin, INPUT);
  Serial.begin(115200); // Start serial communications
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  File dataFile = SD.open(controlsurfaceA,FILE_READ);
  Serial.println(controlsurfaceA);
  dataFile.close();
  
   ///FOR TIMER/////////////////////////////////////////////////
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);

  Serial.begin(115200);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(100);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  //FOR TIMER//////////////////////////////////////////////////
}

void loop ()
{
  //FOR TIMER/////////////////////////////////////////////////
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
    }
  }
  //FOR TIMER//////////////////////////////////////////////
  getcontrolsurfaceA();
}

void getcontrolsurfaceA(){
    String h=String(hour())+":";
    String m=String(minute())+":";
    String s=String(second());
    String TimeNow=h+m+s;
    reading = analogRead(controlAPin);
    measurement=String(reading)+","+TimeNow;
    myFile = SD.open(controlsurfaceA, FILE_WRITE);
// If we get a reading that isn't zero, let's print it
    if(reading != 0){
      Serial.println(measurement); // Print the distance ONLY USING THIS FOR TESTING and CALIBRATION
      myFile.println(measurement);
      delay(1000);//FOR NOW DELAY READINGS TO ONCE PER SECOND.  WILL INCREASE FREQUENCY IN FUTURE to 200Hz
    }
    myFile.close();
  }

/////////FOR TIMER/////////////////////////////////////////////////////////////////////////////////////////
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}
//////FOR TIMER///////////////////////////////////////////////////////////////////////

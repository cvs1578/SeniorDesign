/*------------------------------------------------------------------------------

  LIDARLite Arduino Library
  v3/GetDistancePwm

  This example shows how to read distance from a LIDAR-Lite connected over the
  PWM interface.

  Connections:
  LIDAR-Lite 5 Vdc (red) to Arduino 5v
  LIDAR-Lite Ground (black) to Arduino GND
  LIDAR-Lite Mode control (yellow) to Arduino digital input (pin 3)
  LIDAR-Lite Mode control (yellow) to 1 kOhm resistor lead 1
  1 kOhm resistor lead 2 to Arduino digital output (pin 2)

  (Capacitor recommended to mitigate inrush current when device is enabled)
  680uF capacitor (+) to Arduino 5v
  680uF capacitor (-) to Arduino GND

  See the Operation Manual for wiring diagrams and more information:
  http://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf

------------------------------------------------------------------------------*/
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

File myFile;
unsigned long pulseWidth;
const int chipSelect = BUILTIN_SDCARD;
char lidarfile[16];
void setup()
{
  Serial.begin(115200); // Start serial communications
  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  digitalWrite(2, LOW); // Set trigger LOW for continuous read

  pinMode(3, INPUT); // Set pin 3 as monitor pin
   Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  //char filename[16]; // make it long enough to hold your longest file name, plus a null terminator
  int n = 0;
  snprintf(lidarfile, sizeof(lidarfile),"lidar%03d.txt", n); // includes a three-digit sequence number in the file name
  while(SD.exists(lidarfile)) {
    n++;
    snprintf(lidarfile, sizeof(lidarfile), "lidar%03d.txt", n);
  }
  File dataFile = SD.open(lidarfile,FILE_READ);
  Serial.println(n);
  Serial.println(lidarfile);
  dataFile.close();
  //now filename[] contains the name of a file that doesn't exist


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

void loop()
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
  
  getlidar();
}


//PLAN TO USE A FUNCTION FOR EVERY SENSORS DATA AQUISITION
void getlidar(){
    String h=String(hour())+":";
    String m=String(minute())+":";
    String s=String(second());
    String TimeNow=h+m+s;
    pulseWidth = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
    myFile = SD.open(lidarfile, FILE_WRITE);
// If we get a reading that isn't zero, let's print it
    if(pulseWidth != 0){
      pulseWidth = pulseWidth / 10; // 10usec = 1 cm of distance
      String measurement=String(pulseWidth)+","+TimeNow;
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
//////FOR TIMER///////////////////////////////////////////////////////////////////////////////////

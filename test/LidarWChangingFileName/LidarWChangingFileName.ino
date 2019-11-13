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

File myFile;
unsigned long pulseWidth;
const int chipSelect = BUILTIN_SDCARD;
char filename[16];
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
  snprintf(filename, sizeof(filename),"lidar%03d.txt", n); // includes a three-digit sequence number in the file name
  while(SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "data%03d.txt", n);
  }
  File dataFile = SD.open(filename,FILE_READ);
  Serial.println(n);
  Serial.println(filename);
  dataFile.close();
  //now filename[] contains the name of a file that doesn't exist
}

void loop()
{//////LIDAR
  pulseWidth = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
  myFile = SD.open(filename, FILE_WRITE);
  // If we get a reading that isn't zero, let's print it
  if(pulseWidth != 0)
  {
    pulseWidth = pulseWidth / 10; // 10usec = 1 cm of distance
    Serial.println(pulseWidth); // Print the distance
    myFile.println(pulseWidth);
    delay(1000); 
  }
  myFile.close();
  
}

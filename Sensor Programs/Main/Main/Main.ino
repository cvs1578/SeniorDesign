#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

File myFile;
unsigned long pulseWidth;
const int chipSelect = BUILTIN_SDCARD;
char lidarfile[16] = "lidar";
char surfA[16] = "surfA";
char surfB[16] = "surfB";
char surfC[16] = "surfC";
char surfD[16] = "surfD";
int controlAPin = 23; // linear Hall magnetic sensor analog interface
int controlBPin=22;
int controlCPin=21;
int controlDPin=20;
int TOGGLESWITCH=32;
int BLUELED=24;
void setup()
{
  pinMode(TOGGLESWITCH,INPUT);//SWITCH TO START/STOP DATA AQUISITION
  pinMode(BLUELED,OUTPUT);
  digitalWrite(BLUELED,LOW); //This is to indicate the files are created
  
  Serial.begin(115200); // Start serial communications
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  digitalWrite(2, LOW); // Set trigger LOW for continuous read

  pinMode(3, INPUT); // Set pin 3 as monitor pin
   Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  
  ///////CONTROL SURFACE SENSOR A FILE//////////////
  delay(100);
  fileCreation(surfA, 5);  
  myFile = SD.open(surfA, FILE_WRITE);
  myFile.println("Control Surface A test");
  myFile.close();
  Serial.println("done.");
  ///////CONTROL SURFACE SENSOR A FILE//////////////

  ///////CONTROL SURFACE SENSOR B FILE//////////////
  fileCreation(surfB, 5);  
  myFile = SD.open(surfB, FILE_WRITE);
  myFile.println("Control Surface B test");
  myFile.close();
  Serial.println("done.");
  ///////CONTROL SURFACE SENSOR B FILE//////////////

  ///////CONTROL SURFACE SENSOR C FILE//////////////
  fileCreation(surfC, 5);  
  myFile = SD.open(surfC, FILE_WRITE);
  myFile.println("Control Surface C test");
  myFile.close();
  Serial.println("done.");
  ///////CONTROL SURFACE SENSOR C FILE//////////////


  ///////CONTROL SURFACE SENSOR D FILE//////////////
  fileCreation(surfD, 5);  
  myFile = SD.open(surfD, FILE_WRITE);
  myFile.println("Control Surface D test");
  myFile.close();
  Serial.println("done.");
  ///////CONTROL SURFACE SENSOR D FILE//////////////
  
  ///////LIDAR FILE CREATION////////////////
  fileCreation(lidarfile, 5); // We put in the name of the file we want and the amount of characters already in it
  myFile = SD.open(lidarfile, FILE_WRITE);
  myFile.println("This is a test. Are you still there? Hello?");
  myFile.close();
  Serial.println("done.");
  ////////LIDAR FILE CREATION///////////////
  delay(100);

   

 
  ///FOR TIMER/////////////////////////////////////////////////
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(100);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  //FOR TIMER//////////////////////////////////////////////////

  digitalWrite(BLUELED,HIGH); //This is to indicate the files are created BLUE LED
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


//DATA AQUISITION WON'T START TILL SWITCH IS FLIPPED
  getlidar();
  getcontrolsurf(surfA, controlAPin);
  getcontrolsurf(surfB, controlAPin);
  getcontrolsurf(surfC, controlAPin);
  getcontrolsurf(surfD, controlAPin);
 
  
}


//PLAN TO USE A FUNCTION FOR EVERY SENSORS DATA AQUISITION

///////////LIDAR FUNCTION///////////////////////////////////////
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
///////////LIDAR FUNCTION///////////////////////////////////////



/////////////CONTROL SURFACE FUNCTION////////////////////////
void getcontrolsurf(char filename, int controlpin){
    String h=String(hour())+":";
    String m=String(minute())+":";
    String s=String(second());
    String TimeNow=h+m+s;
    int reading = analogRead(controlpin);
    String measurement=String(reading)+","+TimeNow;
    myFile = SD.open(filename, FILE_WRITE);
// If we get a reading that isn't zero, let's print it
    if(reading != 0){
      Serial.println(measurement); // Print the distance ONLY USING THIS FOR TESTING and CALIBRATION
      myFile.println(measurement);
      delay(500);//FOR NOW DELAY READINGS TO ONCE PER SECOND.  WILL INCREASE FREQUENCY IN FUTURE to 200Hz
    }
    myFile.close();
  }
/////////////CONTROL SURFACE FUNCTION////////////////////////



/////////////////////FILE CREATION/////////////////////////////
void fileCreation(char* file, int x){
  char names[16];
  for (int i = 0; i < x; i++)
  {
    names[i] = file[i]; //First we pass in whatever the word is to this character array
  }
  //Then we add .txt to the end
  names[x] = '.';
  names[x+1] = 't';
  names[x+2] = 'x';
  names[x+3] = 't';

     for (int i = 0; i < x+4; i++)
    {
    Serial.print(names[i]);
    }
    Serial.println("");
  //Then we determine if the file already exists
  if (SD.exists(names))
  {
    Serial.println("Determed file exists");
    char tens = '0';
    char n = '0';
    char newname[16]; //If it does we create a new file
    for (int i = 0; i < x; i++)
    {
      newname[i] = file[i];
    }
    newname[x] = tens; //Only this time we have a number stored before the .txt
    //NOTE: We can only store 100 files with this method. 
    newname[x+1] = n;
    newname[x+2] = '.';
    newname[x+3] = 't';
    newname[x+4] = 'x';
    newname[x+5] = 't';
    while (SD.exists(newname))
    {
      Serial.print("HERE");
      if(n == '9')
      {
        tens++;
        n = '0';
        newname[x] = tens;
        newname[x+1] = n;
      }
      else
      {
        n++; //We increment n until the file doesn't exist
        newname[x+1] = n;
      }
      delay(500);
    }
    
    //This is to determine if the file naming system works
    for (int i = 0; i < x+5; i++)
    {
    Serial.print(newname[i]);
    }

    
    //Open both files
    File newfile = SD.open(newname, FILE_WRITE);
    File oldfile = SD.open(names, FILE_READ);
    
    //Then store the old file into the new file
    while(oldfile.available())
    {
      newfile.write(oldfile.read()); //Store data on new file
    }
    newfile.close();
    oldfile.close();
    SD.remove(names); //This deletes the old file so it starts of blank
  }
  //Now we add the .txt. Since we're passing through the reference we only need to use 'file'
  //This should work for whatever file
  file[x] = '.';
  file[x+1] = 't';
  file[x+2] = 'x';
  file[x+3] = 't';
}

//////////FILE CREATION///////////////////////////////////////////////////////


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

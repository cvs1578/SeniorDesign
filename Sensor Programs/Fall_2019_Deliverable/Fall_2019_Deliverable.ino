#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

File myFile;
unsigned long pulseWidth;
const int chipSelect = BUILTIN_SDCARD;
char lidarfile[16] = "lidar";
char surfaceAfile[16] = "surfaceA";
int controlAPin = A9; // linear Hall magnetic sensor analog interface
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


  ///////LIDAR FILE CREATION////////////////
  fileCreation(lidarfile, 5); // We put in the name of the file we want and the amount of characters already in it
  myFile = SD.open(lidarfile, FILE_WRITE);
  myFile.println("This is a test. Are you still there? Hello?");
  myFile.close();
  Serial.println("done.");
  ////////LIDAR FILE CREATION///////////////

  ///////HALL EFFECT SENSOR A FILE//////////////
  fileCreation(surfaceAfile,8);
  myFile = SD.open(surfaceAfile, FILE_WRITE);
  myFile.println("Control Surface A test");
  myFile.close();
  Serial.println("done.");
  ///////HALL EFFECT SENSOR A FILE//////////////
  
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
  getcontrolsurfaceA();
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

/////////////CONTROL SURFACE A FUNCTION////////////////////////
void getcontrolsurfaceA(){
    String h=String(hour())+":";
    String m=String(minute())+":";
    String s=String(second());
    String TimeNow=h+m+s;
    int reading = analogRead(controlAPin);
    String measurement=String(reading)+","+TimeNow;
    myFile = SD.open(surfaceAfile, FILE_WRITE);
// If we get a reading that isn't zero, let's print it
    if(reading != 0){
      Serial.println(measurement); // Print the distance ONLY USING THIS FOR TESTING and CALIBRATION
      myFile.println(measurement);
      delay(1000);//FOR NOW DELAY READINGS TO ONCE PER SECOND.  WILL INCREASE FREQUENCY IN FUTURE to 200Hz
    }
    myFile.close();
  }
/////////////CONTROL SURFACE A FUNCTION////////////////////////



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
  
  //Then we determine if the file already exists
  if (SD.exists(names))
  {
    char n = '0';
    char newname[16]; //If it does we create a new file
    for (int i = 0; i < x; i++)
    {
      newname[i] = file[i];
    }
    newname[x] = n; //Only this time we have a number stored before the .txt
    //NOTE: We can only store 10 files with this method. 
    newname[x+1] = '.';
    newname[x+2] = 't';
    newname[x+3] = 'x';
    newname[x+4] = 't';
    while (SD.exists(newname))
    {
      n++; //We increment n until the file doesn't exist
      newname[x] = n;
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

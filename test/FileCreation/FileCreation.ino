#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

File myFile;
const int chipSelect = BUILTIN_SDCARD;

//NOTE: We put the filenames here. 
//NOTE: We do NOT put in .txt here
char lidarfile[16] = "lidar";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Start serial communications
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  fileCreation(lidarfile, 5); // We put in the name of the file we want and the amount of characters already in it

  myFile = SD.open(lidarfile, FILE_WRITE);
  myFile.println("This is a test. Are you still there? Hello?");
  myFile.close();
  Serial.println("done.");

}


void loop() {
  // put your main code here, to run repeatedly:

}






//x is size of the word already stored (instance, for Lidar we pass in 5
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

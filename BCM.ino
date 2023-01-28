// Library Setup
#include <millisDelay.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#include <Wire.h>
#include "SparkFun_MMA8452Q.h"

// Pins
const int chipSelect = 53;
#define DHTPIN 8
#define DHTTYPE DHT22

// Library Instantiation 
File myFile;
millisDelay clock;
MMA8452Q accel;
DHT dht(DHTPIN, DHTTYPE);

// Variables
int seconds = 0;
int minutes = 0;
int hours = 0;

void setup() {
  // Opens serial communications
  Serial.begin(9600);

  // Sets up sensor pins
  dht.begin();
  Wire.begin();

  while(!accel.begin()){
    Serial.println("Accelerometer not connected!");
    return;
  }

  // Debug stuff to make sure SD card works
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done");

  // Checks if file already exists and stops program if it does. Will implement led error code system to warn user.
  if (SD.exists("data.txt")) {
    Serial.print("File exists, deleting...");
    while (!SD.remove("data.txt")) {
      ;
    }
    Serial.println("Deleted");
  }
  
  // Makes data file and checks if it exists
  myFile = SD.open("data.txt", FILE_WRITE);
  if (SD.exists("data.txt")) {
    Serial.println("File created successfully");
  } else {
    Serial.println("File creation failed!");
    return;
  }
  myFile.close();

  // Writes Boilerplate code
  myFile = SD.open("data.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing starter code...");
    // Writes Boilerplate Code
    myFile.println("****//BCM//****");
    myFile.println("|Time|AccX,Y,Z|Them|Humidity|");
    myFile.close();
    Serial.println("  done");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
    return;
  }

  // re-open the file for reading. This is only for debugging purposes:
  myFile = SD.open("data.txt");
  if (myFile) {
    Serial.println("Reading...");
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
    Serial.println("done");
  } else {
   //if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    return;
  }
}
void loop() {
  // Clock. Runs 5 seconds.
  if (!clock.isRunning()) {
    clock.start(5000);
  }
  
  if (clock.justFinished()) {
    // Every 5 seconds runs in this. All sensor reading will occur here
    seconds += 5;
    if(seconds >= 60){
      minutes += 1;
      seconds = 0;
    }
    if(minutes >= 60){
      hours += 1;
      minutes = 0;
    }
    Serial.println("");
    // Accelerometer Data Retrival
    float x = accel.getCalculatedX();
    float y = accel.getCalculatedY();
    float z = accel.getCalculatedZ();

    // Temp & Humidity Data Retrival
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(true);

    // Writing to disk - If you can concatante text please LMK cuz this is annoying.
    myFile = SD.open("data.txt", FILE_WRITE);
    if (myFile) {
      myFile.print("| ");
      myFile.print(hours);
      myFile.print(".");
      myFile.print(minutes);
      myFile.print(".");
      myFile.print(seconds);
      myFile.print(" | ");
      myFile.print(x);
      myFile.print(", ");
      myFile.print(y);
      myFile.print(", ");
      myFile.print(z);
      myFile.print(" | ");
      myFile.print(temperature);
      myFile.print(" | ");
      myFile.print(humidity);
      myFile.println(" |");
      myFile.close();
      read();
    }else{
      // if the file didn't open, print an error:
      Serial.println("error opening data.txt");
      return;
    }
    
  }
}

void read(){
  // open the file for reading. This is only for debugging purposes:
  myFile = SD.open("data.txt");
  if (myFile) {
    Serial.println("Reading...");
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
    Serial.println("done");
  } else {
   //if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    return;
  }
}

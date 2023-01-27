// Library Setup
#include <millisDelay.h>
#include <SPI.h>
#include <SD.h>
#include <DallasTemperature.h>
#include <OneWire.h>

// Pins
const int chipSelect = 53;


// Library Instantiation 
File myFile;
millisDelay clock;

// Variables
int timeElapsed = 0;

void setup() {
  // Opens serial communications
  Serial.begin(9600);

  // Sets up sensor pins
  sensors.begin();

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
    myFile.println("|Acc.|MainTherm|Humidity|AuxThem");
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
   if the file didn't open, print an error:
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
    timeElapsed += 5;
    Serial.print("Time Elapsed: ");
    Serial.println(timeElapsed);
  }
}

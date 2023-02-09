// Library Setup
#include <millisDelay.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Pins
const int chipSelect = 53;
#define DHTPIN 8
#define DHTTYPE DHT22
Adafruit_MPU6050 mpu;

// Library Instantiation 
File myFile;
millisDelay clock;
DHT dht(DHTPIN, DHTTYPE);

// Variables
float seconds = 0;
float time = 0.0;
// Change to true if readout is needed after every write
// If left on it will stall the program as the process of printing 
// will take longer than 5 seconds.
bool needRead = false;

void setup() {
  // Opens serial communications
  Serial.begin(115200);
  // Sets up LEDs
  pinMode(11, OUTPUT); // Heater Mosfet Switch. High Turns on Heater, Low Turns off
  pinMode(10, OUTPUT); // Red
  pinMode(9, OUTPUT); // Green

  // Sets up sensor pins
  
  dht.begin();
  
  Wire.begin();
  if (!mpu.begin()) {
		Serial.println("Failed to find MPU6050 chip");
	}
  
  // Mpu setup
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Debug stuff to make sure SD card works
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    sdCardError();
  }
  Serial.println("initialization done");
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  

  // Checks if file already exists and stops program if it does. Will implement led error code system to warn user.
  if (SD.exists("data.txt")) {
    Serial.print("File exists, deleting...");
    while (!SD.remove("data.txt")) {
      ;
    }
    digitalWrite(9, HIGH);
    delay(1000);
    digitalWrite(9, LOW);
    Serial.println("Deleted");
  }
  
  // Makes data file and checks if it exists
  myFile = SD.open("data.txt", FILE_WRITE);
  if (SD.exists("data.txt")) {
    Serial.println("File created successfully");
    digitalWrite(9, HIGH);
    delay(1000);
    digitalWrite(9, LOW);
  } else {
    Serial.println("File creation failed!");
    fileCreationFail();
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
    myFile.println("|Time|AccX|Y|Z|Them|Humidity|iTemp");
    myFile.close();
    Serial.println("  done");
    digitalWrite(9, HIGH);
    delay(1000);
    digitalWrite(9, LOW);
      

  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
    fileOpenError();
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
    fileOpenError();
    return;
    
  }
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  
}
void loop() {
  sensors_event_t a, g, temp;
	mpu.getEvent(&a, &g, &temp);

  digitalWrite(11, HIGH);
  delay(1000);
  digitalWrite(11, LOW);
  // Clock. Runs 5 seconds.
  if (!clock.isRunning()) {
    clock.start(5000);
    digitalWrite(12, HIGH);
  }
  digitalWrite(12, LOW);
  if (clock.justFinished()) {
    // Every 5 seconds runs in this. All sensor reading will occur here
    seconds += 5.00;
    time = (seconds/60.00);
    Serial.println("");
    // Accelerometer Data Retrival
    float accelerometer_x = a.acceleration.x;
    float accelerometer_y = a.acceleration.y; 
    float accelerometer_z = a.acceleration.z;
    float iTemperature = temp.temperature;
    
    // Temp & Humidity Data Retrival
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(true);

    // Writing to disk - If you can concatante text please LMK cuz this is annoying.
    myFile = SD.open("data.txt", FILE_WRITE);
    if (myFile) {
      myFile.print("|");
      myFile.print(time);
      myFile.print("|");
      myFile.print(accelerometer_x);
      myFile.print("|");
      myFile.print(accelerometer_y);
      myFile.print("|");
      myFile.print(accelerometer_z);
      myFile.print("|");
      myFile.print(temperature);
      myFile.print("|");
      myFile.print(humidity);
      myFile.print("|");
      myFile.print(iTemperature);
      myFile.print("|");
      myFile.print(.35);
      myFile.print("|");
      myFile.println(1.23);
      myFile.close();
      digitalWrite(9, HIGH);
      delay(100);
      digitalWrite(9, LOW);
      delay(100);
      digitalWrite(9, HIGH);
      delay(100);
      digitalWrite(9, LOW);
      // WARNING! Printing the file to serial only works as long as the time taken to print the contents is less than 5 seconds. This happens at around ~3 minutes so 
      // use this only for debugging and disable when testing for long periods!
      if(needRead == true){
        read();
      }
    }else{
      // if the file didn't open, print an error:
      Serial.println("error opening data.txt");
      fileOpenError();
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
    digitalWrite(12, HIGH);
    digitalWrite(12, LOW);
  } else {
   //if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    fileOpenError();
  }
}

void sdCardError(){
  while(true){  
    digitalWrite(10, HIGH);
    delay(1000);
    digitalWrite(10, LOW); // SD card error gives one flash
    delay(3000);
  }
}

void fileCreationFail(){
  while(true){  
    for(int i=0; i<6; i++){
      digitalWrite(10, HIGH);
      delay(1000);
      digitalWrite(10, LOW); // SD card error gives one flash
      delay(3000);
    }
  }
}
void accelFail(){
  while(true){  
    for(int i=0; i<3; i++){
      digitalWrite(10, HIGH);
      delay(1000);
      digitalWrite(10, LOW); 
      delay(3000);
    }
  }
}

void fileOpenError(){
  while(true){
    for(int i=0; i<4; i++){
      digitalWrite(10, HIGH);
      delay(1000);
      digitalWrite(10, LOW); 
      delay(3000);
    }
  }
}

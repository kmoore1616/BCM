// Library Setup
#include <millisDelay.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#include <Wire.h>

// Pins
const int chipSelect = 53;
#define DHTPIN 8
#define DHTTYPE DHT22
const int MPU_ADDR = 0x68;

// Library Instantiation 
File myFile;
millisDelay clock;
DHT dht(DHTPIN, DHTTYPE);

// Variables
int seconds = 0;
float time = 0.0;

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
char tmp_str[7];

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}


void setup() {
  // Opens serial communications
  Serial.begin(9600);
  // Sets up LEDs
  pinMode(10, OUTPUT); // Red
  pinMode(9, OUTPUT); // Green

  // Sets up sensor pins
  dht.begin();
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0);
  Wire.endTransmission(true);

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
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true);
  // Clock. Runs 5 seconds.
  if (!clock.isRunning()) {
    clock.start(5000);
    digitalWrite(12, HIGH);
  }
  digitalWrite(12, LOW);
  if (clock.justFinished()) {
    // Every 5 seconds runs in this. All sensor reading will occur here
    seconds += 5;
    time = (seconds/60);
    Serial.println("");
    // Accelerometer Data Retrival
    accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
    String x = convert_int16_to_str(accelerometer_x);
    String y = convert_int16_to_str(accelerometer_y);
    String z = convert_int16_to_str(accelerometer_z);
    String iTemp = convert_int16_to_str(temperature/340.00+36.53);

    x.trim();
    y.trim();
    z.trim();
    iTemp.trim();
    

    // Temp & Humidity Data Retrival
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(true);

    // Writing to disk - If you can concatante text please LMK cuz this is annoying.
    myFile = SD.open("data.txt", FILE_WRITE);
    if (myFile) {
      myFile.print("|");
      myFile.print(time);
      myFile.print("|");
      myFile.print(x);
      myFile.print("|");
      myFile.print(y);
      myFile.print("|");
      myFile.print(z);
      myFile.print("|");
      myFile.print(temperature);
      myFile.print("|");
      myFile.print(humidity);
      myFile.println("|");
      myFile.println(iTemp);
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
      read();
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

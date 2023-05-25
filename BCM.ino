// Library Setup
#include <millisDelay.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "Adafruit_SGP30.h"

const int chipSelect = 4;

Adafruit_MPU6050 mpu; 
Adafruit_SGP30 sgp;
Adafruit_BME680 bme; 
File myFile;
millisDelay clock;
#define SEALEVELPRESSURE_HPA (1013.25)

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

// Variables

double seconds;
double time;
unsigned long mills;

// Change to true if readout is needed after every write
// If left on it will stall the program as the process of printing 
// will take longer than 5 seconds.
bool needRead = false;
bool mpuIsPresent = false;

void setup() {
  // Opens serial communications
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Sets up sensor pins
  Wire.begin();

 if (!bme.begin()) {
   Serial.println("Could not find a valid BME680 sensor, check wiring!");
   bmeFail();
 }


  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  if (! sgp.begin()){
    Serial.println("SGP Sensor not found :(");
    sgpFail();
  }

  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
  
  // if (!mpu.begin()) {
	// 	Serial.println("Failed to find MPU6050 chip");
	// }
  
  // Mpu setup
  // mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  // mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);



  // Debug stuff to make sure SD card works
  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    sdCardError();
  }
  Serial.println("initialization done");

  

  // Checks if file already exists and stops program if it does. Will implement led error code system to warn user.
  if (SD.exists("data.txt")) {
    Serial.print("File exists, deleting...");
    SD.remove("data.txt");

    Serial.println("Deleted");
  }
  Serial.print("Here");
  // Makes data file and checks if it exists
  myFile = SD.open("data.txt", FILE_WRITE);
  if (SD.exists("data.txt")) {
    Serial.println("File created successfully");
  } else {
    Serial.println("File creation failed!");
    fileCreationFail();
    return;
  }
  myFile.close();
  Serial.print("Here");
  // Writes Boilerplate code
  myFile = SD.open("data.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing starter code...");
    // Writes Boilerplate Code
    myFile.println("****//BCM//****");
    myFile.println("|Time|Acc|Temp|Humidity|mpuTemp|Pressure|Altitude|eCO2|TVOC");
    myFile.close();
    Serial.println("  done");
      

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
  
}

int counter = 0;
void loop() {
  sensors_event_t a, g, temp;
	mpu.getEvent(&a, &g, &temp);

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }

  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  // Clock. Runs 5 seconds.
  if (!clock.isRunning()) {
    clock.start(5000);
    
  }
  
  if (clock.justFinished()) {
    // Every 5 seconds runs in this. All sensor reading will occur here

    mills = (millis());
    seconds = mills/1000;
    time = seconds/60;
    Serial.println("");
    // Data Retrival
    if (mpuIsPresent){
      float accelerometer_x = a.acceleration.x;  // Which one is up?
      float accelerometer_y = a.acceleration.y; 
      float accelerometer_z = a.acceleration.z;
      float mpuTemperature = temp.temperature;   
    } 
    float mainTemperature = bme.temperature;
    float pressure = bme.pressure / 100;
    float bmeHumidity = bme.humidity;
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

    float eCO2 = sgp.eCO2;
    float TVOC = sgp.TVOC;

    // Writing to disk
    // Format: |Time|Acc|Temp|Humidity|mpuTemp|Pressure|Altitude|eCO2|TVOC
    myFile = SD.open("data.txt", FILE_WRITE);
    if (myFile) {
      myFile.print("|");
      myFile.print(time);
      myFile.print("|");
      myFile.print(69); // Acceleration
      myFile.print("|");
      myFile.print(mainTemperature);
      myFile.print("|");
      myFile.print(bmeHumidity);
      myFile.print("|");
      myFile.print(420);
      myFile.print("|");
      myFile.print(pressure);
      myFile.print("|");
      myFile.print(altitude);
      myFile.print("|");
      myFile.print(eCO2);
      myFile.print("|");
      myFile.println(TVOC);
      myFile.close();
      
      counter++;
      if (counter == 10) {
        counter = 0;

        uint16_t TVOC_base, eCO2_base;
        if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
          Serial.println("Failed to get baseline readings");
        }
      }
      
      // WARNING! Printing the file to serial only works as long as the time taken to print the contents is less than 5 seconds. This happens at around ~3 minutes so 
      // use this only for debugging and disable when testing for long periods!
      if(needRead == true){
        read();
      }
    }else{
      // if the file didn't open, print an error:
      Serial.println("error opening data.txt");
      // fileOpenError();
      return;
    }
    Serial.println(time);

        
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
    // fileOpenError();
  }
}

void sdCardError(){
  while(true){  
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW); // SD card error gives one flash
    delay(3000);
  }
}
void fileCreationFail(){
  while(true){  
    for(int i=0; i<6; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW); // SD card error gives one flash
      delay(3000);
    }
  }
}
void accelFail(){
  while(true){  
    for(int i=0; i<3; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW); 
      delay(3000);
    }
  }
}
void fileOpenError(){
  while(true){
    for(int i=0; i<4; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW); 
      delay(3000);
    }
  }
}
void bmeFail(){
  while(true){
    for(int i=0; i<5; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW); 
      delay(3000);
    }
  }
}
void sgpFail(){
  while(true){
    for(int i=0; i<6; i++){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW); 
      delay(3000);
    }
  }
}
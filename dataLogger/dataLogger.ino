#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <DS18B20.h>

const int interval = 5; // seconds between each log entry
#define FILENAME "datalog.csv"


const int chipSelect = 4;
uint32_t nextLog = 0;

#define ONE_WIRE_BUS 0 // D3 
OneWire oneWire(ONE_WIRE_BUS);
DS18B20 tempSensor(&oneWire);
uint8_t sensorAddr[8];
bool tempPresent = false;

void fileWrite(String data) {
  File dataFile = SD.open(FILENAME, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // print to the serial port too:
    Serial.println(data);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

bool findTempSensor() {
  if ( !tempSensor.getAddress(sensorAddr)) {
    Serial.println("No sensors found");
    return false;
  }
  
  Serial.print("Found temp sensor with address:");
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(" ");
    Serial.print(sensorAddr[i]);
  }
  Serial.println();
  tempPresent = true;
  return true;
  
}

float getTemp() {
  tempSensor.requestTemperatures();
  while (!tempSensor.isConversionComplete());
  return tempSensor.getTempC();
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
  }
  tempSensor.begin();
  findTempSensor();

  pinMode(A0, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print("Initializing SD card...");

  // check if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) {
      digitalWrite(LED_BUILTIN, (millis()/1000) % 2);
    }
  }
  Serial.println("card initialized.");

  fileWrite("\ntime,sensor");
}



void loop() {
  if (millis() > nextLog) {
    nextLog = millis() + (interval*1000);
    digitalWrite(LED_BUILTIN, LOW);
  
    // make a string for assembling the data to log:
    String dataString = "";
    
    // log the time
    dataString+=millis()/1000;
    dataString += ",";
    Serial.println(sizeof(sensorAddr));
    if (tempPresent) {
    // get temperature
    dataString+=getTemp();
    } else {
      // read other sensor
      int sensor = analogRead(A0);
      dataString += String(sensor);
    }
    
    fileWrite(dataString);

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    
    digitalWrite(LED_BUILTIN, HIGH);
  }
}










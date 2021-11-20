#include <SD.h>
#include <SPI.h>
#include <time.h>
#include <Ticker.h>

#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 7//Pin 10

OneWire oneWire(ONE_WIRE_BUS); //init oneWire bus for temperature sensor
DallasTemperature sensors(&oneWire); //init temperature sensor
DeviceAddress address; //address of temperature sensor
volatile bool toggle = 1; //used by temperature checking subroutine
volatile int8_t temperature; //current temperature
elapsedMillis time1; //multipurpose timer, used for file start/stop

 unsigned long timeSinceStart = millis();

File myFile;

// Teensy 3.5 & 3.6 & 4.1 on-board: BUILTIN_SDCARD
const int chipSelect = BUILTIN_SDCARD;

void printTemp();

Ticker temperatureTicker(printTemp, 1000, 0, MILLIS); //ticker for temperature. calls the function printTemp every 1000 milliseconds continuously. 

void setup()
{
 //double tempData = analogRead(A6);

  Serial.begin(9600); //setup serial monitor 
  delay(1000);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. 
  myFile = SD.open("temp.txt", FILE_WRITE);
  initTemp(); //initializing temperature function 
  temperatureTicker.start();//required to start the timer for the Ticker
  time1=0;
  // if the file opened okay, write to it:



  /* BELOW IS FOR READING FROM THE FILE. 
  // re-open the file for reading:
  myFile = SD.open("temp.txt");
  if (myFile) {
    Serial.println("temp.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening temp.txt");
  }
  */
}//close void setup

/*Below is standard temperature reading if it was a simple breadboard-based temperature sensor.Because the Sensor is not the usual one, an external library will be used. 
 * void printTemp(){
      double tempData = analogRead(7);
      tempData =  tempData / 1024; //find percentage of input reading
      tempData = tempData * 5; //multiply by 5V to get voltage
      tempData = tempData - 0.5; //Subtract the offset 
      tempData = tempData * 100; //Convert to degrees 
      myFile.println(tempData, 2);
}//close printTemp*/


void loop(){
  if (myFile) {
      /*if(timeSinceStart > 10000){
        break;
      }*/
    temperatureTicker.update(); //update the ticker 
  }//close if(myFile) 
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening temp.txt");
  }
  if (millis()>20000){ //I have it set to only print out while the time is greater than 20 seconds. 
    myFile.close();
    Serial.println("done.");
    exit(0);
  }
}

void initTemp(){
  //use the address of the temperature sensor to speed up communications
  sensors.getAddress(address, 0);
  //don't wait for the temperature sensor to convert data, waiting is built in to timer
  sensors.setWaitForConversion(0);
  //init time
  //setSyncProvider(getTeensy3Time); 
}

void printTemp(){
  if(toggle){
    sensors.requestTemperatures();
    toggle = 0;
  }
  else{
    temperature = sensors.getTempC(address);
    myFile.println(temperature);
    Serial.println(temperature);
    toggle = 1;
  }
}

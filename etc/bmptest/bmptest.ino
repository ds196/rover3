#include "Adafruit_BMP3XX.h"
//#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>

#define I2C_SDA_PIN     4 //I2C SDA
#define I2C_SCL_PIN     5 //I2C SCL


#define SEALEVELPRESSURE_HPA (1013.25)
const int SERIAL_BAUD = 115200;
Adafruit_BMP3XX bmp;

void setup() {

  //Initialization
  //**************
  // Initialize LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
  // Turn LED on for initialization
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Configure serial transport
  Wire.begin(); //Join I2C bus
  Serial.begin(SERIAL_BAUD); //115200 set in original code, 9600 default for pico
  delay(100);
  
  // Turn LED off after serial initialization
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Initialization complete");
  Serial.println("-------------");


  // Setup bmp I2C
    if (!bmp.begin_I2C()) {   // hardware I2C mode
      Serial.println("Could not find a valid BMP3 sensor, check wiring!");
      while (1);
    }
    /*int i = 0;
    while(i < 100 && !bmp.begin(I2C)) {
      i++;
      if(bmp.begin(I2C)){
        break;
      }
      delay(50);
    }*/
    digitalWrite(LED_BUILTIN, HIGH);

    // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop() {
  if (Serial.available()) {
    //Take in str input from Serial
    String input = Serial.readStringUntil('\n');
    input.trim();
    //input = toLowerCase(input);

    //Parse into array separated by spaces
    String args[20];
    int argCount = 0;
    while (input.length() > 0) {
      //Stolen from https://forum.arduino.cc/t/how-to-split-a-string-with-space-and-store-the-items-in-array/888813
      int index = input.indexOf(' ');
      if (index == -1) { // No space found
        args[argCount++] = input;
        break;
      } else {
        args[argCount++] = input.substring(0, index);
        input = input.substring(index+1);
      }
    }
    String command = args[0];

    // Begin commands
  
    if        (command == "led_on") {
      digitalWrite(LED_BUILTIN, HIGH);

    } else if (command == "led_off") {
      digitalWrite(LED_BUILTIN, LOW);

    } else if (command == "ping") {
      Serial.println("pong");
      Serial.println("-------------");

    
    } else if (command == "pollbmp") {
      if (! bmp.performReading()) {
        Serial.println("Failed to perform reading :(");
        return;
      }
      //temp record
      Serial.print("Temperature = ");
      Serial.print(bmp.temperature);
      Serial.println(" *C");
      //presure record
      Serial.print("Pressure = ");
      Serial.print(bmp.pressure / 100.0);
      Serial.println(" hPa");
      //approx altitude
      Serial.print("Approx. Altitude = ");
      Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
      Serial.println(" m");
      Serial.println("-------------");
    }
  }

}

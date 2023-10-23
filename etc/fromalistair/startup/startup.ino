#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Servo.h>

int servo1angle=0;
int servo2angle=0;
int servo3angle=0;
int servo4angle=0;
//temp and presure sensor
Adafruit_BMP3XX bmp;
//servos
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
int servopos1=0;
//starting position of servos
int motor1angle =0;
int motor2angle =0;
int motor3angle =0;
int motor4angle =0;

void read_Back(){
  //sends the servo position to the libra
  Wire.beginTransmission(20);
  servo1angle=servo1.read();
  servo2angle=servo2.read();
  servo3angle=servo3.read();
  servo4angle=servo4.read();
  Serial.print("servo1 ");
  Serial.println(servo1angle);
  Serial.print("servo2 ");
  Serial.println(servo2angle);
  Serial.print("servo3 ");
  Serial.println(servo3angle);
  Serial.print("servo4 ");
  Serial.println(servo4angle);
  Wire.write(servo1angle);
  Wire.write(servo2angle);
  Wire.write(servo3angle);
  Wire.write(servo4angle);
  Wire.endTransmission(20);
  return ;
}




void setup() {
  servo1.attach(20,500,2500);
  servo2.attach(19,500,2500);
  servo3.attach(18,500,2500);
  servo4.attach(17,500,2500);
  /*bool a=false;
  while(1==1){
    if(a==true){
      break;
    }
  }*/
  Wire.begin(); 
  Serial.begin(9600);
  //check for conection
  /*
  if (!bmp.begin_I2C()) { 
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }
  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  */
}

void loop() {
  //reads the servos' current angle
  read_Back();/*
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
  */
  delay(200);

  // put your main code here, to run repeatedly:
  /*for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees

    // in steps of 1 degree

    servo3.write(pos);              // tell servo to go to position in variable 'pos'
    read_Back();
    delay(15);                       // waits 15ms for the servo to reach the position

  }

  for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees

    servo3.write(pos);              // tell servo to go to position in variable 'pos'
    read_Back();
    delay(15);                       // waits 15ms for the servo to reach the position

  }*/
  //servo3.write(Serial.read());

}
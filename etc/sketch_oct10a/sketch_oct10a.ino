#include <Wire.h>
#include <SPI.h>
#include <Servo.h>

#define MOTOR_IN1_PIN  14 //Named "MOTOR_AIN1_IO" on schematic
#define MOTOR_IN2_PIN  15 // "
#define MOTOR_PWM_PIN  16 //Motor Controller PWM
#define MOTOR_STBY_PIN 17 //NOT PLUGGED IN!

const int SERIAL_BAUD = 115200;

void setup() {

  Serial.begin(SERIAL_BAUD);
  delay(100);
  
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_STBY_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if(command == "runmotor") {
      move(100, 0);
      Serial.println("Motor ran.");
      Serial.println("-------------");
    } else
    if(command == "stopmotor") {
      stop();
    }
  }
}

void move(int speed, int direction){
//Stolen from https://adam-meyer.com/arduino/TB6612FNG
//Move motor at speed and direction
//speed: 0 is off, and 255 is full speed
//direction: 0 clockwise, 1 counter-clockwise

  digitalWrite(MOTOR_STBY_PIN, HIGH); //disable standby

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if(direction == 1){
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  digitalWrite(MOTOR_IN1_PIN, inPin1);
  digitalWrite(MOTOR_IN2_PIN, inPin2);
  analogWrite(MOTOR_PWM_PIN, speed);
}

void stop(){
//enable standby  
  digitalWrite(MOTOR_STBY_PIN, LOW);
}
/*****************************************************
 * Rover 3 *A.S.T.R.A.
 * PICO CODE
 * v0.2.4
 * 10/05/23
 *****************************************************/

/* Notes:
 ********
 * NOT TESTED: (I wrote these after leaving the lab)
    Motor
    Command parsing
 * This has just about everything from the "startup.ino"
   sent to me 9/20/23 just not read_Back()
*/

/* Libraries used:
 *****************
 * Adafruit BMP3XX Library (not 107)
 * Adafruit BNO055         (not sparkfun IMU)
 * Adafruit Unified Sensor
 * Sparkfun TB6612FNG Motor Driver (same chip, different breakout board)
 *                                 (https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library)
 * idk where Servo SPI or Wire comes from...
 */
//Headers
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_BNO055.h>
#include <Servo.h>
#include <SparkFun_TB6612.h>



//Enable/Disable
#define BMP_ENABLED    1
#define BNO_ENABLED    1
#define SERVO1_ENABLED 1
#define SERVO2_ENABLED 1
#define SERVO3_ENABLED 1
#define SERVO4_ENABLED 1
#define MOTOR_ENABLED  1
#define SD_ENABLED     0 //OpenLog



//Pins
#define SD_TX_PIN       0 //UART TX to OpenLog RX
#define SD_RX_PIN       1 //UART RX to OpenLog TX
#define SD_IO_PIN       2 //DTR/CTS??? DTR on schematic
#define I2C_SDA_PIN     4 //I2C SDA
#define I2C_SCL_PIN     5 //I2C SCL
#define SERVO_1_PIN    10 //Metal Gear Servos
#define SERVO_2_PIN    11 // "
#define SERVO_3_PIN    12 // "
#define SERVO_4_PIN    13 //High Torque Servo
#define MOTOR_IN1_PIN  14 //Named "MOTOR_AIN1_IO" on schematic
#define MOTOR_IN2_PIN  15 // "
#define MOTOR_PWM_PIN  16 //Motor Controller PWM
//#define MOTOR_STBY_PIN 17 //NOT PLUGGED IN!
//#define LED_PWM_PIN    18 //NOT PLUGGED IN! 
#define LED_PIN        25 //On-board LED



//Predeclarations
#define SEALEVELPRESSURE_HPA (1013.25)
#define SERVO_MAX 2500
#define SERVO_MIN  500
const int STOP_INPUT 90;
const int SERIAL_BAUD = 115200;
const int MOTOR_OFFSET = 1;
// BNO
double xPos = 0, yPos = 0, headingVel = 0;
double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees
// OpenLog
String LOG_FILE = "rover3log.txt";



//Setup hardware vars
Adafruit_BMP3XX bmp;
//                                    id,addr
Adafruit_BNO055 bno = Adafruit_BNO055(55,0x28,&Wire);
//OpenLog openLog; // used wrong lib haha
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Motor motor = Motor(MOTOR_IN1_PIN, MOTOR_IN2_PIN, MOTOR_PWM_PIN, MOTOR_OFFSET /*, MOTOR_STBY_PIN*/);



//****************************************************
void setup() {

  //Initialization
  //**************
  // Initialize LED_PIN as an output
  pinMode(LED_PIN, OUTPUT);
  // Turn LED on for initialization
  digitalWrite(LED_PIN, HIGH);
  
  // Configure serial transport
  Wire.begin(); //Join I2C bus
  Serial.begin(SERIAL_BAUD); //115200 set in original code, 9600 default for pico
  delay(100);
  
  // Turn LED off after serial initialization
  digitalWrite(LED_PIN, LOW);
  Serial.println("Initialization complete");
  Serial.println("-------------");


  //Servos
  //******
  if(SERVO1_ENABLED) servo1.attach(SERVO_1_PIN /*, SERVO_MIN, SERVO_MAX*/);
  if(SERVO2_ENABLED) servo2.attach(SERVO_2_PIN);
  if(SERVO3_ENABLED) servo3.attach(SERVO_3_PIN);
  if(SERVO4_ENABLED) servo4.attach(SERVO_4_PIN);


  //BMP Sensor
  //**********
  if(BMP_ENABLED) {
    // Setup bmp I2C
    if (!bmp.begin_I2C()) {   // hardware I2C mode
      Serial.println("Could not find a valid BMP3 sensor, check wiring!");
      while (1);
    }

    // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  } else {
    Serial.println("BMP not enabled, check preprocessor statements");
  }


  //BNO Sensor
  //**********
  if(BNO_ENABLED) {
    if (!bno.begin()) {
      /* There was a problem detecting the BNO055 ... check your connections */
      Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      while (1);
    }
    //Not much to do here lol
  } else {
    Serial.println("BNO not enabled, check preprocessor statements");
  }


  //OpenLog (SD)
  //************
  //Wrong lib hahaha
  //Tried to use Qwiic but we don't have that one
  //Ours uses UART
  /*if(SD_ENABLED) {
    openLog.begin();
    openLog.append(LOG_FILE);
    openLog.println("Begin log");
  } else {
    Serial.println("Openlog not enabled, check preprocessor statements");
  }*/

  Serial.println("Hardware configured.");
  Serial.println("-------------");
  
}



//****************************************************
void loop() {
  /*Command interpreter
   ********************
   * To see a list of commands, see 'help' command
   */int incrementer=2;
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    String commandID=command.substring(0,2);
    
    if(commandID=="sr"){
      for (int i=incrementer;i<command.length();i++){
        if(isDigit(command[i])){
          String servo1str+=command[i];
        }
        else
        {
          incrementer=i+1;
          break;
        }
      }
      for (int i=incrementer;i<command.length();i++){
        if(isDigit(command[i]))
        {
          String servo2str+=command[i];
        }
        else
        {
          incrementer=i+1;
          break;
        }
      }
      for (int i=incrementer;i<command.length();i++){
        if(isDigit(command[i]))
        {
          String servo3str+=command[i];
        }
        else
        {
          incrementer=i+1;
          break;
        }
      }
      for (int i=incrementer;i<command.length();i++){
        if(isDigit(command[i]))
        {
          String servo4str+=command[i];
        }
        else
        {
          incrementer=i+1;
          break;
        }
      }
      for (int i=incrementer;i<command.length();i++){
        if(isDigit(command[i]))
        {
          String motorstr+=command[i];
        }
        else
        {
          incrementer=i+1;
          break;
        }
      }
      
      servo1.write(servo1str.toInt());
      servo2.write(servo2str.toInt());
      servo3.write(servo3str.toInt());
      servo4.write(servo4str.toInt());
    }
    
    //General commands
    //****************

    if        (command == "led_on") {
      digitalWrite(LED_PIN, HIGH);

    } else if (command == "led_off") {
      digitalWrite(LED_PIN, LOW);

    } else if (command == "ping") {
      Serial.println("pong");
      Serial.println("-------------");

    } else if (command == "time") {
      Serial.println(millis());
      Serial.println("-------------");

    } else if (command == "line") {
      Serial.println("-------------");
    
    } else if (command == "help") {
      //Serial.println("Sounds like it sucks to be you");
      Serial.println("led_on; led_off; ping; time; line; pollbmp; pollbno; sweepservo1;");
      Serial.println("startservo2; stopservo2; startmotor")
      Serial.println("-------------");

    //Sensors
    //*******
    
    } else if (command == "pollbmp") {
      if(BMP_ENABLED) {
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
      } else {
        Serial.println("BMP not enabled, check preprocessor statements");
      }
    
    } else if (command == "pollbno") {
      if(BNO_ENABLED) {
        //Poll data from bno
        sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
        bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
        bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
        bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
        bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
        bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
        bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);
        //Print data
        Serial.print("Orientation x: ");
        Serial.println(orientationData.orientation.x);
        Serial.print("Orientation y: ");
        Serial.println(orientationData.orientation.y);
        Serial.print("Accel x: ");
        Serial.println(linearAccelData.orientation.x);
        Serial.print("Accel y: ");
        Serial.println(linearAccelData.orientation.y);
        Serial.println("-------------");
      } else {
        Serial.println("BNO not enabled, check preprocessor statements");
      }
    
    //Servos
    //******

    } else if (command == "sweepservo1") {
      if(SERVO1_ENABLED) {
        for (int pos = 0; pos <= 180; pos += 1) { // goes from 0* to 180*
          servo1.write(pos);
          delay(15);
        }
        for (int pos = 180; pos >= 90; pos -= 1) { // goes from 180* to 0*
          servo1.write(pos);
          delay(15);
        }
        Serial.println("Servo 1 swept.");
        Serial.println("-------------");
      } else {
        Serial.println("Servo 1 not enabled, check preprocessor statements");
      }
    
    } else if (command == "startservo2") {
      if(SERVO2_ENABLED) {
        servo2.write(Servo_input1);
        Serial.println("Servo 2 started.");
        Serial.println("-------------");
      } else {
        Serial.println("Servo 2 not enabled, check preprocessor statements");
      } 
    
    } else if (command == "stopservo2") {
      if(SERVO2_ENABLED) {
        servo2.write(90);
        Serial.println("Servo 2 stopped.");
        Serial.println("-------------");
      } else {
        Serial.println("Servo 2 not enabled, check preprocessor statements");
      }
    
    //Motor
    //*****
    
    } else if (command == "startmotor") {
      if(MOTOR_ENABLED) {
        Serial.println("CAUTION: MOTOR HAS BEEN ATTACHED TO ARM.");
        Serial.println("RUNNING MOTOR MIGHT BREAK OUR ARM.");
        Serial.println("RUN startmotorsure TO RUN MOTOR ANYWAYS");
        Serial.println("WITH UNTESTED CODE.");
        Serial.println("-------------");
      } else {
        Serial.println("Motor not enabled, check preprocessor statements");
      }
    
    } else if (command == "startmotorsure") {
      if(MOTOR_ENABLED) {
        //Consult MotorTestRun.ino example file
        //        speed, duration
        motor.drive(255, 500);
        motor.brake();
        Serial.println("Motor ran.");
        Serial.println("-------------");
      } else {
        Serial.println("Motor not enabled, check preprocessor statements");
      }
    
    }
  }


  //Collect BNO Data
  //****************
  //if(BNO_ENABLED) {
  //  
  //}
}

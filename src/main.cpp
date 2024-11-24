#include <Arduino.h>
#include "functions.h" //For the movement fucntions
#include <motors.h>

//DEFINE
//movement definitions
#define MOTOR_A1 11 //left motor backwords  speed 0/255
#define MOTOR_A2 13 //left motor forward   direction 1/0
#define MOTOR_B1 9  //right motor forward   speed 0/255
#define MOTOR_B2 12  //right motor backwords  direction 1/0
//Ultra sonic sensor
#define TRIGER 3
#define ECHO 2
//Servo motor
#define SERVOPIN 8

Motors motor(11,13,9,12);

const int OPENGRIPPER = 2;
const int CLOSEGRIPPER = 1.5;
void servo(int postition);

void setup() {
  Serial.begin(9600);
  // pinMode(MOTOR_A1, OUTPUT);
  // pinMode(MOTOR_A2, OUTPUT);
  // pinMode(MOTOR_B1, OUTPUT);
  // pinMode(MOTOR_B2, OUTPUT);
  pinMode(TRIGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(SERVOPIN, INPUT);
    motor.setup(
    0, 0, 255, 255
  );
}


void loop() {
  //  moveForward(8, 80);
  //  moveBackwords(8, 80);
  //  turnLeft(4, 80);
  //  turnRight(6.5, 80);
  //  rotateLeft(3, 80);
  //  rotateRight(2, 80);
  //  fullStop();

  //int test = test_distance();
  //delay(1000);
  //move(80);
 //test();
 test2();
}

void servo(int position){
  for(int x=0; x < 40; x++){
    Serial.println(x);
    digitalWrite(SERVOPIN, HIGH); 
    delay(position);
    digitalWrite(SERVOPIN, LOW);
    delay(20);
  }
}

void test(){
  while (test_distance() > 15)
  {
    motor.setLeftMotorSpeed(255);
    motor.setRightMotorSpeed(255);
    motor.forward();
  }
  if(test_distance() < 15){
    motor.setLeftMotorSpeed(255);
    motor.setRightMotorSpeed(255);
    Serial.println("START");
    motor.right();
    Serial.println("RIGHT");
    delay(500);
    motor.stop();
    motor.forward();
    Serial.println("FORWARD");
    delay(1000);
    motor.stop();
    motor.left();
    Serial.println("LEFT");
    delay(500);
    motor.stop();
    motor.forward();
    Serial.println("FORWARD");
    delay(1000);
    motor.stop();
    motor.right();
    Serial.println("RIGHT");
    delay(500);
    motor.stop();
    Serial.println("END");
  }
  
}

  
void test2(){
  //motor.setLeftMotorSpeed(255);
  //  motor.setRightMotorSpeed(255);
  
    motor.zeroLeft();
    delay(1000);
}
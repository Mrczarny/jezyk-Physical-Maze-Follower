#include <Arduino.h>

//Functions
void moveForward(int seconds);
void moveBackwords(int seconds);
void turnRight(int seconds);
void turnLeft(int seconds);

//DEFINE
#define MOTOR_A1 11 //left motor backwords
#define MOTOR_A2 10 //left motor forward
#define MOTOR_B1 6  //right motor forward
#define MOTOR_B2 5  //right motor backword

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(MOTOR_B1, OUTPUT);
  pinMode(MOTOR_B2, OUTPUT);
}

void loop() {
   
}

//Takes the time it must run for
void moveForward(int seconds){
  analogWrite(MOTOR_A2, 255);
  analogWrite(MOTOR_B1, 255);
  delay(seconds * 1000);
  analogWrite(MOTOR_A2, 0);
  analogWrite(MOTOR_B1, 0);
}

//Takes the time it must run for
void moveBackwords(int seconds){
  analogWrite(MOTOR_A1, 200);
  analogWrite(MOTOR_B2, 200);
  delay(seconds * 1000);
  analogWrite(MOTOR_A1, 0);
  analogWrite(MOTOR_B2, 0);
}

//Takes the time it must run for
void turnRight(int seconds){
  analogWrite(MOTOR_A2, 200);
  analogWrite(MOTOR_B2, 0);
  delay(seconds * 1000);
  analogWrite(MOTOR_A2, 0);
  analogWrite(MOTOR_B2, 0);
}

//Takes the time it must run for
void turnLeft(int seconds){
  analogWrite(MOTOR_B1, 200);
  analogWrite(MOTOR_A1, 0);
  delay(seconds * 1000);
  analogWrite(MOTOR_B1, 0);
  analogWrite(MOTOR_A1, 0);
}

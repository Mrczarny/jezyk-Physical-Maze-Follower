#include <Arduino.h>
#include "functions.h" //For the movement fucntions


//DEFINE
//movement definitions
#define MOTOR_A1 11 //left motor backwords
#define MOTOR_A2 10 //left motor forward
#define MOTOR_B1 6  //right motor forward
#define MOTOR_B2 5  //right motor backwords
//Ultra sonic sensor
#define TRIGER 3
#define ECHO 2

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(MOTOR_B1, OUTPUT);
  pinMode(MOTOR_B2, OUTPUT);
  pinMode(TRIGER, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  //  moveForward(8, 80);
  //  moveBackwords(8, 80);
  //  turnLeft(4, 80);
  //  turnRight(6.5, 80);
  //  rotateLeft(3, 80);
  //  rotateRight(2, 80);
  //  fullStop();

  int distance = getDistance();
  delay(1000);
}


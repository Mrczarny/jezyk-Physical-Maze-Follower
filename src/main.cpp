#include <Arduino.h>
#include "movement.h" //For the movement fucntions


//DEFINE
//movement definitions
#define MOTOR_A1 11 //left motor backwords
#define MOTOR_A2 10 //left motor forward
#define MOTOR_B1 6  //right motor forward
#define MOTOR_B2 5  //right motor backwords

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(MOTOR_B1, OUTPUT);
  pinMode(MOTOR_B2, OUTPUT);
}

void loop() {
   moveForward(5, 80);
   moveBackwords(5, 80);
   rotateRight(5, 80);
   rotateLeft(5, 80);
   turnLeft(5, 80);
   turnRight(5, 80);
   fullStop();
}


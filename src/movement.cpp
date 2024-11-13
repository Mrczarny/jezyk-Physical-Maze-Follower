#include <Arduino.h>
#include "movement.h"

//DEFINE
#define MOTOR_A1 11 //left motor backwords
#define MOTOR_A2 10 //left motor forward
#define MOTOR_B1 6  //right motor forward
#define MOTOR_B2 5  //right motor backwords

////////IMPORTANT
////////FOR MOVEMENT USE SPEED IN PERCENTAGE!!!!!!!

//Will get the speed in percentage 0-100% power then return the value between 0-255
int calculatePower(int speed){
  int power = 2.55 * speed;
  return power;
}

void setMotorsOff(){
  analogWrite(MOTOR_A1, 0);
  analogWrite(MOTOR_A2, 0);
  analogWrite(MOTOR_B1, 0);
  analogWrite(MOTOR_B2, 0);
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void moveForward(int seconds, int speed){
  int power = calculatePower(speed);
  analogWrite(MOTOR_A2, power);
  analogWrite(MOTOR_B1, power);
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void moveBackwords(int seconds, int speed){
  int power = calculatePower(speed);
  analogWrite(MOTOR_A1, power);
  analogWrite(MOTOR_B2, power);
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void rotateRight(int seconds, int speed){
  analogWrite(MOTOR_A2, calculatePower(speed));
  analogWrite(MOTOR_B2, 0);
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void rotateLeft(int seconds, int speed){
  analogWrite(MOTOR_B1, calculatePower(speed));
  analogWrite(MOTOR_A1, 0);
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void turnRight(int seconds, int speed){
  analogWrite(MOTOR_A2, calculatePower(speed));
  analogWrite(MOTOR_B1, calculatePower(speed * 0.8));
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void turnLeft(int seconds, int speed){
  analogWrite(MOTOR_A2, calculatePower(speed * 0.8));
  analogWrite(MOTOR_B1, calculatePower(speed));
  delay(seconds * 1000);
  setMotorsOff();
}

//stops the robot
void fullStop(){
  analogWrite(MOTOR_B1, 0);
  analogWrite(MOTOR_A1, 0);
  delay(10000);
}
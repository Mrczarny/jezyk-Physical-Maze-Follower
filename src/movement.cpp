#include <Arduino.h>
#include "functions.h"
#include <motors.h>

//DEFINE
#define MOTOR_A1 11 //left motor backwords
#define MOTOR_A2 13 //left motor forward
#define MOTOR_B1 9  //right motor forward
#define MOTOR_B2 12  //right motor backwords

// Motors motor(11, 13, 12, 9);

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
  analogWrite(MOTOR_A2, 255);
  analogWrite(MOTOR_B1, 255);
  delay(300);
  int power = calculatePower(speed);
  analogWrite(MOTOR_A2, power);
  analogWrite(MOTOR_B1, power+5);
  //delay(seconds * 1000);
  //setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void moveBackwords(int seconds, int speed){
  analogWrite(MOTOR_A1, 255);
  analogWrite(MOTOR_B2, 255);
  delay(300);
  int power = calculatePower(speed);
  analogWrite(MOTOR_A1, power+5);
  analogWrite(MOTOR_B2, power-9);
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void rotateRight(int seconds, int speed){
  analogWrite(MOTOR_A2, 255);
  analogWrite(MOTOR_B2, 255);
  delay(300);
  int power = calculatePower(speed);
  analogWrite(MOTOR_A2, power);
  analogWrite(MOTOR_B2, power);
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void rotateLeft(int seconds, int speed){
  analogWrite(MOTOR_B1, 255);
  analogWrite(MOTOR_A1, 255);
  delay(300);
  int power = calculatePower(speed);
  analogWrite(MOTOR_B1, power);
  analogWrite(MOTOR_A1, power);
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void turnRight(int seconds, int speed){
  analogWrite(MOTOR_A2, 255);
  analogWrite(MOTOR_B1, 255);
  delay(300);
  analogWrite(MOTOR_A2, calculatePower(speed));
  analogWrite(MOTOR_B1, calculatePower(speed * 0.65));
  delay(seconds * 1000);
  setMotorsOff();
}

//Takes the time it must run for and the speed to run at IN PERCENTAGE
void turnLeft(int seconds, int speed){
  analogWrite(MOTOR_A2, 255);
  analogWrite(MOTOR_B1, 255);
  delay(300);
  analogWrite(MOTOR_A2, calculatePower(speed * 0.65)+5);
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

void move(int speed){
  while(test_distance() > 30){
    moveForward(2, speed);
  }
  if(test_distance() < 30){
    fullStop();
    turnRight(1, speed);
    moveForward(1, speed);
    delay(3000);
    turnLeft(1.5, speed);
    moveForward(1, speed);
    delay(3000);
    turnRight(1, speed);
  }
}


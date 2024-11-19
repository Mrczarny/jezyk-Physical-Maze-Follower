//Header file to prevent function decleration issues

// movement.h
#ifndef MOVEMENT_H
#define MOVEMENT_H

// Function Declarations
//movement functions
int calculatePower(int speed);
void setMotorsOff();
void moveForward(int seconds, int speed);
void moveBackwords(int seconds, int speed);
void rotateRight(int seconds, int speed);
void rotateLeft(int seconds, int speed);
void turnRight(int seconds, int speed);
void turnLeft(int seconds, int speed);
void fullStop();

//Ultra sonic
int getDistance();
int test_distance();

#endif // MOVEMENT_H

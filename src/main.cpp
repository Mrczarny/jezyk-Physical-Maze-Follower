#include <Arduino.h>
#include <motors.h>
#include <sonar.h>
#include <rotation.h>

//Motors and rotation sensor

//left motor backwords  speed 0/255 PIN 11 A1
//left motor forward   direction 1/0 PIN 2 A2
//right motor forward   speed 0/255 PIN 9 B1
//right motor backwords  direction 1/0 12 B2

Rotation rotation(3, 2);
Motors motor(11,A1,9,A0);

//Ultra sonic sensor1 FRONT
#define TRIGER1 7
#define ECHO1 10

//Ultra sonic sensor2 RIGHT
#define TRIGER2 5
#define ECHO2 6

//Ultra sonic sensor2 LEFT
#define TRIGER3 12
#define ECHO3 4

//Servo motor
#define SERVOPIN 8
const int OPENGRIPPER = 2;
const int CLOSEGRIPPER = 1.5;

//Ultra sonic sensor
Sonar s1(TRIGER1, ECHO1);
Sonar sRight(TRIGER2, ECHO2);
Sonar sLeft(TRIGER3, ECHO3);

// maze 
int distanceFront = 0;
int distanceRight = 0;
int distanceLeft = 0;


//functions
void distances();
void gapChecker();
void maze();
void tester();
void test();
void checkSide();
void deadend();
void hitWallFront();

void setup() {
  Serial.begin(9600);
  //Movement
  motor.setup(0, 0, 255, 255);
  rotation.setup(&motor, 0, 0);
  // motor.setLeftMotorSpeed(170);
  // motor.setRightMotorSpeed(255);
}


void loop() {
  distances();
  gapChecker();
  maze();
  hitWallFront();
  //test();
  //delay(1000);
}

void maze(){
  //Open area in front
  while(distanceFront > 14){
    motor.forward();
    distances();
    //Serial.println("Forward");
    //record where bot has gone
  }
  //Wall in front
  if(distanceFront < 13){
    motor.stop();
    // delay(400);
    //Serial.println("Stop");
    hitWallFront();
    deadend();
  }
  distances();
}

//if robot is 7cm away from wall it must move back in order to rotate properly
void hitWallFront(){
  distances();
  while(distanceFront < 8 && distanceFront > 0){
    rotation.moveBackwardFor(2);
    distances();
  }
}

//When robot reaches a dead end it must decide what to do
void deadend(){
  distances();
  hitWallFront();
  if(distanceFront < 14){
    //check if right side is open and left side is closed
    if(distanceRight > 13 && distanceLeft < 13){
      rotation.turnDegreesRight(90);
      Serial.println("Turn right");
    }
    //checks if left side is open and right closed
    else if(distanceRight < 13 && distanceLeft > 13){
      rotation.turnDegreesLeft(90);
      Serial.println("Turn left");
    }
    //if both sides are closed
    else if(distanceRight < 13 && distanceLeft < 13){
      distances();
      Serial.println("Go back Dead end");
      rotation.turnDegreesLeft(180);
    }
    //if both sides open
    else if(distanceRight > 13 && distanceLeft > 13){
      rotation.turnDegreesLeft(90);
      Serial.println("Turn left open both sides");
    }
  }
}

//Will check for crossroads
unsigned long timeGapChecker;
void gapChecker(){
    if(millis() > timeGapChecker){
        if(distanceRight > 15){
            //record that there is a opening
            //Serial.println("Opening to the right");
        }
        if(distanceLeft > 15){
            //record that there is a opening
            //Serial.println("Opening to the left");
        }
        timeGapChecker +=250;
    }
}

//Constanly get updated values of sonar distance
int oldFront = 0;
int oldLeft = 0;
int oldRight = 0;
unsigned long timeForUSS=0;
unsigned long sensorSpacing=0;
void distances(){
  static int count=0;
  if(millis() > timeForUSS){
    if(millis() > sensorSpacing){
      switch (count){
        case 0:
          distanceFront = s1.getDistance();
          if(distanceFront == 0){
            distanceFront = oldFront;
            count = 1;
            break;
          }
          oldFront = distanceFront;
          count = 1;
          // Serial.println("Front");
          // Serial.println(distanceFront);
          break;
        case 1:
          distanceRight = sRight.getDistance();
          if(distanceRight == 0){
            distanceRight = oldRight;
            count = 2;
            break;
          }
          oldRight = distanceRight;
          count = 2;
          // Serial.println("Right");
          // Serial.println(distanceRight);
          break;
        case 2:
          distanceLeft = sLeft.getDistance();
          if(distanceLeft == 0){
            distanceLeft = oldLeft;
            count = 0;
            break;
          }
          oldLeft = distanceLeft;
          count = 0;
          // Serial.println("Left");
          // Serial.println(distanceLeft);
          break;
      }
      sensorSpacing = millis() + 30;
    }
    timeForUSS = millis() + 250;
  }
}

void test(){
  rotation.turnDegreesLeft(90);
  delay(1000);
  
}

void checkSide(){
  if(distanceRight > 4){
    motor.right();
    delay(250);
  }
}
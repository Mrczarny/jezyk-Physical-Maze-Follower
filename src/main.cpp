#include <Arduino.h>
#include <motors.h>
#include <sonar.h>
#include <rotation.h>

//Motors and rotation sensor

//left motor backwords  speed 0/255 PIN 11 A1
//left motor forward   direction 1/0 PIN 2 A2
//right motor forward   speed 0/255 PIN 9 B1
//right motor backwords  direction 1/0 12 B2

Rotation rotation(4, 3);
Motors motor(11,A1,9,A0);

//Ultra sonic sensor1 FRONT
#define TRIGER1 7
#define ECHO1 10

//Ultra sonic sensor2 RIGHT
#define TRIGER2 5
#define ECHO2 6

//Ultra sonic sensor2 LEFT
#define TRIGER3 12
#define ECHO3 2

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
  motor.setLeftMotorSpeed(255);
  motor.setRightMotorSpeed(255);
}


void loop() {
  distances();
  gapChecker();
  maze();
  hitWallFront();

}

void maze(){
  //Open area in front
  while(distanceFront > 11){
    motor.forward();
    distances();
    //record where bot has gone
  }
  //Wall in front
  if(distanceFront < 7){
    motor.stop();
    Serial.println("Obstacle in way");
    hitWallFront();
    deadend();
  }
  distances();
}

//if robot is 7cm away from wall it must move back in order to rotate properly
void hitWallFront(){
  distances();
  if(distanceFront < 7 && distanceFront > 0){
    Serial.println("wall");
    Serial.println(distanceFront);
    rotation.moveBackwardFor(3);
    hitWallFront();
  }
}

//When robot reaches a dead end it must decide what to do
void deadend(){
  distances();
  hitWallFront();
  //check if right side is open and left side is closed
  if(distanceRight > 8 && distanceLeft < 8){
    rotation.turnDegreesRight(80);
  }
  //checks if left side is open and right closed
  else if(distanceRight < 8 && distanceLeft > 8){
    rotation.turnDegreesLeft(90);
  }
  //if both sides are closed
  else if(distanceRight < 8 && distanceLeft < 8){
    rotation.moveBackwardFor(10);
  }

}

//Will check for crossroads
unsigned int timeGapChecker;
void gapChecker(){
    if(millis() > timeGapChecker){
        if(distanceRight > 9){
            //record that there is a opening
            //Serial.println("Opening to the right");
        }
        if(distanceLeft > 9){
            //record that there is a opening
            //Serial.println("Opening to the left");
        }
        timeGapChecker +=250;
    }
}

//Constanly get updated values of sonar distance
unsigned int timeFront=0;
unsigned int timeRigthSensor=0;
unsigned int timeLeftSensor=0;

void distances(){
    if(millis() > timeFront){
        distanceFront = s1.getDistance();
        //Serial.println("front");
        //Serial.println(distanceFront);
        timeFront = millis() + 250;
    }
    if(millis() > timeRigthSensor){
        distanceRight = sRight.getDistance();
        //Serial.println("Right");
        //Serial.println(distanceRight);
        timeRigthSensor = millis() + 270;
    }
    if(millis() > timeLeftSensor){
        distanceLeft = sLeft.getDistance();
        //Serial.println("Right");
        //Serial.println(distanceRight);
        timeLeftSensor = millis() + 300;
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
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
void findNextOpeneing();
void checkRotate(int direction);
void rotated(boolean startRun);
void gameStart();

void setup() {
  Serial.begin(9600);
  //Movement
  motor.setup(0, 0, 255, 255);
  rotation.setup(&motor, 0, 0);
  
}

boolean ready = false;
void loop() {
  
  if(ready ==true){
    distances();
    gapChecker();
    maze();
    hitWallFront();
    rotation.checkRotation();
  }else{
    gameStart();
  }
  

}

void maze(){

  //Open area in front
  while(distanceFront > 14){
    motor.forward(50);
    distances();
    checkSide();
    rotation.checkRotation();
    
    //Serial.println("Forward");
    //record where bot has gone
  }
  //Wall in front
  if(distanceFront < 13){
    motor.stop();
    hitWallFront();
    rotation.checkRotation();
    distances();
    deadend();
  }
  distances();
}

//if robot is 7cm away from wall it must move back in order to rotate properly
void hitWallFront(){
  distances();
  while(distanceFront < 7){
    rotation.moveBackwardFor(2);
    distances();
  }
}

//checks if robot can turn, 0 = left, 1 = right, 2 = backwords
void checkRotate(int direction){
  distances();
  switch (direction)
  {
  case 0:
    if(distanceRight < 7){
      Serial.println("Check rotate left");
      motor.backwardFix(80, 0);
      delay(600);
      motor.forwardFix(40, 0);
      delay(600);
      
    }
    rotation.turnDegreesLeft(90);
    break;

  case 1:
    if(distanceLeft < 7){
      Serial.println("Check rotate right");
      motor.backwardFix(0, 20);
      delay(200);
      motor.forwardFix(0, 40);
      delay(200);
      
    }
    rotation.turnDegreesRight(90);
    break;

  case 2:
    if(distanceLeft < 7 || distanceRight < 7){
      motor.backwardFix(0, 20);
      delay(200);
      motor.forwardFix(0, 40);
      delay(200);
      
    }
    else{
      for(int x = 0; x < 1; x++){
        checkRotate(1);
      }
    }
  
    break;
  
  default:
    break;
  }
}

//to be run when robot has made a turn, in order to make sure it has moved before it decides what next to do
void rotated(){
  rotation.moveForwardFor(20);
  distanceFront = s1.getDistance();
  rotation.checkRotation();
  distances();
}

//When robot reaches a dead end it must decide what to do
void deadend(){
  rotation.checkRotation();
  distances();
  hitWallFront();
  if(distanceFront < 14){
    //check if right side is open and left side is closed
    if(distanceRight > 13 && distanceLeft < 13){
      checkRotate(1);
      Serial.println("Turn right");
      rotated();
      distances();
    }
    //checks if left side is open and right closed
    else if(distanceRight < 13 && distanceLeft > 13){
      checkRotate(0);
      Serial.println("Turn left");
      rotated();
      distances();
    }
    //if both sides are closed
    else if(distanceRight < 13 && distanceLeft < 13){
      // distances();
       Serial.println("Go back Dead end");
      // checkRotate(2);
      // distances();
      // distanceFront = s1.getDistance();
      checkRotate(0);
      //Serial.println("Turn left");
      rotated();
      distances();
    }
    //if both sides open
    else if(distanceRight > 13 && distanceLeft > 13){
      checkRotate(0);
      Serial.println("Turn left open both sides");
      rotated();
      distances();
    }
  }
}

//Will check for crossroads
unsigned long timeGapChecker;
void gapChecker(){
  distances();
    if(millis() > timeGapChecker){
        if(distanceRight > 20){
            //record that there is a opening
            Serial.println("Opening to the right");
            delay(200);
            motor.stop();
            rotation.turnDegreesRight(90);
            rotated();
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
unsigned long timeForUSS=0;   //time needed before sensors can be called again
unsigned long sensorSpacing=0;  //time between sensor readings
void distances(){
  static int count=0;   //sensor count 0=front, 1=right, 2=left
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
  
  
}

//will make sure robot travels strait, with the walls around it
void checkSide(){
  int _valid_tolorence = 20;    //max value the distance can be
  int _corectionAmount = 5;   //how much the bot must correct itself
  int _deviationTolorence = 4;  //allowed deviation between distance readings
  distances();
  //checks for valid reading
  if(distanceLeft < _valid_tolorence && distanceRight < _valid_tolorence){
    int deviation = distanceLeft - distanceRight;
    //checks if needs to adjust
    if(abs(deviation) > _deviationTolorence){
      int corection = _corectionAmount * abs(deviation);
      //close to right wall
      if(deviation > 0){
        //Serial.println("Right wall");
        motor.forwardFix((corection + 30), 0);
        rotation.checkRotation();
      }
      //close to left wall
      else if(deviation < 0){
        //Serial.println("Left wall");
        motor.forwardFix(0, (corection + 70));
        rotation.checkRotation();
      }   
    }
    
  }
  gapChecker();

}

boolean startRun = false;
void gameStart(){
  //when is recived bot can go
  if(startRun == false){
    rotation.moveForwardFor(20);
    //close gripper
    rotation.turnDegreesLeft(90);
    rotation.moveForwardFor(10);
    motor.stop();
    delay(1000);
    startRun = true;
    ready = true;
  }
  
}
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

//pathing
//Pathing path;


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
void rotated();

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
  //motor.forward(50);
  //motor.forward();
  //checkSide();
   //distances();
   //checkRotate(0);
  gapChecker();
  maze();
  hitWallFront();
  rotation.checkRotation();

  //checkSide();

  //test();
  // rotation.turnDegreesLeft(90);
  // delay(2000);
  
}

void maze(){
  //Open area in front
  while(distanceFront > 14){
    motor.forward(50);
    distances();
    checkSide();
    rotation.checkRotation();
    //path.addTurn(1);
    //path.visited();
    //Serial.println("Forward");
    //record where bot has gone
  }
  //Wall in front
  if(distanceFront < 13){
    motor.stop();
    // delay(400);
    //Serial.println("Stop");
    hitWallFront();
    rotation.checkRotation();
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
      rotation.moveBackwardFor(30);
      
    }
    else{
      rotation.turnDegreesRight(180);
      rotation.checkRotation();
    }
  
  default:
    break;
  }
}

void rotated(){
  rotation.moveForwardFor(20);
  distanceFront = s1.getDistance();
  rotation.checkRotation();
}

//When robot reaches a dead end it must decide what to do
void deadend(){
  rotation.checkRotation();
  distances();
  hitWallFront();
  if(distanceFront < 14){
    //check if right side is open and left side is closed
    if(distanceRight > 13 && distanceLeft < 13){
      //bool arr1[4] = {false, false, true, true};
      //path.addCrossroad(arr1);
      //rotation.turnDegreesRight(90);
      checkRotate(1);
      Serial.println("Turn right");
      rotated();
      //path.addTurn(2);
    }
    //checks if left side is open and right closed
    else if(distanceRight < 13 && distanceLeft > 13){
      //rotation.turnDegreesLeft(90);
      //bool arr2[4] = {true, false, false, true};
      //path.addCrossroad(arr2);
      
      checkRotate(0);
      Serial.println("Turn left");
      rotated();
      //path.addTurn(0);
    }
    //if both sides are closed
    else if(distanceRight < 13 && distanceLeft < 13){
      //path.addDeadEnd();
      distances();
      Serial.println("Go back Dead end");
      checkRotate(2);
      rotated();
      //path.addTurn(3);
    }
    //if both sides open
    else if(distanceRight > 13 && distanceLeft > 13){
      //bool arr1[4] = {true, false, true, true};
      //path.addCrossroad(arr1);
      checkRotate(0);
      //rotation.turnDegreesLeft(90);
      Serial.println("Turn left open both sides");
      rotated();
      //path.addTurn(0);
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
            //path.addTurn(2);
        }
        if(distanceLeft > 15){
            //record that there is a opening
            //Serial.println("Opening to the left");
            //path.addTurn(0);
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

//to decide what to do next when looking for next unexplored area
// void findNextOpeneing(){
//   int directions[] = {path.searchForNearestUnvisited(path._currentNode, path._currentDirection).directions};
//   //loops through an array of new directions to unexplored area
//   for(int x = 0; x < sizeof(directions); x++){
    
//   }
// }

void test(){
  rotation.turnDegreesLeft(90);
  delay(1000);
  
}

void checkSide(){
  int _valid_tolorence = 20;
  int _corectionAmount = 3;
  int _deviationTolorence = 2;
  distances();
  //checks for valid reading
  if(distanceLeft < _valid_tolorence && distanceRight < _valid_tolorence){
    int deviation = distanceLeft - distanceRight;
    // Serial.println("deviation");
    // Serial.println(deviation);
    //checks if needs to adjust
    if(abs(deviation) > _deviationTolorence){
      int corection = _corectionAmount * abs(deviation);
      // Serial.println("corection");
      // Serial.println(corection);
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

}
#include <Arduino.h>
#include <motors.h>
#include <sonar.h>
#include <rotation.h>
#include <line.h>
#include <gripper.h>

// Motors and rotation sensor

// left motor backwords  speed 0/255 PIN 11 A1
// left motor forward   direction 1/0 PIN 2 A2
// right motor forward   speed 0/255 PIN 9 B1
// right motor backwords  direction 1/0 12 B2

Rotation rotation(3, 2);
Motors motor(11, A1, 9, A0);
Gripper gripper1(8, 2, 1.5);

int linePins[] = {-1, -1, A2, A3, A4, A5, -1, -1};
Line line(8, linePins);
static lineSensorData lineState;

// Ultra sonic sensor1 FRONT
#define TRIGER1 7
#define ECHO1 10

// Ultra sonic sensor2 RIGHT
#define TRIGER2 5
#define ECHO2 6

// Ultra sonic sensor2 LEFT
#define TRIGER3 12
#define ECHO3 4

// Servo motor
#define SERVOPIN 8
const int OPENGRIPPER = 2;
const int CLOSEGRIPPER = 1.5;

// Ultra sonic sensor
Sonar s1(TRIGER1, ECHO1);
Sonar sRight(TRIGER2, ECHO2);
Sonar sLeft(TRIGER3, ECHO3);

// maze
int distanceFront = 0;
int distanceRight = 0;
int distanceLeft = 0;

// functions
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
void beginning();
void startMaze();
void moveWithLine();
uint8_t convertToBinary(lineSensorData line);
int detectChange();
bool tryFindLine();
void findEnd();

void setup()
{
  Serial.begin(9600);
  // Movement
  motor.setup(0, 0, 255, 255);
  rotation.setup(&motor, 0, 0);
  line.setup(700);
  lineState = line.readLine();
}

uint8_t lineForward[] = {
  0b11011011,
  0b11110011,
  0b11110011,
  0b11100011,
  0b11000011,
  0b11111011,
  0b11001111,
  0b11001111,
  0b11000111,
  0b11000011,
  0b11011111
};

boolean ready = false;
void loop()
{
  if (ready == true)
  {
    gripper1.gripper(gripper1._CloseGripper);
    distances();
    gapChecker();
    maze();
    hitWallFront();
    rotation.checkRotation();
    findEnd();
  }
  else
  {
    gripper1.gripper(gripper1._OpenGripper);
    startMaze();
  }
}

void maze()
{

  // Open area in front
  while (distanceFront > 14)
  {
    motor.forward(70);
    distances();
    gapChecker();
    checkSide();
    // rotation.checkRotation();

    // Serial.println("Forward");
    // record where bot has gone
  }
  // Wall in front
  if (distanceFront < 13)
  {
    motor.stop();
    hitWallFront();
    // rotation.checkRotation();
    distances();
    deadend();
  }
  distances();
}

// if robot is 7cm away from wall it must move back in order to rotate properly
void hitWallFront()
{
  distances();
  while (distanceFront < 7)
  {
    rotation.moveBackwardFor(2);
    distances();
  }
}

// checks if robot can turn, 0 = left, 1 = right, 2 = backwords
void checkRotate(int direction)
{
  distances();
  switch (direction)
  {
  case 0:
    if (distanceRight < 7)
    {
      Serial.println("Check rotate left");
      motor.backwardFix(80, 0);
      delay(600);
      motor.forwardFix(40, 0);
      delay(600);
    }
    rotation.turnDegreesLeft(90);
    break;

  case 1:
    if (distanceLeft < 7)
    {
      Serial.println("Check rotate right");
      motor.backwardFix(0, 40);
      delay(200);
      motor.forwardFix(0, 10);
      delay(200);
    }
    rotation.turnDegreesRight(90);
    break;

  case 2:
    if (distanceLeft < 7 || distanceRight < 7)
    {
      motor.backwardFix(0, 20);
      delay(200);
      motor.forwardFix(0, 40);
      delay(200);
    }
    else
    {
      for (int x = 0; x < 1; x++)
      {
        checkRotate(1);
      }
    }

    break;

  default:
    break;
  }
}

// to be run when robot has made a turn, in order to make sure it has moved before it decides what next to do
void rotated()
{
  rotation.moveForwardFor(20);
  distanceFront = s1.getDistance();
  hitWallFront();
  // rotation.checkRotation();
  distanceRight = sRight.getDistance();
  distances();
}

// When robot reaches a dead end it must decide what to do
void deadend()
{
  // rotation.checkRotation();
  distances();
  hitWallFront();
  if (distanceFront < 14)
  {
    // check if right side is open and left side is closed
    if (distanceRight > 13 && distanceLeft < 13)
    {
      checkRotate(1);
      Serial.println("Turn right");
      rotated();
      distances();
    }
    // checks if left side is open and right closed
    else if (distanceRight < 13 && distanceLeft > 13)
    {
      checkRotate(0);
      Serial.println("Turn left");
      rotated();
      distances();
    }
    // if both sides are closed
    else if (distanceRight < 13 && distanceLeft < 13)
    {
      // distances();
      Serial.println("Go back Dead end");
      // checkRotate(2);
      // distances();
      // distanceFront = s1.getDistance();
      checkRotate(1);
      // Serial.println("Turn left");
      rotated();
      distances();
    }
    // if both sides open
    else if (distanceRight > 13 && distanceLeft > 13)
    {
      checkRotate(1);
      Serial.println("Turn right open both sides");
      rotated();
      distances();
    }
  }
}

// Will check for crossroads
unsigned long timeGapChecker;
void gapChecker()
{
  distances();
  if (millis() > timeGapChecker)
  {
    hitWallFront();
    if (distanceRight > 20 && distanceRight < 60)
    {
      // record that there is a opening
      Serial.println("Opening to the right");
      // Serial.println(distanceRight);
      rotation.moveForwardFor(5);
      rotation.turnDegreesRight(90);
      rotated();
    }
    timeGapChecker += 250;
  }
}

// Constanly get updated values of sonar distance
int oldFront = 0;
int oldLeft = 0;
int oldRight = 0;
unsigned long timeForUSS = 0;    // time needed before sensors can be called again
unsigned long sensorSpacing = 0; // time between sensor readings
void distances()
{
  static int count = 0; // sensor count 0=front, 1=right, 2=left
  if (millis() > timeForUSS)
  {
    if (millis() > sensorSpacing)
    {
      switch (count)
      {
      case 0:
        distanceFront = s1.getDistance();
        if (distanceFront == 0)
        {
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
        if (distanceRight == 0)
        {
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
        if (distanceLeft == 0)
        {
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

void test()
{
}

// will make sure robot travels strait, with the walls around it
void checkSide()
{
  int _valid_tolorence = 20;   // max value the distance can be
  int _corectionAmount = 5;    // how much the bot must correct itself
  int _deviationTolorence = 4; // allowed deviation between distance readings
  distances();
  // checks for valid reading
  if (distanceLeft < _valid_tolorence && distanceRight < _valid_tolorence)
  {
    int deviation = distanceLeft - distanceRight;
    // checks if needs to adjust
    if (abs(deviation) > _deviationTolorence)
    {
      int corection = _corectionAmount * abs(deviation);
      // close to right wall
      if (deviation > 0)
      {
        // Serial.println("Right wall");
        motor.forwardFix((corection + 30), 0);
        // rotation.checkRotation();
      }
      // close to left wall
      else if (deviation < 0)
      {
        // Serial.println("Left wall");
        motor.forwardFix(0, (corection + 30));
        // rotation.checkRotation();
      }
    }
  }
}

void startMaze()
{
  gripper1.gripper(gripper1._OpenGripper);
  rotation.moveForwardFor(8);
  motor.stop();
  lineState = line.readLine();
  while (convertToBinary(line.readLine()) != 0b11111111)
  {
    lineState = line.readLine();
    for (size_t i = 0; i < 8; i++)
    {
      Serial.print(" ");
      Serial.print(lineState.linePoints[i].isLine);
    }
    Serial.println();
    motor.forward();
  }
  motor.stop();
  gripper1.gripper(gripper1._CloseGripper);
  lineState = line.readLine();
  uint8_t decimal = convertToBinary(lineState);
  rotation.moveForwardFor(7);
  rotation.turnDegreesLeft(90);
  while (convertToBinary(line.readLine()) != 0b11011011)
  {
    motor.forward();
  }
  while (convertToBinary(line.readLine()) != 0b11000011)
  {
    moveWithLine();
  }
  motor.stop();
  ready = true;
}

void moveWithLine()
{
  Serial.println("Move with line");
  lineState = line.readLine();
  uint8_t decimal = convertToBinary(lineState);
  Serial.println(decimal);
  switch (decimal)
  {
    default:
      motor.forward();  
      break;
    case 0b11011011:
      motor.forward();
      break;
    case 0b11110011:
    case 0b11100011:
    case 0b11111011:
      //left
      motor.zeroLeft();
      break;
    case 0b11001111:
    case 0b11000111:
    case 0b11011111:
      motor.zeroRight();
      break;
    case 0b11111111:
      motor.stop();
      break;
  }
}

void findEnd()
{
  lineState = line.readLine();
  while (convertToBinary(line.readLine()) != 0b11111111)
  {
    lineState = line.readLine();
    if(detectChange() == 1){
      if(tryFindLine() == true){
        moveWithLine();
      }
    }
  }
  motor.stop();
  gripper1.gripper(gripper1._OpenGripper);
  
}

int detectChange()
{
  // line
  lineSensorData newLineState = line.readLine();
  for (int i = 0; i < 8; i++)
  {
    if (newLineState.linePoints[i].isLine != lineState.linePoints[i].isLine)
    {
      lineState = newLineState;
      Serial.println("Change detected");
      return 1;
    }
  }
}

int Find(uint8_t arr[], uint8_t x)
{
  int n = sizeof(arr) / sizeof(arr[0]);
  for (int i = 0; i < n; i++)
    if (arr[i] == x)
      return i;
  return -1;
}

boolean tryFindLine() {
  int timer = millis();
  Serial.println("tryFindLine");
  while (millis() - timer < 5000)
  {
    motor.zeroLeft();
    if (Find(lineForward,convertToBinary(line.readLine())) != -1)
    {
      return true;
    }
  }
  timer = millis();
  while (millis() - timer < 10000)
  {
    motor.zeroRight();
    if (Find(lineForward,convertToBinary(line.readLine())) != -1)
    {
      return true;
    }
  }
  return false;
}

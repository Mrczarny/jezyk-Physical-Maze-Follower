#include <Arduino.h>
#include <motors.h>
#include <sonar.h>
#include <rotation.h>
#include <line.h>
#include <gripper.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

Rotation rotation(3, 2);
Motors motor(11, A1, 9, A0);
Gripper gripper1(8, 2, 1.9);

//Line sensor
int linePins[] = {-1, -1, A2, A3, A4, A5, -1, -1};
Line line(8, linePins);
static lineSensorData lineState;
uint8_t lineForward[] = {
  0b00011000,
  0b00110000,
  0b00110000,
  0b00100000,
  0b00000000,
  0b00111000,
  0b00001100,
  0b00001100,
  0b00000100,
  0b00000000,
  0b00011100
};

// Define RX and TX pins for HC-12
const int HC12_RX = 0;
const int HC12_TX = 1;
SoftwareSerial HC12(HC12_RX, HC12_TX);
//other stuff needed
boolean isOnLine = false;
bool taskActive = false;  //if robot can run
bool finished = false;    //if robot dinished its task
boolean ready = false;    //if robot finsihsed the start process

// Ultra sonic sensor1 FRONT
#define TRIGER1 7
#define ECHO1 10

// Ultra sonic sensor2 RIGHT
#define TRIGER2 5
#define ECHO2 6

// Ultra sonic sensor3 LEFT
#define TRIGER3 12
#define ECHO3 4

// Servo motor
#define SERVOPIN 8

// Ultra sonic sensor
Sonar s1(TRIGER1, ECHO1);
Sonar sRight(TRIGER2, ECHO2);
Sonar sLeft(TRIGER3, ECHO3);

// distances for ultra sonic sensors
int distanceFront = 0;
int distanceRight = 0;
int distanceLeft = 0;

//Neopixel
int LEDPIN = 13;
int NUMPIXELS = 4;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);

void normal_Pixel();
void braking_Pixel();
void back_Pixel();
void right_Pixel();
void left_Pixel();

// functions
void distances();
void gapChecker();
void maze();
void checkSide();
void deadend();
void hitWallFront();
void checkRotate(int direction);
void startMaze();
void moveWithLine();
uint8_t convertToBinary(lineSensorData line);
int detectChange();
bool tryFindLine();
void findEnd();
void communication2();
void splitString(String input, char delimiter, String output[], int &count);

void setup()
{
  Serial.begin(9600);
  // Movement
  motor.setup(0, 0, 255, 255);
  rotation.setup(&motor, 0, 0);
  //line sensor
  line.setup(700);
  lineState = line.readLine();
  //comunication
  HC12.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop()
{
  communication2();
  if(taskActive == true){ //comand to start the robot
    if (ready == true)  //finish start process
    {
      gripper1.gripper(gripper1._CloseGripper);
      distances();
      maze();
      hitWallFront();
      gapChecker();
      rotation.checkRotation();
      findEnd();
    }
    else
    {
      gripper1.gripper(gripper1._OpenGripper);
      startMaze();
    }
  }else{
    motor.stop();
  }
}

void maze()
{
  communication2();
  // Open area in front
  while (distanceFront > 14)
  {
    distances();
    communication2();
    motor.forward(70);
    distances();
    gapChecker();
    checkSide();
    normal_Pixel();
  }
  // Wall in front
  if (distanceFront < 13)
  {
    communication2();
    braking_Pixel();
    motor.stop();
    hitWallFront();
    distances();
    deadend();
  }
  distances();
  communication2();
}

// if robot is 7cm away from wall it must move back in order to rotate properly
void hitWallFront()
{
  communication2();
  distances();
  while (distanceFront < 7)
  {
    //Serial.println("Hit wall front");
    communication2();
    back_Pixel();
    rotation.moveBackwardFor(2);
    distances();
    rotation.checkRotation();
  }
}

// checks if robot can turn, 0 = left, 1 = right, 2 = backwords
void checkRotate(int direction)
{
  communication2();
  distances();
  switch (direction)
  {
  case 0:
    if (distanceRight < 7)
    {
      Serial.println("Check rotate left");
      back_Pixel();
      motor.backwardFix(80, 0);
      delay(600);
      normal_Pixel();
      motor.forwardFix(40, 0);
      delay(600);
    }
    left_Pixel();
    rotation.turnDegreesLeft(90);
    rotation.checkRotation();
    break;

  case 1:
    if (distanceLeft < 7)
    {
      Serial.println("Check rotate right");
      back_Pixel();
      motor.backwardFix(0, 40);
      delay(200);
      normal_Pixel();
      motor.forwardFix(0, 10);
      delay(200);
    }
    right_Pixel();
    rotation.turnDegreesRight(90);
    rotation.checkRotation();
    break;

  case 2:
    if (distanceLeft < 7 || distanceRight < 7)
    {
      back_Pixel();
      motor.backwardFix(0, 20);
      delay(200);
      normal_Pixel();
      motor.forwardFix(0, 40);
      delay(200);
    }
    else
    {
      //turns right twice
      for (int x = 0; x < 1; x++)
      {
        checkRotate(1);
      }
    }
    rotation.turnDegreesRight(180);
    rotation.checkRotation();
    break;

  default:
    break;
  }
}

// to be run when robot has made a turn, in order to make sure it has moved before it decides what next to do
void rotated()
{
  communication2();
  normal_Pixel();
  rotation.moveForwardFor(20);
  rotation.checkRotation();
  distanceFront = s1.getDistance();
  hitWallFront();
  distanceRight = sRight.getDistance();
  distances();
}

// When robot reaches a dead end it must decide what to do
void deadend()
{
  communication2();
  distances();
  hitWallFront();
  if (distanceFront < 14)
  {
    // check if right side is open and left side is closed
    if (distanceRight > 13 && distanceLeft < 13)
    {
      checkRotate(1);
      //Serial.println("Turn right");
      rotated();
      distances();
    }
    // checks if left side is open and right closed
    else if (distanceRight < 13 && distanceLeft > 13)
    {
      checkRotate(0);
      //Serial.println("Turn left");
      rotated();
      distances();
    }
    // if both sides are closed
    else if (distanceRight < 13 && distanceLeft < 13)
    {
      // distances();
      //Serial.println("Go back Dead end");
      checkRotate(1);
      rotated();
      distances();
    }
    // if both sides open
    else if (distanceRight > 13 && distanceLeft > 13)
    {
      checkRotate(1);
      //Serial.println("Turn right open both sides");
      rotated();
      distances();
    }else{
      rotation.moveForwardFor(2);
      distances();
      rotation.checkRotation();
    }
  }
}

// Will check for crossroads
unsigned long timeGapChecker;
void gapChecker()
{
  communication2();
  distances();
  if (millis() > timeGapChecker)
  {
    hitWallFront();
    if (distanceRight > 20 && distanceRight < 90)
    {
      //Serial.println("Opening to the right");
      normal_Pixel();
      rotation.moveForwardFor(5);
      right_Pixel();
      rotation.turnDegreesRight(90);
      rotated();
      rotation.checkRotation();
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

// will make sure robot travels strait, with the walls around it
void checkSide()
{
  communication2();
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
        left_Pixel();
        motor.forwardFix((corection + 30), 0);
      }
      // close to left wall
      else if (deviation < 0)
      {
        // Serial.println("Left wall");
        right_Pixel();
        motor.forwardFix(0, (corection + 30));
        // rotation.checkRotation();
      }
    }
  }
}

//start process to pick up cone and enter maze
void startMaze()
{
  gripper1.gripper(gripper1._OpenGripper);
  normal_Pixel();
  rotation.moveForwardFor(12);
  braking_Pixel();
  motor.stop();
  lineState = line.readLine();
  while (convertToBinary(line.readLine()) != 0b00111100)
  {
    communication2();
    lineState = line.readLine();
    for (size_t i = 0; i < 8; i++)
    {
      //Serial.print(" ");
      //Serial.print(lineState.linePoints[i].isLine);
    }
    //Serial.println();
    //motor.forward();
  }
  braking_Pixel();
  motor.stop();
  gripper1.gripper(gripper1._CloseGripper);
  lineState = line.readLine();
  normal_Pixel();
  rotation.moveForwardFor(7);
  left_Pixel();
  rotation.turnDegreesLeft(90);
  //not working correctly
  // while (convertToBinary(line.readLine()) != 0b00011000)
  // {
  //   communication2();
  //   normal_Pixel();
  //   motor.forward();
  // }
  // while (convertToBinary(line.readLine()) != 0b00000000)
  // {
  //   communication2();
  //   moveWithLine();
  // }
  // if(convertToBinary(line.readLine()) == 0b00000000){
  //   braking_Pixel();
  //   motor.stop();
  //   ready = true;
  //   Serial.println("finished start");
  // }
  // if(millis() - startTime > 10000){
  //   ready = true;
  // }
  rotation.moveForwardFor(30);
  rotation.checkRotation();
  ready = true;
}

//follows a line
void moveWithLine()
{
  communication2();
  //Serial.println("Move with line");
  lineState = line.readLine();
  uint8_t decimal = convertToBinary(lineState);
  switch (decimal)
  {
    default:
      normal_Pixel();
      motor.forward();  
      break;
    case 0b00011000:
      isOnLine = true; //communication
      normal_Pixel();
      motor.forward();
      break;
    case 0b00110000:
    case 0b00100000:
    case 0b00111000:
      isOnLine = true; //communication
      //left
      left_Pixel();
      motor.zeroLeft();
      break;
    case 0b00001100:
    case 0b00000100:
    case 0b00011100:
      isOnLine = true; //communication
      right_Pixel();
      motor.zeroRight();
      break;
    case 0b00111100:
      isOnLine = true; //communication
      braking_Pixel();
      motor.stop();
      break;
  }
}

//code to find the end of the maze with a line
void findEnd()
{
  communication2();
  lineState = line.readLine();
  while (convertToBinary(line.readLine()) != 0b00111100)
  {
    communication2();
    lineState = line.readLine();
    if(detectChange() == 1){
      if(tryFindLine() == true){
        isOnLine = true; //communication
        moveWithLine();
      }
    }
  }
  braking_Pixel();   
  motor.stop();
  gripper1.gripper(gripper1._OpenGripper);
  back_Pixel();
  rotation.moveBackwardFor(10);
  finished = true;
}

//detects changes, so will see if there is a line
int detectChange()
{
  communication2();
  // line
  lineSensorData newLineState = line.readLine();
  for (int i = 0; i < 8; i++)
  {
    if (newLineState.linePoints[i].isLine != lineState.linePoints[i].isLine)
    {
      lineState = newLineState;
      Serial.println("Change detected");
      for (size_t i = 0; i < 8; i++)
      {
        //Serial.print(" ");
        //Serial.print(lineState.linePoints[i].isLine);
      }
      //Serial.println(convertToBinary(lineState));
      return 1;
    }
  }
  return 0;
}

int Find(uint8_t arr[], uint8_t x)
{
  int n = sizeof(arr) / sizeof(arr[0]);
  for (int i = 0; i < n; i++)
    if (arr[i] == x)
      return i;
  return -1;
}

//if line is lost it will try to find it
boolean tryFindLine() {
  communication2();
  isOnLine = false; //communication
  //Serial.println("Try find line");
  long timer = millis();
  while (millis() - timer < 200)
  {
    left_Pixel();
    motor.zeroLeft();
    if (Find(lineForward,convertToBinary(line.readLine())) != -1)
    {
      isOnLine = true; //communication
      return true;
    }
  }
  timer = millis();
  while (millis() - timer < 400)
  {
    communication2();
    motor.zeroRight();
    if (Find(lineForward,convertToBinary(line.readLine())) != -1)
    {
      isOnLine = true; //communication
      return true;
    }
  }
  return false;
}

//Comminication
bool done = false;
unsigned long taskStartTime = 0;
bool taskRunning = false;

void communication2(){
  //waits till it gets a message
  if (HC12.available() > 0) {
    String receivedMessage = HC12.readStringUntil('\n');
    receivedMessage.trim(); //removes whitespace
    //Serial.println("Received: " + receivedMessage);

    // Parse the message
    String fields[3];
    int fieldCount = 0;
    splitString(receivedMessage, '|', fields, fieldCount);

    if (fieldCount > 1) {
      String senderID = fields[0];
      String command = fields[1];
      if(senderID = 3){
        //robot starts
        if (command == "START") {
          taskActive = true;
          taskStartTime = millis();
          //Serial.println("Slave: Task started.");
        } else if (command == "REQUEST" && taskActive) {
          //data to send, front distance, left wheel speed, right wheel speed, line sensor data
          int sonar = distanceFront;
          int sonar2 = distanceLeft;
          int sonar3 = distanceRight;
          float speed = rotation.calculateSpeed();
          // HC12.println does not work, serial print works instead
          if(isOnLine == true){
            Serial.println("3|DATA| speed: " + String(speed)+"| linefollower: True | distance1: "+String(sonar) +
             "| distance2: "+String(sonar2) + "| distance3: "+String(sonar3));
          }else{
            Serial.println("3|DATA| speed: " + String(speed)+"| linefollower: False | distance1: "+String(sonar) +
             "| distance2: "+String(sonar2) + "| distance3: "+String(sonar3));
          }
          //Serial.println("Slave: Sent data update.");
        }else if(finished == true){
          Serial.println("3|DONE\n");
        }
      }
    }
  }
}

// Utility function to split a string
void splitString(String input, char delimiter, String output[], int &count) {
  count = 0;
  int start = 0;
  int end = input.indexOf(delimiter);

  while (end != -1) {
    output[count++] = input.substring(start, end);
    start = end + 1;
    end = input.indexOf(delimiter, start);
  }
  output[count++] = input.substring(start);
}

//code for neopixels
void normal_Pixel()
{
  //pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(75,0,0)); // red at 60
  pixels.setPixelColor(1, pixels.Color(75,0,0)); // red at 60
  pixels.setPixelColor(2, pixels.Color(255, 255, 75)); // white ish headlights 100
  pixels.setPixelColor(3, pixels.Color(255, 255, 75)); // white ish headlights 100
  pixels.show();
}

void braking_Pixel()
{
  //pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(255,0,0)); // red at 100
  pixels.setPixelColor(1, pixels.Color(255,0,0)); // red at 100
  pixels.setPixelColor(2, pixels.Color(255, 255, 75)); // white ish headlights 100
  pixels.setPixelColor(3, pixels.Color(255, 255, 75)); // white ish headlights 100
  pixels.show();
}

void back_Pixel()
{
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(255,0,0)); // red at 100
  pixels.setPixelColor(1, pixels.Color(255,255,255)); // red at 100
  pixels.setPixelColor(2, pixels.Color(255, 255, 75)); // white ish headlights 100
  pixels.setPixelColor(3, pixels.Color(255, 255, 75)); // white ish headlights 100
  pixels.show();
}

  void right_Pixel()
  {
    static bool lightOn = false;
    static unsigned long timer = millis();

    if ((millis() - timer) > 500)
    {
      if(lightOn == true)
      {
        pixels.clear();
        pixels.setPixelColor(0, pixels.Color(75, 0, 0));      // Red at 60
        pixels.setPixelColor(1, pixels.Color(255, 100, 0));   // Orange
        pixels.setPixelColor(2, pixels.Color(255, 100, 0));   // Orange
        pixels.setPixelColor(3, pixels.Color(255, 255, 75));  // White-ish headlights
        pixels.show();
      }
      else
        /*
        0 = leftback
        1 = rightback
        2 = leftfront
        3 = rightfront
        */
      {
        pixels.clear();
        pixels.setPixelColor(0, pixels.Color(75, 0, 0));      // Red at 60
        pixels.setPixelColor(1, pixels.Color(0,0,0));
        pixels.setPixelColor(2, pixels.Color(0,0,0));
        pixels.setPixelColor(3, pixels.Color(255, 255, 75));  // White-ish headlights
        pixels.show(); // Turn off the orange pixels
      }
      lightOn = !lightOn;
      timer = millis();
    }
  }

void left_Pixel() 
{
  static bool lightOn = false;
    static unsigned long timer = millis();

    if ((millis() - timer) > 500)
    {
      if(lightOn == true)
      {
        pixels.clear();
        pixels.setPixelColor(0, pixels.Color(255, 100, 0));   // Orange
        pixels.setPixelColor(1, pixels.Color(75, 0, 0));      // Red
        pixels.setPixelColor(2, pixels.Color(255, 255, 75));  // White
        pixels.setPixelColor(3, pixels.Color(255, 100, 0));   // Orange
        pixels.show();
        }
      else
        /*
        0 = leftback
        1 = rightback
        2 = leftfront
        3 = rightfront
        */
      {
        pixels.clear();
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));      // Red at 60
        pixels.setPixelColor(1, pixels.Color(75, 0, 0));
        pixels.setPixelColor(2, pixels.Color(255, 255, 75));
        pixels.setPixelColor(3, pixels.Color(0, 0, 0));  // White-ish headlights
        pixels.show(); // Turn off the orange pixels
      }
      lightOn = !lightOn;
      timer = millis();
    }
  }

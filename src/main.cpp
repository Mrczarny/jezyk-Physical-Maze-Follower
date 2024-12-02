#include <Arduino.h>
#include <motors.h>
#include <sonar.h>
#include <rotation.h>


//left motor backwords  speed 0/255 PIN 11 A1
//left motor forward   direction 1/0 PIN 2 A2
//right motor forward   speed 0/255 PIN 9 B1
//right motor backwords  direction 1/0 12 B2

// //Ultra sonic sensor1
#define TRIGER1 7
#define ECHO1 10

// //Ultra sonic sensor2
#define TRIGER2 5
#define ECHO2 6

//Servo motor
#define SERVOPIN 8
const int OPENGRIPPER = 2;
const int CLOSEGRIPPER = 1.5;

Rotation rotation(4, 3);
Motors motor(11,2,9,12);

// //Servo motor
Sonar s1(TRIGER1, ECHO1);
Sonar s2(TRIGER2, ECHO2);

// maze 
int distanceFront = 0;
int distanceRight = 0;

unsigned int time2;

//functions
void distances();
void gapChecker();
void maze();


void setup() {
  Serial.begin(9600);
  //Movement
  motor.setup(0, 0, 255, 255);
  rotation.setup(&motor, 0, 0);
}


void loop() {
  maze();
}

// unsigned long time2 = 0;
// void servo(int position){
//   if (millis() > time2){ 
//     digitalWrite(SERVOPIN, HIGH); 
//     delay(position);
//     digitalWrite(SERVOPIN, LOW);
//     delay(20);
//   }
// }

void maze(){
  while(distanceFront > 11){
    motor.forward();
    distances();
    //record where bot has gone
    Serial.println("Forward");
    Serial.println(distanceFront);
  }
  if(distanceFront < 10){
    Serial.println("Obstacle in way");
    Serial.println(distanceFront);
    //check if right side is open
    if(distanceRight > 5){
      //record robot can go right
      Serial.println("Gap to right");
    }
    //turn robot left
    //rotation.turnDegreesLeft(90);
    //motor.stop();
    //check if left side is open
    if(distanceFront > 5){
      //record gap to left
      Serial.println("Gap to left"); 
    }
  }
  distances();
    
}



unsigned int timeGapChecker;
void gapChecker(){
    if(millis() > timeGapChecker){
      // Serial.print("distance");
      // Serial.println(distanceRight);
        if(distanceRight > 9){
            //record that there is a opening
            Serial.println("Opening to the right");
        }
        timeGapChecker +=250;
    }
}

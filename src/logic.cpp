// #include <Arduino.h>
// #include <motors.h>
// #include <sonar.h>
// #include <rotation.h>

// //Ultra sonic sensor1
// #define TRIGER1 7
// #define ECHO1 4

// //Ultra sonic sensor2
// #define TRIGER2 5
// #define ECHO2 3

// //Servo motor
// Sonar s1(TRIGER1, ECHO1);
// Sonar s2(TRIGER2, ECHO2);

// Rotation rotation(12, 1);
// Motors motor(11,13,9,12);


// int distanceFront;
// int distanceRight;

// unsigned int time2;

// //Constanly get updated values of sonar distance
// void distances(){
//     if(millis() > time2){
//         distanceFront = s1.getDistance();
//         distanceRight = s2.getDistance();
//         time2 += 250;
//     }
// }

// void maze(){
//     while(distanceFront > 11){
//         motor.forward();
//         //record where bot has gone
//         Serial.println("Forward");
//     }
//     if(distanceFront < 10){
//         Serial.println("Obstacle in way");
//         //check if right side is open
//         if(distanceRight > 5){
//             //record robot can go right
//             Serial.println("Gap to right");
//         }
//         //turn robot left
//         rotation.turnDegreesLeft(90);
//         //check if left side is open
//         if(distanceFront > 5){
//             //record gap to left
//             Serial.println("Gap to left");
//             maze();
//         }
//     }
    
// }



// unsigned int timeGapChecker;
// void gapChecker(){
//     if(millis() > timeGapChecker){
//         if(distanceRight > 5){
//             //record that there is a opening
//             Serial.println("Opening to the right");
//         }
//         timeGapChecker +=250;
//     }
// }
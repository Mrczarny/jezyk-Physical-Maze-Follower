#include <Arduino.h>
#include "functions.h"

//Ultra sonic sensor
#define TRIGER 3
#define ECHO 2


//gets the distance from the ultra sonic sensor
int getDistance(){
    int time = 0;
    int distance = 0;
    digitalWrite(TRIGER, LOW); 
    delayMicroseconds(2);
    digitalWrite(TRIGER, HIGH); //sends power to the sensor
    delayMicroseconds(10);      
    digitalWrite(TRIGER, LOW);  //sets back to low

    time = pulseIn(ECHO, HIGH); //will return how long it took the sensor to recive a wave
    //if 0 then error occurd
    if(time==0){
        Serial.println("Error getting distance");
    }else{
        distance = (time * 0.0343) / 2; //distance = time*speed of sound per milisecond / 2 (travels to object and back) 
        Serial.println("Distance");
        Serial.println(distance); 
    }
    return distance;
}


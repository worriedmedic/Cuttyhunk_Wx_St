/*
 *  Original code by Bob Hartmann and Dave Vondle for http://labs.ideo.com March 2010
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
*/

#include <Wire.h>
#include "LEDController.h"

// Constructors:

// LEDController(int i2cAddress, int ledAddress ,[optional arguments...]) for Linear Tech I2C LED driver
// or 
// LEDController(int ioPin ,[optional arguments...]) for direct PWM on the Arduino

//optional arguments are:
//  boolean updateIn, int actionIn, int analogHighValueIn, int analogMidValueIn, 
//  int analogLowValueIn, int analogValueIn, int rampUpValueIn, int rampDownValueIn, int rampUpDelayIn, 
//  int rampDownDelayIn, int chaseDelayIn, int chaseNumberOnIn, 
//  boolean blinkStateIn, int blinkTimeHighIn, int blinkTimeLowIn, unsigned long previousTimeIn,
//  int dwellLowTime, int dwellHighTime);


boolean buttonWasUp=true;

LEDController ledREDController(6);
LEDController ledGREENController(5);
LEDController ledBLUEController(3); 

//State machine states
enum{
      STATE_0 = 0,
      STATE_1 = 1
};

int testState=STATE_0;
unsigned long stateStart; // The time that a segment of leds begins its action
unsigned long stateEnd;   // The time that a segment of leds finishes its action

void setup(){
  pinMode(3, OUTPUT);     
  pinMode(5, OUTPUT);    
  pinMode(6, OUTPUT);  
  stateEnd = 0;  
  
 ledREDController.action = BLINK;  
 ledGREENController.action = RAMP_CONT_HIGH_LOW; 
 ledBLUEController.action = RAMP_CONT_HIGH_MID;
}

void loop(){

 // Application code goes here
 switch(testState){
        
    case STATE_0: 
    
        if(millis() > stateEnd){
            stateStart = millis();
            stateEnd = stateStart + 1500;
            testState = STATE_1;
            ledBLUEController.action = RAMP_HIGH;
            ledBLUEController.rampUpValue=4; 
        }
    
      break;

    case STATE_1:
       
        if(millis() > stateEnd){
            stateStart = millis();
            stateEnd = stateStart + 1500;
            testState = STATE_0;
            ledBLUEController.action = RAMP_LOW;
            ledBLUEController.rampDownValue=4;
        }
        
     break;
 }
  
 ledREDController.takeAction();
 ledGREENController.takeAction();
 ledBLUEController.takeAction();
} // end of loop



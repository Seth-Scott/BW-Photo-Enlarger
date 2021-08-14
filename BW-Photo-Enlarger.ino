//LCD libraries
#include <Wire.h>
#include "DFRobot_LCD.h"
DFRobot_LCD lcd(16,2);

//LED libraries and pins
#include <FastLED.h>
#define LED_PIN1 9
#define NUM_LEDS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
      
      int r, g, b;

const int startButton=5;
      int startButtonState;

const int tU=3, tD=4;
      int tUState, tDState;

unsigned long countdownBegin, countdown, timer;

const int cU=6, cD=7; 
      int cUState, cDState, cVal=0;
      
const int safeLight=2;
      int safeLightState;
      bool SL=false;


void setup() {
    //LCD screen
    lcd.init();
    lcd.setRGB(255, 0, 0);
    lcd.clear();

    //LED
    FastLED.addLeds<WS2812B, LED_PIN1, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(255);

    //assigns digital pins as either inputs or outputs 
    pinMode(startButton,INPUT);
    pinMode(tU,INPUT);
    pinMode(tD,INPUT);
    pinMode(cU,INPUT);
    pinMode(cD,INPUT);
    pinMode(safeLight,INPUT);

    //allows the user to use a button without an external resistor
    digitalWrite(startButton,HIGH);
    digitalWrite(tU,HIGH);
    digitalWrite(tD,HIGH);
    digitalWrite(cU,HIGH);
    digitalWrite(cD,HIGH);
    digitalWrite(safeLight,HIGH);

    //debugs an issue where lights don't consistently work on startup
    for(int j = 0; j<=6; j=j+1){
      for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB(255, 0, 0);
      }
      FastLED.show();
      delay(250);
    }

    for(int j = 0; j<=5; j=j+1){
      for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);
      }
      FastLED.show();
    }
}

void loop() {
safeLightState=digitalRead(safeLight);

  //turns on the safe light when the switch is toggled
  if(safeLightState==0){
    for(int i=0; i<NUM_LEDS; i++) {
      leds[i] = CRGB(255, 0, 0);
    }
    FastLED.show();


  //Let's the user know via LCD that safe mode is activated  
  if(SL == false){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("*SAFE MODE*");
    countdownBegin = 0UL;
  }

    
    //SL tracks the status of the safe light so it can be disabled when the switch is turned off
    SL = true;    
  }

  //tracks the state of the safe light and turns the enlarger head off if it was previously on
  if(safeLightState==1){      
    
    //disables the safe light when the switch is turned off
    if(SL == true){
      for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);
      }
      FastLED.show();
      SL = false;
      }
    
    //sets the LED RGB values based on contrast setting
    switch(cVal){
      
      case -1:
      r=255, g=0, b=0;
      break;
      
      case 0:
        r=0, g=252, b=0;
      break;
      
      case 1:  
        r=0, g=224, b=9;
      break; 
        
      case 2:
        r=0, g=196, b=19;
      break;
  
      case 3:
        r=0, g=168, b=28;
      break;
  
      case 4:
        r=0, g=140, b=37;
      break;
        
      case 5:
        r=0, g=112, b=46;
      break;
        
      case 6:
        r=0, g=84, b=56;
      break;
        
      case 7:
        r=0, g=56, b=65;
      break;
        
      case 8:
        r=0, g=28, b=75;
      break;
      
      case 9:
        r=0, g=0, b=84;
      break;
  
      case 10:
      r=255, g=255, b=255;
      break;
  
      default:
        r=0, g=0, b=0;
      break;
    }    
      //checks the state of the button (if you press the button it will read 0, unpressed will read 1)
      startButtonState=digitalRead(startButton);
    
      //if the button is pressed, peg the countdownBegin int to millis() at the moment it was pressed, turns on the light 
      if(startButtonState==0){
        countdownBegin=millis();      
      
      //turns on the LED panel
        for(int i=0; i<NUM_LEDS; i++) {
          leds[i] = CRGB(r, g, b);
        }
        FastLED.show();
      }
    
    //timer section  
      //adds/subtracts time to the countdown;
      tUState=digitalRead(tU);
      tDState=digitalRead(tD);
      //time-up button
      if(tUState==0){
        //add half a second
        countdown=countdown+500UL;
        //caps the time at sixty seconds
        if(countdown>=60000UL){
          countdown=60000UL;
        }
      }
      //time-down button
      if(tDState==0){
        //remove half a second
        countdown=countdown-500UL;
        //caps the time at zero
        if(countdown<=0UL){
          countdown=0UL;
        }
        //prevents time from rolling over to max value from zero
        if(countdown>=60000UL){
          countdown=0UL;
        }
      }
      
      //establishes the timer, which is the timed data the user will see
      if(countdownBegin>0){
        timer = (countdownBegin + countdown) - (millis() );
        if(timer>=countdownBegin+countdown){
          timer=countdown;
          countdownBegin=0;
          lcd.clear();
          
          //turns off the LED panel
          for(int i=0; i<NUM_LEDS; i++) {
          leds[i] = CRGB(0, 0, 0);
          }
          FastLED.show();
        }
      }
      
      else{
        timer=countdown;
      }
  
  
    //contrast section
      //adds/subtracts the contrast
      cUState=digitalRead(cU);
      cDState=digitalRead(cD);        
  
      //contrast up button
      if(cUState == 0){
        cVal = cVal+1;
      }
      if(cVal >= 10){
        cVal = 10;
      }
    //contrast down button
      if(cDState == 0){
        cVal = cVal-1;
      }
      if(cVal <= (-1) ){
        cVal= (-1);
      }
        
      //lcd section, TIMER
      lcd.setCursor(0,0);
      lcd.print("Time:  ");   
      //preceeds the LCD with a zero to fix trailing numbers 
      if(timer<10000UL || countdown<10000UL){
        lcd.print("0");
      }      
      lcd.print(timer*.001);
  
      //lcd section, CONTRAST
      if(cVal >= (0) && cVal <=9){    
        lcd.setCursor(0,1);
        lcd.print("Contrast: ");
        lcd.print(" ");
        lcd.print(cVal);
        lcd.print("   ");     
      }
      if(cVal == (-1) ){
        lcd.setCursor(0,1);
        lcd.print("Safe Light      ");
      }   
      if(cVal == (10) ){
        lcd.setCursor(0,1);
        lcd.print("Full-Spectrum   ");
      }
  }
  }

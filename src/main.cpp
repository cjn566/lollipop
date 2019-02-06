 #define DEBUG
 // #define TIMING


#include <Arduino.h>
#include <Encoder.h>
#include <TimerOne.h>
#include <TimerThree.h>
#include "Animation.h"
#include "util.h"

//          SETTINGS
// Pins
#define ENCODER_A     23
#define ENCODER_B     22
#define ENCODER_BTN   4
#define LED_DATA      17

// UI Timing
#define DEBOUNCE_MILLIS       50
#define EDIT_HOLD_MILLIS      700
#define EDIT_TIMEOUT_MILLIS   4000
#define BLINK_STEPS           FREQ_TO_STEPS(3)

// Parameters
#define NUM_GLOBAL_PARAMS 2
#define INIT_BRIGHTNESS   10
#define INIT_SPEED        128
#define MAX_BRIGHTNESS    255
#define MAX_SPEED         30
#define BRIGH_ADJ_MULT    3
#define SPEED_ADJ_MULT    3
#define SPEED_REDUCTION_FACTOR    2
#define BRIGHT_MACRO_ADJ_THRESH    50


// Video
#define FPS                 60
#define ANIM_STEPPER_FREQ   100

// Misc
#define MAX_MILLIAMPS          1500
#define STARTUP_DELAY          1000
#define ENC_TICKS_PER_INDENT   4

// Macros
#define ANIM animations[$.anim]
#define FREQ_TO_STEPS(f) (ANIM_STEPPER_FREQ / f)

//          VARIABLES AND STUFF
// Hardware
Encoder myEnc(ENCODER_A, ENCODER_B);
//CRGB leds[NUM_LEDS];

// Settings
uint8_t brightness = INIT_BRIGHTNESS;
uint8_t speed = INIT_SPEED;

// State
enum States {
  HOME,
  EDIT_DELAY,
  EDIT
};
enum GlobalParams {
  BRIGHTNESS,
  SPEED,
};
bool blinkState = true;
unsigned int blinkStep = BLINK_STEPS;
States state = HOME;
unsigned int editState = BRIGHTNESS;

//-------------- INTERRUPT HANDLERS -------------------------

volatile bool v_frameKey = false, v_frameClip;
void frameInt(){
  if(v_frameKey)
    v_frameClip = true;
  v_frameKey = true;
}

volatile bool v_animStepKey, v_animStepClip;
void stepAnimationInt(){
  if(v_animStepKey)
    v_animStepClip = true;
  v_animStepKey = true;
}

volatile bool v_debouncing = false;
volatile unsigned long v_debounceStartTime = 0;
void debounceButton(){

    
    #ifdef DEBUG
    Serial.print('b');
    #endif

    if(!v_debouncing){
      v_debouncing = true;
      v_debounceStartTime = millis();
    }
}

//-------------- SETUP -------------------------

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  delay(STARTUP_DELAY);
  
  myEnc.write(0);

  Timer1.initialize(1000000 / FPS);
  Timer1.attachInterrupt(frameInt);
  Timer3.initialize(1000000 / ANIM_STEPPER_FREQ);
  Timer3.attachInterrupt(stepAnimationInt);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), debounceButton, CHANGE);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.addLeds<WS2812B, LED_DATA, ORDER>($.leds, NUM_LEDS);
  FastLED.setBrightness(INIT_BRIGHTNESS);

  ANIM.init();
}

//-------------- STATE / UI FUNCTIONS -------------------------

void changeState(States newState){
  state = newState;
  if(newState == EDIT){
    blinkState = true;
    blinkStep = BLINK_STEPS;
    myEnc.write(0);
  }
  
  #ifdef DEBUG
  Serial.printf("\nstate = %d\n", state);
  #endif
}

unsigned long edit_delay_start_millis = 0, lastActivityMillis;
void handleButton(){ 
  noInterrupts();
  lastActivityMillis = v_debounceStartTime;
  interrupts();
  bool isPressed = !digitalRead(ENCODER_BTN);

  switch (state)
  {    
    case HOME:
      if(isPressed){
        edit_delay_start_millis = lastActivityMillis;
        changeState(EDIT_DELAY);
      }
      break;
    case EDIT_DELAY:    // Start a new animation
      if(!isPressed){
        $.anim = ($.anim + 1) % NUM_ANIMS;
        ANIM.init();
        changeState(HOME);
      }
      break;
    case EDIT:
      if(isPressed){
        editState = (editState + 1) % (NUM_GLOBAL_PARAMS + ANIM.numParams);
      }
      break;
  }
}



void changeValue(bool up){
  switch(editState){
    case BRIGHTNESS:
      brightness = CLAMP_8(brightness + ((brightness > BRIGHT_MACRO_ADJ_THRESH)? (BRIGH_ADJ_MULT * INCDEC) : INCDEC));
      FastLED.setBrightness(brightness);
      #ifdef DEBUG
      Serial.printf("Brightness = %d\n", brightness);
      #endif
      break;
    case SPEED:
      speed = CLAMP_8(speed + (INCDEC * SPEED_ADJ_MULT));
      if(!speed){
        Timer3.stop();
      } else {
        Timer3.setPeriod((1000000 * SPEED_REDUCTION_FACTOR) / speed);
      }
      #ifdef DEBUG
      Serial.printf("speed = %d\n", speed);
      #endif
      break;
    default:
      ANIM.adjParam(editState - NUM_GLOBAL_PARAMS, up);
      break;
  }
}

//-------------- VISUAL FUNCTIONS -------------------------

void doFrame(){
  #ifdef TIMING
  Serial.print("f");
  #endif
  if(state == EDIT){
    $.leds(0, NUM_GLOBAL_PARAMS + ANIM.numParams - 1) = CRGB::CornflowerBlue;
    if(!speed){
      $.leds[editState] = CRGB::Red;
    }
    else if(blinkState){
      $.leds[editState] = CRGB::Green;
    }
    else {
      $.leds[editState] = CRGB::Black;
    }
  }
  FastLED.show();
  ANIM.drawFrame();
  $.stepsSinceLastFrame = 0;
}

void stepAnimation(){
  if((state == EDIT) && (!(blinkStep--))){
    blinkStep = BLINK_STEPS;
    blinkState = !blinkState;
  }

  $.stepsSinceLastFrame++;

  #ifdef TIMING
  Serial.print(".");
  #endif
}

//-------------- THE LOOP -------------------------

void loop() {

  if(v_debouncing && ((millis() - v_debounceStartTime) > DEBOUNCE_MILLIS)){
    v_debouncing = false;
    handleButton();
  }
  if(state == EDIT_DELAY){
    if((millis() - edit_delay_start_millis) > EDIT_HOLD_MILLIS){ 
      //Held down button long enough to enter EDIT mode
      changeState(EDIT);
    }
  }
  if(state == EDIT){    
    if((millis() - lastActivityMillis) > EDIT_TIMEOUT_MILLIS){
      changeState(HOME);    // Edit mode has timed out (no activity), so return to HOME state
    } else {      
      int8_t newPosition = myEnc.read();
      if (newPosition >= 8 || newPosition <= -8) {
        changeValue(newPosition >= 8);
        myEnc.write(0);
        lastActivityMillis = millis();
      }
    }
  }

  if(v_frameKey){
    v_frameKey = false;
    doFrame();
  }

  if(v_frameClip){
    v_frameClip = false;
    #ifdef TIMING
    Serial.print("*");
    #endif
  }

  if(v_animStepKey){
    v_animStepKey = false;
    stepAnimation();
  }

  if(v_animStepClip){
    v_animStepClip = false;
    #ifdef TIMING
    Serial.print("!");
    #endif
  }
}

// -----------------------------------------------------






/*
button press changes animation
Hold button for x seconds to enter settings mode

wait y seconds to exit settings mode;
*/
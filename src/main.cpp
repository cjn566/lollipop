// 
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
#define INIT_BRIGHTNESS   10
#define INIT_SPEED        30
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
#define FREQ_TO_STEPS(f) (ANIM_STEPPER_FREQ / f)
#define animParamIdx (currParamIdx - NUM_GLOBAL_PARAMS)

//          VARIABLES AND STUFF
// Hardware
Encoder encoder(ENCODER_A, ENCODER_B);



// create 1 instance of the global state stuff
state_t globalState;


// create instance of the animation classes

Peppermint pepermint1(2, 5);
Peppermint pepermint2(3, 8);
Peppermint pepermint3(1, 2);

AnimationBase *animations[NUM_ANIMS] = {
	&pepermint1,
	&pepermint2,
	&pepermint3,
};

#define ANIM (*animations[globalState.anim])


// Settings
uint8_t brightness = INIT_BRIGHTNESS;
uint8_t speed = INIT_SPEED;

// State
enum States {
  HOME,
  EDIT_DELAY,
  EDIT
};
States state = HOME;
bool blinkState = true;
uint8_t blinkStep = BLINK_STEPS;
uint8_t currParamIdx = 0;
//Parameter *currParam;

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

volatile bool v_debouncing = false, ledtoggle = false;
volatile unsigned long v_debounceStartTime = 0;
void debounceButton(){

  #ifdef DEBUG
  digitalWrite(LED_BUILTIN, ledtoggle? HIGH : LOW);
  ledtoggle = !ledtoggle;
  #endif
  

    if(!v_debouncing){
      v_debouncing = true;
      v_debounceStartTime = millis();
    }
}

//-------------- SETUP -------------------------
void changeState(States);
void initParam();

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  


  //changeState(EDIT);
  currParamIdx = 3;
  initParam();
  #endif

  delay(STARTUP_DELAY);
  
  encoder.write(0);

  Timer1.initialize(1000000 / FPS);
  Timer1.attachInterrupt(frameInt);
  //Timer3.initialize(1000000 / ANIM_STEPPER_FREQ);
  Timer3.initialize((1000000 * SPEED_REDUCTION_FACTOR) / speed);
  Timer3.attachInterrupt(stepAnimationInt);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), debounceButton, CHANGE);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.addLeds<WS2812B, LED_DATA, ORDER>(globalState.leds, NUM_LEDS);
  FastLED.setBrightness(INIT_BRIGHTNESS);

  ANIM.initAnim();
}

//-------------- UI -> PARAMETERS ---------------------------------

#define NUM_GLOBAL_PARAMS 3
enum GlobalParams {
  BRIGHTNESS,
  SPEED,
  SATURATION
};
//Parameter globalParams[NUM_GLOBAL_PARAMS] = {
//  Parameter{CRGB::Crimson, 4}, // Brightness
//  Parameter{CRGB::White, 2},   // Speed
//  Parameter{CRGB::DarkGray, 2} // Saturation
//};

void changeValue(bool up){
  
  #ifdef DEBUG
  Serial.print(up? "\n++: " : "\n--: ");
  #endif

  switch(currParamIdx){
    case BRIGHTNESS:
      brightness = CLAMP_8(brightness + ((brightness > BRIGHT_MACRO_ADJ_THRESH)? (BRIGH_ADJ_MULT * INCDEC) : INCDEC));
      FastLED.setBrightness(brightness);
      //drawScale.setValue( brightness >> 4 ) ;

      #ifdef DEBUG
      Serial.printf("Brightness = %d\n", brightness);
      #endif
      break;
    case SPEED:
      speed = CLAMP_8(speed + INCDEC);
      if(!speed){
        Timer3.stop();
      } else {
        Timer3.setPeriod((1000000 * SPEED_REDUCTION_FACTOR) / speed);
      }
      //drawScale.setValue(speed >> 4);

      #ifdef DEBUG
      Serial.printf("speed = %d\n", speed);
      #endif
      break;
    case SATURATION:
      globalState.saturation = CLAMP_8(globalState.saturation + INCDEC);
      //drawScale.setValue(globalState.saturation >> 4);

      #ifdef DEBUG
      Serial.printf("Saturation = %d\n", globalState.saturation);
      #endif
      break;
    default:
      ANIM.adjParam(animParamIdx, up);
      break;
  }
}

//-------------- UI -> STATE ---------------------------------

uint8_t ticksToAdjust = 4;
void initParam(){  
    encoder.write(0);
    blinkState = true;
    blinkStep = BLINK_STEPS;
    if(currParamIdx < NUM_GLOBAL_PARAMS){
      //ticksToAdjust = globalParams[currParamIdx].ticksToAdjust;// || 4;
    } else {
      //ticksToAdjust = ANIM.paramList[animParamIdx].ticksToAdjust;// || 4;
    }

    switch (currParamIdx)
    {
      case BRIGHTNESS:
        //drawScale.init(drawScale.NOSIGN, brightness >> 4, CRGB::Gold);
        break;
      case SPEED:
        //drawScale.init(drawScale.NOSIGN, speed >> 4, CRGB::Green);
        break;
      case SATURATION:
        //drawScale.init(drawScale.NOSIGN, globalState.saturation >> 4, CRGB::Blue);
        break;
      default:
        ANIM.initParam(animParamIdx);
        break;
    }
}

void changeState(States newState){
  switch(newState){
    case EDIT:
      initParam();
      break;
    default:
      break;
  }
  state = newState;
  
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
        globalState.anim = (globalState.anim + 1) % NUM_ANIMS;
        ANIM.initAnim();
        changeState(HOME);
      }
      break;
    case EDIT:
      if(isPressed){    // Change parameter to Edit
        //currParamIdx = addmod8(currParamIdx, (uint8_t)1, (uint8_t)(NUM_GLOBAL_PARAMS + ANIM.numParams));
        initParam();
      }
      break;
  }
}

//-------------- VISUAL FUNCTIONS -------------------------

void doFrame(){
  #ifdef TIMING
  Serial.print("f");
  #endif
  if(state == EDIT){
    //drawScale.draw();
	// this tries to read "numParams" -- normally you would provide a function
	// in the class which allows stuff outside the class to get the internal state
    //globalState.leds(0, (NUM_GLOBAL_PARAMS + ANIM.numParams - 1)) = CRGB::LightSeaGreen;
    if(!speed){
      globalState.leds[currParamIdx] = CRGB::Red;
    }
    else if(blinkState){
      globalState.leds[currParamIdx] = CRGB::Green;
    }
    else {
      globalState.leds[currParamIdx] = CRGB::Black;
    }
  }
  FastLED.show();
  ANIM.drawFrame();
  globalState.stepsSinceLastFrame = 0;
}

void stepAnimation(){
  if((state == EDIT) && (!(blinkStep--))){
    blinkStep = BLINK_STEPS;
    blinkState = !blinkState;
  }

  globalState.stepsSinceLastFrame++;

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
      int8_t newPosition = encoder.read();
      if (abs8(newPosition) >= ticksToAdjust) {
        changeValue(newPosition >= ticksToAdjust);
        encoder.write(0);
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

#include <Arduino.h>
#include <Encoder.h>
#include "Animation.h"
#include "util.h"
#include "Visual_UI.h"

// Hardware
Encoder encoder(ENCODER_A, ENCODER_B);

// Parameters
uint8_t brightness = INIT_BRIGHTNESS;

// State
enum UI_State
{
  HOME,
  EDIT_DELAY,
  EDIT
};

enum GlobalParams
{
  PATTERN,
  BRIGHTNESS,
  SPEED,
  SATURATION
};
parameter_t animSelect{
    CRGB::FireBrick,
    2, // max
    3, // ticks
    CHUNKS
};
parameter_t globalParams[NUM_GLOBAL_PARAMS] = {
    animSelect,
    parameter_t{CRGB::Gold}, // Brightness
    parameter_t{CRGB::Green, 127},   // Speed
    parameter_t{CRGB::Red} // Saturation
};

#ifdef DEBUG
    UI_State ui_state = HOME;
#else
    UI_State ui_state = HOME;
#endif
Visual_UI ui;
if_anim_t ledData;
if_ui_t if_ui;  
unsigned long edit_delay_start_millis = 0, lastActivityMillis;

// -------- ANIMATIONS ---------------
#define NUM_ANIMATIONS 3
#include "Animations/peppermint.cpp"
#include "Animations/rainbow.cpp"
#include "Animations/particles.cpp"
Peppermint peppermint = Peppermint();
Rainbow rainbow = Rainbow();
Particles particles = Particles();
AnimationBase *allAnims[NUM_ANIMATIONS] = {
    &particles,
    &peppermint,
    &rainbow
    };
uint8_t currAnimationIdx = 0;

//-------------- INTERRUPT HANDLERS -------------------------

volatile bool v_debouncing = false, ledtoggle = false;
volatile unsigned long v_debounceStartTime = 0;
void debounceButton()
{
  if (!v_debouncing)
  {
    v_debouncing = true;
    v_debounceStartTime = millis();
  }
}

//-------------- SETUP -------------------------
void changeState(UI_State);
void initAnimation();
void initParam();

void setup()
{
  #ifdef DEBUG
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
  #endif

  int mostParams = 0;
  for(int i=0;i<NUM_ANIMATIONS;i++){
    int numParams = allAnims[i]->numParams;
    if (numParams > mostParams) mostParams = numParams;
  }

  delay(STARTUP_DELAY);
  ui.init(globalParams, mostParams);
  encoder.write(0);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), debounceButton, CHANGE);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.addLeds<WS2812B, LED_DATA, ORDER>(ledData.leds, NUM_LEDS);
  FastLED.setBrightness(INIT_BRIGHTNESS);
  FastLED.setMaxRefreshRate(FPS);

  ledData.animation = allAnims[currAnimationIdx];
  initAnimation();
  initParam();
}

//-------------- UI -> PARAMETERS ---------------------------------

void changeSelectedParam(bool up){
  if_ui.paramIdx += INCDEC;
  if(if_ui.edittingGlobalParams){    
    if(if_ui.paramIdx >= NUM_GLOBAL_PARAMS){
      if_ui.paramIdx = 0;
      if_ui.edittingGlobalParams = false;
    } else if(if_ui.paramIdx < 0){
      if_ui.paramIdx = if_ui.numAnimParams;
      if_ui.edittingGlobalParams = false;
    }
  } else {
    if(if_ui.paramIdx >= if_ui.numAnimParams){
      if_ui.paramIdx = 0;
      if_ui.edittingGlobalParams = true;
    } else if(if_ui.paramIdx < 0){
      if_ui.paramIdx = NUM_GLOBAL_PARAMS;
      if_ui.edittingGlobalParams = true;
    }
  }
  initParam();
}

uint8_t fastScrollCtr = 2;
#define FAST_SCROLL ((INCDEC * fastScrollCtr) >> 1)
void adjParam(bool up)
{
  int delay = millis() - lastActivityMillis;
  if(delay < FAST_SCROLL_MS){
    fastScrollCtr = clamp_un0(fastScrollCtr+1, FAST_SCROLL_MAX) ;
  } else if( delay > FAST_SCROLL_RESET){
    fastScrollCtr = 2;
  }

  if(if_ui.edittingGlobalParams){
    switch (if_ui.paramIdx)
    {
    case PATTERN:
      currAnimationIdx = clamp_un0(currAnimationIdx + INCDEC, NUM_ANIMATIONS - 1);
      initAnimation();
      break;
    case BRIGHTNESS:
      brightness = CLAMP_8(brightness + FAST_SCROLL);
      FastLED.setBrightness(brightness);
      ui.setValue(brightness);
      break;
    case SPEED:
      ui.setValue(ledData.animation->adjSpeed(FAST_SCROLL));
      break;
    case SATURATION:
      ledData.saturation = CLAMP_8(ledData.saturation + FAST_SCROLL);
      ui.setValue(ledData.saturation);
    }
  }
  else ui.setValue(ledData.animation->adjParam(if_ui.paramIdx, FAST_SCROLL));
}

void handleSpin(bool up){
  if(if_ui.selectingParams){
    changeSelectedParam(up);
  } else {
    adjParam(up);
  }
}

uint8_t ticksToAdjust = 1;
void initParam()
{
  if(if_ui.edittingGlobalParams){
    if_ui.currParam = &globalParams[if_ui.paramIdx];
    switch(if_ui.paramIdx){
      case BRIGHTNESS:
        ui.setValue(brightness);
        break;
      case SPEED:
        ui.setValue(ledData.animation->speed);
        break;
      case SATURATION:
        ui.setValue(ledData.saturation);
        break;
    }

  } else {
    if_ui.currParam = ledData.animation->getParam(if_ui.paramIdx);
    ui.setValue(ledData.animation->adjParam(if_ui.paramIdx, 0));
  }
  ui.setParameter();
  ticksToAdjust = if_ui.currParam->ticksToAdjust;
}

//-------------- UI -> STATE ---------------------------------

void changeState(UI_State newState)
{
  switch (newState)
  {
  case EDIT: //Entering edit mode
    if_ui.selectingParams = true;
    initParam();
    break;
  default:
    break;
  }
  ui_state = newState;
}

void initAnimation(){
    ledData.animation = allAnims[currAnimationIdx];
    if_ui.numAnimParams = ledData.animation->getNumParams();
    ui.setAnimation();
    ledData.animation->initAnim();
}

void handleButton()
{
  noInterrupts();
  lastActivityMillis = v_debounceStartTime;
  interrupts();
  bool isPressed = !digitalRead(ENCODER_BTN);

  switch (ui_state)
  {
  case HOME:
    if (isPressed)
    {
      edit_delay_start_millis = lastActivityMillis;
      changeState(EDIT_DELAY);
    }
    break;
  case EDIT_DELAY: 
    if (!isPressed) // Clicked the button
    {
      // Special Effect?
    }
    break;
  case EDIT:  // Button is pressed while in edit mode
    if (isPressed)
    {
      if_ui.selectingParams = !if_ui.selectingParams;
      if(if_ui.selectingParams) ticksToAdjust = 1;
      encoder.write(0);
    }
    break;
  }
}

// -------------- VISUAL FUNCTIONS -------------------------

long elapseMillis;
void doFrame()
{
  long now = millis();
  long elapse = now - elapseMillis;
  if(elapse > 50) elapse = 50;
  elapseMillis = now;

  FastLED.show();
  FastLED.clear();
  ledData.animation->drawBase(elapse);

  if ( ui_state == EDIT ) ui.draw();
}

//-------------- THE LOOP -------------------------
void loop()
{
  long now = millis();

  if (v_debouncing && ((now - v_debounceStartTime) > DEBOUNCE_MILLIS))
  {
    v_debouncing = false;
    handleButton();
  }
  if (ui_state == EDIT_DELAY)
  {
    if ((now - edit_delay_start_millis) > EDIT_HOLD_MILLIS)
    {
      //Held down button long enough to enter EDIT mode
      changeState(EDIT);
    }
  }
  if (ui_state == EDIT)
  {
    if ((now - lastActivityMillis) > EDIT_TIMEOUT_MILLIS)
    {
      changeState(HOME); // Edit mode has timed out (no activity), so return to HOME ui_state
    }
    else
    {
      int8_t newPosition = encoder.read();
      if ((abs8(newPosition)>>2) >= ticksToAdjust)
      {
        encoder.write(0);
        lastActivityMillis = now;
        handleSpin(newPosition >= ticksToAdjust);
      }
    }
  }
  doFrame();
}
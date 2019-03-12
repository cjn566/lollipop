#include <Arduino.h>
#include <Encoder.h>
#include "Animation.h"
#include "util.h"
#include "settings.h"

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
  BRIGHTNESS,
  SPEED,
  SATURATION
};
parameter_t globalParams[NUM_GLOBAL_PARAMS] = {
    parameter_t{CRGB::Gold}, // Brightness
    parameter_t{CRGB::Green, 127},   // Speed
    parameter_t{CRGB::Red} // Saturation
};

#ifdef DEBUG
    UI_State ui_state = HOME;
    bool edittingGlobalParams = false;
    uint8_t paramIdx = 4;
    uint8_t numAnimParams, numTotalParams;
    DrawScale drawScale;
    state_t ledData;
#else
    UI_State ui_state = HOME;
    bool edittingGlobalParams = true;
    uint8_t paramIdx = 0;
    uint8_t numAnimParams, numTotalParams;
    DrawScale drawScale;
    state_t ledData;
#endif

// -------- ANIMATIONS ---------------
#define NUM_ANIMATIONS 3
#include "Animations/peppermint.cpp"
#include "Animations/rainbow.cpp"
//#include "Animations/indices.cpp"
#include "Animations/particles.cpp"


Peppermint peppermint = Peppermint();
Rainbow rainbow = Rainbow();
//Indices indices = Indices();
Particles particles = Particles();

AnimationBase *allAnims[NUM_ANIMATIONS] = {
    &particles,
    &rainbow,
    &peppermint,
    //&indices
    };
#define CURR_ANIM allAnims[currAnimationIdx]
uint8_t currAnimationIdx = 0;

//-------------- INTERRUPT HANDLERS -------------------------

volatile bool v_debouncing = false, ledtoggle = false;
volatile unsigned long v_debounceStartTime = 0;
void debounceButton()
{
  #ifdef DEBUG
    digitalWrite(LED_BUILTIN, ledtoggle ? HIGH : LOW);
    ledtoggle = !ledtoggle;
  #endif

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

  delay(STARTUP_DELAY);
  drawScale.init(globalParams);
  encoder.write(0);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), debounceButton, CHANGE);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.addLeds<WS2812B, LED_DATA, ORDER>(ledData.leds, NUM_LEDS);
  FastLED.setBrightness(INIT_BRIGHTNESS);
  FastLED.setMaxRefreshRate(FPS);

  initAnimation();
  initParam();
}

//-------------- UI -> PARAMETERS ---------------------------------

void changeValue(bool up)
{
  if(edittingGlobalParams){
    switch (paramIdx)
    {
    case BRIGHTNESS:
      brightness = CLAMP_8(brightness + FAST_SCROLL);
      FastLED.setBrightness(brightness);
      drawScale.setValue(brightness);
      break;
    case SPEED:
      drawScale.setValue(CURR_ANIM->adjSpeed(FAST_SCROLL));
      break;
    case SATURATION:
      ledData.saturation = CLAMP_8(ledData.saturation + FAST_SCROLL);
      drawScale.setValue(ledData.saturation);
    }
  }
  else drawScale.setValue(CURR_ANIM->adjParam(paramIdx, FAST_SCROLL));
}

uint8_t ticksToAdjust = 1;
void initParam()
{
  ledData.fast_scroll_ctr = 0;
  drawScale.setParameter(edittingGlobalParams, paramIdx);
  encoder.write(0);
  if(edittingGlobalParams){
    ticksToAdjust = globalParams[paramIdx].ticksToAdjust;
    switch(paramIdx){
      case BRIGHTNESS:
        drawScale.setValue(brightness);
        break;
      case SPEED:
        drawScale.setValue(CURR_ANIM->speed);
        break;
      case SATURATION:
        drawScale.setValue(ledData.saturation);
        break;
    }
  }
  else
  {
    ticksToAdjust = CURR_ANIM->getParam(paramIdx).ticksToAdjust;
    drawScale.setValue(CURR_ANIM->adjParam(paramIdx, 0));
  }
}

//-------------- UI -> STATE ---------------------------------

void changeState(UI_State newState)
{
  switch (newState)
  {
  case EDIT:
    initParam();
    break;
  default:
    break;
  }
  ui_state = newState;

#ifdef DEBUG
  Serial.printf("\nui_state = %d\n", ui_state);
#endif
}

void initAnimation(){
    drawScale.setAnimation(CURR_ANIM);
    numAnimParams = CURR_ANIM->getNumParams();
    CURR_ANIM->initAnim();
}

unsigned long edit_delay_start_millis = 0, lastActivityMillis;
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
    if (!isPressed) // Did not finish delay, so quick press = new animation
    {
      currAnimationIdx = mod8(currAnimationIdx + 1, NUM_ANIMATIONS);
      initAnimation();
      changeState(HOME);
    }
    break;
  case EDIT:
    if (isPressed)
    { // Change parameter to Edit
      if(edittingGlobalParams){
        paramIdx++;
        if(paramIdx >= NUM_GLOBAL_PARAMS){
          paramIdx = 0;
          edittingGlobalParams = false;
        }
      } else {
        paramIdx++;
        if(paramIdx >= numAnimParams){
          paramIdx = 0;
          edittingGlobalParams = true;
        }
      }
      initParam();
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
  CURR_ANIM->drawBase(elapse);

  if ( ui_state == EDIT ) drawScale.draw();
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

        int delay = now - lastActivityMillis;
        if(delay < FAST_SCROLL_MS){
          ledData.fast_scroll_ctr = clamp_un0(ledData.fast_scroll_ctr+1, FAST_SCROLL_MAX) ;
        } else if( delay > FAST_SCROLL_RESET){
          ledData.fast_scroll_ctr = 2;
        }
        lastActivityMillis = now;
        changeValue(newPosition >= ticksToAdjust);
      }
    }
  }
  doFrame();
}
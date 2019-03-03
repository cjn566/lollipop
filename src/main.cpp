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

UI_State ui_state = HOME;
bool blinkState = true, edittingGlobalParams = true;
long frameMillis, blinkMillis;
uint8_t globParamIdx = 0, animParamIdx = 0;
uint8_t numAnimParams, numTotalParams;
DrawScale drawScale;
state_t ledData;

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
    &peppermint,
    &particles,
    &rainbow,
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
  encoder.write(0);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), debounceButton, CHANGE);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
  FastLED.addLeds<WS2812B, LED_DATA, ORDER>(ledData.leds, NUM_LEDS);
  FastLED.setBrightness(INIT_BRIGHTNESS);

  initAnimation();
}

//-------------- UI -> PARAMETERS ---------------------------------

#define NUM_GLOBAL_PARAMS 3
enum GlobalParams
{
  BRIGHTNESS,
  SPEED,
  SATURATION
};
parameter_t globalParams[NUM_GLOBAL_PARAMS] = {
    parameter_t{CRGB::Crimson, 4}, // Brightness
    parameter_t{CRGB::White, 2},   // Speed
    parameter_t{CRGB::DarkGray, 2} // Saturation
};

void changeValue(bool up)
{

#ifdef DEBUG
  Serial.print(up ? "\n++: " : "\n--: ");
#endif

  if(edittingGlobalParams){
    switch (globParamIdx)
    {
    case BRIGHTNESS:
      brightness = CLAMP_8(brightness + ((brightness > BRIGHT_MACRO_ADJ_THRESH) ? (BRIGH_ADJ_MULT * INCDEC) : INCDEC));
      FastLED.setBrightness(brightness);
      drawScale.setValue(brightness);
      break;
    case SPEED:
      ledData.speed = CLAMP_8(ledData.speed + INCDEC);
      drawScale.setValue(ledData.speed);
      break;
    case SATURATION:
      ledData.saturation = CLAMP_8(ledData.saturation + INCDEC);
      drawScale.setValue(ledData.saturation);
    }
  }
  else CURR_ANIM->adjParam(animParamIdx, up);
}

uint8_t ticksToAdjust = 4;
void initParam()
{
  encoder.write(0);
  blinkState = true;
  blinkMillis = millis();
  if(edittingGlobalParams){
    ticksToAdjust = globalParams[globParamIdx].ticksToAdjust; // || 4;    
    switch (globParamIdx)
    {
    case BRIGHTNESS:
      drawScale.init(true, 256, brightness, CRGB::Gold);
      break;
    case SPEED:
      drawScale.init(true, 256, ledData.speed, CRGB::Green);
      break;
    case SATURATION:
      drawScale.init(true, 256, ledData.saturation, CRGB::Blue);
      break;
    }
  }
  else
  {
    ticksToAdjust = CURR_ANIM->getParam(animParamIdx).ticksToAdjust; // || 4;
    CURR_ANIM->initParam(animParamIdx);
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
    numAnimParams = CURR_ANIM->getNumParams();
    numTotalParams = numAnimParams + NUM_GLOBAL_PARAMS;
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
  case EDIT_DELAY: // Start a new animation
    if (!isPressed)
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
        globParamIdx++;
        if(globParamIdx >= NUM_GLOBAL_PARAMS){
          globParamIdx = 0;
          edittingGlobalParams = false;
        }
      } else {        
        animParamIdx++;
        if(animParamIdx >= numAnimParams){
          animParamIdx = 0;
          edittingGlobalParams = true;
        }
      }
      initParam();
    }
    break;
  }
}

//-------------- VISUAL FUNCTIONS -------------------------
#define AVERAGES 4
long elapses[AVERAGES];
uint8_t elapseIdx;
long getElapsedAvg(){
  long elapsed = millis() - elapses[elapseIdx];
  elapseIdx = (elapseIdx+1) % AVERAGES;
  elapses[elapseIdx] = elapsed;
  long avg = 0;
  for(int i = 0; i < AVERAGES; i++){
    avg += elapses[i];
  }
  return (avg >> 2);
}

void doFrame()
{
  FastLED.show();
  CURR_ANIM->drawFrame(getElapsedAvg());

  long now = millis();
  if ((ui_state == EDIT) && (now - blinkMillis >= BLINK_MILLIS))
  {
    blinkMillis = now;
    blinkState = !blinkState;
  }

#ifdef TIMING
  Serial.print("f");
#endif
  if (ui_state == EDIT)
  {
    drawScale.draw();
    ledData.leds(0, (NUM_GLOBAL_PARAMS + CURR_ANIM->getNumParams() - 1)) = CRGB::LightSeaGreen;

    //TODO: unfuck
    uint8_t currParamIdx = edittingGlobalParams? globParamIdx : (NUM_GLOBAL_PARAMS + animParamIdx);
    if (!ledData.speed)
    {
      ledData.leds[currParamIdx] = CRGB::Red;
    }
    else if (blinkState)
    {
      ledData.leds[currParamIdx] = CRGB::Green;
    }
    else
    {
      ledData.leds[currParamIdx] = CRGB::Black;
    }
  }
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
      if (abs8(newPosition) >= ticksToAdjust)
      {
        changeValue(newPosition >= ticksToAdjust);
        encoder.write(0);
        lastActivityMillis = now;
      }
    }
  }
  doFrame();
}
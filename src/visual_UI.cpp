#include "Animation.h"

#define RIGHT_OF_MID_IDX 67
#define SCALE_HALF_SIZE 8

#define SCALE_START_IDX (RIGHT_OF_MID_IDX - SCALE_HALF_SIZE)
#define SCALE_FULL_SIZE (SCALE_HALF_SIZE*2)

bool active = false;
int pValue;
int maxVal;
uint8_t hueShiftPerVal, valuesPerLed;
CRGB onColor = CRGB::Red;

void DrawScale::setValue(int val){
    if(val >= 0){
        pValue = (val > maxVal)? maxVal : val;
    } else {
        pValue = (val < -maxVal)? -maxVal : val;
    }
    #ifdef DEBUG
    Serial.printf("scale value: %d\n", val);
    #endif
}

void DrawScale::init(bool isActive, int nMax, int val, CRGB nColor){
    active = isActive;
    maxVal = nMax;
    valuesPerLed = (maxVal >> 4);
    hueShiftPerVal = 256 / valuesPerLed; // Hardcoded to 16 wide scale..
    onColor = nColor;
    setValue(val);
}

void DrawScale::draw(){
    if(active){
        ledData.leds(SCALE_START_IDX, SCALE_START_IDX + (SCALE_HALF_SIZE*2) - 1) = CRGB::White;
        ledData.leds[SCALE_START_IDX - 1] = CRGB::Black;
        ledData.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Black;

        int numFullLeds = pValue / valuesPerLed;
        int valuesRem = pValue % valuesPerLed;

        if(pValue > 0){
            ledData.leds(SCALE_START_IDX, SCALE_START_IDX + numFullLeds)  = onColor;
            if(pValue) ledData.leds(SCALE_START_IDX, (pValue + SCALE_START_IDX))  = onColor;
        } else if(pValue < 0){

        }
    }
}
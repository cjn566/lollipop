#include "Animation.h"

#define RIGHT_OF_MID_IDX 67
#define SCALE_HALF_SIZE 8

#define SCALE_START_IDX (RIGHT_OF_MID_IDX - SCALE_HALF_SIZE)
#define SCALE_END_IDX   (RIGHT_OF_MID_IDX + SCALE_HALF_SIZE - 1)
#define SCALE_FULL_SIZE (SCALE_HALF_SIZE*2)

bool active = false;
int pValue;
int maxVal;
uint16_t brightnessPointsPerValue;
CRGB onColor;

void DrawScale::setValue(int val){
    
    pValue = CLAMP_SN(val, maxVal);
    // if(val >= 0){
    //     pValue = (val > maxVal)? maxVal : val;
    // } else {
    //     pValue = (val < -maxVal)? -maxVal : val;
    // }
    #ifdef DEBUG
    Serial.printf("scale value: %d\n", val);
    #endif
}

void DrawScale::init(bool isActive, int nMax, int val, CRGB nColor){
    active = isActive;
    maxVal = nMax;
    brightnessPointsPerValue = (SCALE_FULL_SIZE * 256) / nMax;
    onColor = nColor;
    setValue(val);
}

void DrawScale::draw(){
    if(active){
        ledData.leds(SCALE_START_IDX, SCALE_START_IDX + (SCALE_HALF_SIZE*2) - 1) = CRGB::White;
        ledData.leds[SCALE_START_IDX - 1] = CRGB::Black;
        ledData.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Black;

        if(pValue != 0){
            
            int brightPoints = pValue * brightnessPointsPerValue;
            int numFullLeds = brightPoints / 256;
            uint8_t remainingBrightness = brightPoints % 256;
            uint8_t begginning, end, partial;

            if(pValue > 0){
                begginning = SCALE_START_IDX;
                end = SCALE_START_IDX + numFullLeds;
                partial = end + 1;
            } else {
                begginning = SCALE_END_IDX + numFullLeds;
                end = SCALE_END_IDX;
                partial = begginning - 1;
            }   

            ledData.leds(begginning, end)  = onColor;            
            ledData.leds[partial] = ledData.leds[partial].lerp8(onColor, remainingBrightness);
        }
    }
}
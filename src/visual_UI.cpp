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
    active = true;
    #ifdef DEBUG
    Serial.printf("scale value: %d\n", val);
    #endif
}

void DrawScale::turnoff(){active=false;};

void DrawScale::init(uint32_t nMax, CRGB nColor){
    active = false;
    maxVal = nMax;
    brightnessPointsPerValue = (SCALE_FULL_SIZE * 256) / nMax;
    onColor = nColor;
}

void DrawScale::init(parameter_t * param_ptr){
    init(param_ptr->max, param_ptr->scaleColor);
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
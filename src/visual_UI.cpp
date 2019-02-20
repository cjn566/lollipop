#include "Animation.h"

#define SCALE_POS_MID_IDX 67
#define SCALE_HALF_SIZE 8
#define SCALE_START_IDX (SCALE_POS_MID_IDX - SCALE_HALF_SIZE)

bool active;
int pValue;
int maxVal;
CRGB onColor = CRGB::Red;

void drawNoSign(){
    if(pValue) ledData.leds(SCALE_START_IDX, (pValue + SCALE_START_IDX))  = onColor;
}

void drawSign(){
    if(pValue) {
        ledData.leds(SCALE_POS_MID_IDX + (sign? 0:-1), SCALE_POS_MID_IDX + pValue + (sign? -1:0))  = onColor;
    }
}

void DrawScale::setValue(int val){
    switch(mode){
        case NOSIGN:
            if(val <= (SCALE_HALF_SIZE*2) && val >= 0) pValue = val;
            break;
        case SIGN:
            if(val <= SCALE_HALF_SIZE && -val >= -SCALE_HALF_SIZE){
                pValue = val;
                sign = (pValue > 0);
            }
            break;
        default:
            break;
    }
    pValue = val;

    #ifdef DEBUG
    Serial.printf("scale value: %d\n", val);
    #endif
}

void DrawScale::init(bool isActive, int nMax, int val, CRGB nColor){
    active = isActive;
    maxVal = nMax;
    onColor = nColor;
    setValue(val);
}

void DrawScale::draw(){
    if(active){
        ledData.leds(SCALE_START_IDX, SCALE_START_IDX + (SCALE_HALF_SIZE*2) - 1) = CRGB::White;
        ledData.leds[SCALE_START_IDX - 1] = CRGB::Black;
        ledData.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Black;
        switch(mode){
            case NOSIGN:
                drawNoSign();
                break;
            case SIGN:
                drawSign();
                break;
            default:
                break;
        }
    }
}
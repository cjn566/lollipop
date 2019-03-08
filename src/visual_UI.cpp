#include "Animation.h"

#define RIGHT_OF_MID_IDX 67
#define SCALE_HALF_SIZE 8

#define SCALE_START_IDX (RIGHT_OF_MID_IDX - SCALE_HALF_SIZE)
#define SCALE_END_IDX   (RIGHT_OF_MID_IDX + SCALE_HALF_SIZE - 1)
#define SCALE_FULL_SIZE (SCALE_HALF_SIZE*2)

#define DFLT_COLOR CRGB::Green

int pValue;
int maxVal;
bool isGlobal;
uint8_t paramIdx, numAnimParams, biggest;
parameter_t * globParams;
AnimationBase * animation;
uint16_t brightnessPointsPerValue;
CRGB globColors[NUM_GLOBAL_PARAMS];
CRGB *paramColors;

DrawScale::DrawScale(parameter_t * globParamsArg){
    globParams = globParamsArg;
    for(int i = 0; i< NUM_GLOBAL_PARAMS; i++){
        globColors[i] = globParams[i].scaleColor;
    }
};

void DrawScale::setAnimation(AnimationBase *animationArg){
    animation = animationArg;
    numAnimParams = animation->getNumParams;
    if(numAnimParams > biggest){
        delete [] paramColors;
        biggest = numAnimParams;
        paramColors = new CRGB[biggest];
    }
    for(int i = 0; i<biggest; i++){
        paramColors[i] = animation->getParam(i).scaleColor;
        if(!paramColors[i]) paramColors[i] = DFLT_COLOR;
    }
}

void DrawScale::setParameter(bool isGlobalArg, uint8_t paramIdxArg){
    isGlobal = isGlobalArg;
    paramIdx = paramIdxArg;
    parameter_t param;
    if(isGlobal){
        param = globParams[paramIdx];
    } else {
        param = animation->getParam(paramIdx);
    }    
    maxVal = param.max==0? 256 : param.max;
    brightnessPointsPerValue = (SCALE_FULL_SIZE * 256) / maxVal;
    
    #ifdef DEBUG        
        //Serial.printf("Init - argMax: %d\targColor: %d\t max: %d\t color: %d\n", nMax, color, maxVal, onColor);
    #endif

}

void DrawScale::setValue(int val){
    pValue = CLAMP_SN(val, maxVal);
    #ifdef DEBUG
        //newValue = true;
        //Serial.printf("scale value: %d\n", val);
    #endif
}
    
void DrawScale::draw(bool blinkOn){

    for(int globs = 0;globs < 2; globs++){
        for(int i = 0; i < (globs? NUM_GLOBAL_PARAMS : numAnimParams);i++){
            
        }
    }

    ledData.leds(0, (NUM_GLOBAL_PARAMS + CURR_ANIM->getNumParams() - 1)) = CRGB::LightSeaGreen;

    uint8_t currParamIdx = edittingGlobalParams? paramIdx : (NUM_GLOBAL_PARAMS + paramIdx);
    if (blinkState)
    {
      ledData.leds[currParamIdx] = edittingGlobalParams? globalParams[paramIdx].scaleColor :
        CURR_ANIM->getParam(paramIdx).scaleColor;
    }
    else
    {
      ledData.leds[currParamIdx] = CRGB::Black;
    }

    
        
    ledData.leds(SCALE_START_IDX, SCALE_START_IDX + (SCALE_HALF_SIZE*2) - 1) = CRGB::White;
    ledData.leds[SCALE_START_IDX - 1] = CRGB::Red;
    ledData.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Red;

    if(pValue != 0){
        if(abs(pValue) == maxVal){
            ledData.leds(SCALE_START_IDX, SCALE_END_IDX)  = onColor;
        } else {         
            int brightPoints = pValue * brightnessPointsPerValue;
            int numFullLeds = brightPoints / 256;
            int16_t remainingBrightness = brightPoints % 256;
            if(remainingBrightness < 0) remainingBrightness = -remainingBrightness;
            uint8_t begginning, end, partial;
            
            if(pValue > 0){
                begginning = SCALE_START_IDX;
                end = SCALE_START_IDX + numFullLeds - 1;
                partial = end + 1;
            } else {
                begginning = SCALE_END_IDX + numFullLeds + 1;
                end = SCALE_END_IDX;
                partial = begginning - 1;
            }   

            if(end >= begginning){
                ledData.leds(begginning, end)  = onColor;
            }

            #ifdef DEBUG
                // if(newValue){
                //     newValue = false;
                //     Serial.printf("n: %d\tb: %d\te: %d\tp: %d\tpb: %d\t\n", numFullLeds, begginning, end, partial, remainingBrightness);
                // }
            #endif
            ledData.leds[partial] = ledData.leds[partial].lerp8(onColor, remainingBrightness);
        }
    }
}
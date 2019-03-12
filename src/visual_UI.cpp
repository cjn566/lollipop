#include "Animation.h"

#define RIGHT_OF_MID_IDX    67
#define SCALE_HALF_SIZE     8
#define ROW_2_CTR_POS       104
#define ROW_3_CTR           134
#define BLINK_SHIFT         9

#define DFLT_HUE              0
#define LEDS_PER_SLCTR      2

#define SCALE_START_IDX (RIGHT_OF_MID_IDX - SCALE_HALF_SIZE)
#define SCALE_END_IDX   (RIGHT_OF_MID_IDX + SCALE_HALF_SIZE - 1)
#define SCALE_FULL_SIZE (SCALE_HALF_SIZE*2)
#define ROW_2_START_IDX (ROW_2_CTR_POS - ((NUM_GLOBAL_PARAMS / 2) * LEDS_PER_SLCTR) - 1)



int pValue;
int maxVal;
bool isGlobal;
DispType type;
uint8_t paramIndex, nAnimParams, mostParams, currParamLedIdx, row3startIdx;
parameter_t * globParams;
AnimationBase * animation;
uint16_t brightnessPointsPerValue;
CRGB globColors[NUM_GLOBAL_PARAMS * LEDS_PER_SLCTR];
CRGB * paramColors;
CRGB currColor;

void DrawScale::init(parameter_t * globParamsArg){
    globParams = globParamsArg;
    for(int i = 0; i< NUM_GLOBAL_PARAMS; i++){
        for(int j = 0; j< LEDS_PER_SLCTR; j++){
            int idx = (i*LEDS_PER_SLCTR) + j;
            globColors[idx] = globParams[i].scaleColor;
        }
    }
};

void DrawScale::setAnimation(AnimationBase *animationArg){
    animation = animationArg;
    nAnimParams = animation->getNumParams();
    if(nAnimParams > mostParams){
        delete [] paramColors;
        mostParams = nAnimParams;
        paramColors = new CRGB[mostParams * LEDS_PER_SLCTR];
    }
    for(int i = 0; i<nAnimParams; i++){
        CRGB c = animation->getParam(i).scaleColor;
        if(!(c.r || c.b ||c.g)){
            c = CHSV(DFLT_HUE + (i* 80), 255,255);
        }
        for(int j = 0; j< LEDS_PER_SLCTR; j++){
            int idx = (i*LEDS_PER_SLCTR) + j;
            paramColors[idx] = c;
        }        
    }
    row3startIdx = (ROW_3_CTR - ((nAnimParams / 2) * LEDS_PER_SLCTR));
}

void DrawScale::setParameter(bool isGlobalArg, uint8_t paramIndexArg){
    isGlobal = isGlobalArg;
    paramIndex = paramIndexArg;
    parameter_t param;
    if(isGlobal){
        param = globParams[paramIndex];
        currParamLedIdx = ROW_2_START_IDX + (paramIndex * LEDS_PER_SLCTR);
        currColor = globColors[paramIndex*LEDS_PER_SLCTR];
    } else {
        param = animation->getParam(paramIndex);
        currParamLedIdx = row3startIdx + (paramIndex * LEDS_PER_SLCTR);
        currColor = paramColors[paramIndex*LEDS_PER_SLCTR];
    }
    type = param.type;
    maxVal = param.max? param.max : 255;
    brightnessPointsPerValue = (SCALE_FULL_SIZE * 256) / (maxVal + (type==CHUNKS?1:0));
}

bool newValue;
void DrawScale::setValue(int val){
    pValue = val;
    if(val > maxVal || val < -maxVal) {
        pValue = 0;
    }
}
    
void DrawScale::draw(){
    memcpy(&ledData.leds[ROW_2_START_IDX], globColors, sizeof(CRGB)*LEDS_PER_SLCTR*NUM_GLOBAL_PARAMS);
    memcpy(&ledData.leds[row3startIdx], paramColors, sizeof(CRGB)*LEDS_PER_SLCTR*nAnimParams);

    if(millis() & (1<<BLINK_SHIFT)){
        ledData.leds(currParamLedIdx, currParamLedIdx -1 + LEDS_PER_SLCTR) = CRGB::Black;
    }
    
    ledData.leds(SCALE_START_IDX, SCALE_START_IDX + (SCALE_HALF_SIZE*2) - 1) = (currColor.r == 0xff && currColor.g == 0xff &&currColor.b == 0xff)? CRGB::Black : CRGB::White;
    ledData.leds[SCALE_START_IDX - 1] = CRGB::Red;
    ledData.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Red;

    switch(type){
        case CHUNKS:
            int numLeds = brightnessPointsPerValue / 256;
            ledData.leds(SCALE_START_IDX + (pValue*numLeds), SCALE_START_IDX + ((pValue + 1) * numLeds))  = currColor;
            break;
        case BOOL:
            ledData.leds(pValue? SCALE_START_IDX : RIGHT_OF_MID_IDX, pValue? RIGHT_OF_MID_IDX - 1: SCALE_END_IDX)  = currColor;
            break;
        case HUE:
            
            break;
        case OTHER:
            if(pValue != 0){
                if(abs(pValue) == maxVal){
                    ledData.leds(SCALE_START_IDX, SCALE_END_IDX)  = currColor;
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
                        ledData.leds(begginning, end)  = currColor;
                    }

                    ledData.leds[partial] = ledData.leds[partial].lerp8(currColor, remainingBrightness);
                }
            }
            break;
    }        
}
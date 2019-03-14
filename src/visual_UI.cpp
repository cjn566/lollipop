#include "Visual_UI.h"
#include "Radii.h"

#define RIGHT_OF_MID_IDX    67
#define ROW_2_CTR_POS       104
#define ROW_3_CTR           134

#define SCALE_START_IDX (RIGHT_OF_MID_IDX - SCALE_HALF_SIZE)
#define SCALE_END_IDX   (RIGHT_OF_MID_IDX + SCALE_HALF_SIZE - 1)
#define ROW_2_START_IDX (ROW_2_CTR_POS - ((NUM_GLOBAL_PARAMS / 2) * LEDS_PER_SLCTR) - 1)

#define BLINK_SHIFT         9
#define DFLT_HUE              0
#define LEDS_PER_SLCTR      2
#define DIM_LEVEL         128

void Visual_UI::init(parameter_t * globParams, int mostParams){
    for(int i = 0; i< NUM_GLOBAL_PARAMS; i++){
        for(int j = 0; j< LEDS_PER_SLCTR; j++){
            int idx = (i*LEDS_PER_SLCTR) + j;
            globColors[idx] = globParams[i].scaleColor;
        }
    }
    paramColors = new CRGB[LEDS_PER_SLCTR * mostParams];
};

void Visual_UI::setAnimation(){
    for(int i = 0; i < if_ui.numAnimParams; i++){
        CRGB c = if_ui.currParam->scaleColor;
        if(!(c.r || c.b ||c.g)){
            c = CHSV(DFLT_HUE + (i* 80), 255,255);
        }
        for(int j = 0; j< LEDS_PER_SLCTR; j++){
            int idx = (i*LEDS_PER_SLCTR) + j;
            paramColors[idx] = c;
        }        
    }
    row3startIdx = (ROW_3_CTR - ((if_ui.numAnimParams / 2) * LEDS_PER_SLCTR)) - 1;
}

void Visual_UI::setParameter(){
    int pickerIdx = if_ui.paramIdx * LEDS_PER_SLCTR;
    if(if_ui.edittingGlobalParams){
        currParamLedIdx = ROW_2_START_IDX + (pickerIdx);
        currColor = globColors[pickerIdx];
    } else {
        currParamLedIdx = row3startIdx + (pickerIdx);
        currColor = paramColors[pickerIdx];
    }
    brightnessPointsPerValue = (SCALE_FULL_SIZE * 256) / (if_ui.currParam->max + (if_ui.currParam->type==CHUNKS?1:0));
}

bool newValue;void Visual_UI::setValue(int val){
    pValue = val;
    if(val > if_ui.currParam->max || val < -if_ui.currParam->max) {
        pValue = 0;
    }
    
    // Make the scale background white, unless the param color is white, then make it black.
    scale =
        (currColor.r == 0xff && currColor.g == 0xff &&currColor.b == 0xff)?
        CRGB::Black : CRGB::White;

    switch(if_ui.currParam->type){
        case CHUNKS:
        {
            int numLeds = brightnessPointsPerValue / 256;
            scale((pValue*numLeds), ((pValue + 1) * numLeds))  = currColor;
            break;
        }
        case BOOL:
            if(pValue){
                scale(SCALE_HALF_SIZE, SCALE_FULL_SIZE - 1)  = currColor;
            } else {
                scale(0, SCALE_HALF_SIZE-1)  = currColor;
            }
            break;
        case OTHER:
            if(pValue != 0){
                if(abs(pValue) == if_ui.currParam->max){
                    scale = currColor;
                } else {  
                    int brightPoints = pValue * brightnessPointsPerValue;
                    int numFullLeds = brightPoints / 256;
                    int16_t remainingBrightness = brightPoints % 256;
                    if(remainingBrightness < 0) remainingBrightness = -remainingBrightness;
                    uint8_t beginning, end, partial;
                    
                    if(pValue > 0){
                        beginning = 0;
                        end = numFullLeds - 1;
                        partial = end + 1;
                    } else {
                        beginning = SCALE_FULL_SIZE + numFullLeds;
                        end = SCALE_FULL_SIZE - 1;
                        partial = beginning - 1;
                    }

                    if(end >= beginning){
                        scale(beginning, end)  = currColor;
                    }

                    scale[partial] = scale[partial].lerp8(currColor, remainingBrightness);
                }
            }
            break;
            default: break;
    }        
}

void Visual_UI::drawForAdjParam(bool blink){
    switch(if_ui.currParam->type){
        case HUE:
            for(int i = 0; i <= 86; i++){
                uint8_t hueVal = radii[i][ANGLE];
                bool close = abs(hueVal - pValue) <= 6;
                scale[i] = CHSV(hueVal, (close && blink)? 0 : 255, 255);
            }
            break;
        default:
            memcpy(&ledData.leds[SCALE_START_IDX], scale, sizeof(CRGB)*SCALE_FULL_SIZE);
            if(blink){
                ledData.leds[SCALE_START_IDX - 1] = CRGB::Black;
                ledData.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Black;
            } else {
                ledData.leds[SCALE_START_IDX - 1] = CRGB::Red;
                ledData.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Red;
            }
            break;
    }
}

void Visual_UI::drawForSelectParam(bool blink){
    ledData.leds.fadeLightBy(DIM_LEVEL);
    
    memcpy(&ledData.leds[ROW_2_START_IDX], globColors, sizeof(CRGB)*LEDS_PER_SLCTR*NUM_GLOBAL_PARAMS);
    memcpy(&ledData.leds[row3startIdx], paramColors, sizeof(CRGB)*LEDS_PER_SLCTR*if_ui.numAnimParams);

    if(blink){
        ledData.leds(currParamLedIdx, currParamLedIdx -1 + LEDS_PER_SLCTR) = CRGB::Black;
    }

    drawForAdjParam(false);
}
    
void Visual_UI::draw(){
    bool blink = millis() & (1<<BLINK_SHIFT);
    if(if_ui.selectingParams){
        drawForSelectParam(blink);
    } else {
        drawForAdjParam(blink);
    }
}

#include "../Animation.h"
#include "util.h"

struct Rainbow : public AnimationBase{

    #define SINGLE_HUE_CYCLE 16
    #define MAX_STRETCH (SINGLE_HUE_CYCLE * 48)


    enum ParamName {
        STRETCH,
    };

    // params vars
    uint16_t stretch = SINGLE_HUE_CYCLE;

    // state vars
    uint8_t currHue = 0;
    int currTime = 0;
    int millisInFullCycle = 5000 * SPEED_SCALE_BASE;

    public:
    Rainbow(){
        numParams = 1;
        params = new parameter_t[numParams];
        params[STRETCH].max = MAX_STRETCH;
        //params[STRETCH].ticksToAdjust = 2;
        params[STRETCH].scaleColor = CRGB::DarkBlue;
    };

    void initAnim(){
    }

    void initParam(uint8_t paramIdx){
        drawScale.init(&params[paramIdx]);
        switch (paramIdx)
        {
            case STRETCH:
                drawScale.setValue(stretch);
                break;
        
            default:
                break;
        }
    }

    void adjParam(uint8_t paramIdx, bool up){
        int adj = (INCDEC * ledData.fast_scroll_ctr) >> 1;
        switch(paramIdx){
            case STRETCH:
                stretch = CLAMP_UN_0(stretch + adj, MAX_STRETCH);
                drawScale.setValue(stretch);
                break;
        }
    }


    void drawFrame(int16_t scaledTimeSinceLastFrame){
        currTime += scaledTimeSinceLastFrame;
        if(currTime > millisInFullCycle) currTime -= millisInFullCycle;
        uint32_t hue16 = ((2<<8) * currTime) / (millisInFullCycle >> 7);

        for(int i = 0; i<NUM_LEDS;i++){
            int hueDelta = ((i * stretch) << 8) / SINGLE_HUE_CYCLE;
            int hue = (hue16 + hueDelta) >> 8;
            ledData.leds[i] = CHSV(hue, ledData.saturation, 255);
        }
    }
};

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

    int adjParam(uint8_t paramIdx, int change){
        switch(paramIdx){
            case STRETCH:
                stretch = clamp_un0(stretch + change, MAX_STRETCH);
                return stretch;
                
            default: return 0;
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
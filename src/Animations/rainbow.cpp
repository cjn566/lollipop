
#include "../Animation.h"
#include "util.h"

struct Rainbow : public AnimationBase{

    #define MAX_STRETCH 8
    enum ParamName {
        STRETCH,
    };

    // params vars
    int8_t stretch = 3;

    // state vars
    uint8_t currHue = 0;
    int currTime = 0;
    int millisInFullCycle = 3000;

    public:
    Rainbow(){
        numParams = 1;
        params = new parameter_t[numParams];
        params[STRETCH] = {CRGB::Red, 4};
    };

    void initAnim(){
    }

    void initParam(uint8_t paramIdx){
        switch(paramIdx){
            case STRETCH:
                drawScale.init(true, MAX_STRETCH, stretch);
                break;
        }
    }

    void adjParam(uint8_t paramIdx, bool up){
        switch(paramIdx){
            case STRETCH:
                stretch = CLAMP_SN(stretch + INCDEC, MAX_STRETCH);
                drawScale.setValue(stretch);
                break;
        }
    }


    void drawFrame(uint8_t millisSinceLastFrame){
        currTime += millisSinceLastFrame;
        if(currTime > millisInFullCycle) currTime -= millisInFullCycle;

        currHue = ((256 * currTime) / millisInFullCycle);


            fill_rainbow(ledData.leds, NUM_LEDS, currHue, stretch);

            // From Gradient:
            // CHSV first =  CHSV(baseHue + currHue, $.saturation, 255);
            // CHSV second = CHSV(baseHue + currHue + stretch, $.saturation, 255);
            // fill_gradient<CRGB>($.leds, (uint16_t)NUM_LEDS, first, second, FORWARD_HUES);
    }
};
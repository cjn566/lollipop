
#include "../Animation.h"
#include "util.h"

struct Rainbow : public AnimationBase{

    #define MAX_STRETCH 8
    #define MILLIS_IN_HUE_CYCLE 4095
    enum ParamName {
        STRETCH,
    };

    // params vars
    uint8_t stretch = 3;

    // state vars
    uint16_t currHue = 0;

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
                break;
        }
    }

    void adjParam(uint8_t paramIdx, bool up){
        switch(paramIdx){
            case STRETCH:
                stretch = CLAMP_SN(stretch + INCDEC, MAX_STRETCH);
                break;
        }
    }

    void drawFrame(uint8_t millisSinceLastFrame){
            currHue += millisSinceLastFrame;
            if(currHue > MILLIS_IN_HUE_CYCLE){
              currHue -= MILLIS_IN_HUE_CYCLE;
            }
            uint8_t hue8 = currHue >> 4;

            fill_rainbow(ledData.leds, NUM_LEDS, hue8, stretch);

            // From Gradient:
            // CHSV first =  CHSV(baseHue + currHue, $.saturation, 255);
            // CHSV second = CHSV(baseHue + currHue + stretch, $.saturation, 255);
            // fill_gradient<CRGB>($.leds, (uint16_t)NUM_LEDS, first, second, FORWARD_HUES);
    }
};
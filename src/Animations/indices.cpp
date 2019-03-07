
#include "../Animation.h"
#include "util.h"

struct Indices : public AnimationBase{

    enum ParamName {
        INDEX,
    };

    // params vars
    uint8_t index = 0;

    public:
    Indices(){
        numParams = 1;
        params = new parameter_t[numParams];
        params[INDEX] = {CRGB::Red, 2};
    };

    void adjParam(uint8_t paramIdx, bool up){
        switch(paramIdx){
            case INDEX:
                index += INCDEC;
                #ifdef DEBUG
                Serial.printf("  %d--%d:%d--%d\n", index, index + 7, index + 8, index + 15);
                #endif
                break;
        }
    }

    void drawFrame(uint8_t stepsSinceLastFrame){
        FastLED.clear();
        ledData.leds[index] = CRGB::White;
    }
};
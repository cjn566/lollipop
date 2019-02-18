
    #include "FastLED.h"
    #include "util.h"
    
    #define NUM_MOST_PARAMS     3
    #define NUM_LEDS            204
    #define ORDER               GRB

    struct {
        uint8_t anim = 0;
        uint8_t stepsSinceLastFrame = 0;
        uint8_t saturation = 255;    
        CRGBArray<NUM_LEDS> leds;
    } $;

    struct Parameter {
        CRGB backgroundColor;
        uint8_t ticksToAdjust;
    };


    struct AnimationBase {
        AnimationBase();
        uint8_t NUM_PARAMS;
        Parameter * params;
        virtual void initAnim() =0;
        virtual void initParam(uint8_t p) =0;
        virtual void adjParam(uint8_t param, bool up) =0;
        virtual void drawFrame() =0;
    };

    static struct Peppermint : public AnimationBase{
        virtual void initAnim();
        virtual void initParam(uint8_t p);
        virtual void adjParam(uint8_t param, bool up);
        virtual void drawFrame();
    } peppermint;

    // Then I need some sort of array that would include peppermint and all the others..
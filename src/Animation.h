#ifndef ANIMATION_H
    #define ANIMATION_H
    
    #include "FastLED.h"

    #define NUM_LEDS            204
    #define ORDER               GRB
    

    typedef struct state_struct      state_t;
    typedef struct parameter_struct  parameter_t;

    struct state_struct {
        uint8_t saturation = 255;    
        CRGBArray<NUM_LEDS> leds;
    };

    extern state_t ledData;

    struct parameter_struct {
        CRGB backgroundColor;
        uint8_t ticksToAdjust;
    };

    struct AnimationBase {
        uint8_t numParams;
        parameter_t *params;
        AnimationBase(){};
        virtual uint8_t     getNumParams(){return numParams;};
        virtual parameter_t getParam(uint8_t paramIdx){return params[paramIdx];};
        virtual void        initAnim(){};
        virtual void        initParam(uint8_t paramIdx){};
        virtual void        adjParam(uint8_t paramIdx, bool up)     =0;
        virtual void        drawFrame(uint8_t)                      =0;
    };


    class DrawScale {
        public:
        void setValue(int);
        void init(bool isActive, int nMax = 256, int val = 0, CRGB = CRGB::Red);        
        void draw();
    };

    extern DrawScale drawScale;

#endif
#ifndef ANIMATION_H
    #define ANIMATION_H
    
    #include "FastLED.h"
    #include "util.h"
    #include "settings.h"

    #define NUM_LEDS            204
    #define ORDER               GRB
    
    struct state_t {
        uint8_t saturation = 255;
        CRGBArray<NUM_LEDS> leds;
    };

    extern state_t ledData;

    struct parameter_t {
        CRGB scaleColor = CRGB::Green;
        uint32_t max = 256;
        uint8_t ticksToAdjust = 1;
    };

    

    class DrawScale {
        public:
        void setValue(int);
        void turnoff();
        void init(uint32_t nMax = 256, CRGB = CRGB::Red);
        void init(parameter_t * param_ptr);
        void draw();
    };

    extern DrawScale drawScale;

    struct AnimationBase {
        int8_t speed = SPEED_SCALE_BASE;
        uint8_t numParams;
        parameter_t *params;
        AnimationBase(){};
        virtual uint8_t     getNumParams(){return numParams;};
        virtual parameter_t getParam(uint8_t paramIdx){return params[paramIdx];};
        virtual void        initAnim(){};
        virtual void        initParam(uint8_t paramIdx){};
        virtual void        adjParam(uint8_t paramIdx, bool up){};
        virtual void        drawFrame(int16_t){};

        void                adjSpeed(bool up){
            speed  = CLAMP_8(speed + INCDEC);
            drawScale.setValue(speed);
        }
        
        void                drawBase(uint16_t millisSinceLastFrame){
            // get modified time passed
            int16_t modTime = millisSinceLastFrame * speed;
            drawFrame(modTime);
        }
    };


#endif
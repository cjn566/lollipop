#ifndef ANIMATION_H
    #define ANIMATION_H
    
    #include "FastLED.h"
    #include "util.h"
    #include "settings.h"

    #define NUM_LEDS            204
    #define ORDER               GRB
    
    struct state_t {
        uint8_t saturation = 255;
        uint8_t fast_scroll_ctr = 2;
        CRGBArray<NUM_LEDS> leds;
    };

    extern state_t ledData;

    struct parameter_t {
        CRGB scaleColor = CRGB::Black;
        int max = 0;
        uint8_t ticksToAdjust = 1;
    };
  
    struct AnimationBase {
        int8_t speed = SPEED_SCALE_BASE;
        uint8_t numParams;
        parameter_t *params;
        AnimationBase(){};
        virtual void        initAnim(){};
        virtual uint8_t     getNumParams(){return numParams;};
        virtual parameter_t getParam(uint8_t paramIdx){return params[paramIdx];};
        virtual int         adjParam(uint8_t paramIdx, int change);
        virtual void        drawFrame(int16_t){};
        uint8_t             adjSpeed(int adj){
            speed  = CLAMP_S8(speed + adj);
            return speed;
        }
        void                drawBase(uint16_t millisSinceLastFrame){
            // get modified time passed
            int16_t modTime = millisSinceLastFrame * speed;
            drawFrame(modTime);
        }
    };

    
    class DrawScale {
        public:
        DrawScale(){};
        void init(parameter_t * globParams);
        void setAnimation(AnimationBase *animationArg);
        void setParameter(bool isGlobal, uint8_t paramIdx);
        void setValue(int);
        void draw();
    };


#endif
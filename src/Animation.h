#ifndef ANIMATION_H
    #define ANIMATION_H
    
    #include "FastLED.h"
    #include "util.h"
    #include "settings.h"

    struct AnimationBase;

    struct if_anim_t {
        uint8_t saturation = 255;
        CRGBArray<NUM_LEDS> leds;
        AnimationBase * animation;
    };    

    enum DispType {
        CHUNKS,
        BOOL,
        HUE,
        OTHER
    };

    struct parameter_t {
        CRGB scaleColor = CRGB::Black;
        int max = 255;
        uint8_t ticksToAdjust = 1;
        DispType type = OTHER;
    };

    struct if_ui_t {
        bool edittingGlobalParams = true, selectingParams = true;
        parameter_t * currParam;
        uint8_t paramIdx, numAnimParams;
    };

    extern if_anim_t ledData;
    extern if_ui_t if_ui;


    struct AnimationBase {
        int8_t speed = SPEED_SCALE_BASE;
        uint8_t numParams;
        parameter_t *params;
        AnimationBase(){};
        virtual void        initAnim(){};
        virtual uint8_t     getNumParams(){return numParams;};
        virtual parameter_t * getParam(uint8_t paramIdx){return &params[paramIdx];};
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

#endif
#ifndef ANIMATION_H
    #define ANIMATION_H

    #include "FastLED.h"
    #include "util.h"
    
    #define NUM_MOST_PARAMS     3
    #define NUM_LEDS            204
    #define ORDER               GRB
    
    #include "radii.h"
    

    // Global LED data
    struct {
        uint8_t anim = 0;
        uint8_t stepsSinceLastFrame = 0;
        uint8_t saturation = 255;    
        CRGBArray<NUM_LEDS> leds;
    } $;

    // Definition of a custom parameter for an animation
    struct Parameter {
        CRGB backgroundColor;
        uint8_t ticksToAdjust;
    };

    // Definition of the struct that would let the main program refer to each animation's data/functions
    struct Animation {
        uint8_t numParams;
        Parameter *paramList;
        void (*initAnim)();
        void (*initParam)(uint8_t);
        void (*adjParam)(uint8_t, bool);
        void (*drawFrame)();
        Animation(int np, Parameter *pl, void (*init)(), void (*ip)(uint8_t), void (*ap)(uint8_t, bool), void (*df)()) : 
            numParams(np),
            paramList(pl),
            initParam(ip),
            adjParam(ap),
            initAnim(init), 
            drawFrame(df) 
            {}
    };


    // ------------------ Full Gradient ----------------  <-- An example animation
    namespace gradient {
        const uint8_t NUM_PARAMS = 2;
        // Params Vars
        uint8_t stretch = 1;
        uint8_t baseHue = 0;
        
        Parameter params[NUM_PARAMS] = {
            Parameter{CRGB::DarkGray, 2},   // Stretch
            Parameter{CRGB::DarkGray, 4},   // Hue
        };

        // Step Vars
        uint8_t currHue = 0;

        void initAnim(){
        }

        void initParam(uint8_t p){ // Called when a new parameter is selected for adjustment
            switch(p){
                case 0: // Stretch
                    
                    break;
                case 1: // Hue

                    break;
            }
        }

        void adjParam(uint8_t param, bool up){ // Called each time a parameter is adjusted
            switch(param){
                case 0:
                    stretch = CLAMP_8(stretch + INCDEC);

                    #ifdef DEBUG
                    Serial.printf("Stretch: %d\n", (uint8_t)stretch);
                    #endif
                    break;
                case 1:
                    baseHue += INCDEC;
                    break;
            }
        }

        void drawFrame(){
            FastLED.clear();
            currHue += $.stepsSinceLastFrame;
            CHSV first =  CHSV(baseHue + currHue, $.saturation, 255);
            CHSV second = CHSV(baseHue + currHue + stretch, $.saturation, 255);
            fill_gradient<CRGB>($.leds, (uint16_t)NUM_LEDS, first, second, FORWARD_HUES);
        }
    }


    // This is the array of animations that the main program cycles through
    // ------------------ COLLECTION ----------------
    #define NUM_ANIMS           1
    #define DECLARE_ANIM        Animation(_ANIM_NAME_::NUM_PARAMS, _ANIM_NAME_::params, _ANIM_NAME_::initAnim, _ANIM_NAME_::initParam, _ANIM_NAME_::adjParam, _ANIM_NAME_::drawFrame),
    Animation animations[NUM_ANIMS] = {
        #define _ANIM_NAME_ gradient
        DECLARE_ANIM
    };


#endif
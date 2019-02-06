#ifndef ANIMATION_H
    #define ANIMATION_H

    #include "FastLED.h"
    #include "util.h"
    
    #define NUM_ANIMS           2
    #define OUTER_LEDS          180
    #define CENTER_LEDS         27
    #define NUM_LEDS            (OUTER_LEDS + CENTER_LEDS)
    #define ORDER               GRB
    

    struct {
        uint8_t anim = 0;
        uint8_t stepsSinceLastFrame = 0;

        CRGBArray<NUM_LEDS> leds;

    } $;

    // TODO: this train of thought..
    struct Parameter {
        CRGB backgroundColor;
        uint8_t ticksToAdjust;
    };

    struct Animation {
        uint8_t numParams;
        Parameter *paramList;
        void (*init)();
        void (*adjParam)(uint8_t, bool);
        void (*drawFrame)();
        Animation(int np, Parameter *pl, void (*init)(), void (*ap)(uint8_t, bool), void (*df)()) : 
            numParams(np),
            paramList(pl),
            init(init), 
            adjParam(ap),
            drawFrame(df) 
            {}
    };


    // ------------------ SOLID ----------------
    namespace solid {
        const uint8_t PARAMS = 1;
        uint8_t hue = 0;

        Parameter params[PARAMS] = {
            Parameter{CRGB::Fuchsia, 1} // hue
        };
        void init(){
        }
        void adjParam(uint8_t param, bool up){
            switch(param){
                case 0:
                    hue += INCDEC;
                    break;
            }
        }
        void drawFrame(){
            $.leds(0, NUM_LEDS - 1) = CHSV(hue, 255,255);
        }
    }

    // ------------------ RAINBOW ----------------
    namespace rainbow {
        const uint8_t PARAMS = 2;
        // Params Vars
        uint8_t baseHue = 0;
        uint8_t stretch = NUM_LEDS;
        
        Parameter params[PARAMS] = {
            Parameter{CRGB::Crimson, 1}, // Hue
            Parameter{CRGB::DarkGray, 8} // Stretch
        };

        // Step Vars
        uint8_t currHue = 0;


        void init(){
        }

        void adjParam(uint8_t param, bool up){
            switch(param){
                case 0:
                    baseHue += INCDEC;
                    break;
                case 1:
                    stretch += INCDEC;
                    break;
            }
        }

        void drawFrame(){
            FastLED.clear();
            currHue += $.stepsSinceLastFrame;
            fill_rainbow($.leds, NUM_LEDS, baseHue + currHue, stretch);
        }
    }

    // ------------------ PEPPERMINT ----------------
    namespace peppermint{
        const uint8_t PARAMS = 2;
        uint8_t baseHue = 0;
        uint8_t currHue = 0;
        uint8_t numSpokes = 6;

        Parameter params[PARAMS] = {
            Parameter(),
            Parameter()
        };

        void init(){
        }

        void adjParam(uint8_t param, bool up){
            switch(param){
                case 0:
                    baseHue += INCDEC;
                    break;
                case 1:
                    numSpokes += INCDEC;
                    break;
            }
        }

        void drawFrame(){
            FastLED.clear();
            currHue += $.stepsSinceLastFrame;
            $.leds(OUTER_LEDS, NUM_LEDS).fill_rainbow(baseHue + currHue);
        }
    }

    
    
    

    // ------------------ COLLECTION ----------------
    Animation animations[NUM_ANIMS] = {
        // #define _ANIM_NAME_ peppermint
        // Animation(_ANIM_NAME_::PARAMS, _ANIM_NAME_::params, _ANIM_NAME_::init, _ANIM_NAME_::adjParam, _ANIM_NAME_::drawFrame),
        // #undef _ANIM_NAME_
        #define _ANIM_NAME_ rainbow
        Animation(_ANIM_NAME_::PARAMS, _ANIM_NAME_::params, _ANIM_NAME_::init, _ANIM_NAME_::adjParam, _ANIM_NAME_::drawFrame),
        #undef _ANIM_NAME_
        #define _ANIM_NAME_ solid
        Animation(_ANIM_NAME_::PARAMS, _ANIM_NAME_::params, _ANIM_NAME_::init, _ANIM_NAME_::adjParam, _ANIM_NAME_::drawFrame)
    };


#endif

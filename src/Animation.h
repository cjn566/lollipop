#ifndef ANIMATION_H
    #define ANIMATION_H

    #include "FastLED.h"
    
    #define NUM_ANIMS           1
    #define NUM_LEDS            185
    #define ORDER               GRB


    struct {
        uint8_t anim = 0;
        uint8_t stepsSinceLastFrame = 0;

        CRGBArray<NUM_LEDS> leds;

    } $;

    struct Animation {
        uint8_t numParams;
        void (*init)();
        void (*adjParam)(uint8_t, int8_t);
        void (*drawFrame)();
        Animation(int np, void (*init)(), void (*ap)(uint8_t, int8_t), void (*df)()) : 
            numParams(np),
            init(init), 
            adjParam(ap),
            drawFrame(df) 
            {}
    };


    // ------------------ SOLID ----------------

    #define NUM_PARAMS_SOLID 1

    struct {
        uint8_t hue = 0;
    } data_solid;

    void init_solid(){
    }
    void adjParam_solid(uint8_t param, int8_t delta){
        switch(param){
            case 0:
                data_solid.hue += delta;
                break;
        }
    }
    void drawFrame_solid(){
        $.leds(0, NUM_LEDS) = CHSV(data_solid.hue, 255,255);
    }


    // ------------------ RAINBOW ----------------
    // #define RAINBOW_TOP 255
    // void rainbowInit(){
    //     $.clear();

    // }
    // void rainbowDraw(){
    //     FastLED.clear();
    //     $.u8_1 += $.stepsSinceLastFrame;
    //     fill_rainbow($.leds, NUM_LEDS, $.hue + $.u8_1, 255);
    // }

    // ------------------ COLLECTION ----------------
    Animation animations[NUM_ANIMS] = {
        Animation(NUM_PARAMS_SOLID, init_solid, adjParam_solid, drawFrame_solid), 
        //Animation(rainbowInit, rainbowDraw)
    };


#endif

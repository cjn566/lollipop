#ifndef ANIMATION_H
    #define ANIMATION_H

    #include "FastLED.h"
    
    #define NUM_ANIMS           2
    #define NUM_LEDS            180
    #define ORDER               GRB

    struct {
        uint8_t anim = 0;
        uint8_t hue = 0;
        uint8_t stepsSinceLastFrame = 0;
        CRGBArray<NUM_LEDS> leds;

        uint8_t u8_1, u8_2;
        int optInt;
        long optLong;
        float optFloat;

        void clear(){
            u8_1 = 0;
            u8_2 = 0;
            optInt = 0;
            optLong = 0;
            optFloat = 0;
        }
    } $;

    struct Animation {
        void (*init)();
        void (*drawFrame)();
        Animation(void (*init)(), void (*df)()) : 
            init(init), 
            drawFrame(df) 
            {}
    };


    // ------------------ SOLID ----------------
    void testInit(){
        $.clear();
    }
    void testDrawFrame(){
        FastLED.showColor(CRGB::White);
    }


    // ------------------ RAINBOW ----------------
    #define RAINBOW_TOP 255
    void rainbowInit(){
        $.clear();

    }
    void rainbowDraw(){
        FastLED.clear();
        $.u8_1 += $.stepsSinceLastFrame;
        fill_rainbow($.leds, NUM_LEDS, $.hue + $.u8_1, 255);
    }

    // ------------------ COLLECTION ----------------
    Animation animations[NUM_ANIMS] = {
        Animation(testInit, testDrawFrame), 
        Animation(rainbowInit, rainbowDraw)
    };


#endif

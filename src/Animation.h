#ifndef ANIMATION_H
    #define ANIMATION_H

    #include "FastLED.h"
    
    #define NUM_ANIMS 1
    #define NUM_LEDS            200

    struct Animation
    {
        unsigned int steps;
        void (*render)(CRGB*, uint8_t);
    };

    void bob(CRGB* leds, uint8_t hue){
    
        for(int i = 0; i < 7; i++){
            leds[i] = CHSV(hue + 8*i, 255, 255);
        }
    };


    Animation animations[NUM_ANIMS] =  
    {
        50, bob
    };


#endif
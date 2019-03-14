
#ifndef SETTINGS_H
    #define SETTINGS_H

    //#define DEBUG
    

    // Pins
    #define ENCODER_A     23
    #define ENCODER_B     22
    #define ENCODER_BTN   4
    #define LED_DATA      17

    // UI Timing
    #define DEBOUNCE_MILLIS       50
    #define EDIT_HOLD_MILLIS      700
    
    #ifdef DEBUG
        #define EDIT_TIMEOUT_MILLIS   10000
    #else
        #define EDIT_TIMEOUT_MILLIS   4000
    #endif

    #define BLINK_MILLIS          500
    #define FAST_SCROLL_MS        60
    #define FAST_SCROLL_RESET     700
    #define FAST_SCROLL_MAX       20

    // parameters
    #define NUM_GLOBAL_PARAMS 3
    #define INIT_BRIGHTNESS   40

    #ifdef DEBUG
        #define SPEED_SCALE_BASE                 80
    #else
        #define SPEED_SCALE_BASE                 80
    #endif

    #define BRIGH_ADJ_MULT    3
    #define SPEED_ADJ_MULT    3
    #define SPEED_REDUCTION_FACTOR    2
    #define BRIGHT_MACRO_ADJ_THRESH    50


    // Video
    #ifdef DEBUG
        #define FPS                 20
    #else
        #define FPS                 150
    #endif

    #define ANIM_STEPPER_FREQ   100

    // Misc
    #define MAX_MILLIAMPS          1500
    #define STARTUP_DELAY          500
    #define ENC_TICKS_PER_INDENT   4

    

    #define NUM_LEDS            204
    #define ORDER               GRB

#endif


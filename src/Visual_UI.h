#ifndef VISUAL_UI_H
    #define VISUAL_UI_H
    #include "Animation.h"

    #define LEDS_PER_SLCTR      2
    #define SCALE_HALF_SIZE     8
    #define SCALE_FULL_SIZE (SCALE_HALF_SIZE*2)

    class Visual_UI{
        int pValue;
        uint16_t brightnessPointsPerValue;
        uint8_t currParamLedIdx, row3startIdx;
        parameter_t * globParams;
        CRGB globColors[NUM_GLOBAL_PARAMS * LEDS_PER_SLCTR];
        CRGB * paramColors;
        CRGBArray<SCALE_FULL_SIZE> scale;
        CRGB currColor;
        void drawForSelectParam(bool blink);
        void drawForAdjParam(bool blink);
        public:
        Visual_UI(){};
        void init(parameter_t * globParams, int mostParams);
        void setAnimation();
        void setParameter();
        void setValue(int);
        void draw();
    };

#endif
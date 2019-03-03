#ifndef PEPPERMINT
    #define PEPPERMINT

    #include "../Animation.h"
    #include "radii.h"
    #include "util.h"

    struct Peppermint: public AnimationBase{

        #define MAX_SPOKES 5
        #define MAX_SKEW 5
        enum ParamName {
            SPOKE,
            D_HUE,
            SKEW
        };

        // params vars
        uint8_t deltaHue = 3;
        uint8_t numSpokes = 3;
        int8_t skew = 0;

        // state vars
        uint8_t currAngle = 0;
        uint8_t halfAngleBetweenSpokes;
        uint8_t angleBetweenSpokes;

        public:
        Peppermint(){
            numParams = 3;
            params = new parameter_t[numParams];
            params[SPOKE] = {CRGB::Red, 4};
            params[D_HUE] = {CRGB::Red, 8};
            params[SKEW] = {CRGB::Red, 8};
        };

        void initAnim(){
            halfAngleBetweenSpokes = (256 / (numSpokes * 2));
            angleBetweenSpokes = halfAngleBetweenSpokes * 2;
            if(!angleBetweenSpokes) angleBetweenSpokes -= 1;
            #ifdef DEBUG
            Serial.printf("spokes: %d, angle: %d\n", numSpokes, angleBetweenSpokes);
            #endif
        }

        void initParam(uint8_t paramIdx){
            switch(paramIdx){
                case 0:
                    break;
            }
        }

        void adjParam(uint8_t paramIdx, bool up){
            switch(paramIdx){
                case SPOKE:
                    numSpokes += INCDEC;
                    if(!numSpokes) numSpokes = MAX_SPOKES;
                    else if(numSpokes > MAX_SPOKES) numSpokes = 1;
                    initAnim();
                    break;
                case D_HUE:
                    deltaHue += INCDEC;
                    if(!deltaHue) deltaHue = numSpokes;
                    else if(numSpokes > numSpokes) deltaHue = 1;
                    break;
                case SKEW:
                    skew = CLAMP_SN(skew + INCDEC, MAX_SKEW);
                    #ifdef DEBUG
                    Serial.printf("Skew: %d\n", skew);
                    #endif
                    break;
            }
        }

        void drawFrame(uint8_t millisSinceLastFrame){

            #ifdef DEBUG
            //Serial.println();
            #endif

            currAngle = mod8(currAngle + millisSinceLastFrame, angleBetweenSpokes);

            for(int i=0;i< NUM_LEDS ;i++){
                uint8_t anglePlusRotation = mod8(sub8(radii[i][ANGLE], currAngle), angleBetweenSpokes);
                if(anglePlusRotation > halfAngleBetweenSpokes){
                    anglePlusRotation = angleBetweenSpokes - anglePlusRotation;
                }
                uint8_t angleWithFullHueMultplier = anglePlusRotation * deltaHue;


                int top = (int)skew * i;
                int adjustment = top >> 5;

                #ifdef DEBUG
                if(millisSinceLastFrame == 1 && i == 120){
                    //Serial.printf("skew: %d, %d\n", top, adjustment);
                }
                #endif
                uint8_t withSkew = angleWithFullHueMultplier + adjustment;


                
                ledData.leds[i] = CHSV(withSkew, ledData.saturation, 255);
            }
        }
    };
#endif
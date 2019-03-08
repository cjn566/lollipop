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
        uint8_t deltaHue;
        uint8_t numSpokes = 2;
        int8_t skew = 0;

        // state vars
        uint8_t currAngle = 0;
        uint8_t halfAngleBetweenSpokes;
        uint8_t angleBetweenSpokes;
        int millisInFullRotation = 4000 * SPEED_SCALE_BASE;
        int millisInFractionalRotation;

        public:
        Peppermint(){
            numParams = 3;
            params = new parameter_t[numParams];
            params[SPOKE].max = MAX_SPOKES;
            params[SPOKE].ticksToAdjust = 2;
            params[SPOKE].scaleColor = CRGB::OrangeRed;

            params[D_HUE].max = MAX_SPOKES;
            params[D_HUE].ticksToAdjust = 2;
            params[D_HUE].scaleColor = CRGB::PaleVioletRed;

            params[SKEW].max = MAX_SKEW;
            params[SKEW].ticksToAdjust = 2;
            params[SKEW].scaleColor = CRGB::Purple;
        };

        void initAnim(){
            millisInFractionalRotation = millisInFullRotation / numSpokes;
            halfAngleBetweenSpokes = (128 / numSpokes);
            angleBetweenSpokes = halfAngleBetweenSpokes * 2;
            if(!angleBetweenSpokes) angleBetweenSpokes -= 1;
            deltaHue = numSpokes;
            #ifdef DEBUG
            Serial.printf("spokes: %d, angle: %d\n", numSpokes, angleBetweenSpokes);
            #endif
        }

        void initParam(uint8_t paramIdx){
            switch(paramIdx){
                case SPOKE:
                    drawScale.init(&params[paramIdx]);
                    drawScale.setValue(numSpokes);
                    break;
                case D_HUE:
                    drawScale.init(numSpokes, params[paramIdx].scaleColor);
                    drawScale.setValue(deltaHue);
                    break;
                case SKEW:
                    drawScale.init(&params[paramIdx]);
                    drawScale.setValue(skew);
                    break;
            }
        }

        void adjParam(uint8_t paramIdx, bool up){
            switch(paramIdx){
                case SPOKE:
                    numSpokes = CLAMP_UN_1(numSpokes + INCDEC, MAX_SPOKES);
                    drawScale.setValue(numSpokes);
                    initAnim();
                    break;
                case D_HUE:
                    deltaHue = CLAMP_UN_1(deltaHue + INCDEC, numSpokes);
                    drawScale.setValue(deltaHue);
                    break;
                case SKEW:
                    skew = CLAMP_SN(skew + INCDEC, MAX_SKEW);
                    drawScale.setValue(skew);
                    #ifdef DEBUG
                    Serial.printf("Skew: %d\n", skew);
                    #endif
                    break;
            }
        }

        int curMillis = 0;

        void drawFrame(int16_t scaledTimeSinceLastFrame){
            curMillis += scaledTimeSinceLastFrame;
            if(curMillis > millisInFractionalRotation) curMillis -= millisInFractionalRotation;            
            currAngle = SCALE32_TO_8(curMillis, millisInFullRotation);

            for(int i=0;i< NUM_LEDS ;i++){
                uint8_t anglePlusRotation = mod8(sub8(radii[i][ANGLE], currAngle), angleBetweenSpokes);


                if(anglePlusRotation > halfAngleBetweenSpokes){
                    anglePlusRotation = angleBetweenSpokes - anglePlusRotation;
                    anglePlusRotation += ((skew * i)/8);
                } else {
                    anglePlusRotation -= ((skew * i)/8);
                }


                anglePlusRotation = anglePlusRotation * deltaHue;
                ledData.leds[i] = CHSV(anglePlusRotation, ledData.saturation, 255);
            }
        }
    };
#endif

    #include "FastLED.h"
    #include "util.h"
 
    #define NUM_ANIMS           3
   
    #define NUM_MOST_PARAMS     3
    #define NUM_LEDS            204
    #define ORDER               GRB

    typedef struct state_struct      state_t;
    typedef struct parameter_struct  parameter_t;

    // this state stuff probably wants to be confined to only main.c
    // not in this header
    struct state_struct {
        uint8_t anim = 0;
        uint8_t stepsSinceLastFrame = 0;
        uint8_t saturation = 255;    
        CRGBArray<NUM_LEDS> leds;
    };

    // only 1 instance is created, in main.cpp.  This lets every other file access it
    extern state_t globalState;

    struct parameter_struct {
        CRGB backgroundColor;
        uint8_t ticksToAdjust;
    };

    class AnimationBase {
      public:
        //uint8_t NUM_PARAMS;
        //parameter_t * params;
        virtual void initAnim() =0;
        virtual void initParam(uint8_t p) =0;
        virtual void adjParam(uint8_t param, bool up) =0;
        virtual void drawFrame() =0;
    };

    class Peppermint : public AnimationBase {
      public:
	Peppermint(uint8_t params, uint8_t spokes);
        virtual void initAnim();
        virtual void initParam(uint8_t p);
        virtual void adjParam(uint8_t param, bool up);
        virtual void drawFrame();
      private:
	uint8_t NUM_PARAMS;
	uint8_t MAX_SPOKES;
	uint8_t deltaHue;
	uint8_t currAngle;
	uint8_t numSpokes;
	uint8_t halfAngleBetweenSpokes;
	uint8_t angleBetweenSpokes;
    };



    // Then I need some sort of array that would include peppermint and all the others..

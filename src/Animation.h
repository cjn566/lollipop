#ifndef ANIMATION_H
    #define ANIMATION_H

    #include "FastLED.h"
    #include "util.h"
    
    #define NUM_MOST_PARAMS     3
    #define NUM_LEDS            204
    #define ORDER               GRB
    
    #include "radii.h"
    

    struct {
        uint8_t anim = 0;
        uint8_t stepsSinceLastFrame = 0;
        uint8_t saturation = 255;    
        CRGBArray<NUM_LEDS> leds;
    } $;

    struct Parameter {
        CRGB backgroundColor;
        uint8_t ticksToAdjust;
    };

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


    struct {
        #define SCALE_POS_MID_IDX 67
        #define SCALE_HALF_SIZE 8
        #define SCALE_START_IDX (SCALE_POS_MID_IDX - SCALE_HALF_SIZE)
        enum Mode {
            OFF,
            NOSIGN,
            SIGN,
            CHUNK
        } mode = OFF;

        private:
        uint8_t chunkSize = 1;
        CRGB onColor = CRGB::Red;
        int8_t pValue = 0;
        bool sign;

        void drawNoSign(){
            if(pValue) $.leds(SCALE_START_IDX, (pValue + SCALE_START_IDX))  = onColor;
        }
        
        void drawSign(){
            if(pValue) {
                $.leds(SCALE_POS_MID_IDX + (sign? 0:-1), SCALE_POS_MID_IDX + pValue + (sign? -1:0))  = onColor;
            }
        }

        void drawChunk(){
            //if(pValue) $.leds(SCALE_START_IDX, (pValue + SCALE_START_IDX))  = onColor;  TODO
        }

        public:
        void setValue(int val){
            switch(mode){
                case NOSIGN:
                    if(val <= (SCALE_HALF_SIZE*2) && val >= 0) pValue = val;
                    break;
                case SIGN:
                    if(val <= SCALE_HALF_SIZE && -val >= -SCALE_HALF_SIZE){
                        pValue = val;
                        sign = (pValue > 0);
                    }
                    break;
                case CHUNK:
                    break;
            }
            pValue = val;

            #ifdef DEBUG
            Serial.printf("scale value: %d\n", val);
            #endif
        }

        void init(Mode nMode, int val = 0, CRGB nColor = CRGB::Green, int nChunkSize = 4){
            mode = nMode;
            onColor = nColor;
            chunkSize = nChunkSize;
            setValue(val);
        }
        
        void draw(){
            if(mode != OFF){
                $.leds(SCALE_START_IDX, SCALE_START_IDX + (SCALE_HALF_SIZE*2) - 1) = CRGB::White;
                $.leds[SCALE_START_IDX - 1] = CRGB::Black;
                $.leds[SCALE_START_IDX + (SCALE_HALF_SIZE*2)] = CRGB::Black;
                switch(mode){
                    case NOSIGN:
                        drawNoSign();
                        break;
                    case SIGN:
                        drawSign();
                        break;
                    case CHUNK:
                        drawChunk();
                        break;
                }
            }
        }
    } drawScale;

    // ------------------ RAINBOW ----------------
    namespace rainbow {
        #define MAX_STRETCH 8
        const int8_t NUM_PARAMS = 1;
        // Params Vars
        int8_t stretch = 1;
        
        Parameter params[NUM_PARAMS] = {
            Parameter{CRGB::DarkGray, 8},   // Stretch
        };

        // Step Vars
        uint8_t currHue = 0;

        void initAnim(){
        }

        void initParam(uint8_t p){
            switch(p){
                case 0: // Stretch
                    drawScale.init(drawScale.SIGN, stretch);
                    break;
            }
        }

        void adjParam(uint8_t param, bool up){
            switch(param){
                case 0:
                    stretch = CLAMP_SN(stretch + INCDEC, MAX_STRETCH);
                    drawScale.setValue(stretch);

                    #ifdef DEBUG
                    Serial.printf("Stretch: %d\n", (uint8_t)stretch);
                    #endif
                    break;
            }
        }

        void drawFrame(){
            FastLED.clear();
            currHue += $.stepsSinceLastFrame;
            fill_rainbow($.leds, NUM_LEDS, currHue, stretch);
        }
    }

        // ------------------ Full Gradient ----------------
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

        void initParam(uint8_t p){
            switch(p){
                case 0: // Stretch
                    drawScale.init(drawScale.NOSIGN, (stretch >> 4));
                    break;
                case 1: // Hue
                    drawScale.init(drawScale.OFF);
                    break;
            }
        }

        void adjParam(uint8_t param, bool up){
            switch(param){
                case 0:
                    stretch = CLAMP_8(stretch + INCDEC);
                    drawScale.setValue((stretch >> 4));

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

    // ------------------ Particles ----------------
    // namespace gradient {
    //     const uint8_t NUM_PARAMS = 2;
    //     // Params Vars
    //     uint8_t stretch = 1;
    //     uint8_t baseHue = 0;
        
    //     Parameter params[NUM_PARAMS] = {
    //         Parameter{CRGB::DarkGray, 2},   // Stretch
    //         Parameter{CRGB::DarkGray, 4},   // Hue
    //     };

    //     struct Particle {
    //         uint16_t location;
    //         fract16 acceleration;
    //         uint16_t velocity;
    //         CRGB color;
    //     } particles[200];

    //     // Step Vars
    //     uint8_t currHue = 0;

    //     void initAnim(){
    //     }

    //     void initParam(uint8_t p){
    //         switch(p){
    //             case 0: // Stretch
    //                 drawScale.init(drawScale.NOSIGN, (stretch >> 4));
    //                 break;
    //             case 1: // Hue
    //                 drawScale.init(drawScale.OFF);
    //                 break;
    //         }
    //     }

    //     void adjParam(uint8_t param, bool up){
    //         switch(param){
    //             case 0:
    //                 stretch = CLAMP_8(stretch + INCDEC);
    //                 drawScale.setValue((stretch >> 4));

    //                 #ifdef DEBUG
    //                 Serial.printf("Stretch: %d\n", (uint8_t)stretch);
    //                 #endif
    //                 break;
    //             case 1:
    //                 baseHue += INCDEC;
    //                 break;
    //         }
    //     }

    //     void drawFrame(){
    //         FastLED.clear();
    //         currHue += $.stepsSinceLastFrame;
    //         CHSV first =  CHSV(baseHue + currHue, $.saturation, 255);
    //         CHSV second = CHSV(baseHue + currHue + stretch, $.saturation, 255);
    //         fill_gradient<CRGB>($.leds, (uint16_t)NUM_LEDS, first, second, FORWARD_HUES);
    //     }
    // }


    // ------------------ FIND INDICES ----------------
    namespace indices {
        const uint8_t NUM_PARAMS = 1;
        // Params Vars
        uint8_t currIdx = 0;
        
        Parameter params[NUM_PARAMS] = {
            Parameter{CRGB::DarkGray, 4} // Index
        };

        void initAnim(){
        }

        void initParam(uint8_t p){
            switch(p){
                case 0: // Index
                    drawScale.init(drawScale.OFF);
                    break;
            }
        }

        void adjParam(uint8_t param, bool up){
            switch(param){
                case 0:
                    currIdx += INCDEC;
                    #ifdef DEBUG
                    Serial.printf("  %d--%d:%d--%d\n", currIdx, currIdx + 7, currIdx + 8, currIdx + 15);
                    #endif
                    break;
            }
        }

        void drawFrame(){
            FastLED.clear();
            $.leds[currIdx] = CRGB::White;
            //$.leds(currIdx, currIdx + 7) = CRGB::Red;     
            //$.leds(currIdx + 8, currIdx + 15) = CRGB::Blue;     
        }
    }


    struct AnimationBase {
        AnimationBase();
        uint8_t NUM_PARAMS;
        Parameter * params;
        virtual void initAnim() =0;
        virtual void initParam(uint8_t p) =0;
        virtual void adjParam(uint8_t param, bool up) =0;
        virtual void drawFrame() =0;
    };

    static struct Peppermint : public AnimationBase{
        static const uint8_t NUM_PARAMS = 2;
        virtual void initAnim();
        virtual void initParam(uint8_t p);
        virtual void adjParam(uint8_t param, bool up);
        virtual void drawFrame();
    } peppermint;



    // ------------------ COLLECTION ----------------  Animation(int np, Parameter *pl, void (*init)(), void (*ip)(uint8_t), void (*ap)(uint8_t, bool), void (*df)()) : 
    #define NUM_ANIMS           3
    #define DECLARE_CLASS_ANIM        Animation(_ANIM_NAME_.NUM_PARAMS, _ANIM_NAME_.params, _ANIM_NAME_.initAnim, _ANIM_NAME_.initParam, _ANIM_NAME_.adjParam, _ANIM_NAME_.drawFrame),
    #define DECLARE_ANIM        Animation(_ANIM_NAME_::NUM_PARAMS, _ANIM_NAME_::params, _ANIM_NAME_::initAnim, _ANIM_NAME_::initParam, _ANIM_NAME_::adjParam, _ANIM_NAME_::drawFrame),
    Animation animations[NUM_ANIMS] = {
        #define _ANIM_NAME_ peppermint
        DECLARE_CLASS_ANIM
        #undef _ANIM_NAME_
        #define _ANIM_NAME_ rainbow
        DECLARE_ANIM
        #undef _ANIM_NAME_
        #define _ANIM_NAME_ indices
        DECLARE_ANIM
        #undef _ANIM_NAME_
    };


#endif



    //     // ------------------ TEMPLATE ----------------
    // namespace * {
    //     const uint8_t NUM_PARAMS = 1;
    //     enum ParamNames {
    //         *
    //     }

    //     // Params Vars
    //     uint8_t * = 0;
        
    //     Parameter params[NUM_PARAMS] = {
    //         Parameter{CRGB::DarkGray, *} // *
    //     };

    //     void initAnim(){
    //     }

    //     void initParam(uint8_t p){
    //         switch(p){
    //             case *:
    //                 drawScale.init(drawScale.OFF);
    //                 break;
    //         }
    //     }

    //     void adjParam(uint8_t param, bool up){
    //         switch(param){
    //             case *:
    //                 * += INCDEC;
    //                 #ifdef DEBUG
    //                 Serial.printf(" %d\n", *);
    //                 #endif
    //                 break;
    //         }
    //     }

    //     void drawFrame(){
    //         FastLED.clear();
    //         for(int i = 0; i< NUM_LEDS; i++){
    //     }
    // }
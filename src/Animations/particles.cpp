#include "../Animation.h"
#include "util.h"
//#include "radii.h"

struct Particles: public AnimationBase{

    #define ENDPOINT 193
    #define MAX_PARTICLES 1

    enum ParamName {
        SPAWN_RATE,
        SPAWN_RATE_VAR,
        VELOCITY,
        VELOCITY_VAR,
        ACCELERATION,
        ACCELERATION_VAR,
        COLOR,
        COLOR_VAR,
        COLOR_CYCLE_RATE
    };

    // params vars
    uint8_t spawnRate;
    uint8_t spawnRateVar;
    int      velocity;
    uint8_t velocityVar;
    int      accel;
    uint8_t accelVar;
    uint8_t hue = 0;
    uint8_t hueVar;
    uint8_t hueCycleRate;

    // state vars
    uint16_t counter = 0, numParticles = 0, newParticleIdx = 0, spawnDelay;
    


    public:
    Particles(){
        accel    =      3;
        velocity =      5;
        spawnDelay = 10;
        numParams = 0;
        params = new parameter_t[numParams];
        //params[*] = {CRGB::Red, 4};
    };

    void initAnim(){
    }

    void initParam(ParamName paramIdx){
        switch(paramIdx){
            case SPAWN_RATE:
                break;
            case SPAWN_RATE_VAR:
                break;
            case VELOCITY:
                break;
            case VELOCITY_VAR:
                break;
            case ACCELERATION:
                break;
            case ACCELERATION_VAR:
                break;
            case COLOR:
                break;
            case COLOR_VAR:
                break;
            case COLOR_CYCLE_RATE:
                break;
        }
    }

    void adjParam(uint8_t paramIdx, bool up){
        switch(paramIdx){
            case SPAWN_RATE:
                //* += INCDEC;
                break;
            case SPAWN_RATE_VAR:
                break;
            case VELOCITY:
                break;
            case VELOCITY_VAR:
                break;
            case ACCELERATION:
                break;
            case ACCELERATION_VAR:
                break;
            case COLOR:
                break;
            case COLOR_VAR:
                break;
            case COLOR_CYCLE_RATE:
                break;
        }
    }

    uint32_t particle[MAX_PARTICLES];

    struct P_values {
        uint16_t timestamp;
        uint8_t hue;
        int8_t velocity;
        int8_t accel;
    };

    #define ACCEL_W  5
    #define VEL_W    5
    #define HUE_W    8
    #define TS_W    14
    #define TS_TRUNC  4   
    #define HUE_SH   (ACCEL_W + VEL_W)
    #define TS_SH   (HUE_SH + HUE_W)
    #define MAKE_MASK(i) (0xffffffff >> (32 - i))

    uint32_t collapseP(P_values p_values){
        uint32_t p;
        p =  (((p_values.timestamp >> TS_TRUNC) & MAKE_MASK(TS_W))  << TS_SH);
        p |= (p_values.hue       << HUE_SH);
        p |= (p_values.velocity  << ACCEL_W);
        p |= p_values.accel;
        return p;
    };

    P_values expandP(uint32_t p){
        P_values p_values;
        p_values.timestamp   = (p >> (TS_SH - TS_TRUNC));
        p_values.hue        = (p >> HUE_SH) & MAKE_MASK(HUE_W);
        p_values.velocity   = (p >> ACCEL_W) & MAKE_MASK(VEL_W);
        p_values.accel      = p & MAKE_MASK(ACCEL_W);
        return p_values;
    };

    void drawFrame(uint8_t millisSinceLastFrame){
        #ifdef DEBUG
        //Serial.println();
        #endif
        FastLED.clear();

        long now = millis();
                // Spawn
        if((millisSinceLastFrame > 0) && (numParticles < MAX_PARTICLES)){
            //Serial.printf("counter: %d\t#p: %d\tidx: %d\tstps: %d >>>  ", counter, numParticles, newParticleIdx, millisSinceLastFrame);
            counter += millisSinceLastFrame;
            if(counter >= spawnDelay){
                counter -= spawnDelay;
                while(particle[newParticleIdx]){	// Find next available slot
                    newParticleIdx = (newParticleIdx + 1) % MAX_PARTICLES;
                }
                // Build new particle
                numParticles++;                
                P_values p_values;
                p_values.timestamp  = millis();
                p_values.hue        = hue;
                p_values.velocity   = velocity;
                p_values.accel      = accel;
                particle[newParticleIdx] = collapseP(p_values);

                hue += 8;
                
                //Serial.printf("hue: %d\n", hue);
                // Set up next particle
            }
        }

        for (int i = 0; i < MAX_PARTICLES; i++){
                //Serial.print(particle[i].active? 'p':'-');
            if(particle[i]){
                P_values pv = expandP(particle[i]);
                uint32_t age = now - pv.timestamp;
                uint32_t location = (pv.velocity*age + ((pv.accel * (age * age)) >> 1));


                
                //Serial.printf("idx: %d\tlp: %d\tlv: %d\n", i, leftPx, leftVal);//, rightVal);
                //Serial.printf("color: %d\n", p->hue);
                ledData.leds[pv.location >> 4] = CHSV(pv.hue, ledData.saturation, 255);//leftVal);
                // if(leftPx < (NUM_LEDS - 1)){
                //     ledData.leds[leftPx + 1] = CHSV(p->color, ledData.saturation, rightVal);
                // }
            
                // move
                // Location is += t*velocity
                pv.location += (pv.velocity * millisSinceLastFrame) ;
                if(pv.location > MAX_LOCATION || pv.location < 1){
                    particle[i] = 0;
                    numParticles--;
                    //Serial.print("deleted");
                } else {
                    pv.velocity += (pv.accel * millisSinceLastFrame);
                    //Serial.printf("loc: %d\tacc: %d\tvel: %d", p->location, p->acceleration, p->velocity);
                }

                
                // Serial.printf("idx: %d\ta: %d\tv: %d\tp: %d\n", i, p->acceleration, p->velocity, p->location);
            }
        }
        //Serial.print(".");
    }
};










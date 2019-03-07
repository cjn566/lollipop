#include "../Animation.h"
//#include "radii.h"




struct Particles: public AnimationBase{

    #define ENDPOINT 193

    
    #ifdef DEBUG
        #define MAX_PARTICLES 3
    #else
        #define MAX_PARTICLES 40
    #endif

    #define ACCEL_W  5
    #define VEL_W    5
    #define HUE_W    8
    #define TS_W    14
    #if ACCEL_W + VEL_W + HUE_W + TS_W != 32
        #error Particle bits not equal to 32!
    #endif
    #define TS_TRUNC    3
    #define HUE_SH   (ACCEL_W + VEL_W)
    #define TS_SH   (HUE_SH + HUE_W)

    #define TS_MOD MAX_UVAL_N_BITS(TS_W)

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
        accel    =      2;
        velocity =      0x1f;
        spawnDelay = 500;
        numParams = 0;
        params = new parameter_t[numParams];
        //params[*] = {CRGB::Red, 4};
    };

    void initAnim(){
        #ifdef DEBUG
            Serial.printf("Max age: %d ms\n", TS_MOD);
        #endif
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
        unsigned long timestamp;
        uint8_t hue;
        int8_t velocity;
        int8_t accel;
    };



    uint32_t collapseP(P_values p_values){
        uint32_t p;
        p =  (p_values.timestamp << TS_SH);
        p |= (p_values.hue       << HUE_SH);
        p |= (p_values.velocity  << ACCEL_W);
        p |= p_values.accel;
        return p;
    };

    P_values expandP(uint32_t p){
        P_values p_values;
        p_values.timestamp   = (p >> TS_SH);
        p_values.hue        = (p >> HUE_SH) & MAKE_MASK(HUE_W);
        p_values.velocity   = (p >> ACCEL_W) & MAKE_MASK(VEL_W);
        p_values.accel      = p & MAKE_MASK(ACCEL_W);
        return p_values;
    };

    void drawFrame(uint8_t millisSinceLastFrame){
        unsigned long now = (millis() >> TS_TRUNC) & MAKE_MASK(TS_W);
        #ifdef DEBUG
            Serial.printf("now: %d\r\n", now);
        #endif
        FastLED.clear();

                // Spawn
        if(numParticles < MAX_PARTICLES){
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
                p_values.timestamp  = now;
                p_values.hue        = hue;
                p_values.velocity   = velocity;
                p_values.accel      = accel;
                particle[newParticleIdx] = collapseP(p_values);

                hue += 120;
                
                //Serial.printf("hue: %d\n", hue);
                // Set up next particle
            }
        }
        #define VEL_MOD     1
        #define ACCEL_MOD   1
        int baseAccel = accel * ACCEL_MOD;
        int baseVel = velocity * VEL_MOD;

        for (int i = 0; i < MAX_PARTICLES; i++){
                //Serial.print(particle[i].active? 'p':'-');
            if(particle[i]){
                P_values pv = expandP(particle[i]);
                long age = now - pv.timestamp;

                #ifdef DEBUG
                    Serial.printf("idx: %d\tts: %d\t  pre-mod age: %d\t", i, pv.timestamp, age);
                #endif

                #define OVFL_TIME_CUTOFF 500

                if(age < 0){
                    if(age > -OVFL_TIME_CUTOFF) // Particle is within one second of overflowing age counter, so retire it
                    {
                        particle[i] = 0;
                        numParticles--;
                    }
                    age += TS_MOD;
                }
                age = SCALE32_8(age, ledData.speed);

                #define VEL_VAR_MOD     1   
                #define ACCEL_VAR_MOD   1
                int p_vel = baseVel + (pv.velocity * VEL_VAR_MOD);
                int p_accel = baseAccel + (pv.accel * ACCEL_VAR_MOD);

                #define LOC_REDUCTION   6
                #define LOC_FRACTIONAL  8
                uint32_t location = (pv.velocity*age + ((pv.accel * (age * age)) >> 1)) >> (LOC_REDUCTION + LOC_FRACTIONAL);
                uint16_t loc_fractional = location & MAKE_MASK(LOC_FRACTIONAL);

                #ifdef DEBUG
                    Serial.printf("new age: %d\tloc: %d\n", age, location);
                #endif
                
                // if(location > (ENDPOINT << LOC_FRACTIONAL) || location < 0){
                if(location > (ENDPOINT) || location < 0){
                    particle[i] = 0;
                    numParticles--;
                } else {
                    ledData.leds[location] = CHSV(pv.hue, ledData.saturation, 255);//leftVal);
                    ledData.leds[location + 1] = CHSV(pv.hue, ledData.saturation, 255);//leftVal);
                }
                
                //Serial.printf("idx: %d\tlp: %d\tlv: %d\n", i, leftPx, leftVal);//, rightVal);
                //Serial.printf("color: %d\n", p->hue);
                //ledData.leds[pv.location >> 4] = CHSV(pv.hue, ledData.saturation, 255);//leftVal);
                // if(leftPx < (NUM_LEDS - 1)){
                //     ledData.leds[leftPx + 1] = CHSV(p->color, ledData.saturation, rightVal);
                // }

            }
        }
        //Serial.print(".");
    }
};










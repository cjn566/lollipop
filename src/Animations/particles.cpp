#include "../Animation.h"
//#include "radii.h"




struct Particles: public AnimationBase{

    #define ENDPOINT 193
    
    #ifdef DEBUG
        #define MAX_PARTICLES 30
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
        COLOR_CYCLE_RATE,
        COLOR_VAR,
        VELOCITY,
        VELOCITY_VAR,
        ACCELERATION,
        ACCELERATION_VAR,
        COLOR,
    };

    // params vars
    uint8_t spawnRate = 20;
    uint8_t spawnRateVar = 0;
    uint8_t velocity = 0;
    uint8_t velocityVar = 0;
    uint8_t acceleration = 0;
    uint8_t accelVar = 0;
    uint8_t hueVar = 0;
    uint8_t hueCycleRate = 1;

    // state vars
    int      baseVelocity;
    int      baseAccel;
    uint16_t hue = 0;
    uint8_t numParticles = 0, newParticleIdx = 0;
    unsigned int spawnDelay, counter = 0, thisSpawnDelay;
    uint32_t particle[MAX_PARTICLES];
    


    public:
    Particles(){
        // accel    =      2;
        // velocity =      0x1f;
        numParams = 4;
        params = new parameter_t[numParams];
        params[SPAWN_RATE].scaleColor = CRGB::HotPink;
        params[SPAWN_RATE_VAR].scaleColor = CRGB::Blue;
    };

    void initAnim(){
        randomSeed(millis() & MAKE_MASK(16));
        spawnDelay = spawnRate * SPEED_SCALE_BASE * 8;
        thisSpawnDelay = spawnDelay;
        for(int i =0; i<MAX_PARTICLES; i++) particle[i] = 0;
        #ifdef DEBUG
            Serial.printf("Max age: %d ms\n", TS_MOD);
        #endif
    }

    void initParam(ParamName paramIdx){
        drawScale.init(&params[paramIdx]);
        switch(paramIdx){
            case SPAWN_RATE:
                drawScale.setValue(spawnRate);
                break;
            case SPAWN_RATE_VAR:
                drawScale.setValue(spawnRateVar);
                break;
            case COLOR_CYCLE_RATE:
                drawScale.setValue(hueCycleRate);
                break;
            case COLOR_VAR:
                drawScale.setValue(hueVar);
                break;
            default:
                break;
        }
    }

    void adjParam(uint8_t paramIdx, bool up){
        switch(paramIdx){
            case SPAWN_RATE:
                spawnRate = CLAMP_8(spawnRate + FAST_SCROLL);
                drawScale.setValue(spawnRate);
                spawnDelay = spawnRate * SPEED_SCALE_BASE * 8;
                break;
            case SPAWN_RATE_VAR:
                spawnRateVar = CLAMP_8(spawnRateVar + FAST_SCROLL);
                drawScale.setValue(spawnRateVar);
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
                hueVar = CLAMP_8(hueVar + FAST_SCROLL);
                drawScale.setValue(hueVar);
                break;
            case COLOR_CYCLE_RATE:
                hueCycleRate = CLAMP_8(hueCycleRate + FAST_SCROLL);
                drawScale.setValue(hueCycleRate);
                break;
        }
    }


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


    void drawFrame(int16_t millisSinceLastFrame){
        unsigned long now = (millis() >> TS_TRUNC) & MAKE_MASK(TS_W);
        #ifdef DEBUG
            //Serial.printf("now: %d\r\n", now);
        #endif

                // Spawn
        if(numParticles < MAX_PARTICLES){
            #ifdef DEBUG
                //Serial.printf("counter: %d\ttime: %d\n", counter, millisSinceLastFrame);
            #endif            
            counter += millisSinceLastFrame;
            if(counter >= thisSpawnDelay || counter < 0){
                if(counter >= thisSpawnDelay)  counter -= thisSpawnDelay;
                else counter += thisSpawnDelay;                
                
                int variance = ((random8() - 127) * spawnRateVar * spawnDelay);
                variance /= (256*127);
                thisSpawnDelay = spawnDelay + variance;

                #ifdef DEBUG    
                   //Serial.printf("baseDelay: %d\tNew Delay: %d\t wtf: %d\n", spawnDelay, thisSpawnDelay, variance);
                #endif     

                while(particle[newParticleIdx]){	// Find next available slot
                    newParticleIdx = (newParticleIdx + 1) % MAX_PARTICLES;
                }
                // Build new particle
                numParticles++;                
                P_values p_values;

                int hueRand = scale8(random8() - 127, hueVar);
                int velRand = scale8(random8() - 127, velocityVar);
                int accelRand = scale8(random8() - 127, accelVar);

                p_values.timestamp  = now;
                p_values.hue        = (hue >> 3) + hueRand;
                p_values.velocity   = velRand >> (8 - VEL_W);
                p_values.accel      = accelRand >> (8 - ACCEL_W);
                particle[newParticleIdx] = collapseP(p_values);

                hue += hueCycleRate;
            }
        }

        //fill_solid(ledData.leds, NUM_LEDS, CRGB(25,0,30));

        #define VEL_MOD     1
        #define ACCEL_MOD   1
        int baseAccel = acceleration * ACCEL_MOD;
        int baseVel = velocity * VEL_MOD;

        for (int i = 0; i < MAX_PARTICLES; i++){
                //Serial.print(particle[i].active? 'p':'-');
            if(particle[i]){
                P_values pv = expandP(particle[i]);
                long age = now - pv.timestamp;

                #ifdef DEBUG
                    //Serial.printf("idx: %d\tts: %d\t  pre-mod age: %d\t", i, pv.timestamp, age);
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

                age = SCALE32_BY_8(age, speed);

                #define VEL_VAR_MOD     1   
                #define ACCEL_VAR_MOD   1
                int p_vel = baseVel + (pv.velocity * VEL_VAR_MOD);
                int p_accel = baseAccel + (pv.accel * ACCEL_VAR_MOD);

                #define LOC_REDUCTION   0
                #define LOC_FRACTIONAL  8
                uint32_t location = (pv.velocity*age + ((pv.accel * (age * age)) >> 1));
                uint8_t loc_idx = (location >> (LOC_REDUCTION + LOC_FRACTIONAL)) + 1;
                uint16_t loc_fractional = location & MAKE_MASK(LOC_FRACTIONAL);

                #ifdef DEBUG
                    //Serial.printf("new age: %d\tloc: %d\n", age, location);
                #endif
                
                // if(location > (ENDPOINT << LOC_FRACTIONAL) || location < 0){
                if(loc_idx > (ENDPOINT) || location < 0){
                    particle[i] = 0;
                    numParticles--;
                } else {
                    ledData.leds[loc_idx - 1] += CHSV(pv.hue, ledData.saturation, 255 - loc_fractional);//leftVal);
                    ledData.leds[loc_idx] += CHSV(pv.hue, ledData.saturation, 255);//leftVal);
                    ledData.leds[loc_idx + 1] += CHSV(pv.hue, ledData.saturation, loc_fractional);//leftVal);
                }
                
                //Serial.printf("idx: %d\tlp: %d\tlv: %d\n", i, leftPx, leftVal);//, rightVal);
                //Serial.printf("color: %d\n", p->hue);
                //ledData.leds[pv.location >> 4] = CHSV(pv.hue, ledData.saturation, 255);//leftVal);
                // if(leftPx < (NUM_LEDS - 1)){
                //     ledData.leds[leftPx + 1] = CHSV(p->color, ledData.saturation, rightVal);
                // }

            }
        }
        // for(int i=0;i<NUM_LEDS;i++){
        //     if(!ledData.leds[i]) ledData.leds[i] = CHSV((uint8_t)(hue>>3) + 127, 255,255);
        // }
        //Serial.print(".");
    }
};










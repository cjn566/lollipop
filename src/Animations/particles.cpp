#include "../Animation.h"

struct Particles: public AnimationBase{

    
    #ifdef DEBUG
        #define MAX_PARTICLES 2
    #else
        #define MAX_PARTICLES 70
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
    #define VEL_LEFT_SH         7
    #define ACCEL_RIGHT_SH      2
    #define LS_LEFT_SH      2
    #define LOC_REDUCTION   6
    #define HUE_CTR_MULT   2
    #define LOC_MAX (ENDPOINT << (LOC_REDUCTION + 8))
    #define ENDPOINT 193
    #define OVFL_TIME_CUTOFF 500

    enum ParamName {
        MODE,
        LIFESPAN,
        SPAWN_RATE,
        SPAWN_RATE_VAR,
        COLOR_CYCLE_RATE,
        COLOR_VAR,
        VELOCITY,
        VELOCITY_VAR,
        ACCELERATION,
        ACCELERATION_VAR,
    };

    enum Mode {
        WRAP,
        BOUNCE,
        DIE
    } mode = WRAP;

    // params vars
    uint8_t lifespan = 20;
    uint8_t spawnRate = 40;
    uint8_t spawnRateVar = 0;
    int8_t velocity = 30;
    uint8_t velocityVar = 255;
    int8_t acceleration = 0;
    uint8_t accelVar = 255;
    uint8_t hueVar = 80;
    int8_t hueCycleRate = 0;

    // state vars
    int      maxAge = 0;
    int      baseVelocity;
    int      baseAccel;
    uint32_t     realLifespan;
    uint16_t hue = 0;
    uint8_t numParticles = 0, newParticleIdx = 0;
    unsigned int spawnDelay, counter = 0, thisSpawnDelay;

    
    uint32_t particle[MAX_PARTICLES];
    


    public:
    Particles(){
        numParams = 10;
        params = new parameter_t[numParams];
        params[MODE].max = 3;
        params[MODE].ticksToAdjust = 3;
        params[VELOCITY].max = 127;
        params[ACCELERATION].max = (2<<(8-ACCEL_RIGHT_SH))-1;
        params[COLOR_CYCLE_RATE].max = 20;

        params[SPAWN_RATE].scaleColor = CRGB::DarkBlue;
        params[SPAWN_RATE_VAR].scaleColor = CRGB::LightBlue;
        params[COLOR_CYCLE_RATE].scaleColor = CRGB::DarkMagenta;
        params[COLOR_VAR].scaleColor = CRGB::Pink;
        params[VELOCITY].scaleColor = CRGB::DarkGreen;
        params[VELOCITY_VAR].scaleColor = CRGB::LightGreen;
        params[ACCELERATION].scaleColor = CRGB::Orange;
        params[ACCELERATION_VAR].scaleColor = CRGB::Yellow;
    };

    void initAnim(){
        random16_add_entropy(millis() & MAKE_MASK(16));

        spawnDelay = spawnRate * SPEED_SCALE_BASE * 8;
        thisSpawnDelay = spawnDelay;
        realLifespan = lifespan << LS_LEFT_SH;

        for(int i =0; i<MAX_PARTICLES; i++) particle[i] = 0; // Reset Particles
        numParticles = 0;
        #ifdef DEBUG
            Serial.printf("Max age: %d ms\n", TS_MOD);
        #endif
    }

    int adjParam(uint8_t paramIdx, int change){
        random16_add_entropy(millis() & MAKE_MASK(16));
        switch(paramIdx){
            case MODE:
            {
                int tempMode = (int)mode + 1;
                if(change) tempMode = clamp_un1(tempMode + (change > 0? 1:-1), 3);
                mode = (Mode)(tempMode - 1);
                return tempMode;
            }
            case LIFESPAN:
                lifespan = CLAMP_8(lifespan + change);
                realLifespan = lifespan << LS_LEFT_SH;
                return lifespan;
                
            case SPAWN_RATE:
                spawnRate = CLAMP_8(spawnRate + change);
                spawnDelay = spawnRate * SPEED_SCALE_BASE * 8;
                return spawnRate;

            case SPAWN_RATE_VAR:
                spawnRateVar = CLAMP_8(spawnRateVar + change);
                return spawnRateVar;

            case COLOR_CYCLE_RATE:
                hueCycleRate = clamp_sn(hueCycleRate + change, (int)params[COLOR_CYCLE_RATE].max);
                return hueCycleRate;
                
            case COLOR_VAR:
                hueVar = CLAMP_8(hueVar + change);
                return hueVar;

            case VELOCITY:
                velocity = CLAMP_S8(velocity + change);
                baseVelocity = velocity << VEL_LEFT_SH;
                return velocity;

            case VELOCITY_VAR:
                velocityVar = CLAMP_8(velocityVar + change);
                return velocityVar;

            case ACCELERATION:
                acceleration = clamp_sn(acceleration + change, params[ACCELERATION].max);
                return acceleration;

            case ACCELERATION_VAR:
                accelVar = CLAMP_8(accelVar + change);
                return accelVar;

            default: return 0;
        }
        return 0;
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
        p |= ((p_values.velocity & MAKE_MASK(VEL_W)) << ACCEL_W);
        p |= (p_values.accel & MAKE_MASK(ACCEL_W));
        return p;
    };

    P_values expandP(uint32_t p){
        P_values p_values;
        p_values.timestamp   = (p >> TS_SH);
        p_values.hue        = (p >> HUE_SH) & MAKE_MASK(HUE_W);
        p_values.velocity   = ((p >> (ACCEL_W + VEL_W - 8)) & 0xff) >> (8-VEL_W);
        p_values.accel      = p & MAKE_MASK(ACCEL_W);
        return p_values;
    };

    void killParticle(int i){
        particle[i] = 0;
        numParticles--;
    }

    int getVelocity(int velRandomness){
        velRandomness = velRandomness << (8-VEL_W);
        velRandomness -= 127;
        return (velocity + velRandomness) << VEL_LEFT_SH;
    }

    int getAccel(int accelRandomness){
        accelRandomness = accelRandomness << (8- ACCEL_W - ACCEL_RIGHT_SH);
        accelRandomness -= (params[ACCELERATION].max >> 1);
        return (acceleration + accelRandomness);
    }

    void drawFrame(int16_t millisSinceLastFrame){
        unsigned long now = (millis() >> TS_TRUNC) & MAKE_MASK(TS_W);

                // Spawn
        if(numParticles < MAX_PARTICLES){     
            counter += millisSinceLastFrame;
            if(counter >= thisSpawnDelay || counter < 0){
                if(counter >= thisSpawnDelay)  counter -= thisSpawnDelay;
                else counter += thisSpawnDelay;                
                
                int variance = ((random8() - 127) * spawnRateVar * spawnDelay);
                variance /= (256*127);
                thisSpawnDelay = spawnDelay + variance;
                
                while(particle[newParticleIdx]){	// Find next available slot
                    newParticleIdx = (newParticleIdx + 1) % MAX_PARTICLES;
                }
                // Build new particle
                numParticles++;                
                P_values p_values;

                int velMax = (1<<(VEL_W-1))-1;

                int hueRand = scale8(random8(), hueVar);
                int velRand = scale_by_n(random8()-127, (int)velocityVar, 255);
                velRand = scale_to_n(velRand, 127, velMax);

                int accelRand = scale8(random8()-127, accelVar);
                accelRand = scale_to_n(accelRand, 255, (1<<ACCEL_W)-1);

                #ifdef DEBUG
                    Serial.printf("velrand:\t%d of %d\naccelrand:\t%d of %d\n", velRand, velMax, accelRand, (1<<ACCEL_W)-1);
                #endif

                p_values.timestamp  = now;
                p_values.hue        = (uint8_t)(hue >> HUE_CTR_MULT) + hueRand;
                p_values.velocity   = velRand;
                p_values.accel      = accelRand;
                particle[newParticleIdx] = collapseP(p_values);

                #ifdef DEBUG
                    P_values testValues = expandP(particle[newParticleIdx]);
                    Serial.printf("unpacked:\t%d\naccelrand:\t%d\n\n", testValues.velocity, testValues.accel);
                #endif

                hue += hueCycleRate;
            }
        }

        for (int i = 0; i < MAX_PARTICLES; i++){
                //Serial.print(particle[i].active? 'p':'-');
            if(particle[i]){
                P_values pv = expandP(particle[i]);
                unsigned long age = (now - pv.timestamp);

                #ifdef DEBUG
                    //Serial.printf("idx: %d\tts: %d\t  pre-mod age: %d\t", i, pv.timestamp, age);
                #endif


                if(age < 0){
                    if(age > -OVFL_TIME_CUTOFF) // Particle is within one second of overflowing age counter, so retire it
                    { 
                        killParticle(i);
                        continue;
                    }
                    age += TS_MOD;
                }

                age = scale_by_n((int)age, (int)speed, 255);
                if(age > realLifespan){
                     killParticle(i);
                     continue;
                }
                
                if(!age) age = 1;
                int velDist = getVelocity(pv.velocity) * age;
                int accelDist = (getAccel(pv.accel) * age * age) >> 1;
                int location = velDist + accelDist;

                

                while((location > LOC_MAX) || (location < 0)){
                    bool under = (location < 0);
                    switch (mode)
                    {
                        case BOUNCE:
                            location = under? (-location) : LOC_MAX - (location % LOC_MAX);
                            break;                            
                        case WRAP:
                            location = under? (LOC_MAX + location) : (location - LOC_MAX);
                            break;                            
                        case DIE:
                            killParticle(i);
                            location = 0;
                            continue;
                            break;
                    }
                }
                uint8_t loc_idx = (location >> (LOC_REDUCTION + 8)) + 1;
                uint16_t loc_fractional = (location >> LOC_REDUCTION) & 0xff;

                #ifdef DEBUG
                    //Serial.printf("new age: %d\tloc: %d\n", age, location);
                #endif
                
                if(particle[i]){
                    ledData.leds[loc_idx - 1] += CHSV(pv.hue, ledData.saturation, 255 - loc_fractional);//leftVal);
                    ledData.leds[loc_idx] += CHSV(pv.hue, ledData.saturation, 255);//leftVal);
                    ledData.leds[loc_idx + 1] += CHSV(pv.hue, ledData.saturation, loc_fractional);//leftVal);
                }
            }
        }
    }
};










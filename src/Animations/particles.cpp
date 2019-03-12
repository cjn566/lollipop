#include "../Animation.h"

struct Particles: public AnimationBase{

    #ifdef DEBUG
        #define MAX_PARTICLES 2
    #else
        #define MAX_PARTICLES 80
    #endif

    #define TS_W        16
    #define TS_TRUNC    3
    #define TS_MOD MAX_UVAL_N_BITS(TS_W)

    #define MAX_VELOC (1 << (LOC_REDUCTION) + 1)                
    #define VEL_LEFT_SH         6
    #define ACCEL_RIGHT_SH      10
    #define LOC_REDUCTION   12

    #define LS_LEFT_SH      10
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
    uint8_t lifespan = 255;
    uint8_t spawnRate = 255;
    uint8_t spawnRateVar = 0;
    int8_t velocity = 1;
    uint8_t velocityVar = 0;
    int8_t acceleration = 0;
    uint8_t accelVar = 0;
    int8_t hueCycleRate = 2;
    uint8_t hueVar = 0;

    // state vars
    uint32_t     realLifespan;
    uint16_t hue = 0;
    uint8_t numParticles = 0, newParticleIdx = 0;
    unsigned int spawnDelay, counter = 0, thisSpawnDelay;

    struct Particle{
        uint32_t birthtime;
        uint8_t start_loc, hue;
        int8_t vel_mod, accel_mod;
    } particle[MAX_PARTICLES];

    public:
    Particles(){
        numParams = 10;
        params = new parameter_t[numParams];
        params[MODE].max = 2;
        params[MODE].scaleColor = CRGB::Amethyst;
        params[MODE].isChunks = true;
        params[MODE].ticksToAdjust = 3;
        params[VELOCITY].max = 127;
        params[ACCELERATION].max = 127;
        params[COLOR_CYCLE_RATE].max = 20;

        params[SPAWN_RATE].scaleColor       = CRGB::DarkBlue;
        params[SPAWN_RATE_VAR].scaleColor   = CRGB::LightBlue;
        params[COLOR_CYCLE_RATE].scaleColor = CRGB::DarkMagenta;
        params[COLOR_VAR].scaleColor        = CRGB::Pink;
        params[VELOCITY].scaleColor         = CRGB::DarkGreen;
        params[VELOCITY_VAR].scaleColor     = CRGB::LightGreen;
        params[ACCELERATION].scaleColor     = CRGB::Orange;
        params[ACCELERATION_VAR].scaleColor = CRGB::Yellow;
    };

    int getSpawnDelay(){
        return spawnRate * SPEED_SCALE_BASE * 8;
    };

    void initAnim(){
        random16_add_entropy(millis() & MAKE_MASK(TS_W));
        thisSpawnDelay = getSpawnDelay();
        realLifespan = lifespan << LS_LEFT_SH;

        for(int i =0; i<MAX_PARTICLES; i++) particle[i].birthtime = 0; // Reset Particles
        numParticles = 0;
        #ifdef DEBUG
        #endif
    }

    int adjParam(uint8_t paramIdx, int change){
        random16_add_entropy(millis() & MAKE_MASK(16));
        switch(paramIdx){
            case MODE:
            {
                mode = (Mode)clamp_un0(mode + (change > 0? 1:-1), 2);
                return mode;
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
                return velocity;

            case VELOCITY_VAR:
                velocityVar = CLAMP_8(velocityVar + change);
                return velocityVar;

            case ACCELERATION:
                acceleration = CLAMP_S8(acceleration + change);
                return acceleration;

            case ACCELERATION_VAR:
                accelVar = CLAMP_8(accelVar + change);
                return accelVar;

            default: return 0;
        }
        return 0;
    }

    void killParticle(int i){
        particle[i].birthtime = 0;
        numParticles--;
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
                thisSpawnDelay = getSpawnDelay() + variance;
                
                while(particle[newParticleIdx].birthtime){	// Find next available slot
                    newParticleIdx = (newParticleIdx + 1) % MAX_PARTICLES;
                }
                // Build new particle
                numParticles++;

                uint8_t loc_rand = random8() % ENDPOINT;
                int hueRand = scale8(random8(), hueVar);
                int velRand = scale_by_n(random8()-127, (int)velocityVar, 255);
                int accelRand = scale_by_n(random8()-127, (int)accelVar, 255);

                particle[newParticleIdx].birthtime  = now;
                particle[newParticleIdx].start_loc  = loc_rand;
                particle[newParticleIdx].hue        = (uint8_t)(hue >> HUE_CTR_MULT) + hueRand;
                particle[newParticleIdx].vel_mod     = velRand;
                particle[newParticleIdx].accel_mod   = accelRand;
                
                hue += hueCycleRate;
            }
        }

        for (int i = 0; i < MAX_PARTICLES; i++){
            if(particle[i].birthtime){
                unsigned long age = (now - particle[i].birthtime);
                if(age < 0){
                    age += TS_MOD;
                }
                age *= speed;
                if(age > realLifespan){
                    killParticle(i);
                    continue;
                }                
                if(!age){
                    age = 1;
                }

                int veloc = (velocity + particle[i].vel_mod) << VEL_LEFT_SH;
                int fullAccel = (acceleration + particle[i].accel_mod) * age;
                int reducedAccel = fullAccel >> ACCEL_RIGHT_SH;
                veloc = clamp_sn(veloc + reducedAccel, MAX_VELOC);
                int location = veloc * age;

                if(particle[i].start_loc) location += ((LOC_MAX/255) * particle[i].start_loc);

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

                int numspread = scale_to_n(veloc, MAX_VELOC, 4) + 1;
                //Serial.printf("%d - ", numspread);
                int fullFractionalValue = 256/numspread;
                for(int i = 1; i<=numspread; i++){
                    uint8_t scaledFraction = scale_to_n((int)loc_fractional, 255, fullFractionalValue);
                    uint8_t forwardValue =  (numspread - i ) * fullFractionalValue + scaledFraction;
                    uint8_t revValue =  (numspread - i + 1) * fullFractionalValue - scaledFraction;
                    if(loc_idx + i < ENDPOINT){
                        ledData.leds[loc_idx + i] += CHSV(particle[i].hue, ledData.saturation, forwardValue);
                    }
                    if(loc_idx - i > 0)
                        ledData.leds[loc_idx - i] += CHSV(particle[i].hue, ledData.saturation, revValue);
                }
                ledData.leds[loc_idx] += CHSV(particle[i].hue, ledData.saturation, 255);
            }
        }
        //Serial.println();
    }
};










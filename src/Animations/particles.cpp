#include "../Animation.h"
#include "util.h"
//#include "radii.h"

struct Particles: public AnimationBase{

    #define ENDPOINT 193
    #define MAX_PARTICLES 30
    #define LOCATION_SHIFT 24
    #define MAX_LOCATION  (unsigned int)(ENDPOINT << LOCATION_SHIFT)
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
    
    /*
    [-pos-10-][-hue-8][]
    00000000000000000000000000000000
    */
    struct Particle {
        unsigned int location;
        int  velocity;
        int  acceleration;
        uint8_t hue;
        bool active = false;
    } particle[MAX_PARTICLES];


    public:
    Particles(){
        accel    =      0x00004000;
        velocity =      0x00030000;
        spawnDelay = 10;
        numParams = 9;
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

    void drawFrame(uint8_t millisSinceLastFrame){

        millisSinceLastFrame = 1;
        #ifdef DEBUG
        //Serial.println();
        #endif
        FastLED.clear();

                // Spawn
        if((millisSinceLastFrame > 0) && (numParticles < MAX_PARTICLES)){
            //Serial.printf("counter: %d\t#p: %d\tidx: %d\tstps: %d >>>  ", counter, numParticles, newParticleIdx, millisSinceLastFrame);
            counter += millisSinceLastFrame;
            if(counter >= spawnDelay){
                counter -= spawnDelay;
                while(particle[newParticleIdx].active){	// Find next available slot
                    newParticleIdx = (newParticleIdx + 1) % MAX_PARTICLES;
                }
                // Build new particle
                numParticles++;
                Particle *p = &particle[newParticleIdx];
                p->active = true;
                p->location = velocity > 0? 0 : MAX_LOCATION;//  beginning or end based on vel? 0 : end
                p->velocity = velocity  + 0;// random variation;
                p->acceleration = accel  + 0;// random variation;
                p->hue = hue;// random variation;
                hue += 8;
                
                //Serial.printf("hue: %d\n", hue);
                // Set up next particle
            }
        }

        Particle *p;
        for (int i = 0; i < MAX_PARTICLES; i++){
                //Serial.print(particle[i].active? 'p':'-');
            if(particle[i].active){
                p = &particle[i];
                
                
                // draw
                // spread based on velocity
                uint8_t leftPx = p->location >> LOCATION_SHIFT;
                unsigned int leftVal = p->location & 0x00FFFFFF;
                // uint8_t rightVal = 0xFF - leftVal;

                //Serial.printf("idx: %d\tlp: %d\tlv: %d\n", i, leftPx, leftVal);//, rightVal);
                //Serial.printf("color: %d\n", p->hue);
                ledData.leds[leftPx] = CHSV(p->hue, ledData.saturation, 255);//leftVal);
                // if(leftPx < (NUM_LEDS - 1)){
                //     ledData.leds[leftPx + 1] = CHSV(p->color, ledData.saturation, rightVal);
                // }
                
                if(millisSinceLastFrame){
                    // move
                    // Location is += t*velocity
                    p->location += (p->velocity * millisSinceLastFrame) ;
                    if(p->location > MAX_LOCATION || p->location < 1){
                        p->active = false;
                        numParticles--;
                        //Serial.print("deleted");
                    } else {
                        p->velocity += (p->acceleration * millisSinceLastFrame);
                        //Serial.printf("loc: %d\tacc: %d\tvel: %d", p->location, p->acceleration, p->velocity);
                    }

                    
                    // Serial.printf("idx: %d\ta: %d\tv: %d\tp: %d\n", i, p->acceleration, p->velocity, p->location);
                }
            }
        }
        //Serial.print(".");
    }
};










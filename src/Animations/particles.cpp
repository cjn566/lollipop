
// spawn rate 		/ variability
// spawn velocity 	/ variability
// acceleration 	/ variability
// color 			/ variability
// color cycle rate

// baseSpawnDelay


// struct {
// 	delay;
// 	velocity;
// 	acceleration;
// 	color;
// } spawn;

// uint8_t counter, numParticles, newParticleIdx;

// struct Particle {
// 	uint16_t location;
// 	uint16_t velocity;
// 	fract16 acceleration;
// 	CRGB color;
// } particle[MAX_PARTICLES];

// void drawFrame(){	
// 	// Spawn
// 	if(numParticles < MAX_PARTICLES){
// 		counter += $.stepsSinceLastFrame;
// 		if(counter >= spawnDelay){
// 			while(particles[newParticleIdx]){	// Find next available slot
// 				newParticleIdx = qmod(newParticleIdx, 1, MAX_PARTICLES - 1);
// 			}
			
// 			// Build new particle
// 			Particle p = particle[newParticleIdx];
// 			p.location = /  beginning or end based on which param? 0 : end
// 			p.velocity = baseVelocity  + / random variation;
// 			p.acceleration = baseAcceleration  + / random variation;
// 			p.color = baseHue  + / random variation;
			
			
// 			// Set up next particle
// 			counter = 0;
// 			spawn.delay = baseSpawnDelay + / random variation;
// 		}
// 	}
	
// 	$leds.clear();
// 	int i = 0;
// 	Particle p;
// 	for (int i = 0; i < MAX_PARTICLES; i++){
// 		if(particle[i]){
// 			p = particle[i];
			
// 			// draw
// 			/ spread based on velocity
// 			$.leds[p.location] += p.color;
			
// 			// move
// 			p.location += p.velocity;
// 			if(location > maxLocation){
// 				/ delete p;
// 			} else {
// 				p.velocity *= p.acceleration;
// 			}
			
// 			i++;
// 		}
// 	}
// }

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



#pragma once

// #include "../Animation.h"
// #include "util.h"
// //#include "radii.h"

// struct *: public AnimationBase{

//     #define * 5
//     enum ParamName {
//         SPOKE,
//     };

//     // params vars
//     uint8_t * = 3;

//     // state vars
//     uint8_t * = 0;

//     public:
//     *(){
//         numParams = *;
//         params = new parameter_t[numParams];
//         params[*] = {CRGB::Red, 4};
//     };

//     void initAnim(){
//     }

//     void initParam(uint8_t paramIdx){
//         switch(paramIdx){
//             case *:
//                 break;
//         }
//     }

//     void adjParam(uint8_t paramIdx, bool up){
//         switch(paramIdx){
//             case *:
//                 * += INCDEC;
//                 break;
//         }
//     }

//     void drawFrame(uint8_t stepsSinceLastFrame){
//         #ifdef DEBUG
//         //Serial.println();
//         #endif
//         for(int i=0;i< NUM_LEDS ;i++){
//             ledData.leds[i] = 
//         }
//     }
// };











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
















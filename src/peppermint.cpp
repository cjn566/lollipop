#include "Animation.h"


// declaration of 'const uint8_t Peppermint::NUM_PARAMS' outside of class is not definition [-fpermissive]
// conflicting declaration 'uint8_t Peppermint::NUM_PARAMS'
// duplicate initialization of 'Peppermint::NUM_PARAMS'
// declaration is incompatible with "const uint8_t Peppermint::NUM_PARAMS" (declared at line 33 of "C:\Users\cjn56\Documents\PlatformIO\Projects\Lollipop\src\Animation.h")

//uint8_t Peppermint::NUM_PARAMS = 2;
//const uint8_t MAX_SPOKES = 5;
//uint8_t deltaHue = 3;
//uint8_t currAngle = 0;
//uint8_t numSpokes = 3;
//uint8_t halfAngleBetweenSpokes;
//uint8_t angleBetweenSpokes;

// 'params' in 'struct Peppermint' does not name a type
//Peppermint::params[peppermint.NUM_PARAMS] = {
    //Parameter{CRGB::Chocolate,4},
    //Parameter{CRGB::Chocolate,4}
//};


// this is the constructor, where you initialize each instance of the class
//
Peppermint::Peppermint(uint8_t params, uint8_t spokes)
{
  NUM_PARAMS = params;
  MAX_SPOKES = spokes;
  deltaHue = 3;
  currAngle = 0;
  numSpokes = 3;
  halfAngleBetweenSpokes = 0;
  angleBetweenSpokes = 0;
}


// Is there a way to avoid qualifying each member with "Peppermint::" ? 
// Some way to say everything inside of some {} set is referring to Peppermint?
void Peppermint::initAnim() {
    halfAngleBetweenSpokes = (256 / (numSpokes * 2));
    angleBetweenSpokes = halfAngleBetweenSpokes * 2;
    if(!angleBetweenSpokes) angleBetweenSpokes -= 1;
    #ifdef DEBUG
    Serial.printf("spokes: %d, angle: %d\n", numSpokes, angleBetweenSpokes);
    #endif
}

void Peppermint::initParam(uint8_t p){
    switch(p){
        case 0:
            break;
    }
}

void Peppermint::adjParam(uint8_t param, bool up){
    switch(param){
        case 0:
            deltaHue += INCDEC;
            if(!deltaHue) deltaHue = numSpokes;
            else if(numSpokes > numSpokes) deltaHue = 1;
            break;
        case 1:
            numSpokes += INCDEC;
            if(!numSpokes) numSpokes = MAX_SPOKES;
            else if(numSpokes > MAX_SPOKES) numSpokes = 1;
            Peppermint::initAnim(); // <-- "a nonstatic member reference must be relative to a specific object"
            break;
    }
}

// Etc, etc .. 

void Peppermint::drawFrame(){

    #ifdef DEBUG
    //Serial.println();
    #endif

    currAngle = mod8(currAngle + globalState.stepsSinceLastFrame, angleBetweenSpokes);
    //uint8_t angle;

    for(int i=0;i< NUM_LEDS ;i++){
        //uint8_t anglePlusRotation = mod8(sub8(radii[i][ANGLE], currAngle), angleBetweenSpokes);
        //if(anglePlusRotation > halfAngleBetweenSpokes){
            //anglePlusRotation = angleBetweenSpokes - anglePlusRotation;
        //}
	uint8_t anglePlusRotation = 1; // quick hack to get this stuff to compile
        uint8_t angleWithFullHueMultplier = anglePlusRotation * deltaHue;
        globalState.leds[i] = CHSV(angleWithFullHueMultplier, globalState.saturation, 255);
    }
}

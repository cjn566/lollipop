#include "Animation.h"
#define vf void Peppermint

// ------------------ PEPPERMINT ----------------


static struct Peppermint : AnimationBase{
//     static const uint8_t NUM_PARAMS;
//     virtual void initAnim();
//     virtual void initParam(uint8_t p);
//     virtual void adjParam(uint8_t param, bool up);
//     virtual void drawFrame();

// class Peppermint{

const uint8_t MAX_SPOKES = 5;
uint8_t deltaHue = 3;
uint8_t currAngle = 0;
uint8_t numSpokes = 3;
uint8_t halfAngleBetweenSpokes;
uint8_t angleBetweenSpokes;

Peppermint::params[peppermint.NUM_PARAMS] = {
    Parameter{CRGB::Chocolate,4},
    Parameter{CRGB::Chocolate,4}
};

vf::initAnim(){
    halfAngleBetweenSpokes = (256 / (numSpokes * 2));
    angleBetweenSpokes = halfAngleBetweenSpokes * 2;
    if(!angleBetweenSpokes) angleBetweenSpokes -= 1;
    #ifdef DEBUG
    Serial.printf("spokes: %d, angle: %d\n", numSpokes, angleBetweenSpokes);
    #endif
}

void initParam(uint8_t p){
    switch(p){
        case 0:
            drawScale.init(drawScale.CHUNK, deltaHue, CRGB::Brown, 3);
            break;
    }
}

void adjParam(uint8_t param, bool up){
    switch(param){
        case 0:
            deltaHue += INCDEC;
            if(!deltaHue) deltaHue = numSpokes;
            else if(numSpokes > numSpokes) deltaHue = 1;
            drawScale.setValue(deltaHue);
            break;
        case 1:
            numSpokes += INCDEC;
            if(!numSpokes) numSpokes = MAX_SPOKES;
            else if(numSpokes > MAX_SPOKES) numSpokes = 1;
            initAnim();
            break;
    }
}

void drawFrame(){

    #ifdef DEBUG
    //Serial.println();
    #endif

    currAngle = mod8(currAngle + $.stepsSinceLastFrame, angleBetweenSpokes);
    uint8_t angle;

    for(int i=0;i< NUM_LEDS ;i++){
        uint8_t anglePlusRotation = mod8(sub8(radii[i][ANGLE], currAngle), angleBetweenSpokes);
        if(anglePlusRotation > halfAngleBetweenSpokes){
            anglePlusRotation = angleBetweenSpokes - anglePlusRotation;
        }
        uint8_t angleWithFullHueMultplier = anglePlusRotation * deltaHue;
        $.leds[i] = CHSV(angleWithFullHueMultplier, $.saturation, 255);
    }
}


// static struct Peppermint : AnimationBase{
// //     static const uint8_t NUM_PARAMS;
// //     virtual void initAnim();
// //     virtual void initParam(uint8_t p);
// //     virtual void adjParam(uint8_t param, bool up);
// //     virtual void drawFrame();

// // class Peppermint{

//     static const uint8_t NUM_PARAMS = 2;
//     const uint8_t MAX_SPOKES = 5;
//     uint8_t deltaHue = 3;
//     uint8_t currAngle = 0;
//     uint8_t numSpokes = 3;
//     uint8_t halfAngleBetweenSpokes;
//     uint8_t angleBetweenSpokes;



//     Parameter params[NUM_PARAMS] = {
//         Parameter{CRGB::Chocolate,4},
//         Parameter{CRGB::Chocolate,4}
//     };

//     void initAnim(){
//         halfAngleBetweenSpokes = (256 / (numSpokes * 2));
//         angleBetweenSpokes = halfAngleBetweenSpokes * 2;
//         if(!angleBetweenSpokes) angleBetweenSpokes -= 1;
//         #ifdef DEBUG
//         Serial.printf("spokes: %d, angle: %d\n", numSpokes, angleBetweenSpokes);
//         #endif
//     }

//     void initParam(uint8_t p){
//         switch(p){
//             case 0:
//                 drawScale.init(drawScale.CHUNK, deltaHue, CRGB::Brown, 3);
//                 break;
//         }
//     }

//     void adjParam(uint8_t param, bool up){
//         switch(param){
//             case 0:
//                 deltaHue += INCDEC;
//                 if(!deltaHue) deltaHue = numSpokes;
//                 else if(numSpokes > numSpokes) deltaHue = 1;
//                 drawScale.setValue(deltaHue);
//                 break;
//             case 1:
//                 numSpokes += INCDEC;
//                 if(!numSpokes) numSpokes = MAX_SPOKES;
//                 else if(numSpokes > MAX_SPOKES) numSpokes = 1;
//                 initAnim();
//                 break;
//         }
//     }

//     void drawFrame(){

//         #ifdef DEBUG
//         //Serial.println();
//         #endif

//         currAngle = mod8(currAngle + $.stepsSinceLastFrame, angleBetweenSpokes);

//         // for(int i=0;i< numSpokes ;i++){
//         //     spokeAngles[i] = i*angleBetweenSpokes + currAngle;
//         // }

//         uint8_t angle;

//         for(int i=0;i< NUM_LEDS ;i++){

//             // int s;
//             // for(s=0;s< numSpokes ;s++){
//             //     angle = sub8(radii[i][ANGLE], spokeAngles[s]);
//             //     if (angle < angleBetweenSpokes){
//             //         if(angle > halfAngleBetweenSpokes){
//             //             angle = sub8(angleBetweenSpokes, angle);
//             //         }
//             //         break;
//             //     }
//             // }                
//             // #ifdef DEBUG
//             // if(angle > angleBetweenSpokes){
//             //     //Serial.printf("led: %d  \tangle: %d\tspoke: %d\n", i, angle, s);
//             // }
//             // #endif
            

//             //bool sideOfSpoke = true;
//             uint8_t anglePlusRotation = mod8(sub8(radii[i][ANGLE], currAngle), angleBetweenSpokes);
//             //uint8_t anglePlusRotation = angleFromUnrotatedSpoke + currAngle;
//             if(anglePlusRotation > halfAngleBetweenSpokes){
//                 //sideOfSpoke = false;
//                 anglePlusRotation = angleBetweenSpokes - anglePlusRotation;
//             }
//             uint8_t angleWithFullHueMultplier = anglePlusRotation * deltaHue;
//             //uint8_t angleWithFullHueMultplier = anglePlusRotation * 1;


//             //uint8_t hue = sub8(currRad, radii[i][ANGLE]) + radii[i][DISTANCE];
//             $.leds[i] = CHSV(angleWithFullHueMultplier, $.saturation, 255);
//         }
//     }

// } peppermint;
#pragma once
#include "ofMain.h"
//--------------------------------------------------------------
// Represents a particle for explosion/exhaust effects.
// 
//  Student Name: Viola Yasuda
//  Date: 12-09-2022
class Particle {
public:
	Particle();
	void    integrate();
	void    draw();
	float   age(); // sec
	ofVec3f position;
	ofVec3f velocity;
	ofVec3f forces;
	float   lifespan;
	float   radius;
	float   birthtime;
};
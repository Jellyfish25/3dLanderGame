#include "Particle.h"
//--------------------------------------------------------------
// Represents a particle for explosion/exhaust effects.
// 
//  Student Name: Viola Yasuda
//  Date: 12-09-2022

// Constructor for the Particle
Particle::Particle() {
	// initialize particle with some reasonable values first;
	position.set(0, 0, 0);
	velocity.set(0, 0, 0);
	forces.set(0, 0, 0);
	lifespan = 5;
	radius = .1;
	birthtime = 0;
}

// Physics integrator for the Particle
void Particle::integrate() {
	// interval for this step
	float dt = 1.0 / 60.0;
	// update position based on velocity
	position += (velocity * dt);
	// update velocity with accumulated paritcles forces
	velocity += forces * dt;
	// clear forces on particle (they get re-added each step)
	forces.set(0, 0, 0);
}

// Draws the particle
void Particle::draw() {
	ofSetColor(ofMap(age(), 0, lifespan, 255, 10), 0, 0);
	ofDrawSphere(position, radius);
}

// Returns the particle's age in seconds
float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime) / 1000.0;
}
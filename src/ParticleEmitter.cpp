
//  Kevin M. Smith - CS 134 SJSU

#include "ParticleEmitter.h"

// Constructor for the ParticleEmitter
ParticleEmitter::ParticleEmitter() {
	sys = new ParticleSystem();
	createdSys = true;
	init();
}

// Deallocates particle system if emitter created one internally
ParticleEmitter::~ParticleEmitter() {
	if (createdSys) delete sys;
}

// Initializes the instance variables
void ParticleEmitter::init() {
	rate = 1;
	velocity = ofVec3f(0, -20, 0);
	lifespan = 3;
	started = false;
	oneShot = false;
	fired = false;
	lastSpawned = 0;
	radius = 1;
	particleRadius = .1;
	visible = false;
	type = DirectionalEmitter;
	groupSize = 1;
}

// Draws the emitter and the particles in its ParticleSystem
void ParticleEmitter::draw() {
	if (visible) {
		ofDrawSphere(position, radius / 10);
	}
	sys->draw();
}

// Starts the emitter
void ParticleEmitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

// Stops the emitter
void ParticleEmitter::stop() {
	started = false;
	fired = false;
}

// Updates the ParticleEmitter and its ParticleSystem
void ParticleEmitter::update() {
	float time = ofGetElapsedTimeMillis();
	if (oneShot && started) {
		if (!fired) {
			// spawn a new particle(s)
			for (int i = 0; i < groupSize; i++) spawn(time);
			lastSpawned = time;
		}
		fired = true;
		stop();
	}
	else if (((time - lastSpawned) > (1000.0 / rate)) && started) {
		// spawn a new particle(s)
		for (int i = 0; i < groupSize; i++) spawn(time);
		lastSpawned = time;
	}
	sys->update();
}

// spawn a single particle.  time is current time of birth
void ParticleEmitter::spawn(float time) {
	Particle particle;
	// set initial velocity and position
	// based on emitter type
	switch (type) {
	case RadialEmitter:
	{
		ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
		float speed = velocity.length();
		particle.velocity = dir.getNormalized() * speed;
		particle.position.set(position);
	}
	break;
	case DirectionalEmitter:
		particle.velocity = velocity;
		particle.position.set(position);
		break;
	}

	// other particle attributes
	particle.lifespan = lifespan;
	particle.birthtime = time;
	particle.radius = particleRadius;

	// add to system
	sys->add(particle);
}

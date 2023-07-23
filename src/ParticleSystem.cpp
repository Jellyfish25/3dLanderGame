
// Kevin M.Smith - CS 134 SJSU

#include "ParticleSystem.h"

// Adds a particle to the system
void ParticleSystem::add(const Particle& p) {
	particles.push_back(p);
}

// Adds a force to the system
void ParticleSystem::addForce(ParticleForce* f) {
	forces.push_back(f);
}

// Draws the particle cloud
void ParticleSystem::draw() {
	for (int i = 0; i < particles.size(); i++) particles[i].draw();
}

// Removes a particle from the system at index i
void ParticleSystem::remove(int i) {
	particles.erase(particles.begin() + i);
}

// Resets it so the forces can be reapplied
void ParticleSystem::reset() {
	for (int i = 0; i < forces.size(); i++) forces[i]->applied = false;
}

// Sets the lifespan of each particle to l
void ParticleSystem::setLifespan(float l) {
	for (int i = 0; i < particles.size(); i++) particles[i].lifespan = l;
}

// Updates the ParticleSystem
void ParticleSystem::update() {
	// check if empty and just return
	if (particles.size() == 0) return;

	vector<Particle>::iterator p = particles.begin();
	vector<Particle>::iterator tmp;
	// check which particles have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, we need to use an iterator.
	while (p != particles.end()) {
		if (p->lifespan != -1 && p->age() > p->lifespan) {
			tmp = particles.erase(p);
			p = tmp;
		}
		else p++;
	}
	// update forces on all particles first 
	for (int i = 0; i < particles.size(); i++) {
		for (int k = 0; k < forces.size(); k++) {
			if (!forces[k]->applied)
				forces[k]->updateForce(&particles[i]);
		}
	}
	// update all forces only applied once to "applied"
	// so they are not applied again.
	for (int i = 0; i < forces.size(); i++) {
		if (forces[i]->applyOnce) forces[i]->applied = true;
	}
	// integrate all the particles in the store
	for (int i = 0; i < particles.size(); i++) particles[i].integrate();
}


// Gravity Force Field 
GravityForce::GravityForce(const ofVec3f& g) {
	gravity = g;
}

void GravityForce::set(ofVec3f g) {
	gravity = g;
}

void GravityForce::updateForce(Particle* particle) {
	particle->forces += gravity;
}


// Turbulence Force Field 
TurbulenceForce::TurbulenceForce(const ofVec3f& turbV) {
	turb = turbV;
}

void TurbulenceForce::set(ofVec3f turbV) {
	turb = turbV;
}

void TurbulenceForce::updateForce(Particle* particle) {
	// We are going to add a little "noise" to a particles
	// forces to achieve a more natual look to the motion
	particle->forces.x += ofRandom(-turb.x, turb.x);
	particle->forces.y += ofRandom(-turb.y, turb.y);
	particle->forces.z += ofRandom(-turb.z, turb.z);
}


// Impulse Radial Force - this is a "one shot" force that
// eminates radially outward in random directions.
ImpulseRadialForce::ImpulseRadialForce(float magnitude) {
	this->magnitude = magnitude;
	applyOnce = true;
}

void ImpulseRadialForce::set(float magnitude) {
	this->magnitude = magnitude;
}

void ImpulseRadialForce::updateForce(Particle* particle) {
	// we basically create a random direction for each particle
	// the force is only added once after it is triggered.
	ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
	particle->forces += dir.getNormalized() * magnitude;
}

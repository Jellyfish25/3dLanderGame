#pragma once
//  Kevin M. Smith - CS 134 SJSU
#include "ofMain.h"
#include "Particle.h"


//  Pure Virtual Function Class - must be subclassed to create new forces.
class ParticleForce {
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(Particle*) = 0;
};

// Represents a system of particles such as an explosion cloud or exhaust fumes
class ParticleSystem {
public:
	void add(const Particle&);
	void addForce(ParticleForce*);
	void draw();
	void remove(int);
	void reset();
	void setLifespan(float);
	void update();
	vector<ParticleForce*> forces;
	vector<Particle> particles;
};

// Represents a gravitational force that can be applied to a ParticleSystem
class GravityForce : public ParticleForce {
	ofVec3f gravity;
public:
	GravityForce(const ofVec3f& gravity);
	void set(ofVec3f);
	void updateForce(Particle*);
};

// Represents an air turbulence force that can be applied to a ParticleSystem
class TurbulenceForce : public ParticleForce {
	ofVec3f turb;
public:
	TurbulenceForce(const ofVec3f& turb);
	void set(ofVec3f);
	void updateForce(Particle*);
};

// Represents an impulse radial force (explosion) that can be applied to a ParticleSystem
class ImpulseRadialForce : public ParticleForce {
	float magnitude;
public:
	ImpulseRadialForce(float magnitude);
	void updateForce(Particle*);
	void set(float magnitude);
};

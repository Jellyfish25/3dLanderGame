#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "ParticleSystem.h"

typedef enum { DirectionalEmitter, RadialEmitter } EmitterType;

//  Base class for any object that needs a transform.
class TransformObject {
public:
	TransformObject() {
		position = ofVec3f(0, 0, 0);
		scale = ofVec3f(1, 1, 1);
		rotation = 0;
	}
	void setPosition(const ofVec3f& pos) {
		position = pos;
	}
	ofVec3f position, scale;
	float rotation;
};

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
class ParticleEmitter : public TransformObject {
public:
	ParticleEmitter();
	~ParticleEmitter();
	void init();
	void draw();
	void start();
	void stop();
	void update();
	void spawn(float time);

	void setLifespan(const float life) { lifespan = life; }
	void setVelocity(const ofVec3f& vel) { velocity = vel; }
	void setRate(const float r) { rate = r; }
	void setParticleRadius(const float r) { particleRadius = r; }
	void setEmitterType(EmitterType t) { type = t; }
	void setGroupSize(int s) { groupSize = s; }
	void setOneShot(bool s) { oneShot = s; }
	
	ParticleSystem* sys;
	float rate; // per sec
	bool oneShot;
	bool fired;
	ofVec3f velocity;
	float lifespan; // sec
	bool started;
	float lastSpawned; // ms
	float particleRadius;
	float radius;
	bool visible = false;
	int groupSize; // number of particles to spawn in a group
	bool createdSys;
	EmitterType type;
};

#pragma once
//--------------------------------------------------------------
// 3D landing game
// 
//  Student Names: Dimitar Dimitrov and Viola Yasuda
//  Date: 12-09-2022
#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include <glm/gtx/intersect.hpp>
#include "ParticleEmitter.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	// custom functions
	void drawAxis(ofVec3f);
	float getAltitude();
	glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p, glm::vec3 n);
	ofVec3f heading();
	ofVec3f siding();
	void loadExhVbo();
	void loadExpVbo();
	void initLightingAndMaterials();
	void updateLighting();
	void integrate();
	bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f& point);
	bool raySelectWithOctree(ofVec3f& pointRet);
	void setCameraTarget();

	float remainingFuelTime;// = 120.0;
	const float initialAngle = lander.getRotationAngle(0);
	glm::vec3 mouseDownPos;
	glm::vec3 mouseLastPos;
	Octree octree;
	ofEasyCam cam[3];
	int camToView = 0;
	ofImage backgroundImage;
	ofxAssimpModelLoader terrain;
	ofxAssimpModelLoader lander;
	Box landerBox;
	TreeNode selectedNode;
	vector<Box> colBoxList;
	// Physics Data
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 thrustForce = glm::vec3(0, 0, 0);
	glm::vec3 impulseForce = glm::vec3(0, 0, 0);
	float angularAcc = 0;
	float angularVel = 0;
	// explosionEmitter data
	ParticleEmitter explosionEmitter;
	TurbulenceForce* turbForce;
	GravityForce* gravityForce;
	ImpulseRadialForce* explosionForce;
	ofxFloatSlider radius;
	ofxFloatSlider lifespan;
	ofxFloatSlider explosionForceVal;
	// exhaustEmitter data
	ParticleEmitter exhaustEmitter;
	ofxIntSlider exhaustRate;
	ofxVec3Slider particleTurb;
	// textures
	ofTexture  particleTex;
	ofTexture explosionTex;
	// shaders
	ofVbo exhVbo;
	ofVbo expVbo;
	ofShader shader;
	// Lighting
	ofLight keyLight, fillLight, rimLight;
	// UI Components
	ofxIntSlider numLevels;
	ofxFloatSlider gravityVal;
	ofxVec3Slider turbulenceVal;
	ofxFloatSlider thrustVal;
	ofxFloatSlider angularAccVal;
	ofxFloatSlider dampingVal;
	ofxFloatSlider restitutionVal;
	ofxIntSlider fuelVal;
	ofxToggle headingTog, sidingTog;
	ofxPanel gui;
	bool bHide;
	// Booleans
	map<int, bool> keymap;
	bool bDisplayOctree = false;
	bool bDisplayPoints;
	bool bExploded = false;
	bool bInDrag = false;
	bool bIsThrusting = false;
	bool bLanderSelected = false;
	bool bPointSelected = false;
	bool bShowAltitude = true;
	bool bShowWireframe;
	bool bGameOn = false;
	// Sound effects
	ofSoundPlayer explosionSound;
	ofSoundPlayer thrustSound;
	ofSoundPlayer backgroundMusic;
};

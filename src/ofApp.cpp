//--------------------------------------------------------------
// 3D landing game
// 
//  Student Names: Dimitar Dimitrov and Viola Yasuda
//  Date: 12-09-2022
#include "ofApp.h"
#include "ray.h"
#include "Util.h"

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
// Student Names: Dimitar Dimitrov and Viola Yasuda
void ofApp::setup() {
	if (!backgroundImage.load("images/background2.png")) {
		cout << "Can't load background image" << endl;
		ofExit();
	}
	// texture loading
	ofDisableArbTex();     // disable rectangular textures
	if (!ofLoadImage(particleTex, "images/particleImage.png")) {
		cout << "Particle Texture File not found" << endl;
		ofExit();
	}
	if (!ofLoadImage(explosionTex, "images/explosionImage.png")) {
		cout << "Explosion Texture File not found" << endl;
		ofExit();
	}
	bShowWireframe = false;
	bDisplayPoints = false;
	cam[0].setDistance(25);
	cam[0].setNearClip(1);
	cam[0].setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam[0].disableMouseInput();
	cam[1].setDistance(50);
	cam[1].setNearClip(1);
	cam[1].setFov(65.5);
	cam[1].disableMouseInput();
	cam[2].setNearClip(1);
	cam[2].disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
	initLightingAndMaterials(); // setup rudimentary lighting 
	if (!terrain.loadModel("geo/kitchen.obj")) {
		cout << "Error: Can't load terrain model\n";
		ofExit();
	}
	terrain.setScaleNormalization(false);
	if (!lander.loadModel("geo/tuna.obj")) {
		cout << "Error: Can't load player model\n";
		ofExit();
	}
	lander.setScaleNormalization(false);
	lander.setPosition(0, 0, 0);
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	landerBox = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	// create sliders for testing
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(fuelVal.setup("Fuel", 10, 0, 120));
	gui.add(gravityVal.setup("Gravity", 1.62, 0, 10));
	gui.add(turbulenceVal.setup("Player Turbulence", ofVec3f(1, 0, 1), ofVec3f(0, 0, 0), ofVec3f(15, 15, 15)));
	gui.add(thrustVal.setup("Thrust", 5.0, 0.1, 20.0));
	gui.add(angularAccVal.setup("Angular Acc", 10.0, 0.1, 50.0));
	gui.add(dampingVal.setup("Damping", 0.99, 0.0, 1.0));
	gui.add(restitutionVal.setup("Restitution", 0.7, 0.0, 1.0));
	gui.add(headingTog.setup("Heading", false));
	gui.add(sidingTog.setup("Siding", false));
	// emitter stuff
	gui.add(radius.setup("Radius", 35, 1, 70));
	gui.add(lifespan.setup("Lifespan", 3.0, .1, 10.0));
	gui.add(explosionForceVal.setup("Explosion Force", 25, 0, 50));
	gui.add(particleTurb.setup("Particle Turbulence", ofVec3f(50, 0, 50), ofVec3f(0, 0, 0), ofVec3f(100, 100, 100)));
	gui.add(exhaustRate.setup("Exhaust Rate", 60, 1, 120));
	bHide = false;
	// set up explosion forces
	turbForce = new TurbulenceForce(ofVec3f(particleTurb));
	gravityForce = new GravityForce(ofVec3f(0, -gravityVal, 0));
	explosionForce = new ImpulseRadialForce(explosionForceVal);
	explosionEmitter.sys->addForce(turbForce);
	explosionEmitter.sys->addForce(gravityForce);
	explosionEmitter.sys->addForce(explosionForce);
	explosionEmitter.setOneShot(true);
	explosionEmitter.setEmitterType(RadialEmitter);
	explosionEmitter.setGroupSize(1000);
	// set up exhaust
	exhaustEmitter.sys->addForce(turbForce);
	exhaustEmitter.sys->addForce(gravityForce);
	exhaustEmitter.setEmitterType(DirectionalEmitter);
	// load the shader
	shader.load("shaders_gles/shader");
	// Create Octree for testing.
	ofMesh envMesh = ofMesh();
	for (int i = 0; i < terrain.getMeshCount(); i++) {
		cout << "Number of Verts: " << terrain.getMesh(i).getNumVertices() << endl;
		envMesh.append(terrain.getMesh(i));
	}
	octree.create(envMesh, 20);
	// load sounds
	explosionSound.setMultiPlay(true);
	if (!explosionSound.load("sounds/hitAudio.mp3")) {
		cout << "Can't load vine-boom.mp3\n";
	}
	//thrustSound.setVolume(5);
	if (!thrustSound.load("sounds/bubbles.wav")) {
		cout << "Can't load neer.mp3\n";
	}
	if (!backgroundMusic.load("sounds/MACINTOSHPLUS(320kbps).mp3")) {
		cout << "Can't load background music\n";
	}
	backgroundMusic.setLoop(true);
	backgroundMusic.setVolume(0.1);

	thrustSound.setLoop(true);
	thrustSound.setVolume(0.5);
	thrustSound.setSpeed(2.0);
	thrustSound.play();
	thrustSound.setPaused(true);
}

//--------------------------------------------------------------
// incrementally update scene (animation)
// Student Names: Dimitar Dimitrov and Viola Yasuda
void ofApp::update() {
	cam[1].lookAt(lander.getPosition());
	cam[2].setPosition(lander.getPosition() + glm::vec3(0, 10.5, -15));
	cam[2].rotateAroundDeg(lander.getRotationAngle(0), glm::vec3(0,1,0), lander.getPosition());
	cam[2].lookAt(lander.getPosition());
	// update explosionEmitter and exhaustEmitter
	ofSeedRandom();
	turbForce->set(ofVec3f(particleTurb));
	gravityForce->set(ofVec3f(0, -gravityVal, 0));
	explosionForce->set(explosionForceVal);
	explosionEmitter.setParticleRadius(radius);
	explosionEmitter.setLifespan(lifespan);
	explosionEmitter.setPosition(lander.getPosition());
	explosionEmitter.update();
	exhaustEmitter.setParticleRadius(radius);
	exhaustEmitter.setLifespan(lifespan);
	exhaustEmitter.setPosition(lander.getPosition());
	exhaustEmitter.setRate(exhaustRate);
	exhaustEmitter.update();

	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	landerBox = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	if (bGameOn) {
		if (!backgroundMusic.getIsPlaying()) {
			backgroundMusic.play();
		}
		thrustForce = glm::vec3(0, 0, 0);
		impulseForce = glm::vec3(0, 0, 0);
		angularAcc = 0;
		if (remainingFuelTime > 0) {
			if (keymap[OF_KEY_UP]) { // move forward
			//thrustForce.z = -1;
				thrustForce += heading();
			}
			else if (keymap[OF_KEY_DOWN]) { // move backward
				//thrustForce.z = 1;
				thrustForce -= heading();
			}
			if (keymap[OF_KEY_LEFT]) { // move left
				//thrustForce.x = -1;
				thrustForce += siding();
			}
			else if (keymap[OF_KEY_RIGHT]) { // move right
				//thrustForce.x = 1;
				thrustForce -= siding();
			}
			if (keymap[OF_KEY_SHIFT]) { // move up
				thrustForce.y = 1;
			}
			if (keymap['q']) { // rotate left
				angularAcc = angularAccVal;
			}
			else if (keymap['e']) { // rotate right
				angularAcc = -angularAccVal;
			}
		}
		colBoxList.clear();
		octree.intersect(landerBox, octree.root, colBoxList);
		if (colBoxList.size() > 0) {
			// P = (e + 1) * ( -v .dot. n ) * n;
			impulseForce = (restitutionVal + 1.0) * (glm::vec3(0, 0, 0) - (glm::dot(velocity, glm::vec3(0, 1, 0)))) * glm::vec3(0, 1, 0);
		}
		// TODO: try to normalize thrustForce
		integrate();
		float maxWinSpeed = 0.5, minLoseSpeed = 2;
		if (colBoxList.size() > 0 && -maxWinSpeed < velocity.x && velocity.x < maxWinSpeed
			&& -maxWinSpeed < velocity.y && velocity.y < maxWinSpeed
			&& -maxWinSpeed < velocity.z && velocity.z < maxWinSpeed) { // player wins
			bGameOn = false;
			bIsThrusting = false;
			velocity = glm::vec3(0, 0, 0);
		}
		else if (lander.getPosition().y < -60.0 || colBoxList.size() > 0 //note: if lander goes below 60 on y-axis, explode (out of bounds)
			&& (velocity.x < -minLoseSpeed || velocity.x > minLoseSpeed
			|| velocity.y < -minLoseSpeed || velocity.y > minLoseSpeed
			|| velocity.z < -minLoseSpeed || velocity.z > minLoseSpeed)) { // player loses, explodes!
			if (!bExploded) {
				explosionEmitter.sys->reset();
				explosionEmitter.start();
				explosionSound.play();
				bExploded = true;
			}
			bGameOn = false;
			bIsThrusting = false;
			velocity = glm::vec3(0, 0, 0);
		}
		if (bIsThrusting && !exhaustEmitter.started) {
			exhaustEmitter.start();
		}
		else if (!bIsThrusting && exhaustEmitter.started) {
			exhaustEmitter.stop();
		}
		if (bIsThrusting) {
			thrustSound.setPaused(false);
		}
		else  {
			thrustSound.setPaused(true);
		}
	}
	else {
		backgroundMusic.stop();
		backgroundMusic.setPosition(0);
		remainingFuelTime = fuelVal;
	}
	// update lighting
	updateLighting();
}

//--------------------------------------------------------------
// Draws the scene
// Student Names: Dimitar Dimitrov and Viola Yasuda
void ofApp::draw() {
	loadExhVbo();
	loadExpVbo();
	glDepthMask(false);
	ofSetColor(ofColor::white);
	backgroundImage.draw(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
	glDepthMask(true);
	cam[camToView].begin();
	ofPushMatrix();
	// draw all the lights
	keyLight.draw();
	fillLight.draw();
	rimLight.draw();
	if (bShowWireframe) { // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		terrain.drawWireframe();
		lander.drawWireframe();
		drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting(); // shaded mode
		terrain.drawFaces();
		ofMesh mesh;
		if (!bExploded) {
			lander.drawFaces();
			if (headingTog) {
				ofSetColor(ofColor::red);
				ofDrawLine(lander.getPosition(), lander.getPosition() + 10 * heading());
			}
			if (sidingTog) {
				ofSetColor(ofColor::green);
				ofDrawLine(lander.getPosition(), lander.getPosition() + 10 * siding());
			}
			if (bLanderSelected) {
				ofSetColor(ofColor::white);
				ofNoFill();
				Octree::drawBox(landerBox);
				// draw colliding boxes
				/*
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}*/
			}
			//exhaustEmitter.draw();
			ofSetColor(ofColor::white);
			ofNoFill();
			ofEnablePointSprites();
			shader.begin();
			particleTex.bind();
			exhVbo.draw(GL_POINTS, 0, (int)exhaustEmitter.sys->particles.size());
			particleTex.unbind();
			shader.end();
			ofDisablePointSprites();
		}
		else {
			//explosionEmitter.draw();
			//using shading instead
			ofEnablePointSprites();
			shader.begin();
			explosionTex.bind();
			expVbo.draw(GL_POINTS, 0, (int)explosionEmitter.sys->particles.size());
			explosionTex.unbind();
			shader.end();
			ofDisablePointSprites();
		}
	}
	drawAxis(ofVec3f(0, 0, 0));
	if (bDisplayPoints) { // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		terrain.drawVertices();
	}
	// recursively draw octree
	ofDisableLighting();
	int level = 0;
	if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}
	// if point selected, draw a sphere
	if (bPointSelected && camToView == 0) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam[0].getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}
	ofPopMatrix();
	cam[camToView].end();
	glDepthMask(false);
	//ofSetColor(ofColor::black); //updated from white
	if (!bHide) gui.draw();
	//draw the background for the top right corner info
	ofFill();
	ofSetColor(ofColor::lightPink);
	ofDrawRectangle(ofGetWindowWidth() - 240, 0, 240, 60);
	ofSetColor(ofColor::black);
	string values = "Frame Rate: " + std::to_string(ofGetFrameRate());
	values += "\nFuel Time: " + std::to_string((int) remainingFuelTime);
	if (bShowAltitude) {
		values += "\nAltitude (AGL): " + std::to_string(getAltitude());
	}
	ofDrawBitmapString(values, ofGetWindowWidth() - 220, 15);
	if (!bGameOn) {
		//draw the background for the control's info
		ofSetColor(ofColor::lightPink);
		ofDrawRectangle(ofGetWindowWidth() / 5 - 10, ofGetWindowHeight() / 5 - 120, 620, 150);
		ofSetColor(ofColor::black);
		string directions = "Press 1, 2, or 3 to switch between camera views.\n";
		directions += "Press a to show/hide altitude.\n";
		directions += "Press c to enable/disable mouse input(to adjust the camera angle).\n";
		directions += "Press f to toggle full screen.\n";
		directions += "Press h to show/hide the GUI.\n";
		directions += "Press the arrow buttons to thrust the fish forwards, back, left, and right.\n";
		directions += "Press SHIFT to thrust the fish upwards.\n";
		directions += "Press q or e to rotate the fish CCW or CW.\n";
		directions += "Press r to reset the camera and fish position.\n";
		directions += "Press the space bar to start.";
		ofSetColor(ofColor::black);  //updated from white
		ofDrawBitmapString(directions, ofGetWindowWidth()/5, ofGetWindowHeight()/10);
	}
	glDepthMask(true);
}

//--------------------------------------------------------------
// Handles different user actions based on the keys pressed
// Student Names: Dimitar Dimitrov and Viola Yasuda
void ofApp::keyPressed(int key) {
	switch (key) {
	case '1':
		camToView = 0;
		break;
	case '2':
		camToView = 1;
		break;
	case '3':
		camToView = 2;
		break;
	case 'A': case 'a':
		bShowAltitude = !bShowAltitude;
		break;
	case 'C': case 'c':
		if (cam[0].getMouseInputEnabled()) cam[0].disableMouseInput();
		else cam[0].enableMouseInput();
		break;
	case 'F': case 'f':
		ofToggleFullscreen();
		break;
	case 'H': case 'h':
		bHide = !bHide;
		break;
	case 'O': case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'R': case 'r':
		lander.setRotation(0, initialAngle, 0, 1, 0); 
		velocity = glm::vec3(0, 0, 0);
		angularVel = 0;
		cam[0].reset();
		cam[0].setDistance(25);
		cam[1].reset();
		cam[1].setDistance(50);
		bGameOn = false;
		bIsThrusting = false;
		bExploded = false;
		lander.setPosition(0, 0, 0);
		break;
	case 'T': case 't':
		setCameraTarget();
		break;
	case 'V': case 'v':
		bDisplayPoints = !bDisplayPoints;
		break;
	case 'W': case 'w':
		bShowWireframe = !bShowWireframe;
		break;
	case ' ':
		bGameOn = true;
		remainingFuelTime = fuelVal;//120.0;
		break;
	default:
		keymap[key] = true;
		if (remainingFuelTime > 0) {
			bIsThrusting = true;
			remainingFuelTime -= (1.0 / 60);
		}
		break;
	}
}

//--------------------------------------------------------------
// Handles different user actions based on the keys released
// Student Names: Viola Yasuda
void ofApp::keyReleased(int key) {
	switch (key) {
	case 'C': case 'c':
	case 'F': case 'f':
	case 'H': case 'h':
	case 'O': case 'o':
	case 'R': case 'r':
	case 'T': case 't':
	case 'V': case 'v':
	case 'W': case 'w':
	case ' ':
		break;
	default:
		keymap[key] = false;
		bIsThrusting = false;
		break;
	}
}

//--------------------------------------------------------------
// Allows user to drag the player model or select the terrain points
// Student Names: Viola Yasuda
void ofApp::mouseDragged(int x, int y, int button) {
	// if moving camera, don't allow mouse interaction
	if (cam[0].getMouseInputEnabled() || camToView != 0) return;
	if (bInDrag) {
		glm::vec3 landerPos = lander.getPosition();
		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam[0].getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
// Allows user to select the player model
// Student Names: Viola Yasuda
void ofApp::mousePressed(int x, int y, int button) {
	// if moving camera, don't allow mouse interaction
	if (cam[0].getMouseInputEnabled() || camToView != 0) return;
	// test for selection of rover
	glm::vec3 origin = cam[0].getPosition();
	glm::vec3 mouseWorld = cam[0].screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	bool hit = landerBox.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
	if (hit) {
		bLanderSelected = true;
		mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam[0].getZAxis());
		mouseLastPos = mouseDownPos;
		bInDrag = true;
	}
	else {
		bLanderSelected = false;
	}
}

//--------------------------------------------------------------
// Deselects the player model if it was selected
// Student Names: Viola Yasuda
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}

//--------------------------------------------------------------
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
void ofApp::drawAxis(ofVec3f location) {
	ofPushMatrix();
	ofTranslate(location);
	ofSetLineWidth(1.0);
	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));
	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));
	ofPopMatrix();
}

//--------------------------------------------------------------
// Gets the player's altitude with respect to the terrain directly
// below.
// Student Names: Dimitar Dimitrov
float ofApp::getAltitude() {
	// Setup our rays
	glm::vec3 origin = lander.getPosition();
	glm::vec3 dir = glm::vec3(0, -1, 0);
	Ray ray = Ray(Vector3(origin.x, origin.y, origin.z), Vector3(dir.x, dir.y, dir.z));
	TreeNode nodeRtn;
	if (octree.intersect(ray, octree.root, nodeRtn)) {
		return origin.y - octree.mesh.getVertex(nodeRtn.points[0]).y;
	}
	return -1;
}

//--------------------------------------------------------------
//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	glm::vec3 origin = cam[0].getPosition();
	glm::vec3 mouseWorld = cam[0].screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;
	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);
	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		// p' = p + s * dir;
		glm::vec3 intersectPoint = origin + distance * mouseDir;
		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

//--------------------------------------------------------------
// Gets the heading of the player.
// Student Names: Viola Yasuda
ofVec3f ofApp::heading() {
	ofVec3f heading = ofVec3f(lander.getModelMatrix() * ofVec4f(1, 0, 0, 0) - lander.getModelMatrix() * ofVec4f(0, 0, 0, 0)).getNormalized();
	return ofVec3f(heading.z, heading.y, heading.x).getNormalized();
}

//--------------------------------------------------------------
// Gets the direction 90 degrees CCW of the player's heading.
// Student Names: Viola Yasuda
ofVec3f ofApp::siding() {
	ofVec3f heading = ofVec3f(lander.getModelMatrix() * ofVec4f(0, 0, 1, 0) - lander.getModelMatrix() * ofVec4f(0, 0, 0, 0)).getNormalized();
	return ofVec3f(heading.z, heading.y, heading.x).getNormalized();
}

//--------------------------------------------------------------
// load exhaust vertex buffer in preparation for rendering
// Student Names: Viola Yasuda
void ofApp::loadExhVbo() {
	if (exhaustEmitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < exhaustEmitter.sys->particles.size(); i++) {
		points.push_back(exhaustEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(radius));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	exhVbo.clear();
	exhVbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	exhVbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

//--------------------------------------------------------------
// load explosion vertex buffer in preparation for rendering
// Student Names: Viola Yasuda
void ofApp::loadExpVbo() {
	if (explosionEmitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < explosionEmitter.sys->particles.size(); i++) {
		points.push_back(explosionEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(radius));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	expVbo.clear();
	expVbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	expVbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

//--------------------------------------------------------------
// setup basic ambient lighting in GL
// Student Names: Dimitar Dimitrov
void ofApp::initLightingAndMaterials() {
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));
	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(glm::vec3(5, 5, 5));
	keyLight.setScale(0);
	keyLight.lookAt(glm::vec3(0, 0, 0));

	fillLight.setup();
	fillLight.enable();
	fillLight.setSpotlight();
	fillLight.setScale(0);
	fillLight.setSpotlightCutOff(15);
	fillLight.setAttenuation(2, .001, .001);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(glm::vec3(0,0,0));

	rimLight.setup();
	rimLight.enable();
	rimLight.setSpotlight();
	rimLight.setScale(0);
	rimLight.setSpotlightCutOff(30);
	rimLight.setAttenuation(.2, .001, .001);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(0, 5, -7);
}

//--------------------------------------------------------------
// update basic ambient lighting in GL
// Student Names: Dimitar Dimitrov
void ofApp::updateLighting() {
	fillLight.setPosition(lander.getPosition());
	rimLight.setPosition(lander.getPosition() + glm::vec3(0, 10.5, -15));
	
	fillLight.lookAt(glm::vec3(lander.getPosition().x, lander.getPosition().y - 20, lander.getPosition().z));
	rimLight.lookAt(lander.getPosition());
}

//--------------------------------------------------------------
// Physics integrator for the player
// Student Names: Viola Yasuda
void ofApp::integrate() {
	glm::vec3 turbulence = turbulenceVal;
	float thrustMag = thrustVal;
	float damping = dampingVal;
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	acceleration.x = ofRandom(-turbulence.x, turbulence.x);
	acceleration.y = ofRandom(-turbulence.y, turbulence.y) - gravityVal;
	acceleration.z = ofRandom(-turbulence.z, turbulence.z);
	acceleration += thrustForce * thrustMag;
	if (impulseForce.y != 0) {
		velocity *= -1 * restitutionVal;
	}
	glm::vec3 landerPos = lander.getPosition();
	landerPos += velocity / 60; // see if hardcoding the framerate is better or worse than using ofGetFrameRate()
	lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
	velocity += acceleration / 60;
	velocity *= damping;

	float landerAngle = lander.getRotationAngle(0);
	landerAngle += angularVel / 60;
	lander.setRotation(0, landerAngle, 0, 1, 0);
	angularVel += angularAcc / 60;
	angularVel *= damping;
}

//--------------------------------------------------------------
// Returns true if the mouse ray intersects the plane, false otherwise.
bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f& point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam[0].screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam[0].getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
// Returns true if a point in the octree is selected by the mouse ray, false otherwise
bool ofApp::raySelectWithOctree(ofVec3f& pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam[0].screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam[0].getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));
	bPointSelected = octree.intersect(ray, octree.root, selectedNode);
	if (bPointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]); //gives an exception sometimes
	}
	return bPointSelected;
}

//--------------------------------------------------------------
// Set the camera to use the selected point as it's new target
// Student Names: Viola Yasuda
void ofApp::setCameraTarget() {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam[0].screenToWorld(glm::vec3(mouseX, mouseY, 0));
	cam[0].setTarget(rayPoint);
}
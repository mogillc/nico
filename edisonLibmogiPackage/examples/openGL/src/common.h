/*
 *  common.h
 *
 *
 *  Created by Matt Bunting on 12/28/15.
 *  Copyright 2015 12 Cent Dwarf. All rights reserved.
 *
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef SDL2_FOUND
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <mogi/simulation/scene.h>
#include <mogi/simulation/inputDevice.h>
#include <mogi/simulation/postprocess.h>
#include <mogi/simulation/shader.h>
#include <mogi/simulation/light.h>
#include <mogi/simulation/importer/importer.h>
#include <mogi/simulation/dynamicShader.h>

#include <mogi/robot/hexapod.h>
#include <mogi/statechart/statechart.h>

class PerformanceMeasure {
private:
	Mogi::Math::Time timer;
	std::map<std::string, double> times;

public:
	PerformanceMeasure() {
		timer.initialize();
	}
	void reset() {
		times.erase(times.begin(), times.end());
		timer.reset();
	}
	void print() {
		std::cout << "Times total:" << timer.runningTime() << std::endl;
		for (std::map<std::string,double>::iterator it = times.begin(); it != times.end(); it++) {
			std::cout << " - " << it->first << ", dTime: " << it->second << ",\t" << it->second/timer.runningTime()*100 << "%" << std::endl;
		}
	}

	void takeMeasurement(const std::string& label) {
		timer.update();
		times[label] = timer.dTime();
	}

};

typedef struct {
	float r;
	float g;
	float b;
} LightColor;

class UIhandler {
private:
	PerformanceMeasure mPerformanceMeasure;

	GLuint _vertexArray; // why?
	Mogi::Math::Time keyboardTimer;
	Mogi::Simulation::Keyboard keyboard;
	Mogi::Simulation::MBmouse mouse;

	int screenWidth;
	int screenHeight;

	Mogi::Math::Vector cameraOrientation;
	Mogi::Math::Vector cameraLocation;

	bool colorMapEnable;
	bool specularMapEnable;
	bool normalMapEnable;
	bool disparityMapEnable;
	bool useHexapodCamera;
	bool gridEnable;
	bool debugFocus;
	bool useBokeh;
	bool useShadows;

	int drawMode;

	float fstop;
	float fDepth;	//m
	float fLength; //mm
	bool autofocus;
	bool vignetteEnable;

	Mogi::Simulation::MBbokeh *bokehPost;
	Mogi::Simulation::MBpostprocess *testFinal;
	Mogi::Simulation::Camera *camera;

	Mogi::Simulation::FrameBuffer *frameBuffer;

	Mogi::Simulation::ShadowShader mShadowShader;
	Mogi::Simulation::MBshader *gShader;
	Mogi::Simulation::MBGBuffer* gBuffer;

#ifdef SDL2_FOUND
	SDL_Window *sdlWindow;
	SDL_GLContext glcontext;
#endif

	Mogi::Math::Vector cameraOffsetLocation;

	// Methods called by initialize()
	void initSDL();
	void initGL();
	void initShaders();
	void initModels();

	// Methods called in mainLoop()
	void updateKeyboardControl();
	void updateModels();
	void render();
	void renderES();
	void renderInterface( Mogi::Math::Vector camLocation, Mogi::Math::Vector camOrientation, Mogi::Simulation::MBshader *shaderToUse);

public:
	UIhandler(int xres = 1280, int yres = 720);

	// The main Scene that handles the environment, where models should be added.
	Mogi::Simulation::Scene* mainScene;

	// At each frame, this callback function will be performd with a reference to the UIhandler object.
	// All updates to the simulation models should be performed here:
	Mogi::StateChart::actionCallback displayCB;

	// Should be called before accessing mainScene and before running printUsage.
	void initialize();

	// Prints the keyboard commands for modifying shaders and camera pose/orientation.
	static void printUsage();

	// Blocking call, runs continuously until the excape key is pressed in the window.
	// This loop repeatedly calls displayCB.
	void mainLoop();

	// Set's the camera location offset to be added to the model:
	void setCameraOffsetLocation(Mogi::Math::Vector& locationOffset);
	// Set's the camera location to be added to the model:
	void setCameraLocation(Mogi::Math::Vector& location);
	// Set's the camera Euler angles
	void setCameraOrientation(Mogi::Math::Vector& orientation);

	// Should resize necessary framebuffers and camera reoslutions for a new screen size
	void resize(int xres, int yres);

	// Set focus parameters
	void setFocus( float fstop, float fDepth, float fLength );
};


// Loads models of a hexapod to a scene for rendering, based on the node structure of mHexapod.
// Directory structure should be as follows:
// locationOfBodySTL
// |--body.stl
// |--leg
// |--|--base.stl
// |--|--coxa.stl
// |--|--femur.stl
// |--|--tibia.stl
void hexapodToScene( Mogi::Simulation::Scene* mScene, Mogi::Robot::Hexapod* mHexapod, std::string locationOfBodySTL);

// Loads models of a hexapod leg to a scene for rendering, based on the node structure of mHexapodLeg.
// This is automatically called by hexapodToScene()
// Directory structure should be as follows:
// locationOfLegSTL
// |--base.stl
// |--coxa.stl
// |--femur.stl
// |--tibia.stl
void hexapodLegToScene( Mogi::Simulation::Scene* mScene, Mogi::Robot::HexapodLeg* mHexapodLeg, std::string locationOfLegSTL);

#endif

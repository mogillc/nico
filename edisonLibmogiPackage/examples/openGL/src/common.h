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

#include <mogi/robot/hexapod.h>
#include <mogi/statechart/statechart.h>

typedef struct {
	float r;
	float g;
	float b;
} LightColor;

class UIhandler {
private:
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
	GLfloat centerDepth;
	GLfloat farClip;
	GLfloat nearClip;

	Mogi::Simulation::MBbokeh *bokehPost;
	Mogi::Simulation::MBpostprocess *testFinal;
	Mogi::Simulation::Camera *camera;

	Mogi::Simulation::FrameBuffer *frameBuffer;

	//Mogi::Simulation::MBshader shader;
	//Mogi::Simulation::MBshader simpleShader;
	Mogi::Simulation::MBshader shadowShader;
	//Mogi::Simulation::MBshader bokehShader;
	Mogi::Simulation::MBshader shaderForES;

#ifdef SDL2_FOUND
	SDL_Window *sdlWindow;
	SDL_GLContext glcontext;
#endif

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

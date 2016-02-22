//
//  common.cpp
//
//  Created by Matt Bunting on 12/28/15.
//
//

#include "common.h"
#include <mogi/simulation/importer/importer.h>
#include <mogi.h>
#include <sstream>

#define N_LIGHTS (2)

LightColor lightColors[] = {
	{255.0/255.0, 147.0/255.0,  41.0/255.0 },	// 0  Candle
	{255.0/255.0, 197.0/255.0, 143.0/255.0 },	// 1  40W Tungsten
	{255.0/255.0, 214.0/255.0, 170.0/255.0 },	// 2  100W tungsten
	{255.0/255.0, 241.0/255.0, 224.0/255.0 },	// 3  Halogen
	{255.0/255.0, 250.0/255.0, 244.0/255.0 },	// 4  Carbon Arc
	{255.0/255.0, 255.0/255.0, 251.0/255.0 },	// 5  High Noon Sun
	{255.0/255.0, 255.0/255.0, 255.0/255.0 },	// 6  Direct Sunlight/Pure White
	{201.0/255.0, 226.0/255.0, 255.0/255.0 },	// 7  Blue Sky
	{255.0/255.0, 244.0/255.0, 229.0/255.0 },	// 8  Warm Fluorescent
	{244.0/255.0, 255.0/255.0, 250.0/255.0 },	// 9  Standard Fluorescent
	{212.0/255.0, 235.0/255.0, 255.0/255.0 },	// 10 Cool White Fluorescent
	{255.0/255.0, 244.0/255.0, 242.0/255.0 },	// 11 Full Spectrum Fluorescent
	{255.0/255.0, 239.0/255.0, 247.0/255.0 },	// 12 Grow Light Fluorescent
	{167.0/255.0,   0.0/255.0, 255.0/255.0 },	// 13 Black Light Fluorescent
	{216.0/255.0, 247.0/255.0, 255.0/255.0 },	// 14 Mercury Vapor
	{255.0/255.0, 209.0/255.0, 178.0/255.0 },	// 15 Sodium Vapor
	{242.0/255.0, 252.0/255.0, 255.0/255.0 },	// 16 Metal Halide
	{255.0/255.0, 183.0/255.0,  76.0/255.0 }	// 17 High Pressure Sodium
};

#ifdef SDL2_FOUND
void CheckSDLError(int line = -1)
{
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
}
#endif

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Math;
	using namespace Simulation;

	void ____doNothing(void* ) {};

	UIhandler::UIhandler(int xres, int yres) {

		colorMapEnable = true;
		specularMapEnable = false;	// Changed this for ES testing
		normalMapEnable = true;
		disparityMapEnable = false;	// Changed this for ES testing
		useHexapodCamera = false;
		gridEnable = true;
		debugFocus = false;
		useBokeh = false;	// Changed this for ES testing
		useShadows = false;

		drawMode = 0;

		fstop = 14;
		fDepth = 1;	//m
		fLength = 10; //mm
		autofocus = true;
		centerDepth = 0;
		farClip = 5;
		nearClip = .001;

		screenWidth = xres;
		screenHeight = yres;

		cameraOrientation.setLength(3);
		cameraLocation.setLength(3);
		cameraLocation(2) = 1;

		displayCB = ____doNothing;
	}

	void UIhandler::initSDL() {
		#ifdef SDL2_FOUND
		if(SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
			exit(1); /* Or die on error */


		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
#ifdef __APPLE__
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
#else
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
#endif
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		//SDL_SetVideoMode(screenWidth,screenHeight,32,SDL_OPENGL);
		//SDL_SetVideoMode( screenWidth, screenHeight, 32, SDL_OPENGL);
		sdlWindow = SDL_CreateWindow("My Simulation Window",
									 SDL_WINDOWPOS_UNDEFINED,
									 SDL_WINDOWPOS_UNDEFINED,
									 screenWidth, screenHeight,
									 SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		// Create an OpenGL context associated with the window.
		CheckSDLError(__LINE__);
		glcontext = SDL_GL_CreateContext(sdlWindow);
		CheckSDLError(__LINE__);
#endif
	}

	///////////////////////////////////////////////////////////////////////////////
	// initialize OpenGL
	// disable unused features
	///////////////////////////////////////////////////////////////////////////////
	void UIhandler::initGL()
	{
// Adrian moved this conditional code for non-Apple OS here
#ifndef __APPLE__
		glewExperimental = GL_TRUE;
		glewInit();
		glGetError(); // Added by Adrian to clear error buffer... hmmm
					  // See: http://stackoverflow.com/questions/10857335/opengl-glgeterror-returns-invalid-enum-after-call-to-glewinit
#endif
		// Some VAO needs to be bound before validating shaders in scene...
		// This VAO is not used for anything else.
		glGenVertexArrays(1, &_vertexArray);
		glBindVertexArray(_vertexArray);

		int i = glGetError();
		if (i != 0) {
			std::cout << " - Broke after VAO created: " << i << std::endl; // Adrian added printing of error value
			exit(-1);
		}

		std::cout << " - Allocating frame buffer." << std::endl;
		frameBuffer = new FrameBuffer;
		frameBuffer->resize(screenWidth, screenHeight);
		frameBuffer->attachFramebuffer();
		std::cout << " - Allocating Bokeh post process." << std::endl;
		bokehPost = new MBbokeh;
		frameBuffer->attachFramebuffer();
		std::cout << " - Allocating final frame buffer." << std::endl;
		testFinal = new MBpostprocess;
		frameBuffer->attachFramebuffer();
		std::cout << " - Allocating camera." << std::endl;
		camera = new Camera;
		camera->setResolution(screenWidth, screenHeight);
		frameBuffer->attachFramebuffer();
		std::cout << " - Allocating main scene." << std::endl;
		mainScene = new Scene;
		
		std::cout << "OpenGL Version info:" << std::endl << "\t" << glGetString(GL_VERSION) << std::endl;
		std::cout << "OpenGL Renderer info:" << std::endl << "\t" << glGetString(GL_RENDERER) << std::endl;
		std::cout << "OpenGL Vendor info:" << std::endl << "\t" << glGetString(GL_VENDOR) << std::endl;
		std::cout << "OpenGL Shading info:" << std::endl << "\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		//std::cout << "OpenGL Extensions info:" << std::endl << "\t" << glGetString(GL_EXTENSIONS) << std::endl;
//#ifndef __APPLE__
//		glewExperimental = GL_TRUE;
//		glewInit();
//#endif
		//glClearDepth(1.0f);                         // 0 is near, 1 is far
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Cull triangles which normal is not towards the camera
		glEnable(GL_CULL_FACE);
	}

	void UIhandler::initShaders() {
		// frame buffers used by shaders:
		bokehPost->setAsFinalRender(false);
		testFinal->setAsFinalRender(true);

		// most shaders are handled by Scene (bleh) except for the shadow shader:
#ifdef BUILD_FOR_IOS
		shadowShader.initialize( "shaders/ios/shadowShader.vsh", "shaders/ios/shadowShader.fsh");
		shaderForES.initialize( "shaders/ios-current/Shader.vsh", "shaders/ios-current/Shader.fsh");
		//shaderForES.initialize( "shaders/ios-current/shadowShader.vsh", "shaders/ios-current/shadowShader.fsh");
#else
		shadowShader.initialize( "Shaders/shadowShader.vsh", "Shaders/shadowShader.fsh");
#endif
	}

	void UIhandler::initModels() {

		mainScene->cameras.push_back(camera);
		Importer::loadObject(mainScene, "paverStones.obj","Objects");	// floor
		//mainScene->loadObject("paverStones.obj","Objects");				// floor
		Importer::loadObject(mainScene, "brickWall.obj", "Objects/brickWall");	// walls

		// simple object for testing:
		Node* node = Importer::loadObject(mainScene, "testCube.ply", "Objects");
		if(node) {
			node->name = "testCube";
		} else {
			std::cout << "Could not load testCube.ply :(" << std::endl;
		}

		char newName[64];

		for (int i = 0; i < N_LIGHTS; i++) {
			node = Importer::loadObject(mainScene, "lightModel.dae", "Objects");
			if (node) {
				node->setLocation(0, 0, -1);
				node->setScale(25.4 / MM_PER_METER * 10.0);
				sprintf(newName, "lightModel %d", i);
				node->name = newName;
				sprintf(newName, "bulbulb %d", i);
				node->findChildByName("bulbulb")->name = newName;
			}
		}
		for (int i = 0; i < mainScene->lights.size(); i++) {
			mainScene->lights[i]->setColor(cos(i*i*4)*cos(i*i*4), cos(i*i*i*8)*cos(i*i*i*8), cos(i*2)*cos(i*2));
		}

		if (mainScene->lights.size() > 0) {
			int lightIndex = 2; // 100W Tungsten
			mainScene->lights[0]->setColor(lightColors[lightIndex].r, lightColors[lightIndex].g, lightColors[lightIndex].b);
		}
		if (mainScene->lights.size() > 1) {
			int lightIndex = 14; // Mercury Vapor
			mainScene->lights[1]->setColor(lightColors[lightIndex].r, lightColors[lightIndex].g, lightColors[lightIndex].b);
		}

	}

	void UIhandler::initialize() {
#ifdef BUILD_FOR_IOS
#else
		std::cout << "Initializing SDL:" << std::endl;
		initSDL();
		std::cout << " - SDL initialized." << std::endl;
#endif
		
		std::cout << "Initializing GL:" << std::endl;
		initGL();
		std::cout << " - GL initialized." << std::endl;

		std::cout << "Initializing Shaders:" << std::endl;
		initShaders();
		std::cout << " - Shaders initialized." << std::endl;

		std::cout << "Initializing Models:" << std::endl;
		initModels();
		std::cout << " - Models initialized." << std::endl;

		keyboardTimer.initialize();
	}

	void UIhandler::updateKeyboardControl()
	{
		keyboardTimer.update();
		double velocity = 1000/MM_PER_METER * keyboardTimer.dTime();

		Matrix xRotation, zRotation;
		xRotation.makeXRotation(cameraOrientation(0));
		zRotation.makeZRotation(cameraOrientation(2));

		if(keyboard.keyPressed('w') || keyboard.keyPressed('W'))
		{
			cameraLocation +=  zRotation * (xRotation * (-velocity*Vector::zAxis));
		}
		if(keyboard.keyPressed('a') || keyboard.keyPressed('A'))
		{
			cameraLocation += zRotation * (xRotation * (-velocity*Vector::xAxis));
		}
		if(keyboard.keyPressed('s') || keyboard.keyPressed('S'))
		{
			cameraLocation +=  zRotation * (xRotation * (velocity*Vector::zAxis));
		}
		if(keyboard.keyPressed('d') || keyboard.keyPressed('D'))
		{
			cameraLocation += zRotation * (xRotation * (velocity*Vector::xAxis));
		}
		if(keyboard.keyPressed('j') || keyboard.keyPressed('J'))
		{
			cameraOrientation(2) += velocity * 100* MOGI_PI/180.0;
		}
		if(keyboard.keyPressed('l') || keyboard.keyPressed('L'))
		{
			cameraOrientation(2) -= velocity * 100* MOGI_PI/180.0;
		}
		if(keyboard.keyPressed('i') || keyboard.keyPressed('I'))
		{
			//cameraOrientation(0) += velocity * 100* MOGI_PI/180.0;
			cameraOrientation(0) += velocity * 100* MOGI_PI/180.0;
		}
		if(keyboard.keyPressed('k') || keyboard.keyPressed('K'))
		{
			//cameraOrientation(0) -= velocity * 100* MOGI_PI/180.0;
			cameraOrientation(0) += -velocity * 100* MOGI_PI/180.0;
		}
		if (keyboard.keyPressed('3')) {
			fstop += .05;
		}
		if (keyboard.keyPressed('e')) {
			fstop -= .05;
		}
		if (keyboard.keyPressed('4')) {
			fLength += 1;
		}
		if (keyboard.keyPressed('r')) {
			fLength -= 1;
		}
		if (keyboard.keyPressed('6')) {
			fDepth += .01;
		}
		if (keyboard.keyPressed('y')) {
			fDepth -= .01;
		}


		// Rising edge commands:
		if (keyboard.risingEdge('X') || keyboard.risingEdge('x')) {
			useShadows ^= 1;
		}
		if (keyboard.risingEdge('f') || keyboard.risingEdge('F')) {
			debugFocus ^= 1;
		}
		if (keyboard.risingEdge('5')) {
			autofocus ^= 1;
		}
		if (keyboard.risingEdge('v') || keyboard.risingEdge('V')) {
			useBokeh ^= 1;
		}
		if (keyboard.risingEdge('c') || keyboard.risingEdge('C')) {
			colorMapEnable ^= 1;
		}
		if (keyboard.risingEdge('h') || keyboard.risingEdge('H')) {
			disparityMapEnable ^= 1;
		}
		if (keyboard.risingEdge('n') || keyboard.risingEdge('N')) {
			normalMapEnable ^= 1;
		}
		if (keyboard.risingEdge('b') || keyboard.risingEdge('B')) {
			specularMapEnable ^= 1;
		}
		if (keyboard.risingEdge('m') || keyboard.risingEdge('M')) {
			drawMode = ++drawMode % 3;
		}
		if (keyboard.risingEdge('g') || keyboard.risingEdge('G')) {
			gridEnable ^= 1;
		}
		if (keyboard.risingEdge('-') || keyboard.risingEdge('_')) {
			drawMode = 0;
			colorMapEnable= false;
			disparityMapEnable = false;
			normalMapEnable = false;
			specularMapEnable = false;
			gridEnable = true;
			useBokeh = false;
			useShadows = false;
		}
		if (keyboard.risingEdge('=') || keyboard.risingEdge('+')) {
			drawMode = 0;
			colorMapEnable= true;
			disparityMapEnable = true;
			normalMapEnable = true;
			specularMapEnable = true;
			gridEnable = false;
			useBokeh = true;
			useShadows = true;
		}
	}

	void UIhandler::printUsage() {
		printf("Usage:\n");

		printf(" - Camera:\n");
		printf("  - a,s,d,w\tMoves camera\n");
		printf("  - j,k,l,i\tOrients camera\n");

		printf("\n - Rendering:\n");
		printf("  - c\tEnable/Disable Color Maps\n");
		printf("  - n\tEnable/Disable Normal Maps\n");
		printf("  - b\tEnable/Disable Specularity Maps\n");
		printf("  - h\tEnable/Disable Height Maps\n");
		printf("  - c\tEnable/Disable Color Maps\n");
		printf("  - x\tEnable/Disable Shadows\n");

		printf("\n - Bokeh:\n");
		printf("  - v\tEnable/Disable Bokeh\n");
		printf("  - 3,e\tIncrease/Decrease F stop\n");
		printf("  - 4,r\tIncrease/Decrease Focal Length\n");
		printf("  - 5\tEnable/Disable Autofocus (center of screen)\n");
		printf("  - 6,y\tIncrease/Decrease Focal Depth\n");

		printf("\n - Other:\n");
		printf("  - g\tEnable/Disable Grid/Walls\n");
		printf("  - 1,q\tSwap ground texture\n");
		printf("  - t\tLatch/Unlatch camera as hexapod camera\n");
		printf("  - -\tDisable all features\n");
		printf("  - +\tEnable all features\n");
		printf("\n");
		
	}

	void UIhandler::updateModels() {
		for (int i = 0; i < N_LIGHTS; i++) {
			char modelName[64];
			sprintf(modelName, "lightModel %d", i);
			Node *node = mainScene->findNodeByName(modelName);
			if (node == NULL) {
			//	std::cout << ":(" << std::endl;
				continue;
			}

			Vector tempPosition(3);//, tempOrientation(3);
			Quaternion tempOrientation, tempO2;
			tempPosition(0) = 0.7 * cos( (0.1*(double)i +.1)*keyboardTimer.runningTime()/2);
			tempPosition(1) = 0.7 * sin( (0.1*(double)i +.1)*keyboardTimer.runningTime()/2);
			tempPosition(2) = 0.5 + 0.2 * sin( (0.1*(double)i +.1)*keyboardTimer.runningTime()/0.25);
			tempPosition *= 2;
			node->setLocation(tempPosition);

			tempOrientation.makeFromAngleAndAxis(80*MOGI_PI/180.0 - atan2(tempPosition(2), sqrt(tempPosition(0)*tempPosition(0) + tempPosition(1)*tempPosition(1))), Vector::xAxis);
			tempO2.makeFromAngleAndAxis(atan2(tempPosition(1), tempPosition(0)) + 90 * MOGI_PI/180.0, Vector::zAxis);
			tempOrientation = tempO2 * tempOrientation;
			node->setOrientation(tempOrientation );
			
		}

		Quaternion tempOrientation, tempO2, tempO3;
		tempOrientation.makeFromAngleAndAxis(cameraOrientation(0), Vector::xAxis);
		tempO2.makeFromAngleAndAxis(cameraOrientation(1), Vector::yAxis);
		tempO3.makeFromAngleAndAxis(cameraOrientation(2), Vector::zAxis);
		tempOrientation =tempO3 * tempO2 * tempOrientation;
		camera->setOrientation( tempOrientation );
		camera->setLocation(cameraLocation);
		camera->setFOV(65);
		camera->update();

		mainScene->colorMapUserEnable = colorMapEnable;
		mainScene->normalMapUserEnable = normalMapEnable;
		mainScene->heightMapUserEnable = disparityMapEnable;
		mainScene->specularityMapUserEnable = specularMapEnable;

		mainScene->update();

	}

	void UIhandler::render() {

		///////////////////////////////////////////////////////////////////////
		// Shadow map rendering:
		// Places the camera at each light to render shadow maps.
		///////////////////////////////////////////////////////////////////////
		for (int i = 0; i < mainScene->lights.size(); i++) {
			mainScene->lights[i]->setEnabled(useShadows);
		}
		mainScene->buildShadowMaps();

		/////////////////////////////////////////////////////////////////////////
		// Scene rendering:
		// This draws the frame to a depth and render buffer to be used by bokeh:
		/////////////////////////////////////////////////////////////////////////
		glViewport( 0, 0, camera->getXresolution(), camera->getYresolution());
		glCullFace(GL_BACK);

#ifdef BUILD_FOR_IOS
		// TODO: same as in scene.cpp, glPolygonmode is not present.
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
#else
		if(drawMode == 0)        // fill mode
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
		else if(drawMode == 1)  // wireframe mode
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
		}
		else                    // point mode
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
		}
#endif

		// Render to frame buffers instead of the default display:
		frameBuffer->attachFramebuffer();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		shadowShader.useProgram();
		{
			for (int i = 0; i < mainScene->lights.size(); i++) {
				mainScene->lights[i]->setShadowUniforms( &shadowShader, i );
			}
			mainScene->draw( camera, &shadowShader);
		}
		shadowShader.stopProgram();

		// Disable the frame buffer:
		frameBuffer->removeFramebuffer();

		///////////////////////////////////////////////////////////////////////
		// Post processing:
		// Using the frame buffers, all post processing may now be handled:
		///////////////////////////////////////////////////////////////////////
		if (useBokeh)
		{
			// Apply the parameters for the bokeh process:
			bokehPost->setAutoFocus(autofocus);
			bokehPost->setDebugMode(debugFocus);
			bokehPost->setFdepth(fDepth);
			bokehPost->setFlength(fLength);
			bokehPost->setFstop(fstop);

			bokehPost->process(frameBuffer->getRenderTexture(FrameBuffer::TEXTURE_TYPE_DIFFUSE), frameBuffer->getDepthTexture(), camera[0]);
			testFinal->process(bokehPost->getRenderTexture(FrameBuffer::TEXTURE_TYPE_DIFFUSE), camera[0]);
		} else {
			testFinal->process(frameBuffer->getRenderTexture(FrameBuffer::TEXTURE_TYPE_DIFFUSE), camera[0]);
		}

		// Write text on the screen, etc.:
//		renderInterface( cameraLocation, cameraOrientation, NULL);

		glEnable(GL_DEPTH_TEST);
	}

	void UIhandler::renderES() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//glViewport( 0, 0, camera->getXresolution(), camera->getYresolution());
		glViewport( 0, 0, screenWidth, screenHeight);
		//shaderForES.enableAttributes();
		shaderForES.useProgram();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mainScene->draw( camera, &shaderForES );

		shaderForES.stopProgram();
	}

	void UIhandler::mainLoop() {
#ifdef BUILD_FOR_IOS
		// TODO: set up the rendering loop.
		keyboardTimer.update();
		updateModels();
		this->cameraOrientation(0) = MOGI_PI/3;
		//this->cameraOrientation(1) = sin(camtime)/4;
		this->cameraOrientation(2) = keyboardTimer.runningTime() * MOGI_PI/16;
		this->cameraLocation(0) = 0.75 * sin(keyboardTimer.runningTime() * MOGI_PI/16);
		this->cameraLocation(1) = -0.75 * cos(keyboardTimer.runningTime() * MOGI_PI/16);
		this->cameraLocation(2) = 0.75;
		renderES();
#else
		Uint32 start;
		SDL_Event event;
		bool running=true;
		while(running)
		{
			start=SDL_GetTicks();
			keyboard.beginEvents();
			while(SDL_PollEvent(&event))
			{
				keyboard.handleEvent(event);
				mouse.handleEvent(event);
				switch(event.type)
				{
					case SDL_QUIT:
						std::cout << " - SDL_QUIT event caught, shutting down..." << std::endl;
						running&=false;
						break;
				}
			}

			//std::cout << " - SDL Status: " << SDL_GetError() << std::endl;
			updateKeyboardControl();
			displayCB(this);
			updateModels();
			render();

			SDL_GL_SwapWindow(sdlWindow);
			//if(1000.0/30>SDL_GetTicks()-start)
			//	SDL_Delay(1000.0/30-(SDL_GetTicks()-start));
			running &= !keyboard.keyPressed(27);
		}

		std::cout << " - Performing SDL_Quit()." << std::endl;
		//delete scene;
		SDL_Quit();
		std::cout << " - Done!" << std::endl << std::endl;

		SDL_GL_DeleteContext(glcontext);
#endif
	}

	void hexapodToScene(Scene* mScene, Robot::Hexapod* mHexapod, std::string locationOfBodySTL) {
		Node* body = mHexapod->getBodyNode();// nodeLocation->child(0);
		mScene->attachMeshToNode(body, Importer::addMesh(mScene, "body.stl", locationOfBodySTL.c_str()));

		std::string locationOfLegSTL = locationOfBodySTL.append("/leg");
		for (std::vector<Robot::HexapodLeg*>::iterator it = mHexapod->legs.begin(); it != mHexapod->legs.end(); it++) {
			Robot::HexapodLeg* leg = *it;
			hexapodLegToScene(mScene, leg, locationOfLegSTL);
		}
	}

	void hexapodLegToScene(Scene* mScene, Robot::HexapodLeg* mHexapodLeg, std::string locationOfLegSTL) {
		Node* base = mHexapodLeg->getBaseNode();
		int bodyMeshID = Importer::addMesh(mScene, "base.stl", locationOfLegSTL);
		mScene->attachMeshToNode(base, bodyMeshID);

		Node* coxa = base->child(0);
		Node* femur = coxa->child(0);
		Node* tibia = femur->child(0);

		int coxaMeshID = Importer::addMesh(mScene, "coxa.stl", locationOfLegSTL);
		int femurMeshID = Importer::addMesh(mScene, "femur.stl", locationOfLegSTL);
		int tibiaMeshID = Importer::addMesh(mScene, "tibia.stl", locationOfLegSTL);

		mScene->attachMeshToNode(coxa, coxaMeshID);
		mScene->attachMeshToNode(femur, femurMeshID);
		mScene->attachMeshToNode(tibia, tibiaMeshID);
	}
	
#ifdef _cplusplus
}
#endif

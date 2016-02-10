/*
 *  simple.cpp
 *  Experimental testing.
 *
 *  Updated by Matt Bunting on 12/29/15.
 *  Copyright 2015 Mogi LLC. All rights reserved.
 *
 */

#ifdef __APPLE__
//#include <GLUT/glut.h>
//#include <SDL2/SDL.h>
//#define GL3_PROTOTYPES 1
//#include <OpenGL/gl3.h>
#else
//#include <GL/glew.h>
//#include <GL/glut.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glext.h>
//#include <SDL/SDL_opengl.h>
#endif


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//#include "glext.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#include <mogi/simulation/scene.h>
#include <mogi/simulation/shader.h>
#include <mogi/simulation/light.h>
#include <mogi/simulation/postprocess.h>

#define N_LIGHTS (2)

using namespace Mogi;
using namespace Simulation;
using namespace Math;

SDL_Window *sdlWindow;

Scene *paverStoneModel;

// GLUT CALLBACK functions
void displayCB();
void reshapeCB(int w, int h);
//void timerCB(int millisec);
void keyboardCB(unsigned char key, int x, int y);

bool key_state[256] = { false };
bool key_state_previous[256] = { false };
void keyboardUpCB( unsigned char key, int x, int y);
void updateKeyboardControl();

void initGL();
//int  initGLUT(int argc, char **argv);
bool initSharedMem();
void clearSharedMem();

// constants
const int   SCREEN_WIDTH    = 1280;
const int   SCREEN_HEIGHT   = 720;
const float CAMERA_DISTANCE = 1; // in m
//const int   TEXT_WIDTH      = 8;
//const int   TEXT_HEIGHT= 13;


// global variables
void *font;// = GLUT_BITMAP_TIMES_ROMAN_10;
int screenWidth = SCREEN_WIDTH;
int screenHeight = SCREEN_HEIGHT;

int drawMode;
int triangles = 0;

// These are just the location to render, not necessarily bokeh specific
FrameBuffer *lightingBuffer;
MBGBuffer *geometryBuffer;

MBshader lightShader;
MBshader gShader;

MBbokeh *bokehPost;
MBpostprocess *testFinal;
MBdeferredLighting *deferredLighting;

Camera *camera;

Vector cameraOrientation(3);
Vector cameraLocation(3);

bool colorMapEnable = false;
bool specularMapEnable = false;
bool normalMapEnable = false;
bool disparityMapEnable = false;
bool gridEnable = true;
bool debugFocus = false;
bool useBokeh = false;
bool useShadows = false;

// Bokeh parameters:
float fstop = 14;
float fDepth = 1;	//m
float fLength = 10; //mm
bool autofocus = true;
GLfloat centerDepth = 0;

Time btime;
Time keyboardTimer;

void printUsage();

void init(int argc, char *argv[])
{
	initGL();

	initSharedMem();

	//printUsage();

	std::cout << "Initializing main render shader...";
	std::string vshader = Scene::getResourceDirectory().append("/shaders/lighting.vsh");
	std::string fshader = Scene::getResourceDirectory().append("/shaders/lighting.fsh");
	lightShader.initialize( vshader.c_str(), fshader.c_str());
	std::cout << "Done." << std::endl;

	vshader = Scene::getResourceDirectory().append("/shaders/geometry.vsh");
	fshader = Scene::getResourceDirectory().append("/shaders/geometry.fsh");

	std::cout << "Initializing Geometry shader...";
	gShader.initialize( vshader.c_str(), fshader.c_str());
	std::cout << "Done." << std::endl;

	geometryBuffer = new MBGBuffer;
	lightingBuffer = new FrameBuffer;
	deferredLighting = new MBdeferredLighting;
	bokehPost = new MBbokeh;
	testFinal = new MBpostprocess;
	deferredLighting->setAsFinalRender(false);
	bokehPost->setAsFinalRender(false);
	testFinal->setAsFinalRender(true);

	camera = new Camera;

	paverStoneModel = new Scene;
	paverStoneModel->cameras.push_back(camera);

	paverStoneModel->loadObject("paverStones.obj","Objects");
	//paverStoneModel->attachObject("tyke.dae", "Objects");
	paverStoneModel->loadObject("brickWall.obj", "Objects/brickWall");
	paverStoneModel->loadObject("testCube.ply", "Objects");

	Node* node = paverStoneModel->loadObject("tyke.dae", "Objects");//paverStoneModel->findNodeByName("Form38");
	if (node) {
		node->setLocation(0, 0, 0.2);
		node->setScale(.1);
	}

	char newName[64];
	for (int i = 0; i < N_LIGHTS; i++) {
		node = paverStoneModel->loadObject("lightModel.dae", "Objects");
		// paverStoneModel->findNodeByName("lightModel");
		if (node) {
			node->setLocation(0, 0, -1);
			node->setScale(25.4 / MM_PER_METER);
			sprintf(newName, "lightModel %d", i);
			node->name = newName;
			sprintf(newName, "bulbulb %d", i);
			node->findChildByName("bulbulb")->name = newName;
		}
	}
	for (int i = 0; i < paverStoneModel->lights.size(); i++) {
		//paverStoneModel->lights[i]->setColor(cos(i*2)*cos(i*2), cos(i*i*4)*cos(i*i*4), cos(i*i*i*8)*cos(i*i*i*8));
	}

	std::cout << "Final node structure for paverstonemodel" << std::endl;
	//paverStoneModel->rootNode.printStructure();

	btime.initialize();
	keyboardTimer.initialize();

}

void checkSDLError(int line = -1)
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

int main(int argc, char *argv[])
{

	//SDL_Window *window;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
		exit(1); /* Or die on error */

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef __APPLE__
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
#else
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
#endif

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	//SDL_SetVideoMode( screenWidth, screenHeight, 32, SDL_OPENGL);
	sdlWindow = SDL_CreateWindow("My Game Window",
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  SCREEN_WIDTH, SCREEN_HEIGHT,
							   SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);



	// Create an OpenGL context associated with the window.
	checkSDLError(__LINE__);
	SDL_GLContext glcontext = SDL_GL_CreateContext(sdlWindow);
	checkSDLError(__LINE__);

	//TTF_Init();
	Uint32 start;
	SDL_Event event;
	bool running=true;


	init(argc, argv);



	//bool b=false;
	while(running)
	{
		start=SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					std::cout << " - SDL_QUIT event caught, shutting down..." << std::endl;
					running=false;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						std::cout << " - SDLK_ESCAPE key pressed, shutting down..." << std::endl;
						running=false;
						break;
					default:
						keyboardCB(event.key.keysym.sym, 0, 0);
						break;
				}
					break;
				case SDL_KEYUP:
					keyboardUpCB(event.key.keysym.sym, 0, 0);
					break;

				case SDL_MOUSEBUTTONDOWN:
					//cam.mouseIn(true);
					break;
			}
		}
		//std::cout << " - SDL Status: " << SDL_GetError() << std::endl;
		displayCB();
		//SDL_GL_SwapBuffers();
		SDL_GL_SwapWindow(sdlWindow);

		//if(1000.0/30>SDL_GetTicks()-start)
		//	SDL_Delay(1000.0/30-(SDL_GetTicks()-start));
	}
	std::cout << " - Clearing Shared Memory." << std::endl;
	clearSharedMem();
	std::cout << " - Performing SDL_Quit()." << std::endl;
	//delete scene;
	SDL_Quit();
	std::cout << " - Done!" << std::endl << std::endl;

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
GLuint _vertexArray;
GLuint _vertexBuffer;
void initGL()
{
	std::cout << "OpenGL Version info:" << std::endl << std::endl << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl << std::endl;
#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif
	//glClearDepth(1.0f);                         // 0 is near, 1 is far
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	//glEnable(GL_BLEND);

	glGenVertexArrays(1, &_vertexArray);
    glBindVertexArray(_vertexArray);

	int i = glGetError();
	if (i != 0) {
		std::cout << "Broke after VAO created:" << std::endl;
	}

}



///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
    screenWidth = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;

	cameraLocation(2) = CAMERA_DISTANCE;

    drawMode = 0; // 0:fill, 1: wireframe, 2:points

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up global vars
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
	delete geometryBuffer;
	printf("Main Loop finished!  Shutting down...\n");

}


///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
	/*
	// backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};

    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
	int line = 1;

    ss << "Triangles: " << triangles << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(TEXT_HEIGHT * line++), color, font);
    ss.str("");

    ss << "FPS: " << btime.favg << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(TEXT_HEIGHT * line++), color, font);
    ss.str("");

	if (useBokeh) {
		ss << std::fixed << std::setprecision(1);
		ss << "F Stop: " << bokehPost->getFstop() << std::ends;
		drawString(ss.str().c_str(), 1, screenHeight-(TEXT_HEIGHT * line++), color, font);
		ss.str("");

		ss << std::fixed << std::setprecision(0);
		ss << "Focal Length: " << bokehPost->getFlength() << std::ends;
		drawString(ss.str().c_str(), 1, screenHeight-(TEXT_HEIGHT * line++), color, font);
		ss.str("");

		ss << std::fixed << std::setprecision(6);
		ss << "Focal Depth: " << bokehPost->getFdepth() << std::ends;
		drawString(ss.str().c_str(), 1, screenHeight-(TEXT_HEIGHT * line++), color, font);
		ss.str("");

		ss << std::fixed << std::setprecision(6);
		ss << "Center Depth: " << bokehPost->getCenterDepth() << std::ends;
		drawString(ss.str().c_str(), 1, screenHeight-(TEXT_HEIGHT * line++), color, font);
		ss.str("");
	}

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
	 */
}


void renderInterface( Vector camLocation, Vector camOrientation, MBshader *shaderToUse)
{
	glUseProgram( 0 ); // kill any program that has been enabled
	//glBindTexture( GL_TEXTURE_2D, 0);

	// Texture 0 MUST be disables:
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0);

    showInfo();     // print max range of glDrawRangeElements
}

//=============================================================================
// CALLBACKS
//=============================================================================
void displayCB()
{
	btime.update();

	Node* node = paverStoneModel->findNodeByName("Form38");
	if (node) {
		node->setOrientation(sin(btime.runningTime()), Vector::yAxis);
	}


	paverStoneModel->colorMapUserEnable = colorMapEnable;
	paverStoneModel->normalMapUserEnable = normalMapEnable;
	paverStoneModel->heightMapUserEnable = disparityMapEnable;
	paverStoneModel->specularityMapUserEnable = specularMapEnable;


	for (int i = 0; i < N_LIGHTS; i++) {
		char modelName[64];
		sprintf(modelName, "lightModel %d", i);
		node = paverStoneModel->findNodeByName(modelName);
		if (node == NULL) {
			std::cout << ":(" << std::endl;
			continue;
		}

		Vector tempPosition(3);//, tempOrientation(3);
		Quaternion tempOrientation, tempO2;
		tempPosition(0) = 0.7 * cos( (0.1*(double)i +.1)*btime.runningTime()/2);
		tempPosition(1) = 0.7 * sin( (0.1*(double)i +.1)*btime.runningTime()/2);
		tempPosition(2) = 0.5 + 0.2 * sin( (0.1*(double)i +.1)*btime.runningTime()/0.25);
		tempPosition *= 2;
		node->setLocation(tempPosition);



		tempOrientation.makeFromAngleAndAxis(80*MOGI_PI/180.0 - atan2(tempPosition(2), sqrt(tempPosition(0)*tempPosition(0) + tempPosition(1)*tempPosition(1))), Vector::xAxis);
		tempO2.makeFromAngleAndAxis(atan2(tempPosition(1), tempPosition(0)) + 90 * MOGI_PI/180.0, Vector::zAxis);
		tempOrientation = tempO2 * tempOrientation;
		node->setOrientation(tempOrientation );

		//paverStoneModel->lights[i]->setFOV(90);

	}



	paverStoneModel->update();

	//lights[0]->setColor(255./255., 209./255., 178./255.);


	updateKeyboardControl();

	Quaternion tempOrientation, tempO2, tempO3;
	tempOrientation.makeFromAngleAndAxis(cameraOrientation(0), Vector::xAxis);
	tempO2.makeFromAngleAndAxis(cameraOrientation(1), Vector::yAxis);
	tempO3.makeFromAngleAndAxis(cameraOrientation(2), Vector::zAxis);
	tempOrientation =tempO3 * tempO2 * tempOrientation;
	camera->setOrientation( tempOrientation );
	camera->setLocation(cameraLocation);
	camera->setFOV(60);
	camera->update();
	//camera->setModelMatrix(node->getModelMatrix());
	//*camera = paverStoneModel->lights[0]->getCamera();

	///////////////////////////////////////////////////////////////////////
	// Shadow map rendering:
	// Place the camera at the light, but save the current camera location:
	///////////////////////////////////////////////////////////////////////
	for (int i = 0; i < paverStoneModel->lights.size(); i++) {
		paverStoneModel->lights[i]->setEnabled(useShadows);
	}
	paverStoneModel->buildShadowMaps();

	/////////////////////////////////////////////////////////////////////////
	// Scene rendering:
	// This draws the frame to a depth and render buffer to be used by bokeh:
	/////////////////////////////////////////////////////////////////////////
	glViewport( 0, 0, camera->getXresolution(), camera->getYresolution());
	glCullFace(GL_BACK);

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

	// Render to frame buffers instead of the default display:
	geometryBuffer->attachFramebuffer();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gShader.useProgram();
	{
		//for (int i = 0; i < paverStoneModel->lights.size(); i++) {
		//	paverStoneModel->lights[i]->setShadowUniforms( &gShader, i );
		//}
		paverStoneModel->draw( camera, &gShader);
	}
	gShader.stopProgram();

	// Disable the frame buffer:
	geometryBuffer->removeFramebuffer();

//	deferredLighting->process(geometryBuffer, camera[0], paverStoneModel->lights);


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

		//bokehPost->process(geometryBuffer->getRenderTexture(MBGBuffer::TEXTURE_TYPE_DIFFUSE), geometryBuffer->getDepthTexture(), camera[0]);
		bokehPost->process(geometryBuffer->getRenderTexture(FrameBuffer::TEXTURE_TYPE_DIFFUSE), geometryBuffer->getDepthTexture(), camera[0]);

		testFinal->process(bokehPost->getRenderTexture(FrameBuffer::TEXTURE_TYPE_DIFFUSE), camera[0]);
	} else {
		//testFinal->process(geometryBuffer->getRenderTexture(MBGBuffer::TEXTURE_TYPE_SPECULARITY), camera[0]);
		testFinal->process(geometryBuffer->getRenderTexture(MBGBuffer::TEXTURE_TYPE_SPECULARITY), camera[0]);
	}

	// Write text on the screen, etc.:
	renderInterface( cameraLocation, cameraOrientation, NULL);



	glEnable(GL_DEPTH_TEST);
}


void reshapeCB(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
}

void printUsage() {
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

void updateKeyboardControl()
{
	keyboardTimer.update();
	double velocity = 1000/MM_PER_METER * keyboardTimer.dTime();
	for (int key=0; key < 256; key++)	// check for any key held down:
	{
		if (key_state[key])
			switch(key)
		{
			case 27: // ESCAPE
				clearSharedMem();
				exit(0);
				break;

			case 'w':
			case 'W':
				cameraLocation(0) += -velocity* sin(cameraOrientation(0) ) * sin(cameraOrientation(2) );
				cameraLocation(1) += velocity * sin(cameraOrientation(0) ) * cos(cameraOrientation(2) );
				cameraLocation(2) += -velocity * cos(cameraOrientation(0) );
				break;

			case 'a':
			case 'A':
				cameraLocation(0) += -velocity * cos(cameraOrientation(2) );
				cameraLocation(1) += -velocity * sin(cameraOrientation(2) );
				break;

			case 's':
			case 'S':
				cameraLocation(0) -= -velocity * sin(cameraOrientation(0)) * sin(cameraOrientation(2) );
				cameraLocation(1) -= velocity * sin(cameraOrientation(0)) * cos(cameraOrientation(2) );
				cameraLocation(2) -= -velocity * cos(cameraOrientation(0) );
				break;

			case 'd':
			case 'D':
				cameraLocation(0) -= -velocity * cos(cameraOrientation(2) );
				cameraLocation(1) -= -velocity * sin(cameraOrientation(2) );
				break;

			case 'j':
			case 'J':
				cameraOrientation(2) += velocity * 100* MOGI_PI/180.0;
				break;

			case 'l':
			case 'L':
				cameraOrientation(2) -= velocity * 100* MOGI_PI/180.0;
				break;

			case 'i':
			case 'I':
				cameraOrientation(0) += velocity * 100* MOGI_PI/180.0;
				break;

			case 'k':
			case 'K':
				cameraOrientation(0) -= velocity * 100* MOGI_PI/180.0;
				break;

			case '3':
				fstop += .05;
				break;

			case 'e':
				fstop -= .05;
				break;

			case '4':
				fLength += 1;
				break;

			case 'r':
				fLength -= 1;
				break;

			case '6':
				fDepth += .01;
				break;

			case 'y':
				fDepth -= .01;
				break;

			default:
				break;
		}


		//for (int key=0; key < 256; key++)	// check for any key held down:
		if ((key_state[key] == true) && (key_state_previous[key] == false) )
			switch(key)
		{
			case 'f':
			case 'F':
				debugFocus ^= 1;
				break;

			case '5':
				autofocus ^= 1;
				break;

			case 'v':
			case 'V':
				useBokeh ^= 1;
				break;

			case 'x':
			case 'X':
				useShadows ^= 1;
				break;

			case 'c':
			case 'C':
				colorMapEnable ^= 1;
				break;

			case 'h':
			case 'H':
				disparityMapEnable ^= 1;
				break;

			case 'n':
			case 'N':
				normalMapEnable ^= 1;
				break;

			case 'b':
			case 'B':
				specularMapEnable ^= 1;
				break;


			case 'm': // switch rendering modes (fill -> wire -> point)
			case 'M':
				drawMode = ++drawMode % 3;
				break;

			case '1': // ground texture
				break;

			case 'q': // ground texture
			case 'Q':


				break;

			case 'g': // switch rendering modes (fill -> wire -> point)
			case 'G':
				gridEnable ^= 1;
				break;

			case '-': // switch rendering modes (fill -> wire -> point)
			case '_':
				drawMode = 0;
				colorMapEnable= false;
				disparityMapEnable = false;
				normalMapEnable = false;
				specularMapEnable = false;
				gridEnable = true;
				useBokeh = false;
				useShadows = false;
				break;

			case '=':
			case '+':
				drawMode = 0;
				colorMapEnable= true;
				disparityMapEnable = true;
				normalMapEnable = true;
				specularMapEnable = true;
				gridEnable = false;
				useBokeh = true;
				useShadows = true;
				break;
				
			default:
				break;
		}
		
		key_state_previous[key] = key_state[key];
	}
	
}

void keyboardUpCB(unsigned char key, int x, int y)
{
	key_state[key] = false;
}

void keyboardCB(unsigned char key, int x, int y)
{
	key_state[key] = true;
}

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
		specularMapEnable = true;
		normalMapEnable = true;
		disparityMapEnable = false;	// Changed this for ES testing
		useHexapodCamera = false;
		gridEnable = true;
		debugFocus = false;
		useBokeh = true;
		useShadows = true;

		drawMode = 0;

		fstop = 2.5;//14;
		fDepth = 1;	//m
		fLength = 12;//10; //mm
		autofocus = false;
		vignetteEnable = false;

		screenWidth = xres;
		screenHeight = yres;

		cameraOrientation.setLength(3);
		cameraLocation.setLength(3);
		cameraLocation(2) = 1;

		cameraOffsetLocation.setLength(3);

		paverStoneNode = NULL;
		dynamicTexture = NULL;

		displayCB = ____doNothing;
	}

	void UIhandler::setCameraOffsetLocation(Mogi::Math::Vector& locationOffset) {
		cameraOffsetLocation = locationOffset;
	}
	void UIhandler::setCameraLocation(Mogi::Math::Vector& location) {
		cameraLocation = location;
	}
	void UIhandler::setCameraOrientation(Mogi::Math::Vector& orientation) {
		cameraOrientation = orientation;
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
									 SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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
#if !defined(__APPLE__) && !defined(ANDROID)
		glewExperimental = GL_TRUE;
		glewInit();
		glGetError(); // Added by Adrian to clear error buffer... hmmm
		// See: http://stackoverflow.com/questions/10857335/opengl-glgeterror-returns-invalid-enum-after-call-to-glewinit
#endif

#ifndef OPENGLES_FOUND
		// Some VAO needs to be bound before validating shaders in scene...
		// This VAO is not used for anything else.
		glGenVertexArrays(1, &_vertexArray);
		glBindVertexArray(_vertexArray);
		int i = glGetError();
		if (i != 0) {
			std::cout << " - Broke after VAO created: " << i << std::endl; // Adrian added printing of error value
			exit(-1);
		}
#endif

		std::cout << " - Allocating frame buffer." << std::endl;
		frameBuffer = new FrameBuffer(screenWidth, screenHeight);
		//		frameBuffer->resize(screenWidth, screenHeight);
		//frameBuffer->attachFramebuffer();
		std::cout << " - Allocating Bokeh post process." << std::endl;
		bokehPost = new MBbokeh(screenWidth, screenHeight);
		//frameBuffer->attachFramebuffer();
		std::cout << " - Allocating final frame buffer." << std::endl;
		testFinal = new MBpostprocess(screenWidth, screenHeight);
		//frameBuffer->attachFramebuffer();
		std::cout << " - Allocating camera." << std::endl;
		camera = new Camera;
		camera->setResolution(screenWidth, screenHeight);
		//frameBuffer->attachFramebuffer();
		std::cout << " - Allocating main scene." << std::endl;
		mainScene = new Scene;

		GeometryShaderParameters gParams;
		gShader = ShaderFactory::getInstance(&gParams);
		gBuffer = new MBGBuffer(screenWidth, screenHeight);

		std::cout << "OpenGL Version info:" << std::endl << "\t" << glGetString(GL_VERSION) << std::endl;
		std::cout << "OpenGL Renderer info:" << std::endl << "\t" << glGetString(GL_RENDERER) << std::endl;
		std::cout << "OpenGL Vendor info:" << std::endl << "\t" << glGetString(GL_VENDOR) << std::endl;
		std::cout << "OpenGL Shading info:" << std::endl << "\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		//std::cout << "OpenGL Extensions info:" << std::endl << "\t" << glGetString(GL_EXTENSIONS) << std::endl;

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
	}

	void UIhandler::initModels() {

		mainScene->getCameras().push_back(camera);
		paverStoneNode = Importer::loadObject(mainScene, "paverStones.obj","Objects");	// floor
		Importer::loadObject(mainScene, "brickWall.obj", "Objects/brickWall");	// walls

		char newName[64];
		for (int i = 0; i < N_LIGHTS; i++) {
			Node* node = Importer::loadObject(mainScene, "lightModel.dae", "Objects");	// Spot light
			if (node) {
				node->setLocation(0, 0, -1);
				node->setScale(25.4 / MM_PER_METER * 10.0);
				sprintf(newName, "lightModel %d", i);
				node->name = newName;
				sprintf(newName, "bulb %d", i);
				node->findChildByName("bulbulb")->name = newName;
			}
		}
		for (int i = 0; i < mainScene->getLights().size(); i++) {
			mainScene->getLights()[i]->setColor(cos(i*i*4)*cos(i*i*4), cos(i*i*i*8)*cos(i*i*i*8), cos(i*2)*cos(i*2));
			mainScene->getLights()[i]->setFOV(60);
		}

		if (mainScene->getLights().size() > 0) {
			int lightIndex = 2; // 100W Tungsten
			mainScene->getLights()[0]->setColor(lightColors[lightIndex].r, lightColors[lightIndex].g, lightColors[lightIndex].b);
		}
		if (mainScene->getLights().size() > 1) {
			int lightIndex = 14; // Mercury Vapor
			mainScene->getLights()[1]->setColor(lightColors[lightIndex].r, lightColors[lightIndex].g, lightColors[lightIndex].b);
		}

	}

	void UIhandler::initTestMeshesAndMaterials() {
		// simple object for testing:
		Node* node = Importer::loadObject(mainScene, "testCube.ply", "Objects");
		if(node) {
			node->setScale(100/MM_PER_METER);
			node->setLocation(0.5, 0.5, 0);
			node->name = "testCube";
		} else {
			std::cout << "Could not load testCube.ply :(" << std::endl;
		}
		// The importer imports a default material when it's preferred to just have it missing.
		// Instead, just remove any associationg by setting the material ID to an invalid value (<0)
		//mainScene->getMeshesFromNode(node)->at(0)->setMaterial(NULL);
		std::vector<Renderable*> renderablesFromNode = mainScene->getRenderablesFromNode(node);
		if (renderablesFromNode.size() > 0) {
			renderablesFromNode[0]->material->setColorSource(ShadowShaderParameters::COLOR_SOURCE_VERTEX_DATA);
		}

		std::vector<MBmesh*> testMeshes;
		std::vector<MBmaterial*> testMaterials;

		MBmesh* boxMesh = new MBmesh;
		boxMesh->makeBox(2, 2, 2);
		for (std::vector<VertexData>::iterator it = boxMesh->getVertexData().begin(); it != boxMesh->getVertexData().end(); it++) {
			it->color = RGBfromHSV((it->U + it->V*2)*5.0/18.0, 1, 1);
		}
		boxMesh->loadVerticesToVertexBufferObject();
		testMeshes.push_back(boxMesh);

		MBmesh* SphereMesh = new MBmesh;
		SphereMesh->makeSphereCorrectUV(1, 20, 20);
		for (std::vector<VertexData>::iterator it = SphereMesh->getVertexData().begin(); it != SphereMesh->getVertexData().end(); it++) {
			it->color = RGBfromHSV((it->U + it->V*2)*5.0/18.0, 1, 1);
		}
		SphereMesh->loadVerticesToVertexBufferObject();
		testMeshes.push_back(SphereMesh);

		MBmesh* ConeMesh = new MBmesh;
		ConeMesh->makeCone(1, 1, 20);
		for (std::vector<VertexData>::iterator it = ConeMesh->getVertexData().begin(); it != ConeMesh->getVertexData().end(); it++) {
			it->color = RGBfromHSV((it->U + it->V*2)*5.0/18.0, 1, 1);
		}
		ConeMesh->loadVerticesToVertexBufferObject();
		testMeshes.push_back(ConeMesh);

		MBmesh* PlaneMesh = new MBmesh;
		PlaneMesh->makePlane(2, 2);
		for (std::vector<VertexData>::iterator it = PlaneMesh->getVertexData().begin(); it != PlaneMesh->getVertexData().end(); it++) {
			it->color = RGBfromHSV((it->U + it->V*2)*5.0/18.0, 1, 1);
		}
		PlaneMesh->loadVerticesToVertexBufferObject();
		testMeshes.push_back(PlaneMesh);

		//				for (int i = 5; i <= 5; i++) {
		//					for (int j = 5; j <= 5; j++) {
		//		// Example adding a custom Renderable in the form of a "tennis ball"
		//		// 1) Add a node to the root to handle the model matrix:
		//		node = mainScene->rootNode.addNode("Ball");
		////		node->setLocation( -0.5, 0.5, 67/MM_PER_METER);
		//						node->setLocation((float)j/10.0 * 67.0*2.0/100, (float)i/10.0 * 67.0*2.0/100, 67/MM_PER_METER);
		//						node->setScale(67/MM_PER_METER);
		//		// 2) Create a mesh in the shape and size:
		////		MBmesh* SphereMesh = new MBmesh;
		////		SphereMesh->makeSphereCorrectUV(67/MM_PER_METER, 20, 20);
		////		SphereMesh->loadVerticesToVertexBufferObject();
		//		// 3) Create a material
		//		MBmaterial* TennisBallMaterial = new MBmaterial;
		//		TennisBallMaterial->setColorDiffuse(198.0/255.0, 237.0/255.0, 44.0/255.0);
		////						TennisBallMaterial->setShininess(10, 0.01);
		////						TennisBallMaterial->setColorDiffuse(((float)i/10.0 + 0.5), ((float)j/10.0 + 0.5), (0.5 - (float)i/10.0));
		//						TennisBallMaterial->setShininess(pow(2,((float)i + 5)+1), 1-((float)j/10.0 + 0.5));
		////		TennisBallMaterial->forceDisable(1, 1, 1, 1);
		//		// 4) Add the renderable to the scene:
		//		mainScene->addRenderable(node, SphereMesh, TennisBallMaterial);
		//
		////						mainScene->addRenderable(node, boxMesh, TennisBallMaterial);
		//					}
		//				}

		MBmaterial* vertexColors = new MBmaterial;
		vertexColors->setShininess(32, 1.0);
		vertexColors->setColorSource(ShadowShaderParameters::COLOR_SOURCE_VERTEX_DATA);
		testMaterials.push_back(vertexColors);

		MBmaterial* shinyRed = new MBmaterial;
		shinyRed->setShininess(64, 1.0);
		shinyRed->setColorDiffuse(1.0, 0, 0);
		testMaterials.push_back(shinyRed);

		MBmaterial* shinyBlue = new MBmaterial;
		shinyBlue->setShininess(16, 1.0);
		shinyBlue->setColorDiffuse(0.0, 0, 1.0);
		testMaterials.push_back(shinyBlue);


		MBmaterial* internalTexture = new MBmaterial;
		Texture* theTexture = new Texture;
		dynamicTexture = theTexture;
		Image8 textureImage(64,64);
		Pixel feltColor;
		feltColor.r = 198;
		feltColor.g = 237;
		feltColor.b = 44;
		//		feltColor.r = 0;
		//		feltColor.g = 0;
		//		feltColor.b = 255;
		Pixel seamColor;
		seamColor.r = 255;
		seamColor.g = 255;
		seamColor.b = 255;
		for (int i = 0; i < textureImage.width(); i++) {
			for (int j = 0; j < textureImage.height(); j++) {
				textureImage(i,j) = feltColor;
			}
		}
		textureImage.DrawCircle(textureImage.width()/2, textureImage.height()/2, (float)textureImage.width()*0.3, (float)textureImage.width()*0.03, seamColor);
		textureImage.DrawLine(textureImage.width()/2, textureImage.height()/2, textureImage.width(), textureImage.height(), seamColor);
		textureImage.DrawLine(textureImage.width()/2, textureImage.height()/2, textureImage.width()/2, textureImage.height(), seamColor);
		//		textureImage.DrawLine(0, textureImage.height()/2, textureImage.width()/2, textureImage.height()/2, seamColor);
		//		textureImage.DrawLine(textureImage.width()/2, textureImage.height()/2, textureImage.width(), textureImage.height()/2, seamColor);
		Pixel testColor;
		testColor.r = 255;
		testColor.g = 0;
		testColor.b = 0;
		//		textureImage.DrawThickLine(textureImage.width()/2, textureImage.height()/2, textureImage.width(), textureImage.height()/4, 2, testColor);
		//		textureImage.DrawThickLine(textureImage.width()/2, textureImage.height()/2, textureImage.width()/2+1, 0, 2, testColor);
		//		textureImage.DrawThickLine(0, textureImage.height()/2, textureImage.width()/2, textureImage.height()/2, 2, testColor);
		textureImage.DrawThickLine(textureImage.width()/4, textureImage.height()/4, textureImage.width()/2, textureImage.height()/2, 2, testColor);
		theTexture->setFromImage(textureImage);
		//		theTexture->setUniformName("colorMap");
		internalTexture->setTexture(theTexture, MBmaterial::COLOR);
		//		internalTexture->addTexture(theTexture);
		//		internalTexture->forceDisable(0, 1, 1, 1);
		internalTexture->setShininess(10, 0.01);

		testMaterials.push_back(internalTexture);

		if (paverStoneNode) {
			std::vector<Renderable*> paverRenderables = mainScene->getRenderablesFromNode(paverStoneNode->child(1));
			if (paverRenderables.size() > 0) {
				testMaterials.push_back(paverRenderables[0]->material);
			}
		}

		for (int i = 0; i < testMeshes.size(); i++) {
			for (int j = 0; j < testMaterials.size(); j++) {
				Node* newNode = mainScene->rootNode.addNode("testMeshAndMaterials");
				newNode->setLocation(-0.8 + 0.2*(float)j, -0.8 + 0.2*(float)i, 67/MM_PER_METER);
				newNode->setScale(67/MM_PER_METER);
				mainScene->addRenderable(newNode, testMeshes[i], testMaterials[j]);
			}
		}

	}

	void UIhandler::initialize() {
#if defined(BUILD_FOR_IOS) || defined(ANDROID)
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
		if (keyboard.risingEdge('2')) {
			vignetteEnable ^= 1;
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
			useBokeh = false;
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
		printf("  - 2\tEnable/Disable Vignette\n");
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
			Node *node = mainScene->rootNode.findChildByName(modelName);
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
		camera->setLocation(cameraLocation + cameraOffsetLocation);
		camera->setFOV(65);
		camera->update();

		//		mShadowShader.getParameters()->colorMapEnable = colorMapEnable;
		//
		//		mShadowShader.getParameters()->mColorSource =
		mShadowShader.getParameters()->normalMapEnable = normalMapEnable;
		mShadowShader.getParameters()->disparityMapEnable = disparityMapEnable;
		mShadowShader.getParameters()->specularMapEnable = specularMapEnable;

		mainScene->update();

		if(dynamicTexture) {
			static int priorSeconds = -1;
			if (priorSeconds != (int)keyboardTimer.runningTime()) {
				priorSeconds = (int)keyboardTimer.runningTime();
				Image8 textureImage(256,256);
				Pixel clearColor;
				clearColor.r = 0;
				clearColor.g = 127;
				clearColor.b = 0;
				for (int x = 0; x < textureImage.width(); x++) {
					for (int y = 0; y < textureImage.height(); y++) {
						textureImage(x,y) = clearColor;
					}

				}
				Pixel circleColor;
				circleColor.r = 0;
				circleColor.g = 0;
				circleColor.b = 255;
				textureImage.DrawCircle(textureImage.width()/2, textureImage.height()/2, textureImage.width()*0.3, textureImage.width()*0.03, circleColor);
				Pixel lineColor;
				lineColor.r = 255;
				lineColor.g = 0;
				lineColor.b = 0;
				textureImage.DrawThickLine(textureImage.width()/2, textureImage.height()/2, textureImage.width()/2 * (sin(priorSeconds * MOGI_PI/30.0)+1), textureImage.width()/2 * (cos(priorSeconds * MOGI_PI/30.0)+1), textureImage.width()*0.04, lineColor);
				dynamicTexture->setFromImage(textureImage);
			}
		}
	}

	void UIhandler::resize(int xres, int yres) {
		screenWidth = xres;
		screenHeight = yres;
		frameBuffer->resize(screenWidth, screenHeight);
		camera->setResolution(screenWidth, screenHeight);
		bokehPost->resize(screenWidth, screenHeight);
		testFinal->resize(screenWidth, screenHeight);
		gBuffer->resize(screenWidth, screenHeight);
	}

	void UIhandler::setFocus( float fstop, float fDepth, float fLength ) {
		this->fstop = fstop;
		this->fDepth = fDepth;
		this->fLength = fLength;
	}

	void UIhandler::render() {
		///////////////////////////////////////////////////////////////////////
		// Shadow map rendering:
		// Places the camera at each light to render shadow maps.
		///////////////////////////////////////////////////////////////////////
		for (int i = 0; i < mainScene->getLights().size(); i++) {
			mainScene->getLights()[i]->setEnabled(useShadows);
		}
		mainScene->buildShadowMaps();

		mPerformanceMeasure.takeMeasurement("shadowMaps");

		/////////////////////////////////////////////////////////////////////////
		// Scene rendering:
		// This draws the frame to a depth and render buffer to be used by bokeh:
		/////////////////////////////////////////////////////////////////////////
		glViewport( 0, 0, camera->getXresolution(), camera->getYresolution());
		glCullFace(GL_BACK);

#if defined(BUILD_FOR_IOS) || defined(ANDROID)
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

		mShadowShader.getParameters()->useShadows = useShadows;
		mShadowShader.getParameters()->numberOfLights = 2;

		// Main render call:
		mainScene->draw( camera, &mShadowShader );

		// Disable the frame buffer:
		frameBuffer->removeFramebuffer();

		mPerformanceMeasure.takeMeasurement("sceneRender");

		///////////////////////////////////////////////////////////////////////
		// Post processing:
		// Using the frame buffers, all post processing may now be handled:
		///////////////////////////////////////////////////////////////////////
		if (useBokeh)
		{
			// Apply the parameters for the bokeh process:
			bokehPost->setAutoFocus(autofocus);
			bokehPost->setVignetteEnabled(vignetteEnable);
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

		mPerformanceMeasure.takeMeasurement("bokehPost");
		//		mPerformanceMeasure.print();
		mPerformanceMeasure.reset();
	}

	void UIhandler::renderES() {

		//		frameBuffer->attachFramebuffer();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glViewport( 0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_CULL_FACE);

		//		mShadowShader.getParameters()->colorMapEnable = colorMapEnable;
		mShadowShader.getParameters()->normalMapEnable = normalMapEnable;
		mShadowShader.getParameters()->disparityMapEnable = disparityMapEnable;
		mShadowShader.getParameters()->specularMapEnable = specularMapEnable;
		mShadowShader.getParameters()->numberOfLights = N_LIGHTS;
		mShadowShader.getParameters()->useShadows = false;

		mainScene->draw( camera, &mShadowShader );

		//		frameBuffer->removeFramebuffer();
		//
		//		testFinal->process(frameBuffer->getRenderTexture(FrameBuffer::TEXTURE_TYPE_DIFFUSE), camera[0]);


		//		gBuffer->attachFramebuffer();
		//
		//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//		glViewport( 0, 0, screenWidth, screenHeight);
		//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//		glCullFace(GL_BACK);
		//		glEnable(GL_DEPTH_TEST);
		//		glEnable(GL_CULL_FACE);
		//
		//		mainScene->draw( camera, gShader );
		//
		//		gBuffer->removeFramebuffer();
		//		int type = ((int)keyboardTimer.runningTime()) % MBGBuffer::NUM_G_TEXTURES+1;
		//		if (type == MBGBuffer::NUM_G_TEXTURES) {
		//			testFinal->process(gBuffer->getDepthTexture(), camera[0]);
		//		} else {
		//			testFinal->process(gBuffer->getRenderTexture(type), camera[0]);
		//		}
	}

	void UIhandler::mainLoop() {
#if defined(BUILD_FOR_IOS) || defined(ANDROID)
		mPerformanceMeasure.takeMeasurement("mainLoopCall");
		// TODO: set up the rendering loop.
		keyboardTimer.update();
		updateModels();
		mPerformanceMeasure.takeMeasurement("updateModels");

		render();
#else
		unsigned int start;
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
					case SDL_WINDOWEVENT:
						switch (event.window.event) {
							case SDL_WINDOWEVENT_RESIZED:
								resize(event.window.data1, event.window.data2);
								break;

							default:
								break;
						}
						break;

					case SDL_QUIT:
						std::cout << " - SDL_QUIT event caught, shutting down..." << std::endl;
						running&=false;
						break;

					default:
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
		//		mScene->attachMeshToNode(body, Importer::addMesh(mScene, "body.stl", locationOfBodySTL.c_str()));
		MBmaterial* bodyMaterial = new MBmaterial;
		bodyMaterial->setColorDiffuse(0.6, 0.6, 0.6);
		mScene->addRenderable(body, Importer::addMesh(mScene, "body.stl", locationOfBodySTL.c_str()), bodyMaterial);

		std::string locationOfLegSTL = locationOfBodySTL.append("/leg");
		for (std::vector<Robot::HexapodLeg*>::iterator it = mHexapod->legs.begin(); it != mHexapod->legs.end(); it++) {
			Robot::HexapodLeg* leg = *it;
			hexapodLegToScene(mScene, leg, locationOfLegSTL);
		}
	}

	void hexapodLegToScene(Scene* mScene, Robot::HexapodLeg* mHexapodLeg, std::string locationOfLegSTL) {
		Node* base = mHexapodLeg->getBaseNode();
		MBmesh* bodyMesh = Importer::addMesh(mScene, "base.stl", locationOfLegSTL);
		//		mScene->attachMeshToNode(base, bodyMesh);
		MBmaterial* legMaterial = new MBmaterial;
		legMaterial->setColorDiffuse(0.6, 0.6, 0.6);
		mScene->addRenderable(base, bodyMesh, legMaterial);

		Node* coxa = base->child(0);
		Node* femur = coxa->child(0);
		Node* tibia = femur->child(0);

		MBmesh* coxaMesh = Importer::addMesh(mScene, "coxa.stl", locationOfLegSTL);
		MBmesh* femurMesh = Importer::addMesh(mScene, "femur.stl", locationOfLegSTL);
		MBmesh* tibiaMesh = Importer::addMesh(mScene, "tibia.stl", locationOfLegSTL);
		
		mScene->addRenderable( coxa,  coxaMesh, legMaterial);
		mScene->addRenderable(femur, femurMesh, legMaterial);
		mScene->addRenderable(tibia, tibiaMesh, legMaterial);
	}
	
#ifdef _cplusplus
}
#endif

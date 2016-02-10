/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#include "postprocess.h"
#include <mogi.h>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBpostprocess::MBpostprocess() {
	frameBuffer = new FrameBuffer();
	frameBuffer->attachFramebuffer();	// TODO: check this, artifact OpenGL->OpenGLES
	isFinalRender = false;

	camera.makeOrthographic(2, 2, -1, 1);
	// the model matrix is non-existent (identity), so place the camera just
	// above.
	camera.setLocation(0, 0, -0.5);

	biasMatrix.makeI(4);
	biasMatrix(0, 0) = 0.5;
	biasMatrix(1, 1) = 0.5;
	biasMatrix(2, 2) = 0.5;
	biasMatrix(0, 3) = 0.5;
	biasMatrix(1, 3) = 0.5;
	biasMatrix(2, 3) = 0.5;

	modelViewProjectionMatrix = camera.getProjectionMatrix()
			* camera.getViewMatrix();
	biasedModelViewProjectionMatrix = biasMatrix * modelViewProjectionMatrix;

	GLfloat tempVertices[] = { -1, -1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1,
			1, 0, 0, 1, -1, 1, 0, 0, 0, 1, 1, 0, 0, 1, -1, -1, 0, 0, 0, 1, 1, 0,
			0, 1, 1, -1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1 };
	planeVertices = new GLfloat[60];
	for (int i = 0; i < 60; i++) {
		planeVertices[i] = tempVertices[i];
	}

	// renderPlane.vertexData = planeVertices;
	// renderPlane.vertexSize = sizeof(tempVertices);

	std::cout << "Loading finalProcess.vsh ...";
#ifdef BUILD_FOR_IOS
	std::string postProcessPlane = "models";
	std::string vertexSource = "shaders/ios";
	std::string fragmentSource = "shaders/ios";
#else // BUILD_FOR_IOS
	#ifdef RESOURCES_DIRECTORY
		std::string postProcessPlane = RESOURCES_DIRECTORY;
		std::string vertexSource = RESOURCES_DIRECTORY;
		std::string fragmentSource = RESOURCES_DIRECTORY;
		postProcessPlane.append("/models");
		vertexSource.append("/shaders");
		fragmentSource.append("/shaders");
	#else
		std::string postProcessPlane = "Objects";
		std::string vertexSource = "Shaders";
		std::string fragmentSource = "Shaders";
	#endif
#endif // BUILD_FOR_IOS
	vertexSource.append("/finalProcess.vsh");
	fragmentSource.append("/finalProcess.fsh");
	basicShader.initialize(vertexSource.c_str(), fragmentSource.c_str());
	std::cout << "Done." << std::endl;

	Scene temp;	// TODO: this is really hacky.
	temp.loadObject("postProcess.dae", postProcessPlane.c_str());
	std::vector<MBmesh*> tempMeshes;

	if (temp.rootNode.numberOfChildren() > 0) {
		tempMeshes = temp.nodeToMeshMap[temp.rootNode.child(0)];
	}
	if (tempMeshes.size() == 1) {	// this is a little strict, but we onl expect a single mesh here.
		renderPlane = *tempMeshes[0];
	} else {
		std::cout << "Error!  Could not load mesh from " << postProcessPlane
				<< "/postProcess.dae, POST PROCESSING WILL NOT WORK!"
				<< std::endl;
	}
}

MBpostprocess::~MBpostprocess() {
	delete[] planeVertices;
}

Texture& MBpostprocess::getDepthTexture() {
	return frameBuffer->getDepthTexture();
}

Texture& MBpostprocess::getRenderTexture(int index) {
	return frameBuffer->getRenderTexture(index);
}

void MBpostprocess::setAsFinalRender(bool value) {
	isFinalRender = value;
}

void MBpostprocess::initBuffers() {
	if (!isFinalRender) {
		frameBuffer->attachFramebuffer();
	}

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef OPENGLES_FOUND	// The polygogonmode function is undefined, as is GL_FILL
		//http://stackoverflow.com/questions/4627770/any-glpolygonmode-alternative-on-iphone-opengl-es
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);	// TODO: figure this out for ES
#else
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	glDisable (GL_DEPTH_TEST);
}

void MBpostprocess::finishBuffers() {
	if (!isFinalRender) {
		frameBuffer->removeFramebuffer();
	}
}

void MBpostprocess::drawPlane(MBshader& shader) {
	shader.sendMatrix("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shader.sendMatrix("biasedModelViewProjectionMatrix",
			biasedModelViewProjectionMatrix);

	renderPlane.draw(&shader);
}

int MBpostprocess::process(Texture& renderTexture, Camera& renderCamera) {
	initBuffers();
	glViewport(0, 0, renderCamera.getXresolution(),
			renderCamera.getYresolution());

	renderTexture.setUniformName("bgl_RenderedTexture");

	basicShader.useProgram();
	{
		renderTexture.sendTextureToShader(&basicShader);
		drawPlane(basicShader);
	}
	basicShader.stopProgram();

	finishBuffers();

	return 0;
}

/////////////////////////////////////////////////////////
//			Bokeh:
/////////////////////////////////////////////////////////
MBbokeh::MBbokeh() :
		MBpostprocess() {
	// Bokeh specific stuff:
	std::cout << "Loading Bokeh shader...";

	std::string vertexSource(Scene::getResourceDirectory());
	std::string fragmentSource(Scene::getResourceDirectory());
#ifdef BUILD_FOR_IOS
	vertexSource.append("/shaders/ios");
	fragmentSource.append("/shaders/ios");
#else
#ifdef RESOURCES_DIRECTORY
	vertexSource.append("/shaders");
	fragmentSource.append("/shaders");
#else
	vertexSource.append("/Shaders");
	fragmentSource.append("/Shaders");
#endif
#endif
	vertexSource.append("/bokeh.vsh");
	fragmentSource.append("/bokeh.fsh");
	bokehShader.initialize(vertexSource.c_str(), fragmentSource.c_str());
	std::cout << "Done." << std::endl;

	autofocus = true;
	fstop = 14;
	focalDepth = 1;
	fLength = 10;
	centerDepth = .5;
	debugFocus = false;
}

int MBbokeh::process(Texture& renderTexture, Texture& depthTexture,
		Camera& renderCamera) {
	initBuffers();
	renderTexture.setUniformName("bgl_RenderedTexture");
	depthTexture.setUniformName("bgl_DepthTexture");

	glViewport(0, 0, renderCamera.getXresolution(),
			renderCamera.getYresolution());
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable (GL_DEPTH_TEST);
	bokehShader.useProgram();
	{
		renderTexture.sendTextureToShader(&bokehShader);
		depthTexture.sendTextureToShader(&bokehShader);

		GLfloat data[renderCamera.getXresolution()
				* renderCamera.getYresolution()];
#ifdef OPENGLES_FOUND
		// TODO: figure out how to get textures from ES
#else
		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
#endif
		centerDepth = data[(renderCamera.getXresolution()
				* (renderCamera.getYresolution() + 1)) / 2];
		GLfloat trueDepth = 2 * centerDepth - 1;
		centerDepth = 2.0 * renderCamera.getNearClip()
				* renderCamera.getFarClip()
				/ (renderCamera.getFarClip() + renderCamera.getNearClip()
						- trueDepth
								* (renderCamera.getFarClip()
										- renderCamera.getNearClip()));
		if (autofocus) {
			focalDepth = centerDepth;
		}

		bokehShader.sendFloat("bgl_RenderedTextureWidth",
				renderCamera.getXresolution());
		bokehShader.sendFloat("bgl_RenderedTextureHeight",
				renderCamera.getYresolution());
		bokehShader.sendFloat("focalDepth", focalDepth);  // in m
		bokehShader.sendFloat("focalLength", fLength);    // in mm
		bokehShader.sendFloat("fstop", fstop);
		bokehShader.sendInteger("showFocus", debugFocus);
		bokehShader.sendInteger("autofocus", 0);

		bokehShader.sendFloat("zfar", renderCamera.getFarClip());
		bokehShader.sendFloat("znear", renderCamera.getNearClip());

		drawPlane(bokehShader);
	}
	bokehShader.stopProgram();
	// Disable bokeh textures:
	// for (int i = 0; i < 2; i++) {
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture( GL_TEXTURE_2D, 0);
	//}

	finishBuffers();
	return 0;
}

/////////////////////////////////////////////////////////
//			lighting:
/////////////////////////////////////////////////////////
MBdeferredLighting::MBdeferredLighting() {
	std::cout << "Loading Lighting shader...";

	std::string vertexSource = Scene::getResourceDirectory();
	std::string fragmentSource = Scene::getResourceDirectory();
#ifdef RESOURCES_DIRECTORY
	vertexSource.append("/shaders");
	fragmentSource.append("/shaders");
#else
	vertexSource.append("Shaders");
	vertexSource.append("Shaders");
#endif
	vertexSource.append("/lighting.vsh");
	fragmentSource.append("/lighting.fsh");
	lightingShader.initialize(vertexSource.c_str(), fragmentSource.c_str());
	std::cout << "Done." << std::endl;
}

int MBdeferredLighting::process(MBGBuffer* geometryBuffer, Camera& renderCamera,
		std::vector<MBlight*>& lights) {
	initBuffers();

	glViewport(0, 0, renderCamera.getXresolution(),
			renderCamera.getYresolution());
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);

	glEnable (GL_BLEND);
	glBlendEquation (GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glDisable (GL_DEPTH_TEST);

	lightingShader.useProgram();
	{
		geometryBuffer->setDepthUniform("uGBufferDepthTex");
		geometryBuffer->setRenderUniform(MBGBuffer::TEXTURE_TYPE_DIFFUSE,
				"uGBufferDiffuseTex");
		geometryBuffer->setRenderUniform(MBGBuffer::TEXTURE_TYPE_NORMAL,
				"uGBufferGeometricTex");
		geometryBuffer->setRenderUniform(MBGBuffer::TEXTURE_TYPE_SPECULARITY,
				"uGBufferMaterialTex");
		geometryBuffer->sendTexturesToShader(&lightingShader);

		float uTanHalfFov = tan(renderCamera.getFOV() / 2);
		float aspect = renderCamera.getAspect();

		lightingShader.sendFloat("uTanHalfFov", uTanHalfFov);
		lightingShader.sendFloat("uAspectRatio", aspect);

		for (int i = 0; i < lights.size(); i++) {
			// lights[i]->
			drawPlane(lightingShader);
		}
	}
	lightingShader.stopProgram();

	finishBuffers();
	glDisable(GL_BLEND);
	return 0;
}

#ifdef _cplusplus
}
#endif

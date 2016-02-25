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
#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	void generatedPostProcessMeshCode(MBmesh* meshToSet);

	MBpostprocess::MBpostprocess(int xRes, int yRes) {
		frameBuffer = new FrameBuffer(xRes, yRes);
		frameBuffer->attachFramebuffer();	// TODO: check this, artifact OpenGL->OpenGLES
		isFinalRender = false;

		camera.makeOrthographic(2, 2, -1, 1);
		// the model matrix is non-existent (identity), so place the camera just above.
		camera.setLocation(0, 0, -0.5);

		biasMatrix.makeI(4);
		biasMatrix(0, 0) = 0.5;
		biasMatrix(1, 1) = 0.5;
		biasMatrix(2, 2) = 0.5;
		biasMatrix(0, 3) = 0.5;
		biasMatrix(1, 3) = 0.5;
		biasMatrix(2, 3) = 0.5;

		modelViewProjectionMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
		biasedModelViewProjectionMatrix = biasMatrix * modelViewProjectionMatrix;

		std::cout << "Loading Final Process shader ... ";
		FinalProcessShaderParameters parameters;
		basicShader = ShaderFactory::getInstance(&parameters);
		std::cout << "Done." << std::endl;

		generatedPostProcessMeshCode(&renderPlane);

		// The below commented code was used just to generate the code for the
		// function: void generatedPostProcessMeshCode(MBmesh* meshToSet);
		// which is used above.

		//	Scene temp;	// TODO: this is really hacky.
		////	temp.loadObject("postProcess.dae", postProcessPlane.c_str());
		//	Importer::loadObject(&temp, "postProcess.dae", postProcessPlane.c_str());
		//
		//	std::vector<MBmesh*> tempMeshes;
		//
		//	if (temp.rootNode.numberOfChildren() > 0) {
		//		tempMeshes = temp.nodeToMeshMap[temp.rootNode.child(0)];
		//	}
		//	if (tempMeshes.size() == 1) {	// this is a little strict, but we onl expect a single mesh here.
		//		renderPlane = *tempMeshes[0];
		//		renderPlane.generateCodeFromMesh();
		//	} else {
		//		std::cout << "Error!  Could not load mesh from " << postProcessPlane
		//				<< "/postProcess.dae, POST PROCESSING WILL NOT WORK!"
		//				<< std::endl;
		//	}
	}

	MBpostprocess::~MBpostprocess() {
		delete frameBuffer;
	}

	void MBpostprocess::resize(int xRes, int yRes) {
		frameBuffer->resize(xRes, yRes);
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

	void MBpostprocess::drawPlane(MBshader* shader) {
		shader->sendMatrix("modelViewProjectionMatrix", modelViewProjectionMatrix);
		shader->sendMatrix("biasedModelViewProjectionMatrix", biasedModelViewProjectionMatrix);

		renderPlane.draw(shader);
	}

	int MBpostprocess::process(Texture& renderTexture, Camera& renderCamera) {
		initBuffers();
		glViewport(0, 0, renderCamera.getXresolution(), renderCamera.getYresolution());

		renderTexture.setUniformName("bgl_RenderedTexture");

		basicShader->useProgram();
		{
			renderTexture.sendTextureToShader(basicShader);
			drawPlane(basicShader);
		}
		basicShader->stopProgram();

		finishBuffers();

		return 0;
	}

	/////////////////////////////////////////////////////////
	//			Bokeh:
	/////////////////////////////////////////////////////////
	MBbokeh::MBbokeh(int xRes, int yRes) :
	MBpostprocess(xRes, yRes) {
		// Bokeh specific stuff:
		std::cout << "Loading Bokeh shader...";
		bokehShader = new BokehShader;// StaticShader;
		std::cout << "Done." << std::endl;

		autofocus = true;
		vignetteEnable = false;
		fstop = 14;
		focalDepth = 1;
		fLength = 10;
		centerDepth = .5;
		debugFocus = false;
	}

	int MBbokeh::process(Texture& renderTexture, Texture& depthTexture, Camera& renderCamera) {
		initBuffers();
		renderTexture.setUniformName("bgl_RenderedTexture");
		depthTexture.setUniformName("bgl_DepthTexture");

		glViewport(0, 0, renderCamera.getXresolution(), renderCamera.getYresolution());
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bokehShader->getParameters()->autoFocusEnable = autofocus;
		bokehShader->getParameters()->vignettingEnable = vignetteEnable;
		bokehShader->getParameters()->debugEnable = debugFocus;

		glDisable (GL_DEPTH_TEST);
		bokehShader->useProgram();
		{
			renderTexture.sendTextureToShader(bokehShader);
			depthTexture.sendTextureToShader(bokehShader);


			if (autofocus) {
#ifdef OPENGLES_FOUND
				// TODO: figure out how to get textures from ES
				//		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
				//glBindRenderbuffer(<#GLenum target#>, <#GLuint renderbuffer#>)
				//			glReadPixels(0, 0, renderCamera.getXresolution(), renderCamera.getYresolution(), GL_DEPTH_COMPONENT, GL_FLOAT, data);
#else
				GLfloat* data = new GLfloat[renderCamera.getXresolution() * renderCamera.getYresolution()];

				glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);

				centerDepth = data[(renderCamera.getXresolution() * (renderCamera.getYresolution() + 1)) / 2];
				GLfloat trueDepth = 2 * centerDepth - 1;
				centerDepth = 2.0 * renderCamera.getNearClip() * renderCamera.getFarClip() / (renderCamera.getFarClip() + renderCamera.getNearClip() - trueDepth * (renderCamera.getFarClip() - renderCamera.getNearClip()));

				focalDepth = centerDepth;
				delete [] data;
#endif
			}

			bokehShader->sendFloat("bgl_RenderedTextureWidth", renderCamera.getXresolution());
			bokehShader->sendFloat("bgl_RenderedTextureHeight", renderCamera.getYresolution());
			bokehShader->sendFloat("focalDepth", focalDepth);  // in m
			bokehShader->sendFloat("focalLength", fLength);    // in mm
			bokehShader->sendFloat("fstop", fstop);
			bokehShader->sendInteger("showFocus", debugFocus);
			//bokehShader->sendInteger("autofocus", 0);

			bokehShader->sendFloat("zfar", renderCamera.getFarClip());
			bokehShader->sendFloat("znear", renderCamera.getNearClip());

			drawPlane(bokehShader);
		}
		bokehShader->stopProgram();
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
	MBdeferredLighting::MBdeferredLighting(int xRes, int yRes)
 :MBpostprocess(xRes, yRes) {
	 std::cout << "Loading Lighting shader...";
	 DeferredLightingShaderParameters parameters;
	 lightingShader = ShaderFactory::getInstance(&parameters);
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

		lightingShader->useProgram();
		{
			geometryBuffer->setDepthUniform("uGBufferDepthTex");
			geometryBuffer->setRenderUniform(MBGBuffer::TEXTURE_TYPE_DIFFUSE, "uGBufferDiffuseTex");
			geometryBuffer->setRenderUniform(MBGBuffer::TEXTURE_TYPE_NORMAL, "uGBufferGeometricTex");
			geometryBuffer->setRenderUniform(MBGBuffer::TEXTURE_TYPE_SPECULARITY, "uGBufferMaterialTex");
			geometryBuffer->sendTexturesToShader(lightingShader);

			float uTanHalfFov = tan(renderCamera.getFOV() / 2);
			float aspect = renderCamera.getAspect();

			lightingShader->sendFloat("uTanHalfFov", uTanHalfFov);
			lightingShader->sendFloat("uAspectRatio", aspect);

			for (int i = 0; i < lights.size(); i++) {
				// lights[i]->
				drawPlane(lightingShader);
			}
		}
		lightingShader->stopProgram();

		finishBuffers();
		glDisable(GL_BLEND);
		return 0;
	}


	void generatedPostProcessMeshCode(MBmesh* meshToSet) {
		VertexData data;
		std::vector<VertexData> vertexData;
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = 1;
		data.position.x = -1;
		data.position.y = -1;
		data.position.z = 0;
		data.color.x = 0.6;
		data.color.y = 0.6;
		data.color.z = 0.6;
		data.tangent.x = 1;
		data.tangent.y = 0;
		data.tangent.z = 0;
		data.U = 0;
		data.V = 0;
		vertexData.push_back(data);
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = 1;
		data.position.x = 1;
		data.position.y = -1;
		data.position.z = 0;
		data.color.x = 0.6;
		data.color.y = 0.6;
		data.color.z = 0.6;
		data.tangent.x = 1;
		data.tangent.y = 0;
		data.tangent.z = 0;
		data.U = 0;
		data.V = 0;
		vertexData.push_back(data);
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = 1;
		data.position.x = 1;
		data.position.y = 1;
		data.position.z = 0;
		data.color.x = 0.6;
		data.color.y = 0.6;
		data.color.z = 0.6;
		data.tangent.x = 1;
		data.tangent.y = 0;
		data.tangent.z = 0;
		data.U = 0;
		data.V = 0;
		vertexData.push_back(data);
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = 1;
		data.position.x = -1;
		data.position.y = 1;
		data.position.z = 0;
		data.color.x = 0.6;
		data.color.y = 0.6;
		data.color.z = 0.6;
		data.tangent.x = 1;
		data.tangent.y = 0;
		data.tangent.z = 0;
		data.U = 0;
		data.V = 0;
		vertexData.push_back(data);
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = 1;
		data.position.x = -1;
		data.position.y = -1;
		data.position.z = 0;
		data.color.x = 0.6;
		data.color.y = 0.6;
		data.color.z = 0.6;
		data.tangent.x = 1;
		data.tangent.y = 0;
		data.tangent.z = 0;
		data.U = 0;
		data.V = 0;
		vertexData.push_back(data);
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = 1;
		data.position.x = 1;
		data.position.y = 1;
		data.position.z = 0;
		data.color.x = 0.6;
		data.color.y = 0.6;
		data.color.z = 0.6;
		data.tangent.x = 1;
		data.tangent.y = 0;
		data.tangent.z = 0;
		data.U = 0;
		data.V = 0;
		vertexData.push_back(data);
		unsigned int index;
		std::vector<unsigned int> indices;
		index = 0;
		indices.push_back(index);
		index = 1;
		indices.push_back(index);
		index = 2;
		indices.push_back(index);
		index = 3;
		indices.push_back(index);
		index = 4;
		indices.push_back(index);
		index = 5;
		indices.push_back(index);
		meshToSet->setVertexData(vertexData);
		meshToSet->setIndices(indices);
		meshToSet->loadVerticesToVertexBufferObject();
	}
	
#ifdef _cplusplus
}
#endif

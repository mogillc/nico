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
#ifdef SDL2_FOUND

#include <scene.h>
#include "importer/importer.h"
#include <iostream>

// Need SDL for image file loading:
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace Mogi;
using namespace Math;
using namespace Simulation;

SDL_Window *mainwindow; /* Our window handle */
SDL_GLContext maincontext; /* Our opengl context handle */

void initiazizeOpenGL();
void destroyOpenGL();
bool testObjectAdd();

int main(int argc, char *argv[]) {
	bool allTestsPass = true;

	initiazizeOpenGL();

	std::cout << "Testing adding objects..." << std::endl;
	allTestsPass = testObjectAdd() ? allTestsPass : false;

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();

	destroyOpenGL();

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testObjectAdd() {
	bool allTestsPass = true;

	Scene *scene = new Scene;

	std::cout << "Trying to add a null stringed object file to the scene ... ";
	Node *result = Importer::loadObject(scene, NULL, NULL);// scene->loadObject(NULL, NULL);
	if (result != NULL) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Trying to add a non-existent object file to the scene ... ";
	result = Importer::loadObject(scene, "/sjdhfsd/ahfa", "fsdfsdf");
	if (result != NULL) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	delete scene;

	return allTestsPass;
}

void initiazizeOpenGL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
		exit(EXIT_FAILURE);

	/* Request opengl 3.2 context.
	 * SDL doesn't have the ability to choose which profile at this time of
	 * writing,
	 * but it should default to the core profile */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);

	/* Turn on double buffering with a 24bit Z buffer.
	 * You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	/* Create our window centered at 512x512 resolution */
	mainwindow = SDL_CreateWindow("testScene", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, 10, 10, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!mainwindow) /* Die if creation failed */
		exit(EXIT_FAILURE);

	// checkSDLError(__LINE__);

	/* Create our opengl context and attach it to our window */
	maincontext = SDL_GL_CreateContext(mainwindow);
	// checkSDLError(__LINE__);

	/* This makes our buffer swap syncronized with the monitor's vertical refresh
	 */
	SDL_GL_SetSwapInterval(1);

	std::cout << "OpenGL Version info:" << std::endl << "\t"
			<< glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Renderer info:" << std::endl << "\t"
			<< glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Vendor info:" << std::endl << "\t"
			<< glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Shading info:" << std::endl << "\t"
			<< glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}
void destroyOpenGL() {
	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();
}

#else // SDL2_FOUND
int main() {
	return 0;
}
#endif // SDL2_FOUND

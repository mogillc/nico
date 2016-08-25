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

#include "mogiGL.h"

#include <string>
#include <iostream>
#include <regex.h>
#include <cstring> // Needed for strlen, strcopy, ect.
#include <cstdlib> // Needed for atoi etc.

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	bool Simulation::replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if(start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	void Simulation::replaceAll(std::string& str, const std::string& from, const std::string& to) {
		if(from.empty())
			return;
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

	std::string Simulation::glGetErrorToString(GLenum Status) {
		switch (Status) {
			case GL_NO_ERROR:
				return "GL_NO_ERROR";
				break;
			case GL_INVALID_ENUM:
				return "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				return "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				return "GL_INVALID_OPERATION";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				return "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				return "GL_OUT_OF_MEMORY";
				break;
		}
		return "???";
	}

	int Simulation::_checkGLError(const char* function, const char* file,int line) {

		GLenum Status = glGetError();
		if (Status != GL_NO_ERROR) {
			std::cerr << "GL Error:" << glGetErrorToString(Status) << "\n\tFile:" << file << "\n\tFunction:" << function << "\n\tLine:" << line << std::endl;
			exit(-1);
		}

		return Status;
	}

	MogiGLInfo* MogiGLInfo::instance = NULL;

	MogiGLInfo::MogiGLInfo() {
		std::string glslVersion;
		glslVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::string glVersion;
		glVersion = (const char*)glGetString(GL_VERSION);

		versionMajorGL = 2;
		versionMinorGL = 0;

		versionGLSL = MOGI_GLSL_VERSION_100;

		useGLES = false;

		regex_t regexCompiled;
		if (regcomp(&regexCompiled, "(OpenGL )(ES )?([0-9])\\.([0-9])", REG_EXTENDED)) {
			std::cerr << "Could not compile regular expression." << std::endl;
			return;
		}

			size_t maxGroups = 5;
			regmatch_t groupArray[maxGroups];
			if (regexec(&regexCompiled, glVersion.c_str(), maxGroups, groupArray, 0))
			{
				std::cerr << "No version found in GLSL version string:\"" << glVersion << "\"" << std::endl;
			} else {
				regoff_t offset = 0;
				for (unsigned int g = 0; g < maxGroups; g++) {
					if (groupArray[g].rm_so == (size_t) -1)
						break;
					if (g == 0)
						offset = groupArray[g].rm_eo;

					char cursorCopy[strlen( glVersion.c_str()) + 1];
					strcpy(cursorCopy, glVersion.c_str() );
					cursorCopy[groupArray[g].rm_eo] = 0;
					std::cout << "Group " << g << ": [" << groupArray[g].rm_so << "-" << groupArray[g].rm_eo << "]: " << cursorCopy + groupArray[g].rm_so << std::endl;

					if (g == 2) {
						std::string group1(cursorCopy + groupArray[g].rm_so);
						if( group1.compare("ES ") == 0) {
							useGLES = true;
						} else {
							useGLES = false;
						}
					}
//					if(useGLES) {
//						if(g == 3) {
//							versionMajorGL = atoi(cursorCopy + groupArray[g].rm_so);
//						} else if (g == 4) {
//							versionMinorGL = atoi(cursorCopy + groupArray[g].rm_so);
//						}
//					} else {
//						if(g == 2) {
//							versionMajorGL = atoi(cursorCopy + groupArray[g].rm_so);
//						} else if (g == 3) {
//							versionMinorGL = atoi(cursorCopy + groupArray[g].rm_so);
//						}
//					}
				}

			}

		regfree(&regexCompiled);

		glGetIntegerv(GL_MAJOR_VERSION, &versionMajorGL);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinorGL);

		int glVersionInt = versionMajorGL*10 + versionMinorGL;

		switch (glVersionInt) {
			case 20:
				if (useGLES) {
					versionGLSL = MOGI_GLSL_VERSION_100;
				} else {
					versionGLSL = MOGI_GLSL_VERSION_110;
				}
   			 break;

			case 21:
				versionGLSL = MOGI_GLSL_VERSION_120;
				break;

			case 30:
				if (useGLES) {
					versionGLSL = MOGI_GLSL_VERSION_300;
				} else {
					versionGLSL = MOGI_GLSL_VERSION_130;
				}
				break;

			case 31:
				versionGLSL = MOGI_GLSL_VERSION_140;
				break;

			case 32:
				versionGLSL = MOGI_GLSL_VERSION_150;
				break;

			case 33:
				versionGLSL = MOGI_GLSL_VERSION_330;
				break;

			case 40:
				versionGLSL = MOGI_GLSL_VERSION_400;
				break;

			case 41:
				versionGLSL = MOGI_GLSL_VERSION_410;
				break;

			case 42:
				versionGLSL = MOGI_GLSL_VERSION_420;
				break;

			case 43:
				versionGLSL = MOGI_GLSL_VERSION_430;
				break;

			case 44:
				versionGLSL = MOGI_GLSL_VERSION_440;
				break;

			case 45:
				versionGLSL = MOGI_GLSL_VERSION_450;
				break;

			default:
				std::cerr << "Warning: unable to detect supported GLSL version for GL version: " << versionMajorGL << "." << versionMinorGL << std::endl;
    			break;
		}

	}

	bool MogiGLInfo::isGLES() {
		return useGLES;
	}

	MogiGLInfo* MogiGLInfo::getInstance() {
		if (instance == NULL) {
			instance = new MogiGLInfo;
		}
		return instance;
	}

	Mogi_GLSL_Version MogiGLInfo::getVersion() {
		return versionGLSL;
	}

	const char* MogiGLInfo::getGLSLVersionStr() {
		switch ( versionGLSL ) {
			case MOGI_GLSL_VERSION_100:
				return "100";
			case MOGI_GLSL_VERSION_110:
				return "110";
			case MOGI_GLSL_VERSION_120:
				return "120";
			case MOGI_GLSL_VERSION_130:
				return "130";
			case MOGI_GLSL_VERSION_140:
				return "140";
			case MOGI_GLSL_VERSION_150:
				return "150";
			case MOGI_GLSL_VERSION_300:
				return "300";
			case MOGI_GLSL_VERSION_330:
				return "330";
			case MOGI_GLSL_VERSION_400:
				return "400";
			case MOGI_GLSL_VERSION_410:
				return "410";
			case MOGI_GLSL_VERSION_420:
				return "420";
			case MOGI_GLSL_VERSION_430:
				return "430";
			case MOGI_GLSL_VERSION_440:
				return "440";
			case MOGI_GLSL_VERSION_450:
				return "450";

		}
	}

#ifdef _cplusplus
}
#endif

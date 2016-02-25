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

	MogiGLInfo* MogiGLInfo::instance = NULL;

	MogiGLInfo::MogiGLInfo() {
		std::string glslVersion;
		glslVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

		versionMajor = 1;
		versionMinor = 00;

		regex_t regexCompiled;
		if (regcomp(&regexCompiled, "([0-9])\\.([0-9][0-9])", REG_EXTENDED)) {
			std::cerr << "Could not compile regular expression." << std::endl;
			return;
		} else {
			size_t maxGroups = 3;
			regmatch_t groupArray[maxGroups];
			if (regexec(&regexCompiled, glslVersion.c_str(), maxGroups, groupArray, 0))
			{
				std::cerr << "No version found in GLSL version string:\"" << glslVersion << "\"" << std::endl;
			} else {
				regoff_t offset = 0;
				for (unsigned int g = 0; g < maxGroups; g++) {
					if (groupArray[g].rm_so == (size_t) -1)
						break;
					if (g == 0)
						offset = groupArray[g].rm_eo;

					char cursorCopy[strlen( glslVersion.c_str()) + 1];
					strcpy(cursorCopy, glslVersion.c_str() );
					cursorCopy[groupArray[g].rm_eo] = 0;
					//std::cout << "Group " << g << ": [" << groupArray[g].rm_so << "-" << groupArray[g].rm_eo << "]: " << cursorCopy + groupArray[g].rm_so << std::endl;

					if (g == 1) {
						versionMajor = atoi(cursorCopy + groupArray[g].rm_so);
					} else if (g == 2) {
						versionMinor = atoi(cursorCopy + groupArray[g].rm_so);
					}
				}

			}
		}
		regfree(&regexCompiled);


	}

	MogiGLInfo* MogiGLInfo::getInstance() {
		if (instance == NULL) {
			instance = new MogiGLInfo;
		}
		return instance;
	}

	int MogiGLInfo::getVersion() {
		return versionMajor*100 + versionMinor;
	}

#ifdef _cplusplus
}
#endif

/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *            This program is distributed under the LGPL, version 2           *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License              *
 *   version 2.1 as published by the Free Software Foundation;                *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   https://github.com/mogillc/nico/tree/master/edisonLibmogiPackage/libmogi *
 *                                                                            *
 *****************************************************************************/

#include "mogi.h"
#include "mogi_version_i.h"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;

std::string Mogi::getResourceDirectory() {
#if defined(BUILD_FOR_IOS) || defined(ANDROID)
	return _getMogiResourceDirectory();
#else // BUILD_FOR_IOS
	#ifdef RESOURCES_DIRECTORY
		return RESOURCES_DIRECTORY;
	#else // RESOURCES_DIRECTORY
		return "";
	#endif // RESOURCES_DIRECTORY
#endif // BUILD_FOR_IOS
}

VersionInfo Mogi::getLibraryVersion(void) {
	VersionInfo version;

	version.major = MOGI_MAJOR_VERSION;
	version.minor = MOGI_MINOR_VERSION;
	version.micro = MOGI_MICRO_VERSION;
	version.version_str = MOGI_VERSION_STRING;
	version.svn_str = MOGI_SVN_VERSION;

	return version;
}

#ifdef _cplusplus
}
#endif

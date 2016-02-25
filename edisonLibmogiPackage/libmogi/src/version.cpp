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

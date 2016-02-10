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

#ifndef MOGI_H
#define MOGI_H

#include <string>

/**
 * @namespace Mogi
 * \brief A set of utilities designed originally for the Mogi hexapods.
 */
namespace Mogi {

/*!
 \struct VersionInfo
 \brief Used for library version information.
 */
typedef struct {
	/*! Major version.
	 */
	int major;
	/*! Minor version.
	 */
	int minor;
	/*! Micro version.
	 */
	int micro;
	/*! Version as a string.
	 */
	const char *version_str;
	/*! Repository version, if checked out through SVN.
	 */
	const char *svn_str;
} VersionInfo;

/*! Returns the installed version information.
 */
VersionInfo getLibraryVersion(void);

/*! Returns the path to the resource directory from installation.
 */
std::string getResourceDirectory();

}  // End namespace Mogi

#endif

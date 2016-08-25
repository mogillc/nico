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

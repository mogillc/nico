/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                          Author: Adrian Lizarraga                          *
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

/*
 * JSONParser.h
 *
 *  Created on: Oct 5, 2014
 *      Author: adrian
 */

#ifndef STRING_BUFFER_H_
#define STRING_BUFFER_H_
#include <iostream>
#include <sstream>


namespace Mogi {
	
	/*! \class StringBuffer
	 \brief Basic stringbuf implementation supporting cross-platform output logging. This StringBuffer class is a wrapper over std::cout. See mogi/Logger.h.
	 */
	class StringBuffer : public std::stringbuf {
	public:
		StringBuffer() {}
		~StringBuffer() {
			pubsync();
		}

		/*! \brief Called when parent stream is flushed to synchronize contents of buffer with associated character sequence
		 */
		int sync() {
			std::cout << str();
			str("");

			return std::cout ? 0 : -1;
		}
	};
	
} /* namespace Mogi */

#endif /* STRING_BUFFER_H_ */
